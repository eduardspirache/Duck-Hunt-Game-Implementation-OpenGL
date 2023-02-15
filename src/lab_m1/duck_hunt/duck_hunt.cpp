#include "lab_m1/duck_hunt/duck_hunt.h"

#include <vector>
#include <iostream>

#include "lab_m1/duck_hunt/transform2D.h"
#include "lab_m1/duck_hunt/object2D.h"
#include "lab_m1/duck_hunt/functions.h"

using namespace std;
using namespace m1;

glm::ivec2 resolution;

Duck_hunt::Duck_hunt()
{
}


Duck_hunt::~Duck_hunt()
{
}


void Duck_hunt::Init()
{
    /* Window initial settings */
    resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);
    time_elapsed = 0.0;
    duck_count = 0;
    draw_hitbox = false;
    speed = 2;
    changed_speed = false;
    
    /* Duck mesh initiation */
    duck_scale = resolution.y / 15;
    duck_color = glm::vec3(0.49, 0.25, 0.08);
    head_color = glm::vec3(0, 0.4, 0);
    beak_color = glm::vec3(1, 0.37, 0.05);
    wing_rotation = 0;

    finished_wrotation = false;
    rotation_angle = 0;
    escaped = true;
    shot = false;
    score = 0;

    /* UI Initiation */
    heart_count = bullet_count = 3;

    /* Create duck object */
    Mesh* body = object2D::CreateTriangle("body", glm::vec3(0, 0, 0), duck_scale, duck_color, true);
    AddMeshToList(body);

    Mesh* head = object2D::CreateCircle("head", glm::vec3(0, 0, 0), duck_scale / 2, head_color, true);
    AddMeshToList(head);
    
    Mesh* beak = object2D::CreateTriangle("beak", glm::vec3(0, 0, 0), duck_scale / 4, beak_color, true);
    AddMeshToList(beak);

    float wing_size = duck_scale - 0.15 * duck_scale;
    Mesh* leftwing = object2D::CreateRightTriangle("leftwing", glm::vec3(0, 0, 0), wing_size, duck_color, 'l', true);
    AddMeshToList(leftwing);

    Mesh* rightwing = object2D::CreateRightTriangle("rightwing", glm::vec3(0, 0, 0), wing_size, duck_color, 'r', true);
    AddMeshToList(rightwing);

    hitbox_width = 2.1 * duck_scale;
    hitbox_height = 3.1 * duck_scale;
    hitbox_corner1 = glm::vec2(0);
    hitbox_corner2 = glm::vec2(hitbox_width, hitbox_height);
    Mesh* hitbox = object2D::CreateRectangle("hitbox", glm::vec3(0, 0, 0), hitbox_width, hitbox_height, glm::vec3(1, 1, 1), false);
    AddMeshToList(hitbox);

    Mesh* corner2 = object2D::CreateSquare("corner2", glm::vec3(0, 0, 0), 5, glm::vec3(1, 0, 0), true);
    AddMeshToList(corner2);
    
    /* Create GUI */
    Mesh* grass = object2D::CreateRectangle("grass", glm::vec3(0, 0, 0), resolution.x, resolution.y/5, glm::vec3(0, 0.65, 0.1), true);
    AddMeshToList(grass);

    Mesh* background = object2D::CreateRectangle("background", glm::vec3(0, 0, 0), resolution.x, resolution.y, glm::vec3(0.5, 0.85, 0.97), true);
    AddMeshToList(background);

    Mesh* score_bar = object2D::CreateRectangle("score_bar", glm::vec3(0, 0, 0), resolution.x/6, resolution.y / 15, glm::vec3(0, 0.1, 0.6), false);
    AddMeshToList(score_bar);

    Mesh* score = object2D::CreateRectangle("score", glm::vec3(0, 0, 0), 3, resolution.y / 15, glm::vec3(0, 0.1, 0.6), true);
    AddMeshToList(score);
    
    Mesh* bullet = object2D::CreateRectangle("bullet", glm::vec3(0, 0, 0), 10, resolution.y / 18, glm::vec3(0.8, 0.1, 0.4), true);
    AddMeshToList(bullet);

    Mesh* heart = object2D::CreateCircle("heart", glm::vec3(0, 0, 0), resolution.y/42, glm::vec3(1, 0.1, 0.1), true);
    AddMeshToList(heart);



    tr = new gfxc::TextRenderer(window->props.selfDir, window->GetResolution().x, window->GetResolution().y);
    tr->Load(window->props.selfDir + "\\assets\\fonts\\Hack-Bold.ttf", 128);

}


void Duck_hunt::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Duck_hunt::Update(float deltaTimeSeconds)
{   
    if (heart_count == 0) {
        tr->RenderText("GAME OVER", resolution.x/2 - resolution.x/8, resolution.y/2 - resolution.y/14, 0.5);
        return;
    }
    if (score > 54) {
        tr->RenderText("WIN", resolution.x / 2 - resolution.x / 17, resolution.y / 2 - resolution.y / 10, 0.5);
        tr->RenderText("Final Score: " + std::to_string(score), resolution.x / 2 - resolution.x / 7, resolution.y / 2 + resolution.y / 26, 0.3);
        return;
    }
    if (duck_count > 0 && duck_count % 5 == 0 && changed_speed == false) {
        speed++;
        changed_speed = true;
    }
    /* ---- Configuring duck spawn and escape ---- */
    time_elapsed += deltaTimeSeconds;
    if (escaped) {
        duck_count++;
        changed_speed = false;
        escaped = false;
        shot = false;
        bullet_count = 3;
        time_elapsed = 0;
        // Generate randon spawn location and flying angle
        random_spawn(spawn_point, direction_angle, resolution);
        current_position = spawn_point;

        if (direction_angle < 0) {
            inc_x = -cos(direction_angle);
            rotation_angle = 1.5708 - module(direction_angle);
        }
        else {
            inc_x = cos(direction_angle);
            rotation_angle = - 1.5708 + direction_angle;
        }

        inc_y = sin(module(direction_angle));

        direction_vector = glm::vec2(inc_x, inc_y);
    }
    direction_angle = rotation_angle;

    /* -------- Configuring Duck Movement -------- */
    current_position += direction_vector * (deltaTimeSeconds * (200 + 50 * speed));
    hitbox_corner1 = current_position;
    hitbox_corner2 = hitbox_corner1 + glm::vec2(hitbox_width, hitbox_height);

    if (time_elapsed > 9) {
        rotation_angle = 0;
        direction_vector = glm::vec2(0, 0.7 + 0.1 * speed);
        if (current_position[1] > resolution.y) {
            escaped = true;
            heart_count--;
        }
    }
    else if (shot == true) {
        rotation_angle = 3.14159265359;
        direction_vector = glm::vec2(0, -0.7 - 0.1 * speed);
        if (current_position[1] < resolution.y / 5) {
            escaped = true;
            score += 2;   
        }
    }
    else {
        hit(current_position, direction_vector, direction_angle, rotation_angle, hits_count, resolution, hitbox_corner2);
    }
    
    /* -------- Configuring Wing Movement -------- */
    if (shot == false) {
        if (finished_wrotation == true) {
            wing_rotation -= deltaTimeSeconds * (1.1 * speed);
            if (wing_rotation < -0.1)
                finished_wrotation = false;
        }
        else {
            wing_rotation += 0.8 * deltaTimeSeconds * (1.05 * speed);
        }
    
        if (wing_rotation > 0.1) {
            finished_wrotation = true;
        }
    }

    /* ---- Render UI & Enviroment ---- */
    glm::mat3 UI_matrix = glm::mat3(1);
    UI_matrix *= transform2D::Translate(resolution.x * 0.8, resolution.y * 0.8);
    
    glm::mat3 scorebar_matrix = UI_matrix;
    RenderMesh2D(meshes["score_bar"], shaders["VertexColor"], scorebar_matrix);
    glm::mat3 score_matrix = scorebar_matrix;
    score_matrix = transform2D::Translate(resolution.x * 0.8, resolution.y * 0.8) * transform2D::Scale(0.001 * score * resolution.x, 1);
    RenderMesh2D(meshes["score"], shaders["VertexColor"], score_matrix);

    glm::mat3 heart_matrix = UI_matrix;
    heart_matrix *= transform2D::Translate(resolution.x/48, resolution.y / 8);
    if (heart_count >= 1) {
        glm::mat3 heart1 = heart_matrix;
        RenderMesh2D(meshes["heart"], shaders["VertexColor"], heart1);
        if (heart_count >= 2) {
            glm::mat3 heart2 = heart1 * transform2D::Translate(resolution.x / 30, 0);
            RenderMesh2D(meshes["heart"], shaders["VertexColor"], heart2);
            if (heart_count >= 3) {
                glm::mat3 heart3 = heart2 * transform2D::Translate(resolution.x / 30, 0);
                RenderMesh2D(meshes["heart"], shaders["VertexColor"], heart3);
            }
        }
    }

    glm::mat3 bullet_matrix = UI_matrix;
    bullet_matrix *= transform2D::Translate(resolution.x / 9, resolution.y / 10);
    if (bullet_count >= 1) {
        glm::mat3 bullet1 = bullet_matrix;
        RenderMesh2D(meshes["bullet"], shaders["VertexColor"], bullet1);
        if (bullet_count >= 2) {
            glm::mat3 bullet2 = bullet1 * transform2D::Translate(resolution.x / 50, 0);
            RenderMesh2D(meshes["bullet"], shaders["VertexColor"], bullet2);
            if (bullet_count >= 3) {
                glm::mat3 bullet3 = bullet2 * transform2D::Translate(resolution.x / 50, 0);
                RenderMesh2D(meshes["bullet"], shaders["VertexColor"], bullet3);
            }
        }
    }

    glm::mat3 grass_matrix = glm::mat3(1);
    RenderMesh2D(meshes["grass"], shaders["VertexColor"], grass_matrix);

    



    /* ----------- Rendering duck body ----------- */
    // Configure a parent matrix that all other duck parts inherit
    glm::mat3 duck_matrix = glm::mat3(1);
    duck_matrix *= transform2D::Translate(current_position[0], current_position[1]);
    duck_matrix *= transform2D::Translate(1.05 * duck_scale, 1.1 * duck_scale) * transform2D::Rotate(rotation_angle) * transform2D::Translate(-1.05 * duck_scale, -1.1 * duck_scale);


    // Draw hitbox
    if (draw_hitbox == true) {
        glm::mat3 hitbox_matrix = glm::mat3(1);
        hitbox_matrix *= transform2D::Translate(hitbox_corner1[0], hitbox_corner1[1]);
        RenderMesh2D(meshes["hitbox"], shaders["VertexColor"], hitbox_matrix);
    }
    
    // Make the body inherit the parent matrix
    glm::mat3 body_matrix = glm::mat3(1);
    body_matrix = duck_matrix * transform2D::Translate(0.55 * duck_scale, 0);
    
    // Transpose everything else according to the body matrix
    glm::mat3 head_matrix = glm::mat3(1);
    head_matrix = body_matrix * transform2D::Translate(0.5 * duck_scale, 2.2 * duck_scale);
    RenderMesh2D(meshes["head"], shaders["VertexColor"], head_matrix);

    // Rendering body under the head
    RenderMesh2D(meshes["body"], shaders["VertexColor"], body_matrix);

    glm::mat3 beak_matrix = glm::mat3(1);
    beak_matrix = body_matrix * transform2D::Translate(0.42 * duck_scale , 2.6 * duck_scale);
    RenderMesh2D(meshes["beak"], shaders["VertexColor"], beak_matrix);

    // The wings rotate by the right angle of the triangle
    glm::mat3 leftwing_matrix = glm::mat3(1);
    leftwing_matrix = body_matrix * transform2D::Translate(0.25 * duck_scale, 0.85 * duck_scale) * transform2D::Rotate(-wing_rotation) * transform2D::Translate(0, 0);
    RenderMesh2D(meshes["leftwing"], shaders["VertexColor"], leftwing_matrix);

    glm::mat3 rightwing_matrix = glm::mat3(1);
    rightwing_matrix = body_matrix * transform2D::Translate(0.75 * duck_scale, 0.85 * duck_scale) * transform2D::Rotate(wing_rotation) * transform2D::Translate(0, 0);
    RenderMesh2D(meshes["rightwing"], shaders["VertexColor"], rightwing_matrix);

    /* Render background */
    glm::mat3 background_matrix = glm::mat3(1);
    RenderMesh2D(meshes["background"], shaders["VertexColor"], background_matrix);
    
}


void Duck_hunt::FrameEnd()
{

}


void Duck_hunt::OnInputUpdate(float deltaTime, int mods)
{
}


void Duck_hunt::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_SPACE && draw_hitbox == true) {
        draw_hitbox = false;
    }
    else if (key == GLFW_KEY_SPACE && draw_hitbox == false) {
        draw_hitbox = true;
    }
}


void Duck_hunt::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Duck_hunt::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Duck_hunt::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    bullet_count--;
    mouseY = resolution.y - mouseY;
    if (mouseX > hitbox_corner1[0] && mouseX < hitbox_corner2[0] && mouseY > hitbox_corner1[1] && mouseY < hitbox_corner2[1] && bullet_count > 0) {
        shot = true;
    }
}


void Duck_hunt::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Duck_hunt::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Duck_hunt::OnWindowResize(int width, int height)
{
}

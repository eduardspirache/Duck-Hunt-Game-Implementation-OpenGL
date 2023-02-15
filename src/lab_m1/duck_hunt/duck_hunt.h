#pragma once

#include "components/simple_scene.h"
#include "components/text_renderer.h"


namespace m1
{
    class Duck_hunt : public gfxc::SimpleScene
    {
     public:
         Duck_hunt();
        ~Duck_hunt();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

     protected:
         float time_elapsed;
         bool draw_hitbox;
         gfxc::TextRenderer* tr;
         int duck_count;

         /* Duck settings */
         int duck_x, duck_y;
         glm::vec3 duck_position;
         glm::vec3 duck_color, head_color, beak_color;
         float duck_scale, wing_size, wing_rotation;
         bool finished_wrotation;
         float rotation_angle;
         float speed;
         bool changed_speed;

         /* Duck movement */
         glm::vec2 spawn_point;
         int hits_count;
         bool escaped;
         bool shot;
         glm::vec2 hitbox_corner1;
         glm::vec2 hitbox_corner2;
         float hitbox_width;
         float hitbox_height;

         /* UI*/
         int heart_count;
         int bullet_count;
         int score;


         /* Direction vector related
         * format: direction_vector(inc_x, inc_y)
         * where:
         *   - inc_x = cos(direction_angle)
         *   - inc_y = sin(direction_angle)
         */
         glm::vec2 current_position;
         glm::vec2 direction_vector;
         float direction_angle;
         float inc_x, inc_y;


    };
}   // namespace m1

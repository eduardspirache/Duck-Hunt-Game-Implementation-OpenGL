# Duck Hunt

Developed a duck shooting game using OpenGL, featuring single-duck appearance and a three-shot limit per turn. Incorporated player lives and loss conditions.

# How it Works

This is a project about creating a game similar to Duck Hunt. The player has to shoot ducks that appear on the screen. The ducks are constructed using 2D geometric primitives (at least four: body, wings, head, and beak). The ducks move in a random direction and at a random speed, and they change direction when they hit the edge of the screen. When a duck is shot, it falls to the ground with a vertical animation, and when it escapes, it flies off the screen with a vertical animation. Only one duck appears on the screen at a time, and a new duck appears after a certain time has passed since the last duck appeared. The player has to shoot as many ducks as possible within a limited time. The game uses hierarchical transformations to animate the ducks.

# How to Run

You can run the project from an IDE, as well as standalone, from anywhere on disk. For example:

-   Windows, one of the following:
    -   `.\bin\Debug\GFXFramework`
    -   or just open the `.sln` file in Visual Studio, then press `F5` to run it

-   Linux and macOS:
    -   `./bin/Debug/GFXFramework`

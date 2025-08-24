#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define MAX_FRAME_DELAY     20
#define MIN_FRAME_DELAY      1

int main(void)
{
    SetTraceLogLevel(LOG_NONE);


    int screenWidth = 800;
    int screenHeight = 450;
    int windowX = 0, windowY = 0;
    
    int windowMoveSpeed = 5;
    int windowScaleSpeed = 5;

    bool isFullscreen = false;


    auto files = LoadDirectoryFilesEx("./", ".gif", true);
    if(files.count == 0) {
        SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
        screenWidth = 250;
        screenHeight = 100;
        InitWindow(screenWidth, screenHeight, "Gif player");
        windowX = GetMonitorWidth(GetCurrentMonitor()) / 2. - screenWidth / 2.;
        windowY = GetMonitorHeight(GetCurrentMonitor()) / 2. - screenHeight / 2.;
        SetWindowPosition(windowX, windowY);
        while (!WindowShouldClose())
        {
            BeginDrawing();
            ClearBackground((Color){0, 0, 0, 0});
            int result = GuiMessageBox((Rectangle){(float)GetScreenWidth()/2 - 250 / 2.f, (float)GetScreenHeight()/2 - 100 / 2.f, 250, 100}, "#191#Error", "No gifs in app directory", "Close");
            EndDrawing();
            if(result >= 0){
                CloseWindow();
                return -1;
            }
        }
    }

    SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_MOUSE_PASSTHROUGH | FLAG_WINDOW_TOPMOST);

    InitWindow(screenWidth, screenHeight, "Gif player");

    bool showHelp = true;

    int currentGif = 0;
    int gifsCount = files.count;

    int animFrames = 0;

    // NOTE: GIF data is always loaded as RGBA (32bit) by default
    // NOTE: Frames are just appended one after another in image.data memory
    Image image = LoadImageAnim(files.paths[0], &animFrames);
    screenWidth = image.width, screenHeight = image.height;
    SetWindowSize(screenWidth, screenHeight);

    windowX = GetMonitorWidth(GetCurrentMonitor()) / 2. - screenWidth / 2.;
    windowY = GetMonitorHeight(GetCurrentMonitor()) / 2. - screenHeight / 2.;

    SetWindowPosition(windowX, windowY);


    // NOTE: We will update this texture when required with next frame data
    Texture2D texture = LoadTextureFromImage(image);

    unsigned int nextFrameDataOffset = 0;  // Current byte offset to next frame in image.data

    int currentAnimFrame = 0;       // Current animation frame to load and draw
    int frameDelay = 0;             // Frame delay to switch between animation frames
    int frameCounter = 0;           // General frames counter

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        frameCounter++;
        if (frameCounter >= frameDelay)
        {
            // Move to next frame
            // NOTE: If final frame is reached we return to first frame
            currentAnimFrame++;
            if (currentAnimFrame >= animFrames) currentAnimFrame = 0;

            // Get memory offset position for next frame data in image.data
            nextFrameDataOffset = image.width*image.height*4*currentAnimFrame;

            // Update GPU texture data with next frame image data
            // WARNING: Data size (frame size) and pixel format must match already created texture
            UpdateTexture(texture, ((unsigned char *)image.data) + nextFrameDataOffset);

            frameCounter = 0;
        }

        windowX = GetWindowPosition().x;
        windowY = GetWindowPosition().y;

        if(!IsWindowMaximized()) {
            isFullscreen = false;
        }

        if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) &&  IsKeyDown(KEY_RIGHT)) {
            screenWidth += windowScaleSpeed;
            SetWindowSize(screenWidth, screenHeight);
        } else if ((IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT)) && IsKeyDown(KEY_RIGHT)) {
            windowX += windowMoveSpeed;
            SetWindowPosition(windowX, windowY);
        }

        if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) &&  IsKeyDown(KEY_LEFT)) {
            screenWidth -= windowScaleSpeed;
            SetWindowSize(screenWidth, screenHeight);
        } else if ((IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT)) && IsKeyDown(KEY_LEFT)) {
            windowX -= windowMoveSpeed;
            SetWindowPosition(windowX, windowY);
        }

        if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) &&  IsKeyDown(KEY_UP)) {
            screenHeight -= windowScaleSpeed;
            SetWindowSize(screenWidth, screenHeight);
        } else if((IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT)) && IsKeyDown(KEY_UP)) {
            windowY -= windowMoveSpeed;
            SetWindowPosition(windowX, windowY);
        }


        if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) &&  IsKeyDown(KEY_DOWN)) {
            screenHeight += windowScaleSpeed;
            SetWindowSize(screenWidth, screenHeight);
        } else if((IsKeyUp(KEY_LEFT_SHIFT) && IsKeyUp(KEY_RIGHT_SHIFT)) && IsKeyDown(KEY_DOWN)) {
            windowY += windowMoveSpeed;
            SetWindowPosition(windowX, windowY);
        }




        if (IsKeyPressed(KEY_RIGHT_BRACKET)) frameDelay++;
        else if (IsKeyPressed(KEY_LEFT_BRACKET)) frameDelay--;
        else if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT)) && IsKeyPressed(KEY_ENTER)) {
            isFullscreen = !isFullscreen;
            if (isFullscreen) {
                MaximizeWindow();
                screenWidth = GetScreenWidth(), screenHeight = GetScreenHeight();
            } else {
                RestoreWindow();
                screenWidth = GetScreenWidth(), screenHeight = GetScreenHeight();
            }
        }
        else if (IsKeyPressed(KEY_ENTER)){
            RestoreWindow();
            screenWidth = image.width, screenHeight = image.height;
            SetWindowSize(screenWidth, screenHeight);
        }
        else if (IsKeyPressed(KEY_PERIOD)) {
            UnloadTexture(texture);
            UnloadImage(image);

            image = LoadImageAnim(files.paths[(++currentGif)%gifsCount], &animFrames);
            screenWidth = image.width, screenHeight = image.height;
            SetWindowSize(screenWidth, screenHeight);

            texture = LoadTextureFromImage(image);
            nextFrameDataOffset = currentAnimFrame = frameDelay = frameCounter = 0;
        }
        else if (IsKeyPressed(KEY_COMMA)) {
            UnloadTexture(texture);
            UnloadImage(image);

            if(currentGif == 0)
                currentGif = std::max(gifsCount - 1, 0);
            else
                --currentGif;
            image = LoadImageAnim(files.paths[currentGif%gifsCount], &animFrames);
            screenWidth = image.width, screenHeight = image.height;
            SetWindowSize(screenWidth, screenHeight);

            texture = LoadTextureFromImage(image);
            nextFrameDataOffset = currentAnimFrame = frameDelay = frameCounter = 0;
        }
        else if (IsKeyPressed(KEY_H)) {
            showHelp = !showHelp;
        }


        if (frameDelay > MAX_FRAME_DELAY) frameDelay = MAX_FRAME_DELAY;
        else if (frameDelay < MIN_FRAME_DELAY) frameDelay = MIN_FRAME_DELAY;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground((Color){0, 0, 0, 0});
            // DrawText(TextFormat("TOTAL GIF FRAMES:  %02i", animFrames), 50, 30, 20, LIGHTGRAY);
            // DrawText(TextFormat("CURRENT FRAME: %02i", currentAnimFrame), 50, 60, 20, GRAY);
            // DrawText(TextFormat("CURRENT FRAME IMAGE.DATA OFFSET: %02i", nextFrameDataOffset), 50, 90, 20, GRAY);
            //
            // DrawText("FRAMES DELAY: ", 100, 305, 10, DARKGRAY);
            // DrawText(TextFormat("%02i frames", frameDelay), 620, 305, 10, DARKGRAY);
            // DrawText("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", 290, 350, 10, DARKGRAY);
            //
            // for (int i = 0; i < MAX_FRAME_DELAY; i++)
            // {
            //     if (i < frameDelay) DrawRectangle(190 + 21*i, 300, 20, 20, RED);
            //     DrawRectangleLines(190 + 21*i, 300, 20, 20, MAROON);
            // }

            // Calculate aspect ratio and scaled dimensions
            // float aspectRatio = (float)image.height / (float)image.width;
            // int scaledWidth = GetScreenWidth();
            // int scaledHeight = (int)(scaledWidth * aspectRatio);

            DrawTexturePro(
                texture,
                (Rectangle){ 0, 0, (float)image.width, (float)image.height },
                (Rectangle){ 0, 0, (float)screenWidth, (float)screenHeight },
                (Vector2){ 0, 0 },
                0.0f,
                WHITE
            );

            if(showHelp) {
                auto msgRect = (Rectangle){ screenWidth / 2.f - 250 / 2.f, screenHeight / 2.f - 200 / 2.f, 250, 200 };

                GuiPanel(msgRect, "#193#Help");
                GuiLabel((Rectangle){msgRect.x + 20, msgRect.y + 20, msgRect.width - 40, msgRect.height - 20}, "Controls\n"
                                                "\"h\" - toggle help\n"
                                                "\".\" - next gif\n"
                                                "\",\" - previous gif\n"
                                                "\"]\" - slow down gif\n"
                                                "\"[\" - speed up gif\n"
                                                "\"key arrows\" - move window\n"
                                                "\"shift + key arrows\" - resize window\n"
                                                "\"Enter\" - set initial gif size\n"
                                                "\"shift + Enter\" - toggle fullscreen");
            }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);
    UnloadImage(image);

    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}

#ifndef GAME_H
#define GAME_H
// protection gate
#include <SDL2/SDL.h>
#include <memory>
#include "../ECS/ECS.h"
#include <memory>
#include "../AssetStore/AssetStore.h"
const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {
private:
    bool isRunning;
    SDL_Window *window;
    bool isDebug;
    SDL_Renderer *renderer;
    int millisecsPreviousFrame = 0;
    std::unique_ptr<Registy> registy;
    std::unique_ptr<AssetStore> assetStore;
    Entity player;
public: // public api
    Game();
    ~Game();
    void Init();
    void Run();
    void ProcessInput();
    void Setup();
    void Update();
    void Render();
    void Destory();
    void LoadLevel(int level);
    int windowWidth;
    int windowHight;
    int levelWidth;
    int levelHeight;
    int score = 0;
};

#endif

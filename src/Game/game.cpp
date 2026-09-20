#include "game.h"
#include "../Components/BoxColliderComponent.h"
#include "../Systems/AnimationSystem.h"
#include "../Components/AnimationComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_timer.h>
#include <fstream>
#include <glm/glm.hpp>
#include <memory>
#include "../Systems/MovementSystem.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/RenderSystem.h"
#include "glm/ext/vector_float2.hpp"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Components/KeyBoardControlComponent.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Components/CameraComponent.h"
#include "../Systems/CameraSystem.h"
#include "../Events/EventBus.h"
#include "../Events/CollisionEvent.h"

// #include <iostream>

Game::Game() {
    isRunning = false;
    isDebug = false;
    registy = std::make_unique<Registy>();
    assetStore = std::make_unique<AssetStore>();
    Logger::Log("Game created!");
}
Game::~Game() { 
    Logger::Log("Game destroyed!");
}
void Game::Init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Err("ERROR init SDL");
        return;
    }
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    windowWidth = 1280;
    windowHight = 720;
    window =
        SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         windowWidth, windowHight, SDL_WINDOW_BORDERLESS);
    if (!window) {
        Logger::Err("Error creating SDL window");
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        Logger::Err("Error creating SDL renderer.");
        return;
    }
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    isRunning = true;
}
void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
                if (sdlEvent.key.keysym.sym == SDLK_d){
                    isDebug = !isDebug;
                }
                break;
        }
    }
}
void Game::LoadLevel(int level){
    registy->AddSystem<MovementSystem>();
    registy->AddSystem<RenderSystem>();
    registy->AddSystem<AnimationSystem>();
    registy->AddSystem<CollisionSystem>();
    registy->AddSystem<RenderColliderSystem>();
    registy->AddSystem<KeyboardControlSystem>();
    registy->AddSystem<CameraSystem>();
    // init game object
    assetStore->AddTexture(renderer,"tank-image","./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer,"truck-image","./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer,"chopper-image","./assets/images/chopper.png");
    assetStore->AddTexture(renderer,"radar-image","./assets/images/radar.png");
    assetStore->AddTexture(renderer,"tilemap-image","./assets/tilemaps/jungle.png");
    int tileSize = 32;
    double tileScale = 2.0;
    int mapNumCols = 25;
    int mapNumRows = 20;
    std::fstream mapFile;
    levelWidth = mapNumCols * tileSize * tileScale;
    levelHeight = mapNumRows * tileSize * tileScale;
    mapFile.open("./assets/tilemaps/jungle.map");
    for (int y = 0; y < mapNumRows; y++){
        for(int x = 0; x < mapNumCols; x++){
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = registy->CreateEntity();
            tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileSize * tileScale)), glm::vec2(tileScale,tileScale), 0.0);
            tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0,srcRectX, srcRectY);
        }
    }
    mapFile.close();

    Entity chopper = registy->CreateEntity();
    chopper.AddComponent<TransformComponent>(glm::vec2(10.0,10.0), glm::vec2(1.0,1.0),0.0);
    chopper.AddComponent<RigidBodyCompoent>(glm::vec2(0.0,0.0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32,32,1);
    chopper.AddComponent<AnimationComponent>(2,15,true);
    chopper.AddComponent<KeyBoardControlComponet>();
    chopper.AddComponent<CameraComponent>();

    Entity radar = registy->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74,10.0), glm::vec2(1.0,1.0),0.0);
    radar.AddComponent<RigidBodyCompoent>(glm::vec2(0.0,0.0));
    radar.AddComponent<SpriteComponent>("radar-image", 64,64,2);
    radar.AddComponent<AnimationComponent>(8,5,true);

    Entity tank = registy->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(100.0,10.0), glm::vec2(1.0,1.0),0.0);
    tank.AddComponent<RigidBodyCompoent>(glm::vec2(-30.0,0.0));
    tank.AddComponent<SpriteComponent>("tank-image", 32,32,2);
    tank.AddComponent<BoxColliderComponent>(32,32);

    Entity track = registy->CreateEntity();
    track.AddComponent<TransformComponent>(glm::vec2(10.0,10.0), glm::vec2(1.0,1.0),0.0);
    track.AddComponent<RigidBodyCompoent>(glm::vec2(20.0,0.0));
    track.AddComponent<SpriteComponent>("truck-image", 32,32,1);
    track.AddComponent<BoxColliderComponent>(32,32);

}
void Game::Setup() {
    EventBus::Subscribe<CollisionEvent>([](CollisionEvent& e){
        Logger::Log("Collision between " + std::to_string(e.a.GetId()) +
                    " and " + std::to_string(e.b.GetId()));
        e.a.Kill();
        e.b.Kill();
    });
    LoadLevel(1);
}
void Game::Update() {
    int timeToWait =
        MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
        SDL_Delay(timeToWait);
    }
    double deltatime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0f;

    millisecsPreviousFrame = SDL_GetTicks();
    
    registy->Update();
    registy->GetSystem<KeyboardControlSystem>().Update();
    registy->GetSystem<MovementSystem>().Update(deltatime);
    registy->GetSystem<CameraSystem>().Update(windowWidth, windowHight, levelWidth, levelHeight);
    registy->GetSystem<AnimationSystem>().Update();
    registy->GetSystem<CollisionSystem>().Update();
}
void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);
    // todo:render all game objects
    // draw a png texture
    // SDL_Surface *surface = IMG_Load("./assets/images/tank-tiger-right.png");
    // SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    // SDL_FreeSurface(surface);
    //
    // SDL_Rect destRect = {static_cast<int>(playerPosition.x),
    //                      static_cast<int>(playerPosition.y), 32, 32};
    // SDL_RenderCopy(renderer, texture, NULL, &destRect);
    // SDL_DestroyTexture(texture);
    //
    glm::vec2 camera = registy->GetSystem<CameraSystem>().cameraPosition;
    registy->GetSystem<RenderSystem>().Update(renderer,assetStore,camera);
    if (isDebug){
        registy->GetSystem<RenderColliderSystem>().Update(renderer,camera);
    }
    SDL_RenderPresent(renderer);
}

void Game::Run() {
    Setup();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
    }
}

void Game::Destory() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

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
#include "../Components/HealthComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/AIComponent.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/EnemyAISystem.h"
#include "../Components/ScriptComponent.h"
#include "../Systems/ScriptingSystem.h"
#include "../Events/EventBus.h"
#include "../Events/CollisionEvent.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include "imgui.h"
#include "imgui_sdl.h"

// #include <iostream>

static void RenderText(SDL_Renderer* renderer, TTF_Font* font,
                       const std::string& text, int x, int y, SDL_Color color) {
    if (!font) return;
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = { x, y, surface->w, surface->h };
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_DestroyTexture(texture);
}

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

    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    ImGui::CreateContext();
    ImGuiSDL::Initialize(renderer, windowWidth, windowHight);

    isRunning = true;
}
void Game::ProcessInput() {
    ImGuiIO& io = ImGui::GetIO();
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_MOUSEMOTION:
                io.MousePos = ImVec2(static_cast<float>(sdlEvent.motion.x),
                                     static_cast<float>(sdlEvent.motion.y));
                break;
            case SDL_MOUSEBUTTONDOWN:
                io.MouseDown[sdlEvent.button.button - 1] = true;
                break;
            case SDL_MOUSEBUTTONUP:
                io.MouseDown[sdlEvent.button.button - 1] = false;
                break;
            case SDL_MOUSEWHEEL:
                io.MouseWheel = static_cast<float>(sdlEvent.wheel.y);
                break;
            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
                if (sdlEvent.key.keysym.sym == SDLK_d){
                    isDebug = !isDebug;
                }
                if (sdlEvent.key.keysym.sym == SDLK_F1){
                    showImGui = !showImGui;
                }
                if (sdlEvent.key.keysym.sym == SDLK_SPACE){
                    Mix_PlayChannel(-1, assetStore->GetSound("shoot"), 0);
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
    registy->AddSystem<ProjectileEmitSystem>();
    registy->AddSystem<ProjectileLifecycleSystem>();
    registy->AddSystem<EnemyAISystem>();
    registy->AddSystem<ScriptingSystem>();
    // init game object
    assetStore->AddTexture(renderer,"tank-image","./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer,"truck-image","./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer,"chopper-image","./assets/images/chopper.png");
    assetStore->AddTexture(renderer,"radar-image","./assets/images/radar.png");
    assetStore->AddTexture(renderer,"tilemap-image","./assets/tilemaps/jungle.png");
    assetStore->AddTexture(renderer,"bullet-image","./assets/images/bullet.png");
    assetStore->AddTexture(renderer,"patrol-image","./assets/images/tank-tiger-right.png");
    assetStore->AddFont("arial", "./assets/fonts/arial.ttf", 16);
    assetStore->AddSound("shoot", "./assets/sounds/helicopter.wav");
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
    chopper.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0,0), "player", false);
    chopper.AddComponent<HealthComponent>(100);
    chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(150.0, 0.0), 10, true, 200);
    player = chopper;

    Entity radar = registy->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74,10.0), glm::vec2(1.0,1.0),0.0);
    radar.AddComponent<RigidBodyCompoent>(glm::vec2(0.0,0.0));
    radar.AddComponent<SpriteComponent>("radar-image", 64,64,2, 0, 0, true);
    radar.AddComponent<AnimationComponent>(8,5,true);

    Entity tank = registy->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(400.0,300.0), glm::vec2(1.0,1.0),0.0);
    tank.AddComponent<RigidBodyCompoent>(glm::vec2(0.0,0.0));
    tank.AddComponent<SpriteComponent>("tank-image", 32,32,2);
    tank.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0,0), "enemy", false);
    tank.AddComponent<HealthComponent>(30);
    tank.AddComponent<AIComponent>(chopper, 40.0);

    Entity track = registy->CreateEntity();
    track.AddComponent<TransformComponent>(glm::vec2(700.0,500.0), glm::vec2(1.0,1.0),0.0);
    track.AddComponent<RigidBodyCompoent>(glm::vec2(0.0,0.0));
    track.AddComponent<SpriteComponent>("truck-image", 32,32,1);
    track.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0,0), "enemy", false);
    track.AddComponent<HealthComponent>(30);
    track.AddComponent<AIComponent>(chopper, 40.0);

    Entity patrol = registy->CreateEntity();
    patrol.AddComponent<TransformComponent>(glm::vec2(500.0,200.0), glm::vec2(1.0,1.0),0.0);
    patrol.AddComponent<RigidBodyCompoent>(glm::vec2(0.0,0.0));
    patrol.AddComponent<SpriteComponent>("patrol-image", 32,32,2);
    patrol.AddComponent<ScriptComponent>("assets/scripts/patrol.lua");

}
void Game::Setup() {
    EventBus::Subscribe<CollisionEvent>([this](CollisionEvent& e){
        // player vs enemy contact: player takes damage, enemy dies
        if ((e.aTag == "player" && e.bTag == "enemy") ||
            (e.bTag == "player" && e.aTag == "enemy")) {
            Entity player = (e.aTag == "player") ? e.a : e.b;
            Entity enemy  = (e.aTag == "player") ? e.b : e.a;

            if (player.HasComponent<HealthComponent>()) {
                player.GetComponent<HealthComponent>().healthPercentage -= 25;
                Logger::Log("Player health: " + std::to_string(
                    player.GetComponent<HealthComponent>().healthPercentage));
            }
            enemy.Kill();

            if (player.HasComponent<HealthComponent>() &&
                player.GetComponent<HealthComponent>().healthPercentage <= 0) {
                Logger::Log("Player died");
                player.Kill();
            }
            return;
        }

        // projectile hits
        if (e.aTag == "projectile" || e.bTag == "projectile") {
            Entity projectile = (e.aTag == "projectile") ? e.a : e.b;
            Entity other      = (e.aTag == "projectile") ? e.b : e.a;
            const auto proj = projectile.GetComponent<ProjectileComponent>();

            if (proj.isFriendly && other.HasComponent<BoxColliderComponent>() &&
                other.GetComponent<BoxColliderComponent>().tag == "enemy") {
                if (other.HasComponent<HealthComponent>()) {
                    other.GetComponent<HealthComponent>().healthPercentage -= proj.damage;
                    if (other.GetComponent<HealthComponent>().healthPercentage <= 0) {
                        other.Kill();
                        score += 10;
                        Logger::Log("Score: " + std::to_string(score));
                    }
                }
                projectile.Kill();
            } else if (!proj.isFriendly && other.HasComponent<BoxColliderComponent>() &&
                       other.GetComponent<BoxColliderComponent>().tag == "player") {
                if (other.HasComponent<HealthComponent>()) {
                    other.GetComponent<HealthComponent>().healthPercentage -= proj.damage;
                    if (other.GetComponent<HealthComponent>().healthPercentage <= 0) {
                        Logger::Log("Player died");
                        other.Kill();
                    }
                }
                projectile.Kill();
            }
            return;
        }
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
    registy->GetSystem<EnemyAISystem>().Update();
    registy->GetSystem<ScriptingSystem>().Update(deltatime);
    registy->GetSystem<ProjectileEmitSystem>().Update();
    registy->GetSystem<MovementSystem>().Update(deltatime);
    registy->GetSystem<ProjectileLifecycleSystem>().Update();
    registy->GetSystem<AnimationSystem>().Update();
    registy->GetSystem<CollisionSystem>().Update();
    registy->GetSystem<CameraSystem>().Update(windowWidth, windowHight, levelWidth, levelHeight);
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

    TTF_Font* font = assetStore->GetFont("arial");
    SDL_Color white = {255, 255, 255, 255};
    int health = player.HasComponent<HealthComponent>()
        ? player.GetComponent<HealthComponent>().healthPercentage : 0;
    RenderText(renderer, font, "Health: " + std::to_string(health), 10, 10, white);
    RenderText(renderer, font, "Score: " + std::to_string(score), 10, 32, white);

    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(windowWidth), static_cast<float>(windowHight));
    io.DeltaTime = MILLISECS_PER_FRAME / 1000.0f;
    ImGui::NewFrame();

    if (showImGui) {
        ImGui::Begin("Debug");
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("Score: %d", score);
        ImGui::Text("Player health: %d", health);
        ImGui::Text("Camera: %.0f, %.0f", camera.x, camera.y);
        ImGui::Text("Level: %d x %d", levelWidth, levelHeight);
        ImGui::Checkbox("Show colliders", &isDebug);
        ImGui::End();
    }

    ImGui::Render();
    ImGuiSDL::Render(ImGui::GetDrawData());

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
    ImGuiSDL::Deinitialize();
    ImGui::DestroyContext();
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

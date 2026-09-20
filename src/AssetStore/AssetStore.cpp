#include "AssetStore.h"
#include "../Logger/Logger.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

AssetStore::AssetStore(){
    Logger::Log("constructor call");
}
AssetStore::~AssetStore(){
    ClearAssets();
    Logger::Log("destructure call");
}

void AssetStore::ClearAssets(){
    for(auto texture: textures){
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();

    for(auto font: fonts){
        TTF_CloseFont(font.second);
    }
    fonts.clear();

    for(auto sound: sounds){
        Mix_FreeChunk(sound.second);
    }
    sounds.clear();
}
void AssetStore::AddTexture(SDL_Renderer* renderer, const std::string& assetId, const std::string& filepath){
    SDL_Surface* surface = IMG_Load(filepath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
    SDL_FreeSurface(surface);

    textures.emplace(assetId,texture);
}
SDL_Texture* AssetStore::GetTexture(const std::string& assetId){
    return textures[assetId];
}

void AssetStore::AddFont(const std::string& assetId, const std::string& filepath, int fontSize){
    TTF_Font* font = TTF_OpenFont(filepath.c_str(), fontSize);
    if (!font){
        Logger::Err("Error loading font: " + filepath);
        return;
    }
    fonts.emplace(assetId, font);
}
TTF_Font* AssetStore::GetFont(const std::string& assetId){
    auto it = fonts.find(assetId);
    if (it == fonts.end()){
        return nullptr;
    }
    return it->second;
}

void AssetStore::AddSound(const std::string& assetId, const std::string& filepath){
    Mix_Chunk* sound = Mix_LoadWAV(filepath.c_str());
    if (!sound){
        Logger::Err("Error loading sound: " + filepath);
        return;
    }
    sounds.emplace(assetId, sound);
}
Mix_Chunk* AssetStore::GetSound(const std::string& assetId){
    auto it = sounds.find(assetId);
    if (it == sounds.end()){
        return nullptr;
    }
    return it->second;
}

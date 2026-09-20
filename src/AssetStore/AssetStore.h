#ifndef ASSERSTORE_H
#define ASSERSTORE_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <string>
#include <map>

class AssetStore{
private:
    std::map<std::string, SDL_Texture*> textures;
    std::map<std::string, TTF_Font*> fonts;
    std::map<std::string, Mix_Chunk*> sounds;
public:
    AssetStore();
    ~AssetStore();

    void ClearAssets();
    void AddTexture(SDL_Renderer* renderer,const std::string& assetId, const std::string& filepath);
    SDL_Texture* GetTexture(const std::string& assetId);

    void AddFont(const std::string& assetId, const std::string& filepath, int fontSize);
    TTF_Font* GetFont(const std::string& assetId);

    void AddSound(const std::string& assetId, const std::string& filepath);
    Mix_Chunk* GetSound(const std::string& assetId);
};

#endif

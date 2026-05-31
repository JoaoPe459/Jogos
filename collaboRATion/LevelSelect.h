#ifndef _LEVEL_SELECT_H_
#define _LEVEL_SELECT_H_

#include "Game.h"
#include "Scene.h"
#include "Sprite.h"
#include "Mouse.h"
#include "MapDoor.h"
#include <vector>

class LevelSelect : public Game {
private:
    Scene* scene = nullptr;
    Sprite* backg = nullptr;
    Sprite* fundo = nullptr;
    Mouse* mouse = nullptr;
    std::vector<MapDoor*> doors;
    Sprite* blackBlock = nullptr;
    bool isTransitioning = false;
    float transitionTimer = 0.5f;
    int targetLevel = -1;
    Sprite* transitionBlock = nullptr;
    bool isOpening = true;
    float openingTimer = 0.0f;

public:
    void Init() override;
    void Update() override;
    void Draw() override;
    void Finalize() override;
};

#endif
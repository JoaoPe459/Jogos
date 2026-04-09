#ifndef _PACMAN_GHOST_H_
#define _PACMAN_GHOST_H_

#include "Entity.h"
#include "Player.h"
#include "PacMan.h"


enum class SpriteType {
    RED,
    BLUE,
    ORANGE,
    PINK,
};

class Ghost : public Entity
{
private:
    Sprite* redSprite = new Sprite("Resources/GhostRedR.png");
    Sprite* blueSprite = new Sprite("Resources/GhostBlueR.png");
    Sprite* orangeSprite = new Sprite("Resources/GhostOrangeD.png");
    Sprite* pinkSprite = new Sprite("Resources/GhostPinkD.png");
    Sprite* sprite;

    MovementType moveType;
    int dirX = 1;
    int dirY = 1;

public:
    Ghost();
    ~Ghost();

    void Control() override;

    void RandomizeSprite();

    void Draw() override;

    void OnCollision(Object* obj) override;

    void RandomizeMovement() {
        moveType = static_cast<MovementType>(rand() % 3);
        dirX = (rand() % 2 == 0) ? 1 : -1;
        dirY = (rand() % 2 == 0) ? 1 : -1;
    }
    void SetSpriteByIndex(int index);

};

#endif
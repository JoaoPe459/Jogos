#ifndef _PACMAN_ENEMY_H_
#define _PACMAN_ENEMY_H_

#include "Entity.h"
#include "PacMan.h"
#include "Animation.h"


class Enemy : public Entity
{
private:
    std::string attackSpritePath;
    TileSet* walking;                // folha de sprites do personagem
    TileSet* idle;                // folha de sprites do personagem
    Animation* animWalk;                   // animação do personagem
    Animation* animIdle;
    Animation* anim;
    int spriteCount = 0;


    MovementType moveType;
    int dirX = 1;
    int dirY = 1;

    Entity* playerTarget = nullptr;
    float attackCooldown = 2.0f;    // Intervalo entre ataques
    float attackTimer = 0.0f;
    float safeDistance = 300.0f;    // Distância para recuar no cooldown

    void AttackPlayer();

public:
    Enemy();
    ~Enemy();

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

    void SetTarget(Entity* target) { playerTarget = target; }

    void Update();
};

#endif
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Entity.h"
#include "Attack.h"
#include "Font.h"

class Player : public Entity {
private:
    Sprite* sprite = nullptr;
    // Atributos exclusivos do Player (não existem em Entity)
    float calories = 0.0f;
    float stamina = 100.0f;
    int   sizeLevel = 1;

    float attackTimer = 0.0f;
    float attackCooldown = 0.3f;

public:
    Player();
    ~Player();

    void Control() override;
    void Draw() override;
    void OnCollision(Object* obj) override;

    void Eat(float amount);

};

#endif
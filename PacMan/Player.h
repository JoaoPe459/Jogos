#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Entity.h"

class Player : public Entity {
private:
    Sprite* sprite = nullptr;

    // Atributos inspirados no ConSumo (Bully)
    float health = 100.0f;
    float maxHealth = 100.0f;
    float calories = 0.0f;
    float stamina = 100.0f;
    int   sizeLevel = 1; // Atua como a "massa" nas colisões

public:
    Player();
    ~Player();

    void Control() override;
    void Draw() override;
    void OnCollision(Object* obj) override;

    // Métodos de gameplay
    void Eat(float amount);
    int  GetSize() { return sizeLevel; }
};

#endif
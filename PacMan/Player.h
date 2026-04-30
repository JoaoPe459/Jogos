#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Entity.h"
#include "Attack.h"
#include "Font.h"
#include "Animation.h"

class Player : public Entity {
private:
    TileSet   * walking;                // folha de sprites do personagem
    Animation * anim;                   // animação do personagem
    // Atributos exclusivos do Player (não existem em Entity)
    float calories = 0.0f;
    float stamina = 100.0f;
    int   sizeLevel = 1;

    float attackTimer = 0.0f;
    float attackCooldown = 0.3f;
    std::vector<Attack*> orbitals;
    void UpdateOrbitalPositions(); // Auxiliar para reorganizar

public:
    Player();
    ~Player();

    void Control() override;
    void Draw() override;
    void Update();
    void OnCollision(Object* obj) override;

};

#endif
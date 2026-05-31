#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "Entity.h"
#include "Attack.h"
#include "Font.h"
#include "Animation.h"

class Player : public Entity {
private:
    TileSet   * animation;               // folha de sprites do personagem
    TileSet   * idle;               // folha de sprites do personagem
    Animation * anim;                   // animação do personagem
    // Atributos exclusivos do Player (não existem em Entity)
    float calories = 0.0f;
    float stamina = 100.0f;
    int   sizeLevel = 1;
    float attackTimer = 0.0f;
    float attackCooldown = 0.3f;
    float prevY = 0.0f;
    
    float deathTimer = 1.0f;
    std::vector<Attack*> orbitals;
    void UpdateOrbitalPositions(); // Auxiliar para reorganizar

public:
    Player();
    ~Player();

    void Control() override;
    void Draw() override;
    void Update();
    void OnCollision(Object* obj) override;
    void Die();

    int totalDamageDealt = 0;
    int totalLevelsVisited = 0;
    bool isDead = false;

};

#endif
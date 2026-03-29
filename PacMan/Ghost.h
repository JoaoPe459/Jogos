#ifndef _PACMAN_GHOST_H_
#define _PACMAN_GHOST_H_

#include "Enemy.h"
#include "Player.h"

class Ghost : public Enemy
{
private:
    Player* player = nullptr; // Para saber onde o jogador está

public:
    Ghost(Player* p);
    ~Ghost();

    // Implementa a IA obrigatória da classe Enemy
    void IA() override;

    // Sobrescrevemos a colisão para lidar com as paredes
    void OnCollision(Object* obj) override;
};

#endif
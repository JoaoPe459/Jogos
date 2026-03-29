#ifndef _PACMAN_PLAYER_H_
#define _PACMAN_PLAYER_H_

#include "Entity.h" // Herda de Entity agora

enum PLAYERSTATE { STOPED, UP, DOWN, LEFT, RIGHT };

class Player : public Entity
{
private:
    // Mantemos os sprites pois são específicos do visual do PacMan
    Sprite* spriteL = nullptr;
    Sprite* spriteR = nullptr;
    Sprite* spriteU = nullptr;
    Sprite* spriteD = nullptr;

public:
    uint currState = STOPED;
    uint nextState = STOPED;

    Player();
    ~Player();

    // Implementação obrigatória da lógica de controle (teclado)
    void Control() override;

    // O Update() agora é herdado da Entity
    // se necessario sobrescrever para adicionar algo muito específico.

    void Draw() override;
    void OnCollision(Object* obj) override;

    // Métodos de utilidade continuam aqui
    void incrementSpeed();
    void decressSpeed();
};

#endif
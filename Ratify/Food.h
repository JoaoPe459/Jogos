/**********************************************************************************
// Food (Arquivo de Cabeçalho)
// 
// Criação:     03 Jan 2013
// Atualização: 04 Mar 2023
// Compilador:  Visual C++ 2022
//
// Descrição:   Comida do PacMan
//
**********************************************************************************/
#ifndef _PACMAN_FOOD_H_
#define _PACMAN_FOOD_H_

#include "Types.h"
#include "Sprite.h"
#include "PacMan.h"
#include "Entity.h"

class Food : public Entity
{
private:
    // --- Novos membros para múltiplos sprites ---
    Sprite** sprites = nullptr;     // Array dinâmico de ponteiros de Sprite
    Sprite* currentSprite = nullptr; // Sprite selecionado para esta instância
    int spriteCount = 0;
    // --------------------------------------------

    MovementType moveType;
    int dirX = 1;
    int dirY = 1;

public:
    Food();
    ~Food();

    void Draw() override;
    void Control() override;
    void HandleScreenWrap();
    void OnCollision(Object* obj) override;
    void Update() override;

    // Função para sortear o sprite
    void RandomizeSprite();
};

#endif
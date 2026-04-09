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

// ---------------------------------------------------------------------------------
// Inclusões

#include "Types.h"                      // tipos específicos da engine
#include "Object.h"                     // interface de Object
#include "Sprite.h"                     // interface de Sprites
#include "PacMan.h"
#include "Entity.h"



// ---------------------------------------------------------------------------------

class Food : public Entity
{
private:
    Sprite* foodSprite = new Sprite("Resources/Food.png");
    Sprite * sprite = nullptr; // sprite da comida
    MovementType moveType;
    int dirX = 1;
    int dirY = 1;

public:
    Food();                             // construtor
    ~Food();                            // destrutor


    void Draw() override;                        // desenho do objeto
    void Control() override;
	void HandleScreenWrap();              // lógica de teletransporte
    void RespawnAtEdge();

    void OnCollision(Object* obj) override;

    void RandomizeMovement() {
        moveType = static_cast<MovementType>(rand() % 3);
        dirX = (rand() % 2 == 0) ? 1 : -1;
        dirY = (rand() % 2 == 0) ? 1 : -1;
    }
};

// ---------------------------------------------------------------------------------





// ---------------------------------------------------------------------------------

#endif
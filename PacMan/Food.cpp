/**********************************************************************************
// Food (Código Fonte)
// 
// Criação:     03 Jan 2013
// Atualização: 04 Mar 2023
// Compilador:  Visual C++ 2022
//
// Descrição:   Comida do PacMan
//
**********************************************************************************/

#include "Food.h"

// ---------------------------------------------------------------------------------

Food::Food()
{
    sprite = new Sprite("Resources/Food.png");
    BBox(new Rect(-5, -5, 5, 5));
    type = FOOD;
    moveType = static_cast<MovementType>(rand() % 3);
    moves->setSpeed(static_cast<float>(500.0f - (rand() % 200)));

    moveType = static_cast<MovementType>(rand() % 3);

    // Sorteia direções iniciais aleatórias (1 ou -1)
    dirX = (rand() % 2 == 0) ? 1 : -1;
    dirY = (rand() % 2 == 0) ? 1 : -1;
}

// ---------------------------------------------------------------------------------

Food::~Food()
{
    delete sprite;

}

// ---------------------------------------------------------------------------------



void Food::Draw()
{
    sprite->Draw(x, y, z);
}


void Food::OnCollision(Object* obj) {
    // 1. Mantém a colisão base (com paredes, etc.)
    Entity::OnCollision(obj);

    // 2. Se tocar em uma parede

    if (obj->Type() == WALL)
    {
        this->RandomizeMovement();
    }

    if (obj->Type() == GHOST) {
        return;
    }

    if (obj->Type() == PLAYER) {
        alive = false;
    }

}

// ---------------------------------------------------------------------------------

void Food::Control() {
    float speed = moves->getSpeed();
    float targetVX = 0;
    float targetVY = 0;

    switch (moveType) {
    case HORIZONTAL:
        targetVX = dirX * speed;
        break;

    case VERTICAL:
        targetVY = dirY * speed;
        break;

    case DIAGONAL:
        targetVX = dirX * speed;
        targetVY = dirY * speed;
        break;
    }

    moves->setVelX(targetVX);
    moves->setVelY(targetVY);

    HandleScreenWrap();
}

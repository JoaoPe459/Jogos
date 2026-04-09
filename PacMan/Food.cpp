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


Food::Food()
{
    
	sprite = foodSprite;
    BBox(new Rect(-5, -5, 5, 5));
    type = FOOD;

    // Inicialização segura
    moves->setSpeed(static_cast<float>(300.0f + (rand() % 100)));
    RespawnAtEdge();
}

Food::~Food()
{
    delete foodSprite; foodSprite = nullptr;
}

void Food::Draw()
{
    sprite->Draw(x, y, z);
}

void Food::OnCollision(Object* obj) {
    // Quando o player come, a comida apenas muda de lugar
    if (obj->Type() == PLAYER || obj->Type() == WALL) {
        this->RespawnAtEdge();
    }
}

void Food::Control() {
    float speed = moves->getSpeed();
    float targetVX = 0, targetVY = 0;

    // 1. Movimentação baseada no tipo atual
    switch (moveType) {
    case HORIZONTAL: targetVX = dirX * speed; break;
    case VERTICAL:   targetVY = dirY * speed; break;
    case DIAGONAL:   targetVX = dirX * speed; targetVY = dirY * speed; break;
    }

    moves->setVelX(targetVX);
    moves->setVelY(targetVY);

    float offset = 50.0f;

    if (X() < -offset || X() > window->Width() + offset ||
        Y() < -offset || Y() > window->Height() + offset)
    {
        RespawnAtEdge();
    }
}

void Food::HandleScreenWrap()
{
	Entity::HandleScreenWrap(); // Aplica o wrap normal
    RespawnAtEdge();
}

void Food::RespawnAtEdge() {
    int edge = rand() % 4;
    float margin = 20.0f;

    // Zera direções antes de definir novas
    dirX = 0;
    dirY = 0;

    switch (edge) {
    case 0: // Topo
        MoveTo(rand() % (window->Width() - 40) + 20, margin);
        dirY = 1;
        moveType = (rand() % 2 == 0) ? VERTICAL : DIAGONAL;
        break;

    case 1: // Base
        MoveTo(rand() % (window->Width() - 40) + 20, window->Height() - margin);
        dirY = -1;
        moveType = (rand() % 2 == 0) ? VERTICAL : DIAGONAL;
        break;

    case 2: // Esquerda
        MoveTo(margin, rand() % (window->Height() - 40) + 20);
        dirX = 1;
        moveType = (rand() % 2 == 0) ? HORIZONTAL : DIAGONAL;
        break;

    case 3: // Direita
        MoveTo(window->Width() - margin, rand() % (window->Height() - 40) + 20);
        dirX = -1;
        moveType = (rand() % 2 == 0) ? HORIZONTAL : DIAGONAL;
        break;
    }
}
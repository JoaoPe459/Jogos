#include "Entity.h"
#include "Physics.h"
#include "PacMan.h" // Para acessar gameTime e tipos de objetos

Entity::Entity() {
    moves = new Moves(0, 0, 150.0f, false);
}

Entity::~Entity() {
    delete moves;
}

void Entity::Update() {
    Control();            // 1. Define a intenção de movimento
    ApplyPhysics();       // 2. Aplica Gravidade e Translação
    HandleScreenWrap();   // 3. Garante limites da tela
}

void Entity::ApplyPhysics() {
    moves->applyGravity(gameTime);
    Translate(moves->getVelX() * gameTime, moves->getVelY() * gameTime);
}

void Entity::OnCollision(Object* obj) {
    if (obj->Type() == WALL) {
        Rect* r1 = (Rect*)this->BBox();
        Rect* r2 = (Rect*)obj->BBox();

        if (!r1 || !r2) return;

        float overlapX = (r1->Left() < r2->Left() ? r1->Right() : r2->Right()) -
            (r1->Left() > r2->Left() ? r1->Left() : r2->Left());
        float overlapY = (r1->Top() < r2->Top() ? r1->Bottom() : r2->Bottom()) -
            (r1->Top() > r2->Top() ? r1->Top() : r2->Top());

        if (overlapX < overlapY) {
            if ((r1->Left() + r1->Right()) < (r2->Left() + r2->Right()))
                Translate(-overlapX, 0.0f);
            else
                Translate(overlapX, 0.0f);
            moves->setVelX(0.0f);
        }
        else {
            if ((r1->Top() + r1->Bottom()) < (r2->Top() + r2->Bottom())) {
                Translate(0.0f, -overlapY);
                moves->setVelY(0.0f);
                moves->setOnGround(true);
            }
            else {
                Translate(0.0f, overlapY);
                moves->setVelY(0.0f);
            }
        }
    }
}

void Entity::HandleScreenWrap() {
    // Teletransporte lateral
    if (X() + 20.0f < 0) MoveTo(window->Width() + 20.0f, Y());
    if (X() - 20.0f > window->Width()) MoveTo(-20.0f, Y());

    // Limite Inferior/Superior (Chão/Teto)
    if (Y() + 20.0f > window->Height()) {
        MoveTo(X(), window->Height() - 20.0f);
        moves->setVelY(0.0f);
        if (Physics::Direction > 0) moves->setOnGround(true);
    }
    if (Y() - 20.0f < 0.0f) {
        MoveTo(X(), 20.0f);
        moves->setVelY(0.0f);
        if (Physics::Direction < 0) moves->setOnGround(true);
    }
}

void Entity::Draw() {
   
}
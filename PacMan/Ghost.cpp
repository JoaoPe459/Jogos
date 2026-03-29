#include "PacMan.h"
#include "Ghost.h"

Ghost::Ghost(Player* p) : Entity() // Chama o construtor de Entity
{
    player = p;
    type = GHOST;

    sprite = new Sprite("Resources/GhostRedR.png");
    BBox(new Rect(-20, -20, 20, 20));
    MoveTo(200.0f, 450.0f);

    // Ajusta a velocidade inicial através do objeto moves da Entity
    moves->setSpeed(static_cast<float>(500.0f - (rand() % 200)));

    moveType = static_cast<MovementType>(rand() % 3);

    // Sorteia direções iniciais aleatórias (1 ou -1)
    dirX = (rand() % 2 == 0) ? 1 : -1;
    dirY = (rand() % 2 == 0) ? 1 : -1;
}

Ghost::~Ghost()
{
    delete sprite;
}

void Ghost::Draw()
{
    sprite->Draw(X(), Y());
}

void Ghost::OnCollision(Object* obj) {
    // 1. Mantém a colisão base (com paredes, etc.)
    Entity::OnCollision(obj);

    // 2. Lógica ao tocar em outro Ghost
    if (obj->Type() == GHOST) {
        // Converte o objeto genérico para um ponteiro de Ghost
        Ghost* other = (Ghost*)obj;

        // 1. Inverte as velocidades atuais para um recuo imediato
        this->moves->setVelX(-this->moves->getVelX());
        this->moves->setVelY(-this->moves->getVelY());

        Rect* r1 = (Rect*)this->BBox();
        Rect* r2 = (Rect*)obj->BBox();
        if (!r1 || !r2) return;

        float overlapX = (r1->Left() < r2->Left() ? r1->Right() : r2->Right()) -
            (r1->Left() > r2->Left() ? r1->Left() : r2->Left());
        float overlapY = (r1->Top() < r2->Top() ? r1->Bottom() : r2->Bottom()) -
            (r1->Top() > r2->Top() ? r1->Top() : r2->Top());

        if (overlapX < overlapY) {
            if (this->X() < obj->X()) Translate(-overlapX / 2.0f, 0.0f);
            else Translate(overlapX / 2.0f, 0.0f);
        }
        else {
            if (this->Y() < obj->Y()) Translate(0.0f, -overlapY / 2.0f);
            else Translate(0.0f, overlapY / 2.0f);
        }

        // Sorteia um novo tipo para ESTE fantasma
        this->moveType = static_cast<MovementType>(rand() % 3);
        // Sorteia novas direções aleatórias
        this->dirX = (rand() % 2 == 0) ? 1 : -1;
        this->dirY = (rand() % 2 == 0) ? 1 : -1;

		// Sorteia o movimento do outro fantasma para evitar que fiquem "presos" um no outro
        other->RandomizeMovement();
    }

    // 3. Lógica de morte ao tocar no Player
    /*if (obj->Type() == PLAYER) {
        this->alive = false;
    }*/
}

void Ghost::Control() {
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
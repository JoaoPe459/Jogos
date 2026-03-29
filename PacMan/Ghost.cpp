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
    moves->setSpeed(300.0f);
}

Ghost::~Ghost()
{
    delete sprite;
}

void Ghost::Draw()
{
    sprite->Draw(X(), Y());
}

void Ghost::Control()
{
    // IA de perseguição: define a intenção de movimento no objeto moves
    if (player) {
        // Movimentação Horizontal
        if (player->X() < this->X()) {
            moves->Left();
        }
        else {
            moves->Right();
        }

        // Lógica de Pulo: se estiver no chão e o player estiver acima
        if (moves->getOnGround() && player->Y() < this->Y() - 50.0f) {
            moves->Up();
            moves->setOnGround(false);
        }
    }
}
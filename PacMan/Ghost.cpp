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

void Ghost::OnCollision(Object* obj) {
    // 1. Mantém a colisão com paredes (lógica da Entity)
    Entity::OnCollision(obj);

    // 2. Lógica para separar fantasmas (evita sobreposição)
    if (obj->Type() == GHOST) {
        Rect* r1 = (Rect*)this->BBox();
        Rect* r2 = (Rect*)obj->BBox();

        if (!r1 || !r2) return;

        // Calcula a sobreposição nos dois eixos
        float overlapX = (r1->Left() < r2->Left() ? r1->Right() : r2->Right()) -
            (r1->Left() > r2->Left() ? r1->Left() : r2->Left());
        float overlapY = (r1->Top() < r2->Top() ? r1->Bottom() : r2->Bottom()) -
            (r1->Top() > r2->Top() ? r1->Top() : r2->Top());

        // Empurra o fantasma na direção de menor resistência
        if (overlapX < overlapY) {
            if (this->X() < obj->X())
                Translate(-overlapX / 2.0f, 0.0f); // Empurra para a esquerda
            else
                Translate(overlapX / 2.0f, 0.0f);  // Empurra para a direita
        }
        else {
            if (this->Y() < obj->Y())
                Translate(0.0f, -overlapY / 2.0f); // Empurra para cima
            else
                Translate(0.0f, overlapY / 2.0f);  // Empurra para baixo
        }
    }

    // 3. Lógica de morte ao tocar no Player (já existente)
    if (obj->Type() == PLAYER) {
        this->alive = false;
    }
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
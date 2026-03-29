#include "PacMan.h"
#include "Ghost.h"

Ghost::Ghost(Player* p) : Enemy()
{
    player = p;
    type = GHOST;

    // Usando o fantasma vermelho dos seus recursos
    sprite = new Sprite("Resources/GhostRedR.png");

    // Define a caixa de colisão (similar ao Player)
    BBox(new Rect(-20, -20, 20, 20));

    // Posição inicial (exemplo)
    MoveTo(200.0f, 450.0f);

    // Ajusta a velocidade inicial se desejar (herdado de Enemy)
    moves->setSpeed(300.0f);
}

Ghost::~Ghost()
{
    // O moves e sprite são deletados automaticamente pelo destrutor de Enemy
}

void Ghost::IA()
{
    // IA Simples: Persegue o jogador horizontalmente
    if (player) {
        if (player->X() < this->X()) {
            moves->Left();
        }
        else {
            moves->Right();
        }

        // Se o fantasma estiver no chão e o player estiver acima dele, ele tenta pular
        if (moves->getOnGround() && player->Y() < this->Y() - 50.0f) {
            moves->Up();
            moves->setOnGround(false);
        }
    }
}

void Ghost::OnCollision(Object* obj)
{
    if (obj->Type() == WALL) {
        // Lógica de resolução de colisão (Vetor de Translação Mínimo)
        // Copiada e adaptada de Player para garantir que o Ghost não atravesse paredes
        Rect* r1 = (Rect*)this->BBox();
        Rect* r2 = (Rect*)obj->BBox();

        float left = (r1->Left() > r2->Left()) ? r1->Left() : r2->Left();
        float right = (r1->Right() < r2->Right()) ? r1->Right() : r2->Right();
        float top = (r1->Top() > r2->Top()) ? r1->Top() : r2->Top();
        float bottom = (r1->Bottom() < r2->Bottom()) ? r1->Bottom() : r2->Bottom();

        float overlapX = right - left;
        float overlapY = bottom - top;

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
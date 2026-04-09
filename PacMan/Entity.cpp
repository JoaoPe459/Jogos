#include "Entity.h"
#include "Physics.h"
#include "PacMan.h" // Para acessar gameTime e tipos de objetos
#include "Ghost.h"
#include "Player.h"

Entity::Entity() {
    moves = new Moves(0, 0, 150.0f, false);
	mass = 1.0f;
    alive = true;
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
    float friction = 0.92f;
    moves->setVelX(moves->getVelX() * friction);
    moves->setVelY(moves->getVelY() * friction);

    Translate(moves->getVelX() * gameTime, moves->getVelY() * gameTime);
}

void Entity::OnCollision(Object* obj) {
    if (obj->Type() == FOOD) {
        return;
    }

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

    if (obj->Type() == PLAYER || obj->Type() == GHOST) {
        float diffX = this->X() - obj->X();
        float diffY = this->Y() - obj->Y();
        float dist = sqrt(diffX * diffX + diffY * diffY);

        if (dist < 1.0f) dist = 1.0f;

        float dirX = diffX / dist;
        float dirY = diffY / dist;

       
        Entity* other = static_cast<Entity*>(obj);

        // 2. Pegamos a velocidade atual de quem bateu
        float otherVX = other->moves->getVelX();
        float otherVY = other->moves->getVelY();
        float speedSum = sqrt(otherVX * otherVX + otherVY * otherVY);

        // 3. Calculamos a força baseada na velocidade e na massa do agressor
        // Adicionamos uma base mínima (ex: 400.0f) para evitar que fiquem colados se estiverem lentos
        float baseForce = 400.0f;
        float impactForce = baseForce + (speedSum * other->getMass());

        // 4. O knockback final é inversamente proporcional à massa de quem recebe o golpe
        float knockback = impactForce / this->mass;

        // 5. Aplica o novo impulso dinâmico
        this->moves->setVelX(dirX * knockback);
        this->moves->setVelY(dirY * knockback);

        if (obj->Type() == GHOST) {
            static_cast<Ghost*>(obj)->RandomizeMovement();
        }
    }
}

void Entity::HandleScreenWrap() {
    float offset = 20.0f; // Margem para evitar que o objeto "pisque" na borda

    // --- Teletransporte Horizontal (Esquerda <-> Direita) ---
    // Se saiu pela esquerda, vai para a direita
    if (X() + offset < 0) {
        MoveTo(window->Width() + offset, Y());
    }
    // Se saiu pela direita, vai para a esquerda
    else if (X() - offset > window->Width()) {
        MoveTo(-offset, Y());
    }

    // --- Teletransporte Vertical (Cima <-> Baixo) ---
    // Se saiu pelo topo, aparece no fundo
    if (Y() + offset < 0) {
        MoveTo(X(), window->Height() + offset);
    }
    // Se saiu pelo fundo, aparece no topo
    else if (Y() - offset > window->Height()) {
        MoveTo(X(), -offset);
    }
}

void Entity::Draw() {
   
}
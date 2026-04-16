#include "Entity.h"
#include "Physics.h"
#include "PacMan.h"
#include "LevelMake.h" 
#include "Attack.h"
#include "Ghost.h"

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

    float frictionDecay = 0.10f;
    float currentFriction = pow(frictionDecay, gameTime);

    moves->setVelX(moves->getVelX() * currentFriction);
    moves->setVelY(moves->getVelY() * currentFriction);

    Translate(moves->getVelX() * gameTime, moves->getVelY() * gameTime);
}

void Entity::OnCollision(Object* obj) {
    if (obj->Type() == FOOD) return;

    // Caso: Ataque
    if (obj->Type() == ATTACK) {
        Attack* atk = static_cast<Attack*>(obj);
        if (atk->GetOwner() == this) return;

        // Usamos uma força maior para ataques (ou a base + 5000)
        ApplyKnockback(obj, 5000.0f);
        this->Die();
        return;
    }

    // Caso: Parede (Mantemos a lógica de colisão estática/AABB)
    if (obj->Type() == WALL) {
        HandleWallCollision(obj); // Opcional: mover a lógica de parede para outra função também
        return;
    }

    // Caso: Entidades Físicas (Player, Ghost, etc)
    if (obj->Type() == PLAYER || obj->Type() == GHOST) {
        Entity* other = static_cast<Entity*>(obj);

        // Calculamos a força baseada na velocidade do outro objeto
        float otherVX = other->moves->getVelX();
        float otherVY = other->moves->getVelY();
        float speedSum = sqrt(otherVX * otherVX + otherVY * otherVY);
        float impactForce = speedSum * other->getMass();

        ApplyKnockback(obj, impactForce);

        if (obj->Type() == GHOST) {
            static_cast<Ghost*>(obj)->RandomizeMovement();
        }
    }
}

void Entity::Die() {
    // 1. Marca como morto para parar atualizações lógicas internas
    alive = false;

    // 2. Acessa a cena atual através do Engine para remover o objeto
    if (Engine::game) {
        LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
        if (lvl && lvl->GetScene()) {
            // Remove da lista de objetos MOVING (ou a lista que você estiver usando)
            lvl->GetScene()->Delete(this, MOVING);
        }
    }
}

void Entity::HandleWallCollision(Object* wall) {
    Rect* r1 = static_cast<Rect*>(this->BBox());
    Rect* r2 = static_cast<Rect*>(wall->BBox());

    if (!r1 || !r2) return;

    // 1. Calcula a área de intersecção nos dois eixos
    float overlapX = (r1->Left() < r2->Left() ? r1->Right() : r2->Right()) -
        (r1->Left() > r2->Left() ? r1->Left() : r2->Left());

    float overlapY = (r1->Top() < r2->Top() ? r1->Bottom() : r2->Bottom()) -
        (r1->Top() > r2->Top() ? r1->Top() : r2->Top());

    // 2. Resolve a colisão pelo eixo de menor penetração
    if (overlapX < overlapY) {
        // Colisão Horizontal (Esquerda/Direita)
        float centerX1 = r1->Left() + r1->Right();
        float centerX2 = r2->Left() + r2->Right();

        if (centerX1 < centerX2) {
            Translate(-overlapX, 0.0f); // Empurra para a esquerda
        }
        else {
            Translate(overlapX, 0.0f);  // Empurra para a direita
        }

        moves->setVelX(0.0f);
    }
    else {
        // Colisão Vertical (Cima/Baixo)
        float centerY1 = r1->Top() + r1->Bottom();
        float centerY2 = r2->Top() + r2->Bottom();

        if (centerY1 < centerY2) {
            // Empurra para cima (Personagem está em cima da parede/chão)
            Translate(0.0f, -overlapY);
            moves->setVelY(0.0f);
            moves->setOnGround(true);
        }
        else {
            // Empurra para baixo (Personagem bateu a cabeça no teto)
            Translate(0.0f, overlapY);
            moves->setVelY(0.0f);
        }
    }
}

void Entity::ApplyKnockback(Object* source, float extraForce) {
    // 1. Cálculo do Vetor Direção (Normalizado)
    float diffX = this->X() - source->X();
    float diffY = this->Y() - source->Y();
    float dist = sqrt(diffX * diffX + diffY * diffY);

    if (dist < 1.0f) dist = 1.0f; // Evita divisão por zero

    float dirX = diffX / dist;
    float dirY = diffY / dist;

    // 2. Cálculo da Força de Impacto
    // Definimos uma base padrão (ex: 15000) e somamos a força extra vinda do objeto
    float baseForce = 15000.0f;
    float totalImpact = baseForce + extraForce;

    // 3. O knockback é inversamente proporcional à massa de quem recebe
    float knockback = totalImpact / this->mass;

    // 4. Aplica a velocidade instantânea
    this->moves->setVelX(dirX * knockback);
    this->moves->setVelY(dirY * knockback);
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
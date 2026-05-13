#include "Entity.h"
#include "Physics.h"
#include "PacMan.h"
#include "LevelMake.h" 
#include "Attack.h"
#include "Ghost.h"

Entity::Entity() {
    moves = new Moves(500, 500, 0 , false);

    hp = 10;
    maxHp = 10;
    alive = true;
    mass = 1.0f;
    invulnerable = false;
    invulTimer = 0.0f;
	damage = 1;
}

Entity::~Entity() {
    delete moves;
}


void Entity::Update() {
    if (hp <= 0 || !alive) {
        hp = 0;
        this->Die();
		LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
        lvl->ghostAlive--;
        lvl->totalEnemiesDefeated++;
    }
    // Gerenciamento de Invulnerabilidade
    if (invulnerable) {
        invulTimer -= gameTime;
        if (invulTimer <= 0.0f) {
            invulnerable = false;
            invulTimer = 0.0f;
        }
    }
    ApplyPhysics();
    HandleScreenLimits();
}

void Entity::TakeDamage(Object* source) {
    if (!alive || invulnerable) return;
	int danoSofrido = source ? static_cast<Attack*>(source)->GetDamage() : 1;
    hp -= danoSofrido;
    if (source) {
        ApplyKnockback(source, 200.0f);
    }
    else {
        // Ativa frames de invencibilidade ao sofrer dano (ex: 0.1 segundos)
        SetInvulnerable(0.1f);
    }
}

void Entity::Heal(int amount) {
    if (!alive) return;
    hp += amount;
    if (hp > maxHp) hp = maxHp;
}

void Entity::SetInvulnerable(float time) {
    invulnerable = true;
    invulTimer = time;
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
    if (!obj || !alive) return;

    if (obj->Type() == FOOD) return;

    if (obj->Type() == WALL) {
        // Se for um Block morto, ignora completamente
        Block* block = dynamic_cast<Block*>(obj);
        if (block && block->isDead) return;

        HandleWallCollision(obj);
        return;
    }

    Entity* other = static_cast<Entity*>(obj);

    if (other && other->IsAlive()) {
        if (obj->Type() == PLAYER || obj->Type() == GHOST) {
            float otherVX = other->moves->getVelX();
            float otherVY = other->moves->getVelY();
            float speedSum = sqrt(otherVX * otherVX + otherVY * otherVY);

            ApplyKnockback(obj, 20);

            if (obj->Type() == GHOST) {
                static_cast<Ghost*>(obj)->RandomizeMovement();
            }
        }
    }
}

void Entity::SetMaxHp(int value) {
    maxHp = value;
    if (hp > maxHp) hp = maxHp;
}

void Entity::SetHp(int value) {
    hp = value;
    if (hp > maxHp)
    {
         hp = maxHp;
    }
}

int Entity::GetHp() const { return hp; }
int Entity::GetMaxHp() const { return maxHp; }
bool Entity::IsAlive() const { return alive; }
void Entity::setMass(float m) {
    mass = m;
}

float Entity::getMass() const {
    return mass;
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
        float centerX1 = r1->Left() + r1->Right();
        float centerX2 = r2->Left() + r2->Right();

        if (centerX1 < centerX2) {
            Translate(-overlapX, 0.0f);
        }
        else {
            Translate(overlapX, 0.0f);
        }
        moves->setVelX(0.0f);
    }
    else {
        float centerY1 = r1->Top() + r1->Bottom();
        float centerY2 = r2->Top() + r2->Bottom();

        if (centerY1 < centerY2) {
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
    float baseForce = 1000.0f;
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

void Entity::HandleScreenLimits() {
    Rect* r = static_cast<Rect*>(BBox());
    if (!r) return;

    float halfWidth = (r->Right() - r->Left()) / 2.0f;
    float halfHeight = (r->Bottom() - r->Top()) / 2.0f;

    // Usa a bounding box para manter o objeto inteiro dentro do chao jogavel.
    const float minX = PlayArea::Left + halfWidth;
    const float maxX = PlayArea::Right - halfWidth;
    const float minY = PlayArea::Top + halfHeight;
    const float maxY = PlayArea::Bottom - halfHeight;

    // Trava o movimento horizontal nas paredes laterais do background.
    if (X() < minX) {
        MoveTo(minX, Y());
        moves->setVelX(0.0f);
    }
    else if (X() > maxX) {
        MoveTo(maxX, Y());
        moves->setVelX(0.0f);
    }

    // Trava o movimento vertical nas paredes superior e inferior do background.
    if (Y() < minY) {
        MoveTo(X(), minY);
        moves->setVelY(0.0f);
    }
    else if (Y() > maxY) {
        MoveTo(X(), maxY);
        moves->setVelY(0.0f);
    }
}

void Entity::ApplyImpulse(float vx, float vy)
{
}

void Entity::Die() {
    if (!alive) return;
    alive = false;

    LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
    lvl->GetScene()->Delete(this, this->Type());
}

void Entity::Draw() {
    if (!alive) return;
}

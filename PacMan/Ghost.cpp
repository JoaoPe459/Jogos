#include "PacMan.h"
#include "Ghost.h"
#include "Attack.h"



Ghost::Ghost() : Entity()
{
    type = GHOST;
    RandomizeSprite();

    BBox(new Rect(-20, -20, 20, 20));
    MoveTo(200.0f, 450.0f);

    // Ajusta a velocidade inicial através do objeto moves da Entity
    moves->setSpeed(static_cast<float>(500.0f - (rand() % 200)));
    moveType = static_cast<MovementType>(rand() % 3);

    dirX = (rand() % 2 == 0) ? 1 : -1;
    dirY = (rand() % 2 == 0) ? 1 : -1;
    setMass(1.2f);
}

Ghost::~Ghost()
{
    delete redSprite; redSprite = nullptr;
    delete blueSprite; blueSprite = nullptr;
    delete orangeSprite; orangeSprite = nullptr;
    delete pinkSprite; pinkSprite = nullptr;
    //delete sprite; sprite = nullptr;
}

void Ghost::Draw()
{
    sprite->Draw(X(), Y());
}

void Ghost::OnCollision(Object* obj) {
    Entity::OnCollision(obj);

    // 2. Lógica específica do Ghost: mudar direção ao bater em algo sólido
    if (obj->Type() == WALL || obj->Type() == GHOST || obj->Type() == PORTAL) {
        this->RandomizeMovement();
    }
}

void Ghost::Control() {
    if (!playerTarget) {
        // Se não houver alvo, mantém o comportamento de Wrap da tela
        HandleScreenWrap();
        return;
    }

    attackTimer += gameTime;

    // Vetor de direção até o jogador
    float diffX = playerTarget->X() - X();
    float diffY = playerTarget->Y() - Y();
    float distance = sqrt(diffX * diffX + diffY * diffY);

    // Normalização do vetor de direção
    float dirToPlayerX = (distance > 0) ? diffX / distance : 0;
    float dirToPlayerY = (distance > 0) ? diffY / distance : 0;

    float targetVX = 0;
    float targetVY = 0;
    float speed = moves->getSpeed();

    // ESTADO: PRONTO PARA ATACAR
    if (attackTimer >= attackCooldown) {
        // Aproxima-se o máximo possível
        targetVX = dirToPlayerX * speed;
        targetVY = dirToPlayerY * speed;

        // Se estiver perto o suficiente, ataca e reseta o timer
        if (distance < 150.0f) {
            AttackPlayer();
            attackTimer = 0.0f;
        }
    }
    // ESTADO: EM COOLDOWN (Fuga)
    else {
        if (distance < safeDistance) {
            // Inverte a direção para se afastar
            targetVX = -dirToPlayerX * speed;
            targetVY = -dirToPlayerY * speed;
        }
        else {
            // Se já estiver longe o suficiente, fica parado ou circula
            targetVX = 0;
            targetVY = 0;
        }
    }

    // Aplicação suave da velocidade usando o Lerp que você já criou
    float accelerationRate = 5.0f;
    float lerpFactor = accelerationRate * gameTime;
    if (lerpFactor > 1.0f) lerpFactor = 1.0f;

    float currentVX = moves->getVelX();
    float currentVY = moves->getVelY();

    moves->setVelX(currentVX + (targetVX - currentVX) * lerpFactor);
    moves->setVelY(currentVY + (targetVY - currentVY) * lerpFactor);

    HandleScreenWrap();
}

void Ghost::RandomizeSprite() {
    int randomValue = rand() % 4;
    SetSpriteByIndex(randomValue);
}

void Ghost::SetSpriteByIndex(int index) {
    switch (index) {
    case 0: this->sprite = redSprite; break;
    case 1: this->sprite = blueSprite; break;
    case 2: this->sprite = orangeSprite; break;
    case 3: this->sprite = pinkSprite; break;
    default:
        this->sprite = redSprite;
        break;
    }
}

void Ghost::AttackPlayer() {
    if (!playerTarget) return;

    // Calcula direção do projétil
    float diffX = playerTarget->X() - X();
    float diffY = playerTarget->Y() - Y();
    float distance = sqrt(diffX * diffX + diffY * diffY);

    float projVel = 400.0f; // Velocidade do tiro
    float velX = (diffX / distance) * projVel;
    float velY = (diffY / distance) * projVel;

    // Cria o ataque: owner, lifetime, damage, type, impulseX, impulseY, knockback
    Attack* bullet = new Attack(this, 1.5f, 5, Attack::AttackType::PROJECTILE, velX, velY, 100.0f);

}
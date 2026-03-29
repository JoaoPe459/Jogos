#include "Player.h"
#include "Ghost.h"
#include <cmath>
#include "PacMan.h"

Player::Player() : Entity() {
    type = PLAYER;
    sprite = new Sprite("Resources/PacManR.png");
    BBox(new Rect(-20, -20, 20, 20));
    moves->setSpeed(500.0f);
}

void Player::OnCollision(Object* obj) {
    // 1. Resolve colisão física (paredes) via Entity
    Entity::OnCollision(obj);

    // 2. Lógica de Impacto contra Ghosts
    if (obj->Type() == GHOST) {
        // Calcula o vetor de direção do impacto (do Ghost para o Player)
        float diffX = this->X() - obj->X();
        float diffY = this->Y() - obj->Y();
        float dist = sqrt(diffX * diffX + diffY * diffY);

        if (dist < 1.0f) dist = 1.0f;

        // Normaliza o vetor
        float dirX = diffX / dist;
        float dirY = diffY / dist;

        // A força diminui conforme o sizeLevel (massa) do player aumenta
        float impactForce = 1500.0f;
        float knockback = impactForce / (float)this->sizeLevel;

        // APLICAÇÃO VIA MOVES:
        this->moves->setVelX(dirX * knockback);
        this->moves->setVelY(dirY * knockback);

        // Ghost muda de direção após a batida
        static_cast<Ghost*>(obj)->RandomizeMovement();

        this->health -= 5.0f;
    }
}

void Player::Eat(float amount) {
    calories += amount;
    if (calories >= 100.0f) {
        sizeLevel++;
        calories = 0.0f;
        
    }
}

void Player::Control() {
    float baseSpeed = moves->getSpeed() - (sizeLevel * 10.0f);

    // Quanto menor a aceleração, mais tempo dura o efeito do empurrão
    float acceleration = 0.06f;

    float targetVX = 0;
    float targetVY = 0;

    if (window->KeyDown(VK_LEFT))  targetVX = -baseSpeed;
    if (window->KeyDown(VK_RIGHT)) targetVX = baseSpeed;
    if (window->KeyDown(VK_UP))    targetVY = -baseSpeed;
    if (window->KeyDown(VK_DOWN))  targetVY = baseSpeed;

    // INTERPOLAÇÕES NOS MOVIMENTOS:
    // O player tenta alcançar a targetVelocity
    float currentVX = moves->getVelX();
    float currentVY = moves->getVelY();

    moves->setVelX(currentVX + (targetVX - currentVX) * acceleration);
    moves->setVelY(currentVY + (targetVY - currentVY) * acceleration);
}

void Player::Draw() {
    sprite->Draw(X(), Y());
}


Player::~Player() {
    if (sprite) {
        delete sprite;
    }
}
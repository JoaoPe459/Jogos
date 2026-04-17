#include "Player.h"
#include "Ghost.h"
#include <cmath>
#include "PacMan.h"
#include "Engine.h"
#include "LevelMake.h"
#include "Portal.h"

Player::Player() : Entity() {
    type = PLAYER;
    sprite = new Sprite("Resources/Rato.png");
    BBox(new Rect(-80, -40, 80, 40));
    moves->setSpeed(500.0f);
}

void Player::OnCollision(Object* obj) {
    Entity::OnCollision(obj);

    if (obj->Type() == PORTAL) {
        Portal* p = (Portal*)obj;

        if (Engine::game) {
            LevelMake* lvl = static_cast<LevelMake*>(Engine::game);

            // Chamamos a função que troca o cenário e os portais
            lvl->SetStage(p->targetBG);

            // Move the player to the desired position when entering the new map
            // Example: a fixed or calculated position
            this->MoveTo(lvl->GetSpawnX(p->targetBG), lvl->GetSpawnY(p->targetBG));
        }
    }

    // 2. Lógica específica do Player (Comida)
    if (obj->Type() == FOOD) {
		Eat(10.0f);
    }
}

void Player::Eat(float amount) {
	calories += amount;
    setMass(1.0f + (sizeLevel * 0.5f));
}

void Player::Control() {
    float baseSpeed = moves->getSpeed() - (sizeLevel * 10.0f);
    float accelerationRate = 4.0f;

    float targetVX = 0;
    float targetVY = 0;

    if (window->KeyDown(VK_LEFT))  targetVX = -baseSpeed;
    if (window->KeyDown(VK_RIGHT)) targetVX = baseSpeed;
    if (window->KeyDown(VK_UP))    targetVY = -baseSpeed;
    if (window->KeyDown(VK_DOWN))  targetVY = baseSpeed;

    float currentVX = moves->getVelX();
    float currentVY = moves->getVelY();

    float lerpFactor = accelerationRate * gameTime;
    if (lerpFactor > 1.0f) lerpFactor = 1.0f;

    moves->setVelX(currentVX + (targetVX - currentVX) * lerpFactor);
    moves->setVelY(currentVY + (targetVY - currentVY) * lerpFactor);

	// Lógica de ataque (Espaço)
    if (window->KeyDown(VK_SPACE)) { // Dispara uma cacetada enquanto tu apertar espaço

        // 1. Determinar a direção do ataque basada na velocidade atual ou teclas
        // Se o player estiver parado, o ataque sai com um pequeno impulso padrão para a direita
        float attackImpulseX = (targetVX != 0) ? targetVX * 1.5f : (targetVY == 0 ? 200.0f : 0);
        float attackImpulseY = (targetVY != 0) ? targetVY * 1.5f : -50.0f; // Um leve pulinho para cima

        // 2. Criar a instância do ataque
        // Fazer um vetor de ponteiro de ataque e depois deletar para evitar vazamento de memoria
        Attack* atk = new Attack(this, 1.0f, attackImpulseX, attackImpulseY);

        // 3. Adicionar ao motor de jogo
        // Adiciona o ataque à cena (movendo) via Engine::game (LevelMake)
        // Criar uma função específica em Entity ou em qualquer outro lugar, será mais elegante
        if (Engine::game) {
            LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
            if (lvl && lvl->GetScene()) {
                lvl->GetScene()->Add(atk, MOVING);
            }
        }
    }
}

void Player::Draw() {
    sprite->Draw(X(), Y());
}


Player::~Player() {
    if (sprite) {
        delete sprite;
    }
}
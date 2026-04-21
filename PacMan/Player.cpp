#include "Player.h"
#include "Ghost.h"
#include <cmath>
#include "PacMan.h"
#include "Engine.h"
#include "LevelMake.h"
#include "Portal.h"
#include "Attack.h"
#include <string>

Player::Player() : Entity() {
    type = PLAYER;
    sprite = new Sprite("Resources/Rato.png");
    BBox(new Rect(-80, -40, 80, 40));
    moves->setSpeed(500.0f);

    type = PLAYER;      
    SetMaxHp(100);      
    SetHp(100);
    setMass(1.5f);

    calories = 0.0f;
    stamina = 100.0f;

}

void Player::OnCollision(Object* obj) {
    Entity::OnCollision(obj);

    if (obj->Type() == PORTAL) {
        Portal* p = (Portal*)obj;
        LevelMake* lvl = static_cast<LevelMake*>(Engine::game);

        if (lvl && !lvl->IsChangingStage()) {
            lvl->BeginStageChange();

            int nextStage = p->targetBG;
            lvl->SetStage(nextStage);

            const float SCREEN_WIDTH = 1300.0f;
            const float SCREEN_HEIGHT = 800.0f;
            const float MARGIN = 80.0f; // Distância da borda para não spawnar dentro do portal

            float newX = SCREEN_WIDTH / 2;  // Default Centro
            float newY = SCREEN_HEIGHT / 2; // Default Centro


            if (p->Y() < 100) {          // Portal no Topo
                newX = p->X();          
                newY = SCREEN_HEIGHT - MARGIN;
            }
            else if (p->Y() > 700) {     // Portal na Base
                newX = p->X();
                newY = MARGIN;
            }
            else if (p->X() < 100) {     // Portal na Esquerda
                newX = SCREEN_WIDTH - MARGIN;
                newY = p->Y();          
            }
            else if (p->X() > 1200) {    // Portal na Direita
                newX = MARGIN;
                newY = p->Y();
            }

            this->MoveTo(newX, newY);
            lvl->SetStageChangeCooldown(0.2f);
        }
    }

    // 2. Lógica específica do Player (Comida)
    if (obj->Type() == FOOD) {
        SetHp(GetHp() + 10);
    }
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


    // Atualiza o timer de cooldown
    if (attackTimer > 0) { attackTimer -= gameTime; }

    if (window->KeyDown(VK_SPACE) && attackTimer <= 0) {

        float atkVelX = 0.0f;
        float atkVelY = 0.0f;

        if (targetVX != 0 || targetVY != 0) {
            atkVelX = targetVX * 2.0f;
            atkVelY = targetVY * 2.0f;
        }

        Attack* atk = new Attack(
            this,               // Criador
            0.2f,               // Duração (lifetime)
            10,                  // Dano
            Attack::AttackType::PROJECTILE, // Tipo do ataque
            atkVelX,            // Velocidade X
            atkVelY,            // Velocidade Y
            2000.0f,             // Força de knockback
			30				  // Tamanho da hitbox
        );


        // 3. Resetar o cooldown
        attackTimer = attackCooldown;
    }
}

void Player::Draw() {
    sprite->Draw(X(), Y());
}

void Player::Update() {
    Entity::Update();
    Control();
}


Player::~Player() {
    if (sprite) {
        delete sprite;
    }
}
#include "Player.h"
#include "Ghost.h"
#include <cmath>
#include "PacMan.h"
#include "Engine.h"
#include "LevelMake.h"
#include "Portal.h"
#include "Attack.h"
#include <string>

void Player::UpdateOrbitalPositions() {
    int total = orbitals.size();
    for (int i = 0; i < total; i++) {
        orbitals[i]->SetOrbitParams(i, total);
    }
}

Player::Player() : Entity() {
    type = PLAYER;
    sprite = new Sprite("Resources/Player/Rato.png");
    BBox(new Rect(-80, -40, 80, 40));
    moves->setSpeed(500.0f);

    type = PLAYER;      
    SetMaxHp(100);      
    SetHp(100);
    setMass(1.5f);

    calories = 0.0f;
    stamina = 100.0f;
    damage = 15;

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
    // No OnCollision do Player
    if (obj->Type() == FOOD) {
        SetHp(GetHp() + 10);

        // Cria o orbital (duration 0.0f pois ele é permanente no Update)
        Attack* orb = new Attack(
            this, 0.0f, damage, Attack::AttackType::ORBITAL,
            0, 0, 500.0f, 20 // sizeBox menor para a aura
        );

        // Adiciona na lista do Player e atualiza todos
        orbitals.push_back(orb);
        UpdateOrbitalPositions();

        // Lógica original do seu projeto
        LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
        if (lvl) { 
            lvl->comeuItem = true;
        }

    }
}


void Player::Control() {
    float baseSpeed = moves->getSpeed() - (sizeLevel * 10.0f);
    float accelerationRate = 4.0f;

    float targetVX = 0;
    float targetVY = 0;

    // Detecta entrada
    if (window->KeyDown('A')) targetVX = -baseSpeed;
    if (window->KeyDown('D')) targetVX = baseSpeed;
    if (window->KeyDown('W')) targetVY = -baseSpeed;
    if (window->KeyDown('S')) targetVY = baseSpeed;

    // --- CORREÇÃO DE MOVIMENTO ---
    // Se estiver movendo na diagonal, normaliza para não ser mais rápido
    if (targetVX != 0 && targetVY != 0) {
        float factor = 0.7071f; // (1 / sqrt(2))
        targetVX *= factor;
        targetVY *= factor;
    }

    // Aplica o Lerp (Aceleração/Fricção)
    float currentVX = moves->getVelX();
    float currentVY = moves->getVelY();
    float lerpFactor = accelerationRate * gameTime;
    if (lerpFactor > 1.0f) lerpFactor = 1.0f;

    moves->setVelX(currentVX + (targetVX - currentVX) * lerpFactor);
    moves->setVelY(currentVY + (targetVY - currentVY) * lerpFactor);

    // --- LÓGICA DE ATAQUE ---
    if (attackTimer > 0) { attackTimer -= gameTime; }

    if (window->KeyDown(VK_SPACE) && attackTimer <= 0) {

        // Define uma direção padrão caso o jogador esteja parado (ex: atirar para a direita)
        float atkVelX = baseSpeed * 2.0f;
        float atkVelY = 0.0f;

        // Se o jogador estiver segurando alguma tecla, atira naquela direção
        if (targetVX != 0 || targetVY != 0) {
            atkVelX = targetVX * 2.0f;
            atkVelY = targetVY * 2.0f;
        }
        // Opcional: Se ele estiver parado, você pode usar a última direção salva
        // ou a velocidade atual do corpo (moves->getVelX())

        Attack* atk = new Attack(
            this, 0.3f, damage, Attack::AttackType::PROJECTILE,
            atkVelX, atkVelY, 1000.0f, 10
        );

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
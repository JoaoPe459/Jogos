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
    walking = new TileSet("Resources/Walking.png", 55, 95, 8, 40);
    anim = new Animation(walking, 0.060f, true);

    uint SeqUp[8] = { 16, 17, 18, 19, 20, 21, 22, 23 };
    uint SeqDown[8] = { 24, 25, 26, 27, 28, 29, 30, 31 };
    uint SeqLeft[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    uint SeqRight[8] = { 15, 14, 13, 12, 11, 10, 9, 8 };
    uint SeqStill[1] = { 32 };

    anim->Add(WALKUP, SeqUp, 8);
    anim->Add(WALKDOWN, SeqDown, 8);
    anim->Add(WALKLEFT, SeqLeft, 8);
    anim->Add(WALKRIGHT, SeqRight, 8);
    anim->Add(STILL, SeqStill, 1);

    state = STILL;
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
        uint SeqUp[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        uint SeqDown[8] = { 9, 10, 11, 12, 13, 14, 15, 16 };
        uint SeqLeft[8] = { 17, 18, 19, 20, 21, 22, 23, 24 };
        uint SeqRight[8] = { 25 , 26, 27, 28, 29, 30, 31, 32 };
        uint SeqStill[1] = { 32 };
        Attack* orb = new Attack("Resources/Effects/Attackplayer.png",
            64,
            64,
            8,
            8,
            SeqUp,
            SeqDown,
            SeqLeft,
            SeqRight,
            SeqStill,
            this,
            0.0f,
            10,
            500.0f,
            Attack::AttackType::EXPLOSION,
            20,
            100,
            100
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

    // --- LOGICA DE MOVIMENTAÇÃO (WASD) ---[cite: 4]
    if (window->KeyUp('W') && window->KeyUp('S') && window->KeyUp('A') && window->KeyUp('D')) {
        state = STILL;
    }

    if (window->KeyDown('A')) { targetVX = -baseSpeed; state = WALKLEFT; }
    if (window->KeyDown('D')) { targetVX = baseSpeed; state = WALKRIGHT; }
    if (window->KeyDown('W')) { targetVY = -baseSpeed; state = WALKUP; }
    if (window->KeyDown('S')) { targetVY = baseSpeed; state = WALKDOWN; }

    anim->Select(state);
    anim->NextFrame();

    if (targetVX != 0 && targetVY != 0) {
        float factor = 0.7071f; // (1 / sqrt(2))
        targetVX *= factor;
        targetVY *= factor;
    }

    float currentVX = moves->getVelX();
    float currentVY = moves->getVelY();
    float lerpFactor = accelerationRate * gameTime;
    if (lerpFactor > 1.0f) lerpFactor = 1.0f;

    moves->setVelX(currentVX + (targetVX - currentVX) * lerpFactor);
    moves->setVelY(currentVY + (targetVY - currentVY) * lerpFactor);

    if (attackTimer > 0) { attackTimer -= gameTime; }

    // Detecta direção pelas setas
    bool shootUp = window->KeyDown(VK_UP);
    bool shootDown = window->KeyDown(VK_DOWN);
    bool shootLeft = window->KeyDown(VK_LEFT);
    bool shootRight = window->KeyDown(VK_RIGHT);

    if ((shootUp || shootDown || shootLeft || shootRight) && attackTimer <= 0) {
        float atkVelX = 0.0f;
        float atkVelY = 0.0f;
        float projectileSpeed = baseSpeed * 2.5f; // Velocidade do tiro

        if (shootUp)    atkVelY = -projectileSpeed;
        if (shootDown)  atkVelY = projectileSpeed;
        if (shootLeft)  atkVelX = -projectileSpeed;
        if (shootRight) atkVelX = projectileSpeed;

        // Normaliza diagonal do tiro
        if (atkVelX != 0 && atkVelY != 0) {
            atkVelX *= 0.7071f;
            atkVelY *= 0.7071f;
        }

        uint SeqUp[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
        uint SeqDown[8] = { 9, 10, 11, 12, 13, 14, 15, 16 };
        uint SeqLeft[8] = { 17, 18, 19, 20, 21, 22, 23, 24 };
        uint SeqRight[8] = { 25 , 26, 27, 28, 29, 30, 31, 32 };
        uint SeqStill[1] = { 32 };

        new Attack("Resources/Effects/Attackplayer.png",
            64, 64, 8, 8,
            SeqUp, SeqDown, SeqLeft, SeqRight, SeqStill,
            this,
            0.4f,        // Duração
            15,          // Dano
            500.0f,      // Knockback
            Attack::AttackType::PROJECTILE, // Tipo projétil para mover
            25,          // SizeBox
            atkVelX,     // Velocidade X calculada pelas setas
            atkVelY      // Velocidade Y calculada pelas setas
        );

        attackTimer = attackCooldown;
    }
}

void Player::Draw()
{
    anim->Draw(x, y, z);
}

void Player::Update() {
    Entity::Update();
    Control();
}


Player::~Player() {
    delete anim;
    delete walking;
}
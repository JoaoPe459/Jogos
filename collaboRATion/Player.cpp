#include "Player.h"
#include "Ghost.h"
#include <cmath>
#include "PacMan.h"
#include "Engine.h"
#include "LevelMake.h"
#include "Portal.h"
#include "Attack.h"
#include "Enemy.h"
#include <string>

void Player::UpdateOrbitalPositions() {
    int total = orbitals.size();
    for (int i = 0; i < total; i++) {
        orbitals[i]->SetOrbitParams(i, total);
    }
}

Player::Player() : Entity() {
    type = PLAYER;
    animation = new TileSet("Resources/Player/Rato2.png", 64, 64, 15, 30);
    anim = new Animation(animation, 0.040f, true);
    

    // Primeira fileira anda para direita; segunda fileira anda para esquerda.
    uint SeqRightIdle[1] = {29};
    uint SeqLeftIdle[1] = {0};
    uint SeqRightWalk[4] = { 28,27,26,25};
    uint SeqLeftWalk[4] = { 1,2,3,4};
    uint SeqRightJump[1] = { 0 };
    uint SeqLeftJump[1] = { 0 };
    uint SeqLeftDeath[9] = { 25,24,23,22,21,20,19,18,17 };
    uint SeqRightDeath[9] = { 5,6,7,8,9,10,11,12,13 };

    // A spritesheet nao tem animacao vertical, entao W/S reaproveitam a fileira da direita.
    anim->Add(JUMPLEFT, SeqLeftJump, 1);
    anim->Add(JUMPLEFT, SeqRightJump, 1);
    anim->Add(WALKLEFT, SeqLeftWalk, 4);
    anim->Add(WALKRIGHT, SeqRightWalk, 4);
    anim->Add(IDLELEFT, SeqLeftIdle, 1);
    anim->Add(IDLERIGHT, SeqRightIdle, 1);
    anim->Add(DEATHLEFT, SeqRightIdle, 9);
    anim->Add(DEATHRIGHT, SeqRightIdle, 9);

    state = IDLERIGHT;
    // Bounding box alinhada ao novo tamanho de quadro do rato.
    BBox(new Rect(-20, -25, 23, 25));
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

            // Ponto padrao seguro: centro do chao jogavel.
            float newX = (PlayArea::Left + PlayArea::Right) / 2.0f;
            float newY = (PlayArea::Top + PlayArea::Bottom) / 2.0f;


            // Ao atravessar um portal, nasce do lado oposto ja dentro do piso.
            if (p->Y() <= PlayArea::Top + 40.0f) {          // Portal no Topo
                newX = p->X();          
                newY = PlayArea::Bottom - PlayArea::SpawnMargin;
            }
            else if (p->Y() >= PlayArea::Bottom - 40.0f) {     // Portal na Base
                newX = p->X();
                newY = PlayArea::Top + PlayArea::SpawnMargin;
            }
            else if (p->X() <= PlayArea::Left + 40.0f) {     // Portal na Esquerda
                newX = PlayArea::Right - PlayArea::SpawnMargin;
                newY = p->Y();          
            }
            else if (p->X() >= PlayArea::Right - 40.0f) {    // Portal na Direita
                newX = PlayArea::Left + PlayArea::SpawnMargin;
                newY = p->Y();
            }

            this->MoveTo(newX, newY);
            lvl->SetStage(nextStage);
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

        LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
        if (lvl) { 
            lvl->comeuItem = true;
        }
        totalLevelsVisited++;

    }

    if (obj->Type() == ENEMY) {
        Enemy* enemy = (Enemy*)obj;

        SetHp(GetHp() - damage);    

        // 2. Calcula a direção do Knockback (Afastamento)
        float diffX = this->X() - enemy->X();
        float diffY = this->Y() - enemy->Y();
        float distance = sqrt(diffX * diffX + diffY * diffY);

        if (distance > 0) {
            float pushForce = 200.0f;

            moves->setVelX((diffX / distance) * pushForce);
            moves->setVelY((diffY / distance) * pushForce);
        }
    }
}



void Player::Control() {
    float baseSpeed = moves->getSpeed() - (sizeLevel * 10.0f);
    float accelerationRate = 4.0f;

    float targetVX = 0;
    float targetVY = 0;

    // --- LOGICA DE MOVIMENTAÇÃO (WASD) ---[cite: 4]
    if (window->KeyUp('W') && window->KeyUp('S') && window->KeyUp('A')) {
        
        state = IDLERIGHT;
        
    }

    if (window->KeyDown('A')) { targetVX = -baseSpeed; state = WALKLEFT; }
    if (window->KeyDown('D')) { targetVX = baseSpeed; state = WALKRIGHT; }
    if (window->KeyDown('W')) {
        if (state == IDLELEFT || state == IDLERIGHT) { targetVY = -baseSpeed; state = WALKRIGHT; }
        else { targetVY = -baseSpeed; state = state; }
    }
    if (window->KeyDown('S')) {
        if (state == IDLELEFT || state == IDLERIGHT) { targetVY = baseSpeed; state = WALKRIGHT; }
        else { targetVY = baseSpeed; state = state; }
    }
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
		totalDamageDealt += damage;
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
    delete animation;
}

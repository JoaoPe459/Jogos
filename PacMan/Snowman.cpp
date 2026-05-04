#include "PacMan.h"
#include "Enemy.h"
#include "Attack.h"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;


Enemy::Enemy() : Entity()
{
    type = ENEMY;
    struct Enemy {
        std::string walkPath;
        std::string idlePath;
        std::string attackPath;
        int frameWidth;
        int frameHeight;
        int columns;
        int totalFrames;
    };

    std::vector<Enemy> enemyVec = {
        // Inimigo 0: Snowman
        {"Resources/Enemy/snowman/SnowManWalk.png", "Resources/Enemy/snowman/SnowManIdle.png", "Resources/Effects/SnowAttack.png", 128, 128, 6, 40},
        {"Resources/Enemy/ghost/GhostWalk.png", "Resources/Enemy/ghost/GhostIdle.png", "Resources/Effects/GhostAttack.png",128, 128, 6, 40},
        {"Resources/Enemy/doctor/DoctorWalk.png", "Resources/Enemy/doctor/DoctorIdle.png", "Resources/Effects/Attackplayer.png",128, 128, 6, 40},
        {"Resources/Enemy/skeleton/SkeletonWalk.png", "Resources/Enemy/skeleton/SkeletonIdle.png", "Resources/Effects/BoneAttack.png",128, 128, 6, 40}
    };

    int randomIndex = rand() % enemyVec.size();
    Enemy chosenEnemy = enemyVec[randomIndex];

    this->attackSpritePath = chosenEnemy.attackPath;

    walking = new TileSet(chosenEnemy.walkPath.c_str(), chosenEnemy.frameWidth, chosenEnemy.frameHeight, chosenEnemy.columns, chosenEnemy.totalFrames);
    idle = new TileSet(chosenEnemy.idlePath.c_str(), chosenEnemy.frameWidth, chosenEnemy.frameHeight, chosenEnemy.columns, chosenEnemy.totalFrames);


    animWalk = new Animation(walking, 0.060f, true);
    animIdle = new Animation(idle, 0.060f, true);

    uint SeqUp[6] = { 18, 19, 20, 21, 22, 23,};
    uint SeqDown[6] = { 0, 1, 2, 3, 4, 5};
    uint SeqLeft[6] = { 6, 7, 8, 9, 10, 11};
    uint SeqRight[6] = { 12, 13, 14, 15, 16, 17};
    uint SeqStill[4] = {0,1,2,3};

    animWalk->Add(WALKUP, SeqUp, 6);
    animWalk->Add(WALKDOWN, SeqDown, 6);
    animWalk->Add(WALKLEFT, SeqLeft, 6);
    animWalk->Add(WALKRIGHT, SeqRight, 6);

    animIdle->Add(STILL, SeqStill, 4);
    
    anim = animIdle;
    state = STILL;
    anim->Select(state);

    if (walking) {

        float h = (float)walking->Height()/(walking->Columns()*1.5);
        float w = (float)walking->Width()/((walking->Size()/walking->Columns()*1.5));
        // Exemplo: Rect(topo, esquerda, baixo, direita) relativo ao centro
        BBox(new Rect(-h / 2, -w / 2, h / 2, w / 2));
    }

    // --- 4. AJUSTES DE MOVIMENTAÇÃO E SPAWN ---
    moves->setSpeed(static_cast<float>(500.0f - (rand() % 200)));
    moveType = static_cast<MovementType>(rand() % 3);

    dirX = (rand() % 2 == 0) ? 1 : -1;
    dirY = (rand() % 2 == 0) ? 1 : -1;
    setMass(1.2f);

    damage = 30;
    hp = 100;
    maxHp = 100;
}

Enemy::~Enemy() {   
    delete animWalk;
    delete animIdle;
    delete walking;
    delete idle;
}

void Enemy::Draw()
{
    {
        anim->Draw(x, y, z);
    }
}

void Enemy::Update() {
    Entity::Update();
    Control();
}

void Enemy::OnCollision(Object* obj) {
    Entity::OnCollision(obj);

    // 2. Lógica específica do Ghost: mudar direção ao bater em algo sólido
    if (obj->Type() == WALL || obj->Type() == GHOST || obj->Type() == PORTAL || obj->Type() == FOOD || obj->Type() ==  ENEMY) {
        this->RandomizeMovement();
    }

    if (obj->Type() == ENEMY) {
        float diffX = this->X() - obj->X();
        float diffY = this->Y() - obj->Y();
        float distance = sqrt(diffX * diffX + diffY * diffY);

        // Garante que não há divisão por zero se nascerem no exato mesmo pixel
        if (distance > 0) {
            this->MoveTo(this->X() + (diffX / distance) * 1.5f, this->Y() + (diffY / distance) * 1.5f);

            moves->setVelX((diffX / distance) * 150.0f);
            moves->setVelY((diffY / distance) * 150.0f);
        }
    }
}

void Enemy::Control() {
    if (!playerTarget) {
        HandleScreenLimits();
        return;
    }

    attackTimer += gameTime;

    float diffX = playerTarget->X() - X();
    float diffY = playerTarget->Y() - Y();
    float distance = sqrt(diffX * diffX + diffY * diffY);

    float dirToPlayerX = (distance > 0) ? diffX / distance : 0;
    float dirToPlayerY = (distance > 0) ? diffY / distance : 0;

    float targetVX = 0;
    float targetVY = 0;
    float speed = moves->getSpeed();

    // Se estiver em cooldown, ele foge se o player chegar perto
    if (attackTimer < attackCooldown) {
        if (distance < 200.0f) { // Se o player chegar perto durante o cooldown
            targetVX = -dirToPlayerX * (speed*0.7f);
            targetVY = dirToPlayerY * (speed*0.7f);
        }
        else {
            // Movimento errático ou lento enquanto recarrega
            targetVX = moves->getVelX() * 0.9f;
            targetVY = moves->getVelY() * 0.9f;
        }
    }
    else {
        // Modo Perseguição
        targetVX = dirToPlayerX * speed;
        targetVY = dirToPlayerY * speed;

        if (distance < 150.0f) {
            AttackPlayer();
            attackTimer = 0.0f;
        }
    }

    if(abs(targetVX) < 40.0f && abs(targetVY) < 40.0f) {
        state = STILL;
        anim = animIdle;  // Troca para a spritesheet de Idle
    }
    else {
        anim = animWalk;
        if (abs(targetVX) > abs(targetVY)) {
            if (targetVX > 0) state = WALKRIGHT;
            else state = WALKLEFT;
        }
        else {
            if (targetVY > 0) state = WALKDOWN;
            else state = WALKUP;
        }
    }
    anim->Select(state);
    anim->NextFrame();

    // Aumente o accelerationRate se parecer "lento" para virar
    float accelerationRate = 10.0f;
    float lerpFactor = accelerationRate * gameTime;
    if (lerpFactor > 1.0f) lerpFactor = 1.0f;

    moves->setVelX(moves->getVelX() + (targetVX - moves->getVelX()) * lerpFactor);
    moves->setVelY(moves->getVelY() + (targetVY - moves->getVelY()) * lerpFactor);

    // Garante que a perseguicao nao leve o Snowman para fora do chao.
    HandleScreenLimits();
}

void Enemy::AttackPlayer() {
    if (!playerTarget) return;

    // Calcula direção do projétil
    float diffX = playerTarget->X() - X();
    float diffY = playerTarget->Y() - Y();
    float distance = sqrt(diffX * diffX + diffY * diffY);

    float projVel = 600.0f;
    float velX = (diffX / distance) * projVel;
    float velY = (diffY / distance) * projVel;
    // Cria o ataque: owner, lifetime, damage, type, impulseX, impulseY, knockback
    uint SeqUp[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint SeqDown[8] = { 9, 10, 11, 12, 13, 14, 15, 16 };
    uint SeqLeft[8] = { 17, 18, 19, 20, 21, 22, 23, 24 };
    uint SeqRight[8] = { 25 , 26, 27, 28, 29, 30, 31, 32 };
    uint SeqStill[1] = { 32 };
    Attack* bullet = new Attack(this->attackSpritePath,
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
        0.6f,
        damage,
        500.0f,
        Attack::AttackType::EXPLOSION,
        15,
        velX,
        velY
    );

}


#include "PacMan.h"
#include "Enemy.h"
#include "Attack.h"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;


Enemy::Enemy() : Entity()
{
    type = ENEMY;

    walking = new TileSet("Resources/Enemy/snowman/SnowManWalk.png", 128, 128, 6, 40);
    idle = new TileSet("Resources/Enemy/snowman/SnowManIdle.png", 128, 128, 6, 40);

    anim = new Animation(walking, 0.060f, true);

    uint SeqUp[6] = { 18, 19, 20, 21, 22, 23,};
    uint SeqDown[6] = { 0, 1, 2, 3, 4, 5};
    uint SeqLeft[6] = { 6, 7, 8, 9, 10, 11};
    uint SeqRight[6] = { 12, 13, 14, 15, 16, 17};
    uint SeqStill[4] = {0,1,2,3};

    anim->Add(WALKUP, SeqUp, 6);
    anim->Add(WALKDOWN, SeqDown, 6);
    anim->Add(WALKLEFT, SeqLeft, 6);
    anim->Add(WALKRIGHT, SeqRight, 6);
    anim->Add(STILL, SeqStill, 6);
    
    state = STILL;
    anim->Select(state);

    if (walking) {

        float h = (float)walking->Height()/walking->Columns();
        float w = (float)walking->Width()/(walking->Size()/walking->Columns());
        // Exemplo: Rect(topo, esquerda, baixo, direita) relativo ao centro
        BBox(new Rect(-h / 2, -w / 2, h / 2, w / 2));
    }

    // --- 4. AJUSTES DE MOVIMENTAÇÃO E SPAWN ---
    moves->setSpeed(static_cast<float>(500.0f - (rand() % 200)));
    moveType = static_cast<MovementType>(rand() % 3);

    dirX = (rand() % 2 == 0) ? 1 : -1;
    dirY = (rand() % 2 == 0) ? 1 : -1;
    setMass(1.2f);

    float margin = 50.0f;
    int rangeX = (int)(window->Width() - (margin * 2));
    int rangeY = (int)(window->Height() - (margin * 2));

    if (rangeX <= 0) rangeX = 1;
    if (rangeY <= 0) rangeY = 1;

    float randomX = (float)(rand() % rangeX) + margin;
    float randomY = (float)(rand() % rangeY) + margin;

    this->MoveTo(randomX, randomY);

    damage = 50;
    hp = 100;
    maxHp = 100;
}

Enemy::~Enemy() {
    delete anim;
    delete walking;
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
}

void Enemy::Control() {
    if (!playerTarget) {
        HandleScreenWrap();
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
            targetVX = -dirToPlayerX * speed;
            targetVY = -dirToPlayerY * speed;
        }
        else {
            // Movimento errático ou lento enquanto recarrega
            targetVX = moves->getVelX();
            targetVY = moves->getVelY();
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

   
        if (abs(targetVX) > abs(targetVY)) {
            if (targetVX > 0) state = WALKRIGHT;
            else state = WALKLEFT;
        }
        else {
            if (targetVY > 0) state = WALKDOWN;
            else state = WALKUP;
        }

    anim->Select(state);
    anim->NextFrame();

    // Aumente o accelerationRate se parecer "lento" para virar
    float accelerationRate = 10.0f;
    float lerpFactor = accelerationRate * gameTime;
    if (lerpFactor > 1.0f) lerpFactor = 1.0f;

    moves->setVelX(moves->getVelX() + (targetVX - moves->getVelX()) * lerpFactor);
    moves->setVelY(moves->getVelY() + (targetVY - moves->getVelY()) * lerpFactor);

    HandleScreenWrap();
}

void Enemy::AttackPlayer() {
    if (!playerTarget) return;

    // Calcula direção do projétil
    float diffX = playerTarget->X() - X();
    float diffY = playerTarget->Y() - Y();
    float distance = sqrt(diffX * diffX + diffY * diffY);

    float projVel = 400.0f;
    float velX = (diffX / distance) * projVel;
    float velY = (diffY / distance) * projVel;
    // Cria o ataque: owner, lifetime, damage, type, impulseX, impulseY, knockback
    uint SeqUp[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    uint SeqDown[8] = { 9, 10, 11, 12, 13, 14, 15, 16 };
    uint SeqLeft[8] = { 17, 18, 19, 20, 21, 22, 23, 24 };
    uint SeqRight[8] = { 25 , 26, 27, 28, 29, 30, 31, 32 };
    uint SeqStill[1] = { 32 };
    Attack* bullet = new Attack("Resources/Effects/Attackplayer.png",
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
        0.3f,
        damage,
        500.0f,
        Attack::AttackType::EXPLOSION,
        15,
        velX,
        velY
    );

}


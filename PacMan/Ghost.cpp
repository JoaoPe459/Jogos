#include "PacMan.h"
#include "Ghost.h"
#include "Attack.h"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;


Ghost::Ghost() : Entity()
{
    type = ENEMY;

    // --- 1. CARREGAMENTO AUTOMÁTICO DA PASTA ---
    std::string path = "Resources/Enemy";
    std::vector<std::string> filePaths;

    if (fs::exists(path) && fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            std::string ext = entry.path().extension().string();
            if (ext == ".png" || ext == ".jpg") {
                filePaths.push_back(entry.path().string());
            }
        }
    }

    spriteCount = static_cast<int>(filePaths.size());

    // Inicializa o array dinâmico se houver arquivos
    if (spriteCount > 0) {
        sprites = new Sprite * [spriteCount];
        for (int i = 0; i < spriteCount; i++) {
            sprites[i] = new Sprite(filePaths[i].c_str());
        }
    }

    // --- 2. DEFINIÇÃO DO SPRITE ATUAL ---
    RandomizeSprite();

    // --- 3. DEFINIÇÃO DA BBOX (Baseada no sprite carregado) ---
    if (currentSprite) {
        // Criamos a BBox centralizada no sprite sorteado
        float h = (float)currentSprite->Height();
        float w = (float)currentSprite->Width();
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
}

Ghost::~Ghost() {
    if (sprites != nullptr) {
        for (int i = 0; i < spriteCount; i++) {
            delete sprites[i];
        }
        delete[] sprites;
    }
}

void Ghost::Draw()
{
    if (currentSprite) {
        currentSprite->Draw(X(), Y());
    }
}

void Ghost::Update() {
    Entity::Update();
    Control();
}

void Ghost::OnCollision(Object* obj) {
    Entity::OnCollision(obj);

    // 2. Lógica específica do Ghost: mudar direção ao bater em algo sólido
    if (obj->Type() == WALL || obj->Type() == GHOST || obj->Type() == PORTAL || obj->Type() == FOOD) {
        this->RandomizeMovement();
    }
}

void Ghost::Control() {
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

    // Aumente o accelerationRate se parecer "lento" para virar
    float accelerationRate = 10.0f;
    float lerpFactor = accelerationRate * gameTime;
    if (lerpFactor > 1.0f) lerpFactor = 1.0f;

    moves->setVelX(moves->getVelX() + (targetVX - moves->getVelX()) * lerpFactor);
    moves->setVelY(moves->getVelY() + (targetVY - moves->getVelY()) * lerpFactor);

    HandleScreenWrap();
}

void Ghost::RandomizeSprite() {
    if (spriteCount > 0 && sprites != nullptr) {
        currentSprite = sprites[rand() % spriteCount];
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
    Attack* bullet = new Attack(this, 1.5f, 10, Attack::AttackType::PROJECTILE, velX, velY, 100.0f);

}


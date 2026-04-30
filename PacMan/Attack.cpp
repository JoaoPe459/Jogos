#include "Attack.h"
#include "Entity.h"
#include "PacMan.h"
#include "Engine.h"
#include "LevelMake.h"


class Scene;

namespace fs = std::filesystem;

Attack::Attack(string path,
    int titleWidth,
    int titleHeigth,
    int titleColuns,
    int titleLines,
    uint up[8],
    uint down[8],
    uint left[8],
    uint right[8],
    uint still[1],
    Entity* creator,
    float lifeTime,
    int dmg,
    float knockbackForce,
    AttackType type,
    float sizeBox,
    float velX,
    float velY
)
{
    walking = new TileSet(path, titleWidth, titleHeigth, titleColuns, titleLines);
    currentAnimation = new Animation(walking, 0.060f, true);
    currentAnimation->Add(WALKUP, up, 8);
    currentAnimation->Add(WALKDOWN, down, 8);
    currentAnimation->Add(WALKLEFT, left, 8);
    currentAnimation->Add(WALKRIGHT,right, 8);
    currentAnimation->Add(STILL, still, 1);


    // 2. Atribuição de Atributos
    this->owner = creator;
    this->duration = lifeTime;
    this->timer = 0.0f;
    this->damage = dmg;
    this->knockback = knockbackForce;
    this->attackType = type;
    this->type = ATTACK;

    // 3. Física e Posicionamento
    BBox(new Rect(-sizeBox, -sizeBox, sizeBox, sizeBox));

    if (owner) {
        MoveTo(owner->X(), owner->Y());
    }

    this->mass = 0.5f;
    moves->setVelX(velX);
    moves->setVelY(velY);
    // 4. Registro na Cena (Segurança extra com cast)
    if (auto* lvl = dynamic_cast<LevelMake*>(Engine::game)) {
        if (auto* scene = lvl->GetScene()) {
            scene->Add(this, STATIC);
        }
    }
}

Attack::~Attack() {
    delete walking;
    delete currentAnimation;
}

void Attack::Control() {
}

void Attack::Update() {
    timer += gameTime;
    currentAnimation->Select(WALKUP);
    currentAnimation->NextFrame();
    // Se for ORBITAL, ele ignora o timer de morte (duration)
    if (attackType == AttackType::ORBITAL && owner) {
        // Incrementa o ângulo global
        angle += orbitSpeed * gameTime;

        // Calcula o espaçamento (360 graus / total de objetos)
        // 2 * PI = 6.28318f
        float spacing = 6.28318f / totalOrbitals;
        float finalAngle = angle + (orbitIndex * spacing);

        // Define a nova posição baseada no seno e cosseno
        float newX = owner->X() + cos(finalAngle) * orbitRadius;
        float newY = owner->Y() + sin(finalAngle) * orbitRadius;

        MoveTo(newX, newY);
    }
    else {
        // Lógica original para projéteis e melee
        if (timer >= duration) {
            this->Die();
        }

        if (attackType == AttackType::MELEE && owner) {
            MoveTo(owner->X(), owner->Y());
        }
        else {
            ApplyPhysics();
        }
    }
}

void Attack::OnCollision(Object* obj) {
    if (!obj || !owner || !alive) return;
    if (obj == owner || obj->Type() == ATTACK) return;

    if (obj->Type() == GHOST || obj->Type() == ENEMY || obj->Type() == PLAYER) {
        Entity* target = static_cast<Entity*>(obj);
        if (target) {
            target->TakeDamage(this);

            // REGRA: Se NÃO for orbital, morre ao tocar. Se for, continua vivo.
            if (attackType != AttackType::ORBITAL) {
                this->Die();
            }
        }
    }
}

void Attack::Draw() {
    currentAnimation->Draw(x, y, z);
}

Entity* Attack::GetOwner() const { return owner; }
int Attack::GetDamage() const { return damage; }
Attack::AttackType Attack::GetType() const { return attackType; }
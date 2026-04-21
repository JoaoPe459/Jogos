#include "Attack.h"
#include "Entity.h"
#include "PacMan.h"
#include "Engine.h"
#include "LevelMake.h"

class Scene;

Attack::Attack(Entity* creator, float lifeTime, int dmg, AttackType type, float impulseX, float impulseY, float knockbackForce, int sizeBox) {
    owner = creator;
    duration = lifeTime;
    timer = 0.0f;

    damage = dmg;
    knockback = knockbackForce;
    attackType = type;

    this->type = ATTACK;

    BBox(new Rect(-sizeBox, -sizeBox, sizeBox, sizeBox));

    if (owner) {
        MoveTo(owner->X(), owner->Y());
    }

    mass = 0.5f;
    moves->setVelX(impulseX);
    moves->setVelY(impulseY);

    if (Engine::game) {
        LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
        if (lvl && lvl->GetScene()) {
            lvl->GetScene()->Add(this, STATIC);
        }
    }
}

Attack::~Attack() {
}

void Attack::Control() {
}

void Attack::Update() {
    timer += gameTime;

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
    // Implementação visual (Sprite ou formas geométricas)
}

Entity* Attack::GetOwner() const { return owner; }
int Attack::GetDamage() const { return damage; }
Attack::AttackType Attack::GetType() const { return attackType; }
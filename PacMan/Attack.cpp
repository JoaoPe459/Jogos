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

void Attack::OnCollision(Object* obj) {
    if (!obj || !owner || !alive) return;

    // 1. Não colidir com o próprio dono ou outros ataques
    if (obj == owner || obj->Type() == ATTACK) return;

    // 2. Se colidir com Parede ou Comida (Projectile morre)
    if (obj->Type() == WALL || obj->Type() == FOOD) {
        return;
    }

    // 3. Se colidir com inimigo OU Player (Dano e desaparece)
    if (obj->Type() == GHOST || obj->Type() == ENEMY || obj->Type() == PLAYER) {
        Entity* target = static_cast<Entity*>(obj);
        if (target) {
            target->TakeDamage(this);
            this->Die();
        }

    }
}

void Attack::Draw() {
    // Implementação visual (Sprite ou formas geométricas)
}

Entity* Attack::GetOwner() const { return owner; }
int Attack::GetDamage() const { return damage; }
Attack::AttackType Attack::GetType() const { return attackType; }
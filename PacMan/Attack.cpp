#include "Attack.h"
#include "Entity.h"
#include "PacMan.h"
#include "Engine.h"
#include "LevelMake.h"

Attack::Attack(Entity* creator, float lifeTime, int dmg, AttackType type, float impulseX, float impulseY, float knockbackForce) {
    owner = creator;
    duration = lifeTime;
    timer = 0.0f;

    damage = dmg;
    knockback = knockbackForce;
    attackType = type;

    this->type = ATTACK;

    BBox(new Rect(-5, -5, 5, 5));

    if (owner) {
        MoveTo(owner->X(), owner->Y());
    }

    mass = 0.5f;
    moves->setVelX(impulseX);
    moves->setVelY(impulseY);

    if (Engine::game) {
        LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
        if (lvl && lvl->GetScene()) {
            lvl->GetScene()->Add(this, MOVING);
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
        return;
    }

    // Se for um ataque MELEE, ele "gruda" no dono
    if (attackType == AttackType::MELEE && owner) {
        MoveTo(owner->X(), owner->Y());
    }
    else {
        ApplyPhysics();
    }
}

void Attack::OnCollision(Object* obj) {
    if (!obj || !owner || !alive) return;

    // 1. Não colidir com o próprio dono
    if (obj == owner) return;

    // 2. Parede destrói projéteis, mas talvez não ataques corpo-a-corpo
    if (obj->Type() == WALL) {
        if (attackType == AttackType::PROJECTILE) {
            this->Die();
        }
        return;
    }

    // 3. Se colidir com uma Entity (Player, Ghost, etc.)
    // Verificamos se o objeto colidido herda de Entity usando o Type()
    if (obj->Type() == PLAYER || obj->Type() == GHOST || obj->Type() == ENEMY) {
        Entity* target = static_cast<Entity*>(obj);

        // Aplica o dano
        target->TakeDamage(this);

        // Se não for um ataque persistente (área), o ataque morre ao atingir o primeiro alvo
        if (attackType == AttackType::PROJECTILE) {
            this->Die();
        }
    }

    // 4. Se colidir com um portal
	if (obj->Type() == PORTAL) {
		this->Die();
        return;
    }
}

void Attack::Draw() {
    // Implementação visual (Sprite ou formas geométricas)
}

Entity* Attack::GetOwner() const { return owner; }
int Attack::GetDamage() const { return damage; }
Attack::AttackType Attack::GetType() const { return attackType; }
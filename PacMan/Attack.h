#ifndef _ATTACK_H_
#define _ATTACK_H_

#include "Entity.h"

class Attack : public Entity {
public:

    enum class AttackType {
        GENERIC,
        PROJECTILE,
        MELEE,
        EXPLOSION,
        ORBITAL
    };

private:
    float timer;
    float duration;
    float angle = 0.0f;
    float orbitRadius = 150.0f;
    float orbitSpeed = 3.0f;
    int orbitIndex = 0;      // Para posicionar múltiplos orbitais
    int totalOrbitals = 1;   // Para distribuir uniformemente no círculo
    int damage;
    float knockback;

    Entity* owner;

    AttackType attackType;

public:
    Attack(Entity* creator,
    float lifeTime,
    int damage,
    AttackType type = AttackType::GENERIC,
    float impulseX = 0.0f,
    float impulseY = 0.0f,
    float knockbackForce = 0.0f,
    int sizeBox = 40);

    ~Attack();

    void Update() override;
    void OnCollision(Object* obj) override;
    void Draw() override;
    void Control() override;

    Entity* GetOwner() const;
    int GetDamage() const;
    AttackType GetType() const;
    void SetOrbitParams(int index, int total) {
        orbitIndex = index;
        totalOrbitals = total;
    }
};

#endif
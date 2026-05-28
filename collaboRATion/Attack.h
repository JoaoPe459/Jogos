#ifndef _ATTACK_H_
#define _ATTACK_H_

#include "Entity.h"
#include "Animation.h"
#include "TileSet.h"
#include <filesystem>
#include <vector>

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
    TileSet* walking;
    Animation* currentAnimation = nullptr; // Sprite selecionado para esta instância
    int spriteCount = 0;


public:
    

    Attack(string path, int titleWidth, int titleHeigth, int titleColuns, int titleLines, uint up[8], uint down[8], uint left[8], uint right[8], uint still[1], Entity* creator, float lifeTime, int dmg, float knockbackForce, AttackType type, float sizeBox, float velX, float velY);

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
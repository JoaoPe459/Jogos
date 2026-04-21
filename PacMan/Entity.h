#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "Object.h"
#include "Moves.h"
#include "Sprite.h"
#include "Engine.h"

class LevelMake;

class Entity : public Object {
protected:
    Moves* moves = nullptr;
    float mass = 0.0f;

    // --- Sistema de Vida / Combate ---
    int hp = 1;
    int maxHp = 1;
    bool alive = true;

    // --- Sistema de Invencibilidade (i-frames) ---
    bool invulnerable = false;
    float invulTimer = 0.0f;

	int damage = 1; // Dano base causado por este Entity (pode ser usado por ataques)

    void Die();
    void HandleWallCollision(Object* wall);
    void ApplyKnockback(Object* source, float force);

    void HandleScreenWrap();

public:
    Entity();
    virtual ~Entity();
    virtual void Update() override;
    virtual void Draw() override;
    virtual void OnCollision(Object* obj) override;
    virtual void Control() = 0;

    void ApplyPhysics();
    void HandleScreenLimits();
    void ApplyImpulse(float vx, float vy);

    // --- Getters e Setters de Combate ---
    void SetMaxHp(int value);
    void SetHp(int value);
	void SetAlive(bool value) { alive = value; }
    int GetHp() const;
    int GetMaxHp() const;
    bool IsAlive() const;

    // --- Métodos de Ação de Combate ---
    virtual void TakeDamage(Object* source = nullptr);
    void Heal(int amount);
    void SetInvulnerable(float time);

    void setMass(float m);
    float getMass() const;
};

#endif
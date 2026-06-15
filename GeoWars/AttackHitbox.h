#ifndef ATTACKHITBOX_H
#define ATTACKHITBOX_H

#include "Object.h"
#include "GeoWars.h"
#include "IDamageable.h"

// -------------------------------------------------------------------------------

class AttackHitbox : public Object
{
public:
    AttackHitbox(float px, float py, int dmg, uint creator, float duration = 0.12f)
        : damage(dmg), creatorType(creator), timer(duration), hitCount(0), maxHits(3)
    {
        MoveTo(px, py);
        BBox(new Rect(28.0f, 24.0f, 0, 0));
        type = ATTACK;
    }

    ~AttackHitbox() {}

    void Update() override
    {
        timer -= gameTime;
        if (timer <= 0 || hitCount >= maxHits)
            GeoWars::scene->Delete(this, STATIC);
    }

    void Draw() override {}

    void OnCollision(Object* obj) override
    {
        // Evita friendly-fire: não acerta objetos do mesmo tipo do criador
        if (obj->Type() == creatorType)
            return;

        IDamageable* target = dynamic_cast<IDamageable*>(obj);
        if (target)
        {
            target->TakeDamage(damage);
            hitCount++;
        }
    }

    int GetDamage() const { return damage; }

private:
    int   damage;
    uint  creatorType;  // PLAYER ou ENEMY — quem criou o hitbox
    float timer;
    int   hitCount;
    int   maxHits;      // evita acertar o mesmo grupo várias vezes no mesmo swing
};

// -------------------------------------------------------------------------------

#endif
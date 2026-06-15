#ifndef ATTACKHITBOX_H
#define ATTACKHITBOX_H

#include "Object.h"
#include "GeoWars.h"
#include "IDamageable.h"

// -------------------------------------------------------------------------------

class AttackHitbox : public Object
{
public:
    AttackHitbox(float px, float py, int dmg, float duration = 0.12f)
        : damage(dmg), timer(duration), hitCount(0), maxHits(3)
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
        if (obj->Type() == ENEMY || obj->Type() == BOSS)
        {
            IDamageable* e = dynamic_cast<IDamageable*>(obj);
            if (e)
            {
                e->TakeDamage(damage);
                hitCount++;
            }
        }
    }

    int GetDamage() const { return damage; }

private:
    int   damage;
    float timer;
    int   hitCount;
    int   maxHits;    // evita acertar o mesmo grupo várias vezes no mesmo swing
};

// -------------------------------------------------------------------------------

#endif
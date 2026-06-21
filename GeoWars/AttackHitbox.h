#ifndef ATTACKHITBOX_H
#define ATTACKHITBOX_H

#include "Object.h"
#include "GeoWars.h"
#include "IDamageable.h"
#include <vector>
#include <algorithm>

// -------------------------------------------------------------------------------

class AttackHitbox : public Object
{
public:
    // 'owner' é quem disparou o golpe (player ou inimigo). Se informado, o
    // hitbox passa a seguir a posição de 'owner' durante toda sua duração,
    // mantendo o mesmo offset relativo de quando foi criado — assim ele não
    // fica "para trás" se o atacante continuar se movendo durante o swing.
    AttackHitbox(float px, float py, int dmg, uint creator, Object* owner = nullptr,
        float duration = 0.12f, int maxTargets = 3, float width = 48.0f, float height = 48.0f, float velX = 0.0f)
        : damage(dmg), creatorType(creator), timer(duration),
        maxTargets(maxTargets), markedForDelete(false),
        owner(owner),
        offsetX(owner ? px - owner->X() : 0.0f),
        offsetY(owner ? py - owner->Y() : 0.0f),
        velocityX(velX)
    {
        MoveTo(px, py);
        BBox(new Rect(-width / 2, -height / 2, width / 2, height / 2));
        type = ATTACK;
        hitTargets.reserve(maxTargets);
    }

    ~AttackHitbox() {}

    void Update() override
    {
        timer -= gameTime;

        if (owner)
        {
            MoveTo(owner->X() + offsetX, owner->Y() + offsetY);
        }
        else if (velocityX != 0.0f)
        {
            Translate(velocityX * gameTime, 0.0f);
        }

        if (!markedForDelete &&
            (timer <= 0 || (int)hitTargets.size() >= maxTargets))
        {
            markedForDelete = true;
            GeoWars::scene->Delete(this, MOVING);
        }
    }

    void Draw() override {}

    void OnCollision(Object* obj) override
    {
        // Já vai ser destruído ou já bateu no limite de alvos: ignora colisões extras
        // (importante caso o Delete da cena seja processado só no fim do frame)
        if (markedForDelete || (int)hitTargets.size() >= maxTargets)
            return;

        // Evita friendly-fire: não acerta objetos do mesmo "time" do criador
        if (obj->Type() == creatorType)
            return;

        // Evita acertar o mesmo alvo mais de uma vez dentro do mesmo swing,
        // mesmo que o hitbox continue sobrepondo o alvo por vários frames
        if (std::find(hitTargets.begin(), hitTargets.end(), obj) != hitTargets.end())
            return;

        IDamageable* target = dynamic_cast<IDamageable*>(obj);
        if (target)
        {
            target->TakeDamage(damage);
            hitTargets.push_back(obj);
        }
    }

    int GetDamage() const { return damage; }

private:
    int   damage;
    uint  creatorType;   // PLAYER ou ENEMY — quem criou o hitbox
    float timer;
    int   maxTargets;    // número máximo de alvos DISTINTOS atingidos no swing
    bool  markedForDelete;

    Object* owner;       // apenas para seguir a posição; nunca é deletado por aqui
    float   offsetX;
    float   offsetY;
    float velocityX;

    std::vector<Object*> hitTargets; // alvos já atingidos neste swing (não dereferenciados,
    // só usados para comparação de ponteiro)
};

// -------------------------------------------------------------------------------

#endif
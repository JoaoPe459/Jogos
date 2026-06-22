#ifndef GEOPICKUP_H
#define GEOPICKUP_H

#include "BaseEnemy.h"
#include "Sprite.h"
#include "Player.h"
#include "GeoWars.h"

class GeoPickup : public BaseEnemy
{
public:
    GeoPickup(float startX, float startY, int value)
        : BaseEnemy(1, 0),   // 1 hp (não vai morrer por dano), 0 geo drop
        value(value), bobTimer(0), lifetime(12.0f), collected(false)
    {
        sprite = new Sprite("Resources/Geo.png");
        BBox(new Circle(10.0f));
        MoveTo(startX, startY);
        type = PICKUP;
        hasGravity = true;
        hw = 10.0f; hh = 10.0f;

        // Impulso inicial para baixo com leve spread horizontal
        Random rndX{ -60.0f, 60.0f };
        speed->ScaleTo(0.0f);
        speed->Add(Vector(270.0f, 80.0f));      // cai para baixo
        speed->Add(Vector(0.0f, rndX.Rand()));  // leve desvio X

        hasGravity = true;
        state = ES_IDLE;   // evita lógica de ES_DEAD/ES_HURT do BaseEnemy
    }

    ~GeoPickup()
    {
        delete sprite;
    }

   

    void Draw() override
    {
        //float bobOffset = sinf(bobTimer * 3.0f) * 2.5f;
        //sprite->Draw(x, y + bobOffset, Layer::LOWER, 1.0f);
    }

    void OnCollision(Object* obj) override
    {
        if (obj->Type() == PLAYER && !collected)
        {
            collected = true;
            Player* p = dynamic_cast<Player*>(obj);
            if (p)
            {
                p->AddGeo(value);
                p->AddSoul(5);
            }
            GeoWars::audio->Play(GEO_SFX);
            GeoWars::scene->Delete(this, MOVING);
        }
        if (obj && obj->Type() == PLATFORM)
            obj->OnCollision(this);
    }

    // TakeDamage bloqueado — pickup não toma dano
    void TakeDamage(int) override {}

protected:
    // Exigidos pelo BaseEnemy mas não usados no pickup
    void UpdateAI(float) override {}
    void DrawSprite()    override {}

private:
    Sprite* sprite;
    int     value;
    float   bobTimer;
    float   lifetime;
    bool    collected;
};

#endif
/**********************************************************************************
// GeoPickup (Arquivo de Cabeçalho + Código Fonte)
//
// Descrição:   Moeda (Geo) dropada por inimigos ao morrer.
//              Tem física com gravidade e é coletada ao contato com o player.
//
**********************************************************************************/

#ifndef GEOPICKUP_H
#define GEOPICKUP_H

#include "Object.h"
#include "Sprite.h"
#include "Physics.h"
#include "Player.h"
#include "GeoWars.h"

// -------------------------------------------------------------------------------

class GeoPickup : public Object
{
public:
    GeoPickup(float startX, float startY, int value)
        : value(value), velX(0), velY(0),
        bobTimer(0), lifetime(12.0f),
        collected(false)
    {
        sprite = new Sprite("Resources/Geo.png");
        BBox(new Circle(10.0f));

        // Joga para cima com velocidade aleatória
        Random rndX{ -80.0f, 80.0f };
        Random rndY{ -200.0f, -120.0f };
        velX = rndX.Rand();
        velY = rndY.Rand();

        MoveTo(startX, startY);
        type = PICKUP;
    }

    ~GeoPickup()
    {
        delete sprite;
    }

    void Update() override
    {
        float dt = gameTime;
        lifetime -= dt;
        bobTimer += dt;

        if (lifetime <= 0 || collected)
        {
            GeoWars::scene->Delete(this, PICKUP);
            return;
        }

        // Gravidade
        velY += Physics::GRAVITY * dt;
        if (velY > Physics::MAX_FALL_SPEED) velY = Physics::MAX_FALL_SPEED;

        //x += velX * dt;
        //y += velY * dt;

        // Resolução simples de tiles
        /*if (Player::tilemap)
        {
            bool onG = false, onC = false, wL = false, wR = false;
            //Player::tilemap->ResolveAABB(x, y, 6.0f, 6.0f, velX, velY, onG, onC, wL, wR);
            if (onG)  velX *= 0.7f;     // fricção ao pousar
            if (wL || wR) velX = -velX * 0.4f;
        }*/
    }

    void Draw() override
    {
        // Leve flutuação senoidal quando parado
        float bobOffset = sinf(bobTimer * 3.0f) * 2.5f;
        sprite->Draw(x, y + bobOffset, Layer::LOWER, 1.0f);
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
                p->AddSoul(5);       // matar inimigos recarrega soul
            }
            GeoWars::audio->Play(GEO_SFX);
            GeoWars::scene->Delete(this, PICKUP);
        }
    }

private:
    Sprite* sprite;
    int     value;
    float   velX, velY;
    float   bobTimer;
    float   lifetime;
    bool    collected;
};

// -------------------------------------------------------------------------------

#endif

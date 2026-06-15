/**********************************************************************************
// EnemyProjectile (Arquivo de Cabeçalho + Código Fonte)
//
// Descrição:   Projétil disparado por inimigos.
//              Causa dano ao player no contato.
//
**********************************************************************************/

#ifndef ENEMYPROJECTILE_H
#define ENEMYPROJECTILE_H

#include "Object.h"
#include "Sprite.h"
#include "Particles.h"
#include "Vector.h"
#include "Player.h"
#include "GeoWars.h"

// -------------------------------------------------------------------------------

class EnemyProjectile : public Object
{
public:
    EnemyProjectile(float startX, float startY, float angle, float magnitude)
        : lifetime(3.0f)
    {
        speed = new Vector(angle, magnitude);

        sprite = new Sprite("Resources/EnemyBullet.png");
        BBox(new Circle(6.0f));
        MoveTo(startX, startY);
        type = PROJECTILE;

        Generator trail;
        trail.imgFile = "Resources/Spark.png";
        trail.angle = 0.0f;
        trail.spread = 20.0f;
        trail.lifetime = 0.15f;
        trail.frequency = 0.005f;
        trail.percentToDim = 0.4f;
        trail.minSpeed = 10.0f;
        trail.maxSpeed = 50.0f;
        trail.color = { 0.2f, 0.8f, 0.3f, 1.0f };
        trailParticles = new Particles(trail);
    }

    ~EnemyProjectile()
    {
        delete speed;
        delete sprite;
        delete trailParticles;
    }

    void Update() override
    {
        float dt = gameTime;
        lifetime -= dt;

        if (lifetime <= 0)
        {
            GeoWars::scene->Remove();
            return;
        }

        // Translada pelo vetor velocidade
        Translate(speed->XComponent() * dt,
            -speed->YComponent() * dt);

        // Remove ao sair dos limites do mapa
        if (x < 0 || x > 3840.0f || y < 0 || y > 2160.0f)
        {
            GeoWars::scene->Delete();
            return;
        }

        // Trail aponta no sentido oposto ao movimento
        trailParticles->Config().angle = speed->Angle() + 180.0f;
        trailParticles->Generate(x, y);
        trailParticles->Update(dt);
    }

    void Draw() override
    {
        sprite->Draw(x, y, Layer::MIDDLE, 1.0f, -speed->Angle() + 90.0f);
        trailParticles->Draw(Layer::LOWER, 1.0f);
    }

    void OnCollision(Object* obj) override
    {
        if (obj->Type() == PLAYER)
        {
            Player* p = dynamic_cast<Player*>(obj);
            if (p && !p->IsDashing())
            {
                p->TakeDamage(1);
                GeoWars::scene->Delete();
            }
        }
    }

private:
    Sprite* sprite;
    Particles* trailParticles;
    Vector* speed;
    float      lifetime;
};

// -------------------------------------------------------------------------------

#endif
/**********************************************************************************
// BaseEnemy (Arquivo de Cabe�alho)
//
// Descri��o:   Classe base para todos os inimigos.
//              Herda de Object e implementa IDamageable.
//              Cont�m f�sica com gravidade, sistema de HP, knockback,
//              drop de geo e integra��o com tilemap.
//
**********************************************************************************/

#ifndef BASEENEMY_H
#define BASEENEMY_H

#include "Object.h"
#include "IDamageable.h"
#include "Particles.h"
//#include "TileMap.h"
#include "Physics.h"

// -------------------------------------------------------------------------------

// Estados gen�ricos dos inimigos
enum EnemyState
{
    ES_IDLE,
    ES_PATROL,
    ES_CHASE,
    ES_ATTACK,
    ES_HURT,
    ES_DEAD,
    ES_ALERT,    // viu o player, vai come�ar a perseguir
};

// -------------------------------------------------------------------------------

class BaseEnemy : public Object, public IDamageable
{
public:
    BaseEnemy(int maxHp, int geoDrop);
    virtual ~BaseEnemy();

    // Object
    void Update()   override;
    void Draw()     override;
    void OnCollision(Object* obj) override;

    // IDamageable
    void TakeDamage(int dmg) override;
    bool IsAlive()    const  override { return hp > 0; }

    int  GetHP()      const { return hp; }
    int  GetMaxHP()   const { return maxHp; }

protected:
    // ---- subclasses implementam AI ----
    virtual void UpdateAI(float dt) = 0;
    virtual void DrawSprite() = 0;

    // ---- f�sica ----
    float    velX, velY;
    bool     onGround;
    bool     facingRight;
    void     ApplyGravity(float dt);
    void     ResolveTiles();

    // ---- estado ----
    EnemyState state;
    float      hurtTimer;
    float      deadTimer;      // espera antes de remover da cena
    float      alertTimer;

    // ---- stats ----
    int        hp, maxHp;
    int        geoDrop;        // geo que cai ao morrer

    // ---- half-extents AABB ----
    float      hw, hh;

    // ---- part�culas ----
    Particles* deathParticles;
    Particles* hurtParticles;

    // ---- helper: dist�ncia ao player ----
    float DistToPlayer() const;
    float AngleToPlayer() const;
    bool  PlayerInRange(float range) const;
    bool  PlayerInSight(float range) const;  // verifica linha de visada

private:
    void SpawnGeoPickup();
};

// -------------------------------------------------------------------------------

#endif

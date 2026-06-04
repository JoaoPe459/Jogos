/**********************************************************************************
// IDamageable (Interface)
//
// Descri��o:   Interface para objetos que podem receber dano
//
**********************************************************************************/

#ifndef IDAMAGEABLE_H
#define IDAMAGEABLE_H

// -------------------------------------------------------------------------------

class IDamageable
{
public:
    virtual ~IDamageable() {}
    virtual void TakeDamage(int dmg) = 0;
    virtual bool IsAlive() const = 0;
};

// -------------------------------------------------------------------------------

// Tipos de objeto (estende a enum existente em Types.h)
enum ObjectTypes
{
    // tipos originais do GeoWars
    PLAYER = 1,
    MISSILE = 2,
    MAGENTA = 3,
    BLUE = 4,
    GREEN = 5,
    ORANGE = 6,
    WALLHIT = 7,

    // novos tipos SilkSong
    ATTACK = 10,   // hitbox do ataque do player
    ENEMY = 11,   // inimigo gen�rico
    BOSS = 12,   // chefe
    PICKUP = 13,   // item no ch�o (geo, soul, vida)
    PLATFORM = 14,   // plataforma m�vel
    PROJECTILE = 15, // proj�til de inimigo
};

// -------------------------------------------------------------------------------

// IDs de sons (estende os do GeoWars)
enum SoundID
{
    // originais
    THEME = 0,
    FIRE = 1,
    HITWALL = 2,
    EXPLODE = 3,
    START = 4,

    // novos
    JUMP_SFX = 5,
    LAND_SFX = 6,
    DASH_SFX = 7,
    ATTACK_SFX = 8,
    HURT_SFX = 9,
    DEATH_SFX = 10,
    HEAL_SFX = 11,
    GEO_SFX = 12,
    BOSS_SFX = 13,
};

// -------------------------------------------------------------------------------

#endif

/**********************************************************************************
// Player (Arquivo de Cabeçalho)
// 
// Criação:     10 Out 2012
// Atualização: 01 Nov 2021
// Compilador:  Visual C++ 2022
//
// Descrição:   Define a classe jogador
//
**********************************************************************************/

#ifndef _GEOWARS_PLAYER_H_
#define _GEOWARS_PLAYER_H_

// ---------------------------------------------------------------------------------

#include "Object.h"                        // objetos do jogo
#include "Sprite.h"                        // desenho de sprites
#include "Animation.h"
#include "Vector.h"                        // representação de vetores
#include "Particles.h"                    // sistema de partículas
#include "IDamageable.h"                    // interface de objetos que podem receber dano
// ---------------------------------------------------------------------------------

enum PlayerState
{
    PS_IDLE,
    PS_RUNNING,
    PS_JUMPING,
    PS_FALLING,
    PS_DASHING,
    PS_WALL_SLIDING,
    PS_ATTACKING,
    PS_HURT,
    PS_DEAD,
};

class Player : public Object, public IDamageable
{
private:
    TileSet * attack;
    TileSet * damage;
    TileSet * death;
    TileSet * idle;
    TileSet * jump;
    TileSet * turn;
    TileSet * walk;

    Animation* animAttack;
    Animation* animDamage;
    Animation* animDeath;
    Animation* animIdle;
    Animation* animJump;
    Animation* animTurn;
    Animation* animWalk;
    Animation* prevAnim;
    Animation* anim;
    int animState = 0;
    bool animStart;
    float walkStartTimer;
    bool prevFacingRight;
    bool skipStartAnim;
    bool jumpLoopStarted;
    float jumpStartTimer;

    Particles * tail;                   // calda do jogador

    Particles* dustParticles;   // p� ao correr / pousar
    Particles* dashParticles;   // rastro do dash
    Particles* hitParticles;    // impacto do ataque

    Particles* ambientParticles; // Partículas flutuantes ao redor
    float      ambientSpawnTimer; // Controla a frequência de geração

    // ---- estado de f�sica ----
    PlayerState state;
    bool        facingRight;
    bool        onGround;
    bool        onCeiling;
    bool        onWallLeft;
    bool        onWallRight;

    // ---- timers e flags de pulo ----
    int         jumpsLeft;          // pulos dispon�veis (2 = duplo)
    float       coyoteTimer;        // coyote time
    float       jumpBufferTimer;    // buffer de pulo antecipado
    bool        jumpHeld;

    // ---- dash ----
    float       dashTimer;
    float       dashCooldown;
    float       dashDirX;
    bool        canDash;

    // ---- ataque ----
    float       attackTimer;
    float       attackCooldown;
    int         attackCombo;        // combo 0-2
    float       comboResetTimer;

    // ---- hurt / invencibilidade ----
    float       hurtTimer;          // dura��o do estado hurt
    float       invincibleTimer;    // i-frames ap�s dano
    bool        invincible;

    // ---- stats ----
    int         hp, maxHp;
    int         soul;               // recurso de cura (0-99)
    int         geo;                // moeda
    float       healTimer;         // tempo de anima��o de cura

    // ---- helpers ----
    void HandleInput(float dt);
    void ApplyGravity(float dt);
    void ResolveTiles();
    void UpdateState();
    void UpdateParticles(float dt);
    void SpawnAttackHitbox();

    static constexpr float HW = 14.0f;   // meia-largura
    static constexpr float HH = 22.0f;   // meia-altura
public:
    float velX, velY;
    Vector* speed;                    // velocidade e direção

    Player();                           // construtor
    ~Player();                          // destrutor
    
    void Move(Vector && v);             // movimenta jogador
    void Update();                      // atualização
    void Draw();                        // desenho
   
    void OnCollision(Object* obj);

    void TakeDamage(int dmg);
    void Heal(int amount);
    bool IsAlive()      const { return hp > 0; }
    bool IsDashing()    const { return state == PS_DASHING; }
    bool IsAttacking()  const { return state == PS_ATTACKING; }
    int  GetHP()        const { return hp; }
    int  GetMaxHP()     const { return maxHp; }
    int  GetSoul()      const { return soul; }   // recurso de cura
    int  GetGeo()       const { return geo; }    // moeda
    void AddGeo(int g) { geo += g; }
    void AddSoul(int s) { soul = min(soul + s, 99); }


}; 
// ---------------------------------------------------------------------------------

#endif
/**********************************************************************************
// Player (Código Fonte)
//
// Descrição:   Jogador com mecânicas completas de plataformer (sistema vetorial)
//
**********************************************************************************/

#include "Player.h"
#include "IDamageable.h"
#include "GeoWars.h"
#include "Physics.h"
#include "AttackHitbox.h"
#include <algorithm>

using std::min;
using std::max;
using std::clamp;

// -------------------------------------------------------------------------------


// -------------------------------------------------------------------------------

Player::Player()
    : speed(nullptr),
    state(PS_IDLE), facingRight(true),
    onGround(false), onCeiling(false), onWallLeft(false), onWallRight(false),
    jumpsLeft(2), coyoteTimer(0), jumpBufferTimer(0), jumpHeld(false),
    dashTimer(0), dashCooldown(0), dashDirX(1), canDash(true),
    attackTimer(0.50f), attackCooldown(0), attackCombo(0), comboResetTimer(0),
    hurtTimer(0), invincibleTimer(0), invincible(false),
    hp(5), maxHp(5), soul(0), geo(0), healTimer(0)
{
    // carrega sprites
    sprite = new Sprite("Resources/Player.png");

    // vetor velocidade — começa parado apontando para cima
    speed = new Vector(90.0f, 0.0f);

    // AABB como bounding box
    BBox(new Circle(32.0f));

    // posição inicial: ponto de spawn do mapa
    MoveTo(window->CenterX(), window->CenterY());

    type = PLAYER;

    // ---- partículas de pó (corre / pousa) ----
    Generator dust;
    dust.imgFile = "Resources/Dust.png";
    dust.angle = 90.0f;
    dust.spread = 40.0f;
    dust.lifetime = 0.25f;
    dust.frequency = 0.0f;
    dust.percentToDim = 0.5f;
    dust.minSpeed = 20.0f;
    dust.maxSpeed = 60.0f;
    dust.color = { 0.8f, 0.7f, 0.6f, 0.8f };
    dustParticles = new Particles(dust);

    // ---- partículas do dash ----
    Generator dash;
    dash.imgFile = "Resources/Spark.png";
    dash.angle = 0.0f;
    dash.spread = 15.0f;
    dash.lifetime = 0.15f;
    dash.frequency = 0.005f;
    dash.percentToDim = 0.4f;
    dash.minSpeed = 30.0f;
    dash.maxSpeed = 120.0f;
    dash.color = { 0.4f, 0.8f, 1.0f, 1.0f };
    dashParticles = new Particles(dash);

    // ---- partículas de impacto ----
    Generator hit;
    hit.imgFile = "Resources/Spark.png";
    hit.angle = 0.0f;
    hit.spread = 360.0f;
    hit.lifetime = 0.2f;
    hit.frequency = 0.0f;
    hit.percentToDim = 0.5f;
    hit.minSpeed = 60.0f;
    hit.maxSpeed = 200.0f;
    hit.color = { 1.0f, 0.9f, 0.3f, 1.0f };
    hitParticles = new Particles(hit);
}

// -------------------------------------------------------------------------------

Player::~Player()
{
    delete sprite;
    delete speed;
    delete dustParticles; delete dashParticles; delete hitParticles;
}

// -------------------------------------------------------------------------------

// Adiciona ao vetor velocidade e limita à magnitude máxima
void Player::Move(Vector&& v)
{
    speed->Add(v);

    if (speed->Magnitude() > Physics::MAX_SPEED)
        speed->ScaleTo(Physics::MAX_SPEED);
}

// -------------------------------------------------------------------------------

void Player::Update()
{
    float dt = gameTime;

    // Decrementar timers
    if (dashCooldown > 0) dashCooldown -= dt;
    if (attackCooldown > 0) attackCooldown -= dt;
    if (comboResetTimer > 0) comboResetTimer -= dt; else attackCombo = 0;
    if (hurtTimer > 0) hurtTimer -= dt;
    if (invincibleTimer > 0) invincibleTimer -= dt; else invincible = false;
    if (healTimer > 0) healTimer -= dt;
    if (coyoteTimer > 0) coyoteTimer -= dt;
    if (jumpBufferTimer > 0) jumpBufferTimer -= dt;
    if (dashTimer > 0) dashTimer -= dt;
    if (attackTimer > 0) attackTimer -= dt;

    // Morreu?
    if (!IsAlive())
    {
        state = PS_DEAD;
        // Respawn após 2s (gerenciado pelo SilkSong)
        return;
    }

    // Hurt: não aceita input, apenas física passiva
    if (state == PS_HURT && hurtTimer > 0)
    {
        ApplyGravity(dt);
        UpdateParticles(dt);
        return;
    }

    // Dash em andamento: impulso vetorial horizontal puro, sem gravidade
    if (state == PS_DASHING && dashTimer > 0)
    {
        // Zera vetor e reaplica apenas componente horizontal do dash
        speed->ScaleTo(0.0f);
        Move(Vector(facingRight ? 0.0f : 180.0f, Physics::DASH_SPEED * dt));

        dashParticles->Config().angle = facingRight ? 180.0f : 0.0f;
        dashParticles->Generate(x, y);
        dashParticles->Update(dt);
        return;
    }
    else if (state == PS_DASHING)
    {
        // Dash terminou: amortecer para metade da velocidade atual
        speed->ScaleTo(speed->Magnitude() * 0.5f);
        state = PS_FALLING;
    }

    HandleInput(dt);
    ApplyGravity(dt);
    UpdateState();
    UpdateParticles(dt);

    if (x < 50)
        MoveTo(50, y);
    if (y < 50)
        MoveTo(x, 50);
    if (x > game->Width() - 50)
        MoveTo(game->Width() - 50, y);
    if (y > game->Height() - 50)
        MoveTo(x, game->Height() - 50);
}

// -------------------------------------------------------------------------------

void Player::HandleInput(float dt)
{
    // ---- Movimento horizontal ----
    bool right = window->KeyDown(VK_RIGHT) || window->KeyDown('D');
    bool left = window->KeyDown(VK_LEFT) || window->KeyDown('A');

    // Calcula aceleração horizontal desejada
    float targetMagX = 0.0f;
    if (right) { targetMagX = Physics::MOVE_SPEED; facingRight = true; }
    if (left) { targetMagX = -Physics::MOVE_SPEED; facingRight = false; }

    // Componente horizontal atual do vetor velocidade
    float curVX = speed->XComponent();

    float newVX;
    if (onGround)
        newVX = targetMagX;                              // resposta imediata no chão
    else
        newVX = curVX + (targetMagX - curVX) * 8.0f * dt; // lerp suave no ar

    // Freia gradualmente se nenhuma tecla horizontal pressionada
    if (!right && !left)
    {
        if (speed->Magnitude() > 0.1f)
            Move(Vector(speed->Angle() + 180.0f, Physics::FRICTION * dt));
        else
            speed->ScaleTo(0.0f);
    }
    else
    {
        // Substitui componente X mantendo componente Y intacto
        float curVY = speed->YComponent();
        speed->ScaleTo(0.0f);
        // Reconstrói vetor com novo X e Y preservado
        if (newVX != 0.0f || curVY != 0.0f)
        {
            float angle = atan2f(curVY, newVX) * (180.0f / 3.14159f);
            float mag = sqrtf(newVX * newVX + curVY * curVY);
            Move(Vector(angle, mag));
        }
    }

    // Aplica translação pelo vetor velocidade
    Translate(speed->XComponent() * dt,
        -speed->YComponent() * dt);

    // ---- Pulo ----
    bool jumpPressed = window->KeyPress(VK_UP) || window->KeyPress('W') ||
        window->KeyPress(VK_SPACE) || window->KeyPress('Z');
    bool jumpDown = window->KeyDown(VK_UP) || window->KeyDown('W') ||
        window->KeyDown(VK_SPACE) || window->KeyDown('Z');

    if (jumpPressed)
        jumpBufferTimer = Physics::JUMP_BUFFER;

    // Pulo variável: soltar cedo encurta o pulo (atenua componente Y)
    if (!jumpDown && speed->YComponent() > 0 && state == PS_JUMPING)
        speed->ScaleTo(speed->Magnitude() * 0.85f);

    // Executa pulo se buffer ativo e condições ok
    if (jumpBufferTimer > 0)
    {
        bool canJump = (onGround || coyoteTimer > 0 || jumpsLeft > 0);
        if (canJump)
        {
            jumpBufferTimer = 0;
            coyoteTimer = 0;
            bool isDouble = (!onGround && coyoteTimer <= 0);
            float jumpForce = isDouble ? Physics::DOUBLE_JUMP : Physics::JUMP_FORCE;

            // Injeta impulso vetorial para cima (90°)
            Move(Vector(90.0f, jumpForce));

            if (jumpsLeft > 0) jumpsLeft--;
            onGround = true;
            state = PS_JUMPING;

            dustParticles->Generate(x, y + HH);
            GeoWars::audio->Play(JUMP_SFX);
        }
    }

    // ---- Wall-slide ----
    if (!onGround && (onWallLeft || onWallRight))
    {
        // Limita descida ao máximo de wall-slide
        // YComponent positivo = descendo (eixo Y invertido no motor)
        if (-speed->YComponent() > Physics::WALL_SLIDE_SPD)
        {
            // Reconstrói vetor com Y limitado
            float curVX2 = speed->XComponent();
            speed->ScaleTo(0.0f);
            float angle = atan2f(-Physics::WALL_SLIDE_SPD, curVX2) * (180.0f / 3.14159f);
            float mag = sqrtf(curVX2 * curVX2 + Physics::WALL_SLIDE_SPD * Physics::WALL_SLIDE_SPD);
            Move(Vector(angle, mag));
            state = PS_WALL_SLIDING;
        }

        // Wall-jump: impulso diagonal oposto à parede
        if (jumpPressed)
        {
            float wallJumpAngle = onWallLeft ? 45.0f : 135.0f;
            speed->ScaleTo(0.0f);
            Move(Vector(wallJumpAngle, Physics::JUMP_FORCE * 0.9f));
            jumpsLeft = 1;
            jumpBufferTimer = 0;
            state = PS_JUMPING;
            GeoWars::audio->Play(JUMP_SFX);
        }
    }

    // ---- Dash ----
    bool dashPressed = window->KeyPress(VK_SHIFT) || window->KeyPress('X');
    if (dashPressed && canDash && dashCooldown <= 0 && state != PS_HURT)
    {
        float dashAngle = facingRight ? 0.0f : 180.0f;
        speed->ScaleTo(0.0f);
        Move(Vector(dashAngle, Physics::DASH_SPEED));

        dashTimer = Physics::DASH_DURATION;
        dashCooldown = Physics::DASH_COOLDOWN;
        dashDirX = facingRight ? 1.0f : -1.0f;
        canDash = false;
        state = PS_DASHING;
        GeoWars::audio->Play(DASH_SFX);
    }

    // ---- Ataque ----
    bool attackPressed = window->KeyPress('C') || window->KeyPress(VK_LCONTROL);
    if (attackPressed && attackCooldown <= 0 && state != PS_HURT && state != PS_DASHING)
    {
        attackTimer = 0.25f;
        attackCooldown = 0.35f;
        comboResetTimer = 0.55f;
        attackCombo = (attackCombo + 1) % 3;
        state = PS_ATTACKING;
        SpawnAttackHitbox();
        GeoWars::audio->Play(ATTACK_SFX);
    }

    // ---- Cura (usa soul) ----
    bool healPressed = window->KeyPress('F') || window->KeyPress(VK_RETURN);
    if (healPressed && soul >= 33 && hp < maxHp && onGround && state == PS_IDLE)
    {
        soul -= 33;
        hp = min(hp + 1, maxHp);
        healTimer = 0.6f;
        hitParticles->Generate(x, y);
        GeoWars::audio->Play(HEAL_SFX);
    }
}

// -------------------------------------------------------------------------------

void Player::ApplyGravity(float dt)
{
    if (state == PS_DASHING) return;

    // Gravidade: subtrai do componente Y (Y positivo = cima no sistema vetorial)
    Move(Vector(270.0f, Physics::GRAVITY * dt));

    // Limita velocidade de queda (componente Y negativo = descendo)
    if (-speed->YComponent() > Physics::MAX_FALL_SPEED)
    {
        float curVX = speed->XComponent();
        speed->ScaleTo(0.0f);
        float angle = atan2f(-Physics::MAX_FALL_SPEED, curVX) * (180.0f / 3.14159f);
        float mag = sqrtf(curVX * curVX + Physics::MAX_FALL_SPEED * Physics::MAX_FALL_SPEED);
        Move(Vector(angle, mag));
    }
}

void Player::ResolveTiles()
{
    const float MAP_W = 3840.0f;
    const float MAP_H = 2160.0f;

    bool prevGround = onGround;

    // Bordas horizontais
    if (x - HW < 0)
    {
        MoveTo(HW, y);
        float curVY = speed->YComponent();
        speed->ScaleTo(0.0f);
        if (curVY != 0.0f)
            Move(Vector(curVY > 0 ? 90.0f : 270.0f, fabsf(curVY)));
        onWallLeft = true;
    }
    else if (x + HW > MAP_W)
    {
        MoveTo(MAP_W - HW, y);
        float curVY = speed->YComponent();
        speed->ScaleTo(0.0f);
        if (curVY != 0.0f)
            Move(Vector(curVY > 0 ? 90.0f : 270.0f, fabsf(curVY)));
        onWallRight = true;
    }
    else
    {
        onWallLeft = false;
        onWallRight = false;
    }

    // Bordas verticais
    if (y - HH < 0)
    {
        MoveTo(x, HH);
        onCeiling = true;
        float curVX = speed->XComponent();
        speed->ScaleTo(0.0f);
        if (curVX != 0.0f)
            Move(Vector(curVX > 0 ? 0.0f : 180.0f, fabsf(curVX)));
    }
    else if (y + HH >= MAP_H)
    {
        MoveTo(x, MAP_H - HH);
        onGround = true;
        onCeiling = false;
        float curVX = speed->XComponent();
        speed->ScaleTo(0.0f);
        if (curVX != 0.0f)
            Move(Vector(curVX > 0 ? 0.0f : 180.0f, fabsf(curVX)));
    }
    else
    {
        onGround = false;
        onCeiling = false;
    }

    // Pousou no chão
    if (!prevGround && onGround)
    {
        jumpsLeft = 2;
        canDash = true;
        coyoteTimer = 0;
        dustParticles->Generate(x, y + HH, 12);
        GeoWars::audio->Play(LAND_SFX);
    }

    // Iniciou queda livre → coyote time
    if (prevGround && !onGround)
        coyoteTimer = Physics::COYOTE_TIME;
}

// -------------------------------------------------------------------------------



// -------------------------------------------------------------------------------

void Player::UpdateState()
{
    if (state == PS_DASHING)                   return;
    if (state == PS_HURT && hurtTimer > 0)     return;
    if (attackTimer > 0) { state = PS_ATTACKING; return; }

    if (!onGround)
    {
        if (onWallLeft || onWallRight)
            state = PS_WALL_SLIDING;
        else if (speed->YComponent() > 0)   // Y positivo = subindo
            state = PS_JUMPING;
        else
            state = PS_FALLING;
    }
    else
    {
        // Velocidade horizontal significativa = correndo
        state = (fabsf(speed->XComponent()) > 10.0f) ? PS_RUNNING : PS_IDLE;
    }
}

// -------------------------------------------------------------------------------

void Player::UpdateParticles(float dt)
{
    if (state == PS_RUNNING && onGround)
        dustParticles->Generate(x, y + HH);

    dustParticles->Update(dt);
    dashParticles->Update(dt);
    hitParticles->Update(dt);
}

// -------------------------------------------------------------------------------

void Player::SpawnAttackHitbox()
{
    float ox = facingRight ? HW + 16.0f : -(HW + 16.0f);
    float oy = 0;
    int   dmg = (attackCombo == 2) ? 2 : 1;

    GeoWars::scene->Add(new AttackHitbox(x + ox, y + oy, dmg, type), STATIC);
    hitParticles->Generate(x + ox, y + oy, 8);
}

// -------------------------------------------------------------------------------

void Player::TakeDamage(int dmg)
{
    if (invincible || !IsAlive()) return;

    hp = max(hp - dmg, 0);

    if (hp <= 0)
    {
        state = PS_DEAD;
        GeoWars::audio->Play(DEATH_SFX);
        return;
    }

    // Knockback vetorial oposto à direção do player
    speed->ScaleTo(0.0f);
    Move(Vector(facingRight ? 135.0f : 45.0f, 250.0f)); // diagonal para cima e para trás

    state = PS_HURT;
    hurtTimer = 0.5f;
    invincible = true;
    invincibleTimer = 1.5f;

    GeoWars::audio->Play(HURT_SFX);
}

// -------------------------------------------------------------------------------

void Player::Heal(int amount)
{
    hp = min(hp + amount, maxHp);
}

// -------------------------------------------------------------------------------

void Player::OnCollision(Object* obj)
{
    if (obj && obj->Type() == PLATFORM)
        obj->OnCollision(this);  // delega para a Platform resolver
}

// -------------------------------------------------------------------------------

void Player::Draw()
{
    // Pisca durante invencibilidade
    if (invincible && (int)(invincibleTimer * 10) % 2 == 0) return;

    
    if (healTimer > 0)
        hitParticles->Draw(Layer::MIDDLE, 1.0f);

    dustParticles->Draw(Layer::LOWER, 1.0f);
    dashParticles->Draw(Layer::MIDDLE, 0.8f);
    hitParticles->Draw(Layer::UPPER, 1.0f);
}

// -------------------------------------------------------------------------------
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
    attack = new TileSet("Resources/Player/attack.png", 136, 138, 5, 10);
    damage = new TileSet("Resources/Player/damage.png", 124, 138, 6, 12);
    death = new TileSet("Resources/Player/death.png", 116, 138, 18, 36);
    idle = new TileSet("Resources/Player/idle.png", 61, 138, 9, 18);
    jump = new TileSet("Resources/Player/jump.png", 103, 138, 12, 24);
    turn = new TileSet("Resources/Player/turn.png", 72, 138, 2, 4);
    walk = new TileSet("Resources/Player/walk.png", 86, 138, 13, 26);

    animAttack = new Animation(attack, 0.06f, true);
    animDamage = new Animation(damage, 0.1f, false);
    animDeath = new Animation(death, 0.06f, false);
    animIdle = new Animation(idle, 0.06f, true);
    animJump = new Animation(jump, 0.06f, true);
    animTurn = new Animation(turn, 0.06f, false);
    animWalk = new Animation(walk, 0.06f, true);

    uint SeqAttackleft[5] = { 0,1,2,3,4 };
    uint SeqAttackright[5] = { 9,8,7,6,5 };
    animAttack->Add(0, SeqAttackleft, 5);
    animAttack->Add(1, SeqAttackright, 5);

    uint SeqIdleleft[9] = { 0,1,2,3,4,5,6,7,8 };
    uint SeqIdleright[9] = { 17,16,15,14,13,12,11,10,9 };
    animIdle->Add(0, SeqIdleleft, 9);
    animIdle->Add(1, SeqIdleright, 9);

    uint SeqDamageleft[6] = { 0,1,2,3,4,5 };
    uint SeqDamageright[6] = { 11,10,9,8,7,6 };
    animDamage->Add(0, SeqDamageleft, 6);
    animDamage->Add(1, SeqDamageright, 6);

    uint SeqDeathleft[18] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17 };
    uint SeqDeathright[18] = { 35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18 };
    animDeath->Add(0, SeqDeathleft, 18);
    animDeath->Add(1, SeqDeathright, 18);

    uint SeqJumpleft[12] = { 0,1,2,3,4,5,6,7,8,9,10,11 };
    uint SeqJumpright[12] = { 23,22,21,20,19,18,17,16,15,14,13,12 };
    animJump->Add(0, SeqJumpleft, 12);
    animJump->Add(1, SeqJumpright, 12);
    uint SeqJumpLoopleft[3] = { 9,10,11 };
    uint SeqJumpLoopright[3] = { 14,13,12 };
    animJump->Add(2, SeqJumpLoopleft, 3);
    animJump->Add(3, SeqJumpLoopright, 3);

    uint SeqTurnleft[2] = { 0,1 };
    uint SeqTurnright[2] = { 3,2 };
    animTurn->Add(0, SeqTurnleft, 2);
    animTurn->Add(1, SeqTurnright, 2);

    uint SeqWalkleft[13] = { 0,1,2,3,4,5,6,7,8,9,10,11,12 };
    uint SeqWalkright[13] = { 25,24,23,22,21,20,19,18,17,16,15,14,13 };
    animWalk->Add(0, SeqWalkleft, 13);
    animWalk->Add(1, SeqWalkright, 13);
    uint SeqWalkCycleleft[9] = { 4,5,6,7,8,9,10,11,12 };
    uint SeqWalkCycleright[9] = { 21,20,19,18,17,16,15,14,13 };
    animWalk->Add(2, SeqWalkCycleleft, 9);
    animWalk->Add(3, SeqWalkCycleright, 9);

    anim = animIdle;
    animState = 0;
    anim->Select(animState);


    // vetor velocidade — começa parado apontando para cima
    speed = new Vector(90.0f, 0.0f);

    // AABB como bounding box
    BBox(new Rect(-27.0f, -61.0f, 27.0f, 61.0f));

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

    Generator ambient;
    ambient.imgFile = "Resources/Spark.png"; // Troque para um png de poeira se preferir
    ambient.angle = 0.0f;
    ambient.spread = 360.0f;       // Vai para qualquer direção aleatória
    ambient.lifetime = 4.0f;       // Duram bastante tempo na tela
    ambient.frequency = 0.0f;      // Deixe 0, vamos forçar a geração manualmente
    ambient.percentToDim = 0.8f;
    ambient.minSpeed = 5.0f;       // Movimentação bem lenta
    ambient.maxSpeed = 20.0f;
    ambient.color = { 0.6f, 0.8f, 0.6f, 0.4f }; // Cor suave e um pouco transparente

    ambientParticles = new Particles(ambient);
    ambientSpawnTimer = 0.0f;
}

// -------------------------------------------------------------------------------

Player::~Player()
{
    delete attack;
    delete damage;
    delete death;
    delete idle;
    delete jump;
    delete turn;
    delete walk;

    delete animAttack;
    delete animDamage;
    delete animDeath;
    delete animIdle;
    delete animJump;
    delete animTurn;
    delete animWalk;

    delete speed;
    delete dustParticles; delete dashParticles; delete hitParticles;
    delete ambientParticles;
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
        Translate(speed->XComponent() * dt, -speed->YComponent() * dt);
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
            Move(Vector(speed->Angle() + 280.0f, Physics::FRICTION * dt));
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
    prevAnim = anim;

    if (state == PS_DASHING)
    {
        anim = animWalk;
        animState = facingRight ? 1 : 0;
        anim->Select(animState);
        return;
    }
    if (state == PS_HURT && hurtTimer > 0)
    {
        anim = animDamage;
        animState = facingRight ? 1 : 0;
        anim->Select(animState);
        return;
    }
    if (attackTimer > 0)
    {
        state = PS_ATTACKING;
        anim = animAttack;
        animState = facingRight ? 1 : 0;
        anim->Select(animState);
        return;
    }

    if (!onGround)
    {
        if (onWallLeft || onWallRight)
        {
            state = PS_WALL_SLIDING;
            anim = animJump;
            animState = facingRight ? 1 : 0;
            anim->Select(animState);
        }
        else
        {
            state = (speed->YComponent() > 0) ? PS_JUMPING : PS_FALLING;
            anim = animJump;

            if (prevAnim != animJump)
            {
                // começou a pular agora
                jumpLoopStarted = false;
                jumpStartTimer = 0.0f;
                animState = facingRight ? 1 : 0;
                anim->Select(animState);
                anim->Restart();
            }
            else if (!jumpLoopStarted)
            {
                jumpStartTimer += gameTime;
                if (jumpStartTimer >= 9 * 0.06f)
                {
                    jumpLoopStarted = true;
                    animState = facingRight ? 3 : 2;
                    anim->Select(animState);
                }
            }
            if (facingRight != prevFacingRight)
            {
                animState = jumpLoopStarted ? (facingRight ? 3 : 2)
                    : (facingRight ? 1 : 0);
                anim->Select(animState);
            }
        }

        if (onGround)
        {
            jumpLoopStarted = false;
            jumpStartTimer = 0.0f;
        }
    }
    else
    {
        if (fabsf(speed->XComponent()) > 10.0f)
        {
            state = PS_RUNNING;

            // mudou de direção enquanto andava
            if (prevAnim == animWalk && facingRight != prevFacingRight)
            {
                anim = animTurn;
                animState = facingRight ? 1 : 0;
                anim->Select(animState);
                anim->Restart();
                animStart = false;
                walkStartTimer = 0.0f;
            }
            // turn terminou: inicia arrancada na nova direção
            else if (anim == animTurn && animTurn->Inactive())
            {
                anim = animWalk;
                animStart = true;
                skipStartAnim = true;
                animState = facingRight ? 3 : 2;
                anim->Select(animState);
            }
            else if (anim != animTurn) // só atualiza walk se não estiver no turn
            {
                anim = animWalk;

                if (prevAnim != animWalk)
                {
                    animStart = false;
                    walkStartTimer = 0.0f;
                    animState = facingRight ? 1 : 0;
                    anim->Select(animState);
                    anim->Restart();
                }
                else if (!animStart)
                {
                    walkStartTimer += gameTime;
                    if (walkStartTimer >= 13 * 0.06f)
                    {
                        animStart = true;
                        animState = facingRight ? 3 : 2;
                        anim->Select(animState);
                    }
                }
            }
        }
        else
        {
            state = PS_IDLE;
            anim = animIdle;
            animStart = false;
            animState = facingRight ? 1 : 0;
            anim->Select(animState);
        }
    }
    prevFacingRight = facingRight;
}

// -------------------------------------------------------------------------------

void Player::UpdateParticles(float dt)
{
    if (state == PS_RUNNING && onGround)
        dustParticles->Generate(x, y + HH);

    // --- Spawn de Partículas Ambiente ---
    ambientSpawnTimer += dt;

    if (ambientSpawnTimer > 0.05f)
    {
        ambientSpawnTimer = 0.0f;

        // Gera um ângulo aleatório (0 a 360 convertido para radianos)
        float angle = (rand() % 360) * 3.14159f / 180.0f;

        // Gera uma distância aleatória do player, de 0 até 1000 pixels
        float distance = (float)(rand() % 1000);

        float offsetX = distance * cos(angle);
        float offsetY = distance * sin(angle);

        ambientParticles->Generate(x + offsetX, y + offsetY, 1);
    }

    dustParticles->Update(dt);
    dashParticles->Update(dt);
    hitParticles->Update(dt);
    ambientParticles->Update(dt); // Atualiza as partículas ambiente
}

// -------------------------------------------------------------------------------

void Player::SpawnAttackHitbox()
{
    // Aumentamos o offset novamente. Como o hitbox agora tem 80 de largura, 
    // colocar o centro dele em "HW + 28" faz com que a ponta da espada vá bem longe,
    // e a "metade de trás" do hitbox ainda cubra a área grudada no player.
    float ox = facingRight ? HW + 28.0f : -(HW + 28.0f);
    float oy = 0;
    int   dmg = (attackCombo == 2) ? 1 : 1;

    if (attackCombo == 2)
    {
        // --- GOLPE 3: PROJÉTIL (Onda de Choque) ---
        float projSpeed = facingRight ? 750.0f : -750.0f;

        GeoWars::scene->Add(new AttackHitbox(
            x + ox, y + oy,
            dmg,
            type,
            nullptr,
            0.5f,
            5,
            64.0f, 64.0f,
            projSpeed
        ), MOVING);

        hitParticles->Generate(x + ox, y + oy, 20);
    }
    else
    {
        GeoWars::scene->Add(new AttackHitbox(
            x + ox, y + oy,
            dmg,
            type,
            this,
            0.20f, // Nova duração
            3,
            80.0f, 64.0f // Nova largura e altura
        ), MOVING);

        hitParticles->Generate(x + ox, y + oy, 8);
    }
}

// -------------------------------------------------------------------------------

void Player::TakeDamage(int dmg)
{
    if (invincible || !IsAlive()) return;

    hp = max(hp - dmg, 0);

    if (hp <= 0)
    {
        //state = PS_DEAD;
        GeoWars::audio->Play(DEATH_SFX);
        hp = 5;
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

    float flipX = facingRight ? 1.0f : -1.0f;
    //sprite->Draw(x, y, Layer::FRONT, 1.0f, 0.0f, flipX);
    anim->NextFrame();
    anim->Draw(x, y, z);

    if (healTimer > 0)
        hitParticles->Draw(Layer::MIDDLE, 1.0f);

    ambientParticles->Draw(Layer::LOWER, 0.6f);
    dustParticles->Draw(Layer::LOWER, 1.0f);
    dashParticles->Draw(Layer::MIDDLE, 0.8f);
    hitParticles->Draw(Layer::UPPER, 1.0f);
}

// -------------------------------------------------------------------------------
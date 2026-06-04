#include "Flyer.h"
#include "GeoWars.h"
#include <cmath>

Flyer::Flyer(float startX, float startY)
    : BaseEnemy(2, 3),         // 2 HP, dropa 3 geo
    orbitAngle(0.0f),
    orbitRadius(80.0f),
    orbitSpeed(90.0f),       // graus/segundo
    diveSpeed(320.0f),
    diveTimer(0.0f),
    diveCooldown(0.0f),
    idleY(startY - 80.0f),
    diving(false)
{
    animation = new TileSet("Resources/Player/Rato.png", 64, 64, 15, 30);
    anim = new Animation(animation, 0.130f, true);

    hw = 12.0f;
    hh = 12.0f;
    BBox(new Circle(20.0f));

    MoveTo(startX, startY);
    state = ES_PATROL;
}

// -------------------------------------------------------------------------------

Flyer::~Flyer()
{
    delete anim;
    delete animation;
}

// -------------------------------------------------------------------------------

void Flyer::UpdateAI(float dt)
{
    if (diveCooldown > 0) diveCooldown -= dt;

    switch (state)
    {
        // ---- Patrulha: órbita senoidal em volta da posição inicial ----
    case ES_PATROL:
    {
        orbitAngle += orbitSpeed * dt;
        velX = cosf(orbitAngle * 3.14159f / 180.0f) * 60.0f;
        velY = sinf(orbitAngle * 3.14159f / 180.0f) * 30.0f;

        // Avista o player: muda para alerta
        if (PlayerInSight(250.0f))
        {
            state = ES_ALERT;
            alertTimer = 0.3f;
            velX = velY = 0;
        }
        break;
    }

    // ---- Alerta ----
    case ES_ALERT:
    {
        velX = velY = 0;
        if (alertTimer <= 0) state = ES_CHASE;
        break;
    }

    // ---- Perseguição: circula acima do player e mergulha ----
    case ES_CHASE:
    {
        // posição alvo: levemente acima do player
        float targetX = GeoWars::player->X();
        float targetY = GeoWars::player->Y() - 120.0f;

        float dx = targetX - x;
        float dy = targetY - y;
        float dist = sqrtf(dx * dx + dy * dy);

        // Aproxima suavemente
        if (dist > 5.0f)
        {
            velX = (dx / dist) * 90.0f;
            velY = (dy / dist) * 90.0f;
        }
        else
        {
            velX = velY = 0;
        }

        facingRight = GeoWars::player->X() > x;

        // Mergulha se próximo o suficiente e cooldown zerado
        if (dist < 160.0f && diveCooldown <= 0)
        {
            state = ES_ATTACK;
            diveTimer = 0.5f;
            diveCooldown = 1.8f;
            float adx = GeoWars::player->X() - x;
            float ady = GeoWars::player->Y() - y;
            float len = sqrtf(adx * adx + ady * ady);
            if (len > 0) { adx /= len; ady /= len; }
            velX = adx * diveSpeed;
            velY = ady * diveSpeed;
        }

        // Perdeu visão
        if (!PlayerInSight(350.0f))
            state = ES_PATROL;

        break;
    }

    // ---- Ataque: mergulho em linha reta ----
    case ES_ATTACK:
    {
        diveTimer -= dt;
        if (diveTimer <= 0)
        {
            velX = velY = 0;
            state = ES_CHASE;
        }
        break;
    }

    default:
        break;
    }
}

// -------------------------------------------------------------------------------

void Flyer::DrawSprite()
{
    float flipX = facingRight ? 1.0f : -1.0f;
    float dt = gameTime;

    switch (state)
    {
    case ES_ATTACK:
        anim->Draw(x, y, z, 1.0f, 0);
        break;
    case ES_HURT:
        anim->Draw(x, y, z, 1.0f, 0);
        break;
    default:
        anim->Draw(x, y, z, 1.0f, 0);
        break;
    }
}

// -------------------------------------------------------------------------------
#include "Flyer.h"
#include "GeoWars.h"
#include "AttackHitbox.h"
#include <cmath>

Flyer::Flyer(float startX, float startY)
    : BaseEnemy(3, 3),
    orbitAngle(0.0f),
    orbitRadius(80.0f),
    orbitSpeed(90.0f),
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

    // O mergulho usa diveTimer pra controlar sua própria duração/estado —
    // sem isso, o BaseEnemy reverte ES_ATTACK->ES_PATROL sozinho no meio do mergulho
    customAttackState = true;
    hasGravity = false;

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
    if (GetHP() < 1)
    {
        GeoWars::enemies.Remove(this);
        GeoWars::scene->Delete();
        return;
    }

    if (diveCooldown > 0) diveCooldown -= dt;

    switch (state)
    {
        // ---- Patrulha: órbita senoidal ----
    case ES_PATROL:
    {
        orbitAngle += orbitSpeed * dt;

        // Converte posição da órbita em vetor polar
        float vx = cosf(orbitAngle * 3.14159f / 180.0f) * 60.0f;
        float vy = sinf(orbitAngle * 3.14159f / 180.0f) * 30.0f;

        speed->ScaleTo(0.0f);
        if (vx != 0.0f || vy != 0.0f)
        {
            float angle = atan2f(vy, vx) * (180.0f / 3.14159f);
            float mag = sqrtf(vx * vx + vy * vy);
            speed->Add(Vector(angle, mag));
        }

        if (PlayerInSight(250.0f))
        {
            state = ES_ALERT;
            alertTimer = 0.3f;
            speed->ScaleTo(0.0f);
        }
        break;
    }

    // ---- Alerta ----
    case ES_ALERT:
    {
        speed->ScaleTo(0.0f);
        if (GeoWars::player)
            facingRight = (GeoWars::player->X() > x);
        if (alertTimer <= 0) state = ES_CHASE;
        break;
    }

    // ---- Perseguição: circula acima do player e mergulha ----
    case ES_CHASE:
    {
        // Alvo: levemente acima do player
        float targetX = GeoWars::player->X();
        float targetY = GeoWars::player->Y() - 120.0f;

        float dx = targetX - x;
        float dy = -(targetY - y);   // inverte Y para sistema vetorial
        float dist = sqrtf(dx * dx + dy * dy);

        facingRight = (GeoWars::player->X() > x);

        // Aproxima suavemente na direção do alvo
        speed->ScaleTo(0.0f);
        if (dist > 5.0f)
        {
            float angle = atan2f(dy, dx) * (180.0f / 3.14159f);
            speed->Add(Vector(angle, 90.0f));
        }

        // Mergulha se próximo o suficiente e cooldown zerado
        if (dist < 160.0f && diveCooldown <= 0)
        {
            // Calcula ângulo exato em direção ao player
            float adx = GeoWars::player->X() - x;
            float ady = -(GeoWars::player->Y() - y); // inverte Y
            float angle = atan2f(ady, adx) * (180.0f / 3.14159f);

            speed->ScaleTo(0.0f);
            speed->Add(Vector(angle, diveSpeed));

            state = ES_ATTACK;
            diveTimer = 0.5f;
            diveCooldown = 1.8f;

            Attack();
        }

        if (!PlayerInSight(350.0f))
        {
            speed->ScaleTo(0.0f);
            state = ES_PATROL;
        }
        break;
    }

    // ---- Ataque: mantém o vetor do mergulho até acabar o timer ----
    case ES_ATTACK:
    {
        diveTimer -= dt;
        if (diveTimer <= 0)
        {
            speed->ScaleTo(0.0f);
            state = ES_CHASE;
        }
        break;
    }

    default:
        break;
    }

    // Aplica translação pelo vetor velocidade
    Translate(speed->XComponent() * dt,
        -speed->YComponent() * dt);

    // Restringe às bordas do mapa
    if (x - hw < 0)       MoveTo(hw, y);
    if (x + hw > 3840.0f) MoveTo(3840.0f - hw, y);
    if (y - hh < 0)       MoveTo(x, hh);
    if (y + hh > 2160.0f) MoveTo(x, 2160.0f - hh);
}

// -------------------------------------------------------------------------------

void Flyer::DrawSprite()
{
    float flipX = facingRight ? 1.0f : -1.0f;

    switch (state)
    {
    case ES_ATTACK:
    case ES_HURT:
    default:
        anim->Draw(x, y, Layer::MIDDLE, 1.0f, 0, flipX);
        break;
    }
}

// -------------------------------------------------------------------------------

void Flyer::OnCollision(Object* obj)
{
    // Checagem de morte saiu daqui — agora roda todo frame em UpdateAI.
    if (obj && obj->Type() == PLATFORM)
        obj->OnCollision(this);
}

// -------------------------------------------------------------------------------

void Flyer::Attack()
{
    // Flyer gerencia seu próprio estado de ataque (mergulho),
    // apenas spawna o hitbox e ajusta timers sem alterar state
    if (attackCooldown > 0) return;
    if (state == ES_DEAD || state == ES_HURT) return;

    attackCooldown = 1.0f;
    attackTimer = 0.5f;    // evita que BaseEnemy resolva ES_ATTACK prematuramente

    float ox = facingRight ? hw + 24.0f : -(hw + 24.0f);
    GeoWars::scene->Add(new AttackHitbox(x + ox, y, attackDamage, type, this), MOVING);
}
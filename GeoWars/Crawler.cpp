// -------------------------------------------------------------------------------
// IMPLEMENTAÇÃO
// -------------------------------------------------------------------------------

#include "Crawler.h"
#include "GeoWars.h"

Crawler::Crawler(float startX, float startY)
    : BaseEnemy(3, 2),
    patrolSpeed(60.0f),
    chaseSpeed(110.0f),
    patrolTimer(0.0f),
    edgeCheckOffset(20.0f)
{
    animation = new TileSet("Resources/Player/Rato.png", 64, 64, 15, 30);
    anim = new Animation(animation, 0.130f, true);

    hw = 14.0f;
    hh = 12.0f;
    BBox(new Circle(45.0f));

    MoveTo(startX, startY);
    facingRight = true;
    state = ES_PATROL;

    // vetor velocidade — começa parado
    speed = new Vector(0.0f, 0.0f);
}

// -------------------------------------------------------------------------------

Crawler::~Crawler()
{
    delete speed;
    delete animation;
    delete anim;
}

// -------------------------------------------------------------------------------

bool Crawler::CheckEdgeAhead() const
{
    return false; // sem tilemap por enquanto
}

// -------------------------------------------------------------------------------

void Crawler::UpdateAI(float dt)
{
    switch (state)
    {
        // ---- Patrulha ----
    case ES_PATROL:
    {
        // Move horizontalmente na direção atual
        float angle = facingRight ? 0.0f : 180.0f;
        speed->ScaleTo(0.0f);
        speed->Add(Vector(angle, patrolSpeed));

        if (CheckEdgeAhead())
        {
            facingRight = !facingRight;
            patrolTimer = 0.3f;
        }

        if (PlayerInSight(200.0f))
        {
            state = ES_ALERT;
            alertTimer = 0.4f;
            speed->ScaleTo(0.0f);
        }
        break;
    }

    // ---- Alerta (avistou o player) ----
    case ES_ALERT:
    {
        speed->ScaleTo(0.0f);
        if (alertTimer <= 0)
            state = ES_CHASE;
        break;
    }

    // ---- Perseguição ----
    case ES_CHASE:
    {
        Player* player = GeoWars::player;
        if (player)
        {
            // Calcula ângulo exato em direção ao player
            float dx = player->X() - x;
            float dy = -(player->Y() - y); // inverte Y para sistema vetorial
            float angle = atan2f(dy, dx) * (180.0f / 3.14159f);

            facingRight = (dx > 0);

            // Aplica velocidade de perseguição na direção do player
            speed->ScaleTo(0.0f);
            speed->Add(Vector(angle, chaseSpeed));
        }

        if (CheckEdgeAhead())
        {
            speed->ScaleTo(0.0f);
            state = ES_PATROL;
        }

        if (!PlayerInSight(280.0f))
        {
            speed->ScaleTo(0.0f);
            state = ES_PATROL;
        }

        // Ataca se estiver próximo do player
        if (PlayerInRange(attackRange))
        {
            Attack();
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
    if (x - hw < 0)           MoveTo(hw, y);
    if (x + hw > 3840.0f)     MoveTo(3840.0f - hw, y);
    if (y - hh < 0)           MoveTo(x, hh);
    if (y + hh > 2160.0f)     MoveTo(x, 2160.0f - hh);
}

// -------------------------------------------------------------------------------

void Crawler::DrawSprite()
{
    float flipX = facingRight ? 1.0f : -1.0f;

    switch (state)
    {
    case ES_IDLE:
    case ES_ALERT:
    case ES_PATROL:
    case ES_CHASE:
    case ES_HURT:
    default:
        anim->Draw(x, y, Layer::MIDDLE, 1.0f, 0, flipX);
        break;
    }
}

void Crawler::OnCollision(Object* obj)
{
    if (GetHP() < 1)
    {
        GeoWars::scene->Delete(this, this->Type());
    }
}


#include "Crawler.h"
#include "GeoWars.h"

Crawler::Crawler(float startX, float startY)
    : BaseEnemy(3, 2),
    patrolSpeed(60.0f),
    chaseSpeed(110.0f),
    patrolTimer(0.0f),
    edgeCheckOffset(20.0f)
{
    spawnX = startX;

    animation = new TileSet("Resources/Player/Rato.png", 64, 64, 15, 30);
    anim = new Animation(animation, 0.130f, true);

    hw = 14.0f;
    hh = 12.0f;
    BBox(new Circle(45.0f));

    MoveTo(startX, startY);
    facingRight = true;
    state = ES_PATROL;

    // A classe BaseEnemy JÁ inicializa o 'speed'. 
    // Não faça 'speed = new Vector()' aqui para não causar memory leak.
}

// -------------------------------------------------------------------------------

Crawler::~Crawler()
{
    // A classe BaseEnemy JÁ faz o 'delete speed'.
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
    // A checagem de "GetHP() < 1" foi removida daqui!
    // A classe BaseEnemy::Update() já cuida de matar, explodir e remover da Scene.

    switch (state)
    {
        // ---- Patrulha ----
    case ES_PATROL:
    {
        float angle = facingRight ? 0.0f : 180.0f;
        speed->ScaleTo(0.0f);
        speed->Add(Vector(angle, patrolSpeed));

        if (patrolTimer > 0.0f)
            patrolTimer -= dt;

        const float kPatrolRange = 180.0f;
        bool hitRightEdge = facingRight && (x - spawnX) > kPatrolRange;
        bool hitLeftEdge = !facingRight && (spawnX - x) > kPatrolRange;

        if (patrolTimer <= 0.0f && (CheckEdgeAhead() || hitRightEdge || hitLeftEdge))
        {
            facingRight = !facingRight;
            patrolTimer = 0.3f;
        }

        if (PlayerInSight(200.0f))
        {
            state = ES_ALERT;
            alertTimer = 0.4f;
            facingRight = (GeoWars::player->X() > x);
            speed->ScaleTo(0.0f);
        }
        break;
    }

    // ---- Alerta (avistou o player) ----
    case ES_ALERT:
    {
        speed->ScaleTo(0.0f);
        alertTimer -= dt;

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
            float dx = player->X() - x;
            float dy = -(player->Y() - y);
            float angle = atan2f(dy, dx) * (180.0f / 3.14159f);

            facingRight = (dx > 0);

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

        if (PlayerInRange(attackRange))
        {
            Attack();
        }
        break;
    }

    default:
        break;
    }
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
    // Chama a classe base PRIMEIRO para garantir que o Crawler vai causar dano ao Player
    BaseEnemy::OnCollision(obj);

    if (obj && obj->Type() == PLATFORM)
        obj->OnCollision(this);
}
// -------------------------------------------------------------------------------
// IMPLEMENTAÇÃO
// -------------------------------------------------------------------------------

#include "Crawler.h"
#include "GeoWars.h"

Crawler::Crawler(float startX, float startY)
    : BaseEnemy(3, 2),      // 3 HP, dropa 2 geo
    patrolSpeed(60.0f),
    chaseSpeed(110.0f),
    patrolTimer(0.0f),
    edgeCheckOffset(20.0f)
{


    animation = new TileSet("Resources/Player/Rato.png", 64, 64, 15, 30);
    anim = new Animation(animation, 0.130f, true);

    hw = 14.0f;
    hh = 12.0f;
    BBox(new Circle(20.0f));

    MoveTo(startX, startY);
    facingRight = true;
    state = ES_PATROL;
}

// -------------------------------------------------------------------------------

Crawler::~Crawler()
{
    delete animation;
    delete anim;
}

// -------------------------------------------------------------------------------

bool Crawler::CheckEdgeAhead() const
{
    //if (!Player::tilemap) return false;

    float checkX = facingRight ? x + hw + edgeCheckOffset : x - hw - edgeCheckOffset;
    float checkY = y + hh + 4.0f;    // logo abaixo dos pés

    //TileType t = Player::tilemap->GetAt(checkX, checkY);
    //return (t == TILE_EMPTY);          // beira: vazio abaixo
    return false;
}

// -------------------------------------------------------------------------------

void Crawler::UpdateAI(float dt)
{
    switch (state)
    {
        // ---- Patrulha ----
    case ES_PATROL:
    {
        velX = facingRight ? patrolSpeed : -patrolSpeed;

        // Vira ao detectar beira ou parede
        if (CheckEdgeAhead())
        {
            facingRight = !facingRight;
            patrolTimer = 0.3f;      // pequena pausa ao virar
        }

        // Passa para alerta ao ver o player
        if (PlayerInSight(200.0f))
        {
            state = ES_ALERT;
            alertTimer = 0.4f;        // pequena pausa de "avistamento"
            velX = 0;
        }
        break;
    }

    // ---- Alerta (avistou o player) ----
    case ES_ALERT:
    {
        velX = 0;
        if (alertTimer <= 0)
            state = ES_CHASE;
        break;
    }

    // ---- Perseguição ----
    case ES_CHASE:
    {
        //float dx = SilkSong::player->X() - x;
        //facingRight = (dx > 0);
        //velX = facingRight ? chaseSpeed : -chaseSpeed;

        // Beira: para de perseguir, volta a patrulhar
        if (CheckEdgeAhead())
        {
            velX = 0;
            state = ES_PATROL;
        }

        // Perdeu linha de visada: volta a patrulhar
        if (!PlayerInSight(280.0f))
        {
            state = ES_PATROL;
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
    float dt = gameTime;

    switch (state)
    {
    case ES_IDLE:
    case ES_ALERT:
        anim->Draw(x, y, Layer::MIDDLE, 1.0f, 0, flipX);
        break;

    case ES_PATROL:
    case ES_CHASE:
        anim->Draw(x, y, Layer::MIDDLE, 1.0f, 0, flipX);
        break;

    case ES_HURT:
        anim->Draw(x, y, Layer::MIDDLE, 1.0f, 0, flipX);
        break;

    default:
        anim->Draw(x, y, Layer::MIDDLE, 1.0f, 0, flipX);
        break;
    }
}

// -------------------------------------------------------------------------------
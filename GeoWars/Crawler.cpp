#include "Crawler.h"
#include "GeoWars.h"

// -------------------------------------------------------------------------------
// IMPORTANTE: adicione ao Crawler.h (seção private) — usado pela patrulha por
// raio, já que CheckEdgeAhead() ainda não tem tilemap pra detectar borda real:
//
//     float spawnX;   // posição X de origem, usada como centro da patrulha
// -------------------------------------------------------------------------------

Crawler::Crawler(float startX, float startY)
    : BaseEnemy(1, 2),
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
    // Checagem de morte movida pra cá: antes só rodava dentro de OnCollision,
    // então um Crawler que tomasse o dano fatal sem estar colidindo com nada
    // naquele frame (ex.: dano à distância) ficava "vivo" até a próxima colisão.
    if (GetHP() < 1)
    {
        GeoWars::scene->Delete(this, this->Type());
        return;
    }

    switch (state)
    {
        // ---- Patrulha ----
    case ES_PATROL:
    {
        // Move horizontalmente na direção atual
        float angle = facingRight ? 0.0f : 180.0f;
        speed->ScaleTo(0.0f);
        speed->Add(Vector(angle, patrolSpeed));

        if (patrolTimer > 0.0f)
            patrolTimer -= dt;

        // Sem tilemap pra detectar borda real (CheckEdgeAhead é só um stub por
        // enquanto), então patrulhamos num raio a partir do ponto de spawn —
        // assim o Crawler vai e volta ao invés de andar reto até a borda do mapa
        const float kPatrolRange = 180.0f;
        bool hitRightEdge = facingRight && (x - spawnX) > kPatrolRange;
        bool hitLeftEdge = !facingRight && (spawnX - x) > kPatrolRange;

        if (patrolTimer <= 0.0f && (CheckEdgeAhead() || hitRightEdge || hitLeftEdge))
        {
            facingRight = !facingRight;
            patrolTimer = 0.3f; // evita virar várias vezes seguidas na borda
        }

        if (PlayerInSight(200.0f))
        {
            state = ES_ALERT;
            alertTimer = 0.4f;
            facingRight = (GeoWars::player->X() > x); // já vira de frente pro player
            speed->ScaleTo(0.0f);
        }
        break;
    }

    // ---- Alerta (avistou o player) ----
    case ES_ALERT:
    {
        speed->ScaleTo(0.0f);

        // Faltava decrementar — antes disso o Crawler nunca saía do alerta
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

        // Ataca se estiver próximo do player — BaseEnemy::Attack() já tem
        // seu próprio cooldown interno (attackCooldown), então é seguro
        // chamar aqui sem gating adicional
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

    if (obj && obj->Type() == PLATFORM)
        obj->OnCollision(this);
}
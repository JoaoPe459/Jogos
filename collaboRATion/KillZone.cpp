/**********************************************************************************
// KillZone (Código Fonte)
**********************************************************************************/

#include "KillZone.h"
#include "Player.h"
#include "PacMan.h"
#include <cmath>

// ─────────────────────────────────────────────────────────────────────────────

KillZone::KillZone(float px, float py, float w, float h,
                   bool isLethal, const std::string& t)
{
    type     = KILLZONE;
    lethal   = isLethal;
    tag      = t;

    MoveTo(px, py);
    BBox(new Rect(-w * 0.5f, -h * 0.5f, w * 0.5f, h * 0.5f));
}

// ─────────────────────────────────────────────────────────────────────────────

void KillZone::Update()
{
    // tickTimer avança no OnCollision — nada a fazer aqui
}

void KillZone::Draw()
{
    
}

// ─────────────────────────────────────────────────────────────────────────────

void KillZone::OnCollision(Object* obj)
{
    if (!active || obj->Type() != PLAYER) return;

    Player* p = static_cast<Player*>(obj);

    if (lethal && !p->isDead)
    {
        p->Die();
    }
}

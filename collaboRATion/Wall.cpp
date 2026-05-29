#include "PacMan.h"
#include "Wall.h"
#include "LevelMake.h"
#include <algorithm>
#include <cmath>

static constexpr float DEG2RAD = 3.14159265f / 180.0f;

// ─── Construtor / Destrutor ───────────────────────────────────────

Wall::Wall()
{
    type = WALL;
}

Wall::~Wall()
{
    blocks.clear();
}

// ─── Helpers internos ────────────────────────────────────────────

// Recalcula posição e bbox de todos os blocos aplicando rotação + escala.
// Chamado sempre que angle, scaleX ou scaleY mudam.
static void SyncBlock(Block* b, float ownerX, float ownerY,
                      float angle, float sx, float sy)
{
    if (!b || b->isDead) return;

    float rad  = angle * DEG2RAD;
    float offX = b->offsetX * sx;
    float offY = b->offsetY * sy;

    float wx = ownerX + offX * cosf(rad) - offY * sinf(rad);
    float wy = ownerY + offX * sinf(rad) + offY * cosf(rad);

    b->MoveTo(wx, wy);

    // Redimensiona bbox do bloco proporcionalmente
    float hw = b->width  * sx * 0.5f;
    float hh = b->height * sy * 0.5f;
    b->BBox(new Rect(-hw, -hh, hw, hh));
}

// ─── AddBlock ────────────────────────────────────────────────────

Block* Wall::AddBlock(float offX, float offY,
    float w, float h,
    const char* spriteFile,
    const std::string& tag)
{
    Block* b = new Block(this, offX, offY, w, h, spriteFile, tag);
    blocks.push_back(b);

    LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
    lvl->GetScene()->Add(b, b->Type());

    SyncBlock(b, X(), Y(), angle, scaleX, scaleY);
    return b;
}

Block* Wall::AddRect1(float offX, float offY,
    float w, float h,
    const char* spriteFile,
    const std::string& tag)
{
    Block* b = new Block(this, offX,      offY, w, h, spriteFile, tag);
    Block* c = new Block(this, offX + 32, offY, w, h, spriteFile, tag);
    Block* d = new Block(this, offX + 64, offY, w, h, spriteFile, tag);
    blocks.push_back(b); blocks.push_back(c); blocks.push_back(d);

    LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
    lvl->GetScene()->Add(b, b->Type());
    lvl->GetScene()->Add(c, c->Type());
    lvl->GetScene()->Add(d, d->Type());

    SyncBlock(b, X(), Y(), angle, scaleX, scaleY);
    SyncBlock(c, X(), Y(), angle, scaleX, scaleY);
    SyncBlock(d, X(), Y(), angle, scaleX, scaleY);
    return b;
}

Block* Wall::AddRect2(float offX, float offY,
    float w, float h,
    const char* spriteFile,
    const std::string& tag)
{
    AddRect1(offX,      offY, w, h, spriteFile, tag);
    AddRect1(offX + 96, offY, w, h, spriteFile, tag);
    return blocks.empty() ? nullptr : blocks.front();
}

Block* Wall::AddRect3(float offX, float offY,
    float w, float h,
    const char* spriteFile,
    const std::string& tag)
{
    AddRect1(offX, offY,      w, h, spriteFile, tag);
    AddRect1(offX, offY + 32, w, h, spriteFile, tag);
    return blocks.empty() ? nullptr : blocks.front();
}

Block* Wall::AddRect1v(float offX, float offY,
    float w, float h,
    const char* spriteFile,
    const std::string& tag)
{
    Block* b = new Block(this, offX, offY,      w, h, spriteFile, tag);
    Block* c = new Block(this, offX, offY + 32, w, h, spriteFile, tag);
    Block* d = new Block(this, offX, offY + 64, w, h, spriteFile, tag);
    blocks.push_back(b); blocks.push_back(c); blocks.push_back(d);

    LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
    lvl->GetScene()->Add(b, b->Type());
    lvl->GetScene()->Add(c, c->Type());
    lvl->GetScene()->Add(d, d->Type());

    SyncBlock(b, X(), Y(), angle, scaleX, scaleY);
    SyncBlock(c, X(), Y(), angle, scaleX, scaleY);
    SyncBlock(d, X(), Y(), angle, scaleX, scaleY);
    return b;
}

Block* Wall::AddRect2v(float offX, float offY,
    float w, float h,
    const char* spriteFile,
    const std::string& tag)
{
    AddRect1v(offX,      offY, w, h, spriteFile, tag);
    AddRect1v(offX + 32, offY, w, h, spriteFile, tag);
    return blocks.empty() ? nullptr : blocks.front();
}

Block* Wall::AddRect3v(float offX, float offY,
    float w, float h,
    const char* spriteFile,
    const std::string& tag)
{
    AddRect1v(offX, offY,      w, h, spriteFile, tag);
    AddRect1v(offX, offY + 96, w, h, spriteFile, tag);
    return blocks.empty() ? nullptr : blocks.front();
}

// ─── RemoveBlock ─────────────────────────────────────────────────

void Wall::RemoveBlock(Block* block)
{
    auto it = std::find(blocks.begin(), blocks.end(), block);
    if (it == blocks.end()) return;
    blocks.erase(it);
    block->isDead = true;
}

void Wall::RemoveBlock(const std::string& tag)
{
    auto it = std::find_if(blocks.begin(), blocks.end(),
        [&](Block* b) { return b->tag == tag; });
    if (it == blocks.end()) return;
    (*it)->isDead = true;
    blocks.erase(it);
}

void Wall::RemoveBlock(int index)
{
    if (index < 0 || index >= (int)blocks.size()) return;
    blocks[index]->isDead = true;
    blocks.erase(blocks.begin() + index);
}

void Wall::RemoveBlocksInRadius(float cx, float cy, float radius)
{
    std::vector<Block*> toRemove;
    for (Block* b : blocks)
    {
        float dx = b->X() - cx;
        float dy = b->Y() - cy;
        if (sqrtf(dx * dx + dy * dy) <= radius)
            toRemove.push_back(b);
    }
    for (Block* b : toRemove) RemoveBlock(b);
}

// ─── Rotação ─────────────────────────────────────────────────────

void Wall::Rotate(float degrees)
{
    angle += degrees;
    for (Block* b : blocks)
        SyncBlock(b, X(), Y(), angle, scaleX, scaleY);
}

void Wall::SetAngle(float degrees)
{
    angle = degrees;
    for (Block* b : blocks)
        SyncBlock(b, X(), Y(), angle, scaleX, scaleY);
}

// ─── Escala ───────────────────────────────────────────────────────

// Escala proporcional — X e Y crescem/encolhem igualmente
void Wall::SetScale(float s)
{
    SetScale(s, s);
}

// Escala independente — pode esticar só em X ou só em Y
void Wall::SetScale(float sx, float sy)
{
    scaleX = sx;
    scaleY = sy;
    for (Block* b : blocks)
        SyncBlock(b, X(), Y(), angle, scaleX, scaleY);
}

// ─── Queda ────────────────────────────────────────────────────────

void Wall::StartFalling(float initialVelY)
{
    falling  = true;
    fallVelY = initialVelY;
}

void Wall::StopFalling()
{
    falling  = false;
    fallVelY = 0.0f;
}

// ─── Busca ────────────────────────────────────────────────────────

Block* Wall::GetBlockByTag(const std::string& tag)
{
    for (Block* b : blocks)
        if (b->tag == tag) return b;
    return nullptr;
}

// ─── Update / Draw / Collision ────────────────────────────────────

void Wall::Update()
{
    if (falling)
    {
        fallVelY += gravity * gameTime;
        MoveTo(X(), Y() + fallVelY * gameTime);

        for (Block* b : blocks)
            SyncBlock(b, X(), Y(), angle, scaleX, scaleY);
    }
}

void Wall::Draw()
{
    // Blocos se desenham via Draw() próprio
}

void Wall::OnCollision(Object* obj)
{
    // Colisões relevantes ficam no Block::OnCollision
}

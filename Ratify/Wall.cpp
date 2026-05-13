#include "PacMan.h"
#include "Wall.h"
#include "LevelMake.h"
#include <algorithm>
#include <cmath>

// ─── Construtor / Destrutor ───────────────────────────────────────

Wall::Wall()
{
    type = WALL;
}

Wall::~Wall()
{
    blocks.clear();
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
    return b;
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
    // Coleta primeiro para não invalidar o iterador durante o erase
    std::vector<Block*> toRemove;
    for (Block* b : blocks)
    {
        float dx = b->X() - cx;
        float dy = b->Y() - cy;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist <= radius)
            toRemove.push_back(b);
    }

    for (Block* b : toRemove)
        RemoveBlock(b);
}

// ─── Rotação / Ângulo ─────────────────────────────────────────────

void Wall::Rotate(float degrees)
{
    angle += degrees;
    for (Block* b : blocks)
        b->SyncToOwner();
}

void Wall::SetAngle(float degrees)
{
    angle = degrees;
    for (Block* b : blocks)
        b->SyncToOwner();
}

// ─── Queda ────────────────────────────────────────────────────────

void Wall::StartFalling(float initialVelY)
{
    falling = true;
    fallVelY = initialVelY;
}

void Wall::StopFalling()
{
    falling = false;
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
            b->SyncToOwner();
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
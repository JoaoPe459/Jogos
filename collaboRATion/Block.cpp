#include "PacMan.h"
#include "Block.h"
#include "Wall.h"
#include <cmath>

static constexpr float DEG2RAD = 3.14159265f / 180.0f;

Block::Block(Wall* owner, float offX, float offY,
    float w, float h,
    const char* spriteFile, const std::string& tag)
    : owner(owner), offsetX(offX), offsetY(offY),
    width(w), height(h), tag(tag)
{
    type = WALL;

    if (spriteFile && strlen(spriteFile) > 0)
        sprite = new Sprite(spriteFile);

    BBox(new Rect(-w * 0.5f, -h * 0.5f, w * 0.5f, h * 0.5f));
    SyncToOwner();
}

Block::~Block()
{
    delete sprite;
}

void Block::SyncToOwner()
{
    if (!owner || isDead) return;

    float ang = owner->GetAngle();
    float rad = ang * DEG2RAD;
    float wx  = owner->X() + offsetX * cosf(rad) - offsetY * sinf(rad);
    float wy  = owner->Y() + offsetX * sinf(rad) + offsetY * cosf(rad);

    MoveTo(wx, wy);
}

void Block::Update()
{
    if (isDead) return;
    SyncToOwner();
}

void Block::Draw()
{
    if (isDead || !sprite) return;
    sprite->Draw(X(), Y(), Layer::MIDDLE);
}

void Block::OnCollision(Object* obj)
{
    if (isDead) return;

    if (obj->Type() == FLOOR && owner && owner->IsFalling())
        owner->StopFalling();
}
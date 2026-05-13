#include "PacMan.h"
#include "Wall.h"
#include "LevelMake.h"


Wall::Wall()
{
    type = WALL;
    // Wall em si não precisa de BBox — os Blocks têm os próprios
}

Wall::~Wall()
{
    // Os blocks são removidos da cena pelo engine normalmente;
    // aqui só limpamos o vetor de referências
    blocks.clear();
}

Block* Wall::AddBlock(float offX, float offY,
    float w, float h,
    const char* spriteFile,
    const std::string& tag)
{
    ;
    Block* b = new Block(this, offX, offY, w, h, spriteFile, tag);
    blocks.push_back(b);

    LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
    lvl->GetScene()->Add(b, b->Type());
    return b;
}

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

Block* Wall::GetBlockByTag(const std::string& tag)
{
    for (Block* b : blocks)
        if (b->tag == tag) return b;
    return nullptr;
}

void Wall::Update()
{
    if (falling) {
        fallVelY += gravity * gameTime;
        MoveTo(X(), Y() + fallVelY * gameTime);

        // Propaga nova posição para todos os blocos
        for (Block* b : blocks)
            b->SyncToOwner();
    }
}

void Wall::Draw()
{
    // Blocos se desenham sozinhos via Draw() próprio;
    // Wall não precisa desenhar nada
}

void Wall::OnCollision(Object* obj)
{
    // Wall em si raramente colide (sem BBox ativo)
    // A lógica de parar a queda fica nos Blocks
}
#ifndef _BLOCK_H_
#define _BLOCK_H_

#include "Object.h"
#include "Sprite.h"
#include "Geometry.h"
#include <string>

class Wall;

class Block : public Object
{
public:
    bool        isDead = false;
    float       offsetX = 0.0f;
    float       offsetY = 0.0f;
    float       width = 0.0f;
    float       height = 0.0f;
    bool        solid = true;
    std::string tag;
    Sprite* sprite = nullptr;
    Wall* owner = nullptr;

    Block(Wall* owner, float offX, float offY, float w, float h,
        const char* spriteFile = nullptr, const std::string& tag = "");
    ~Block();

    void SyncToOwner();

    void Update()              override;
    void Draw()                override;
    void OnCollision(Object* obj);
};

#endif
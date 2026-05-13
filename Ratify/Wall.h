#ifndef _PACMAN_WALL_H_
#define _PACMAN_WALL_H_

#include "Object.h"
#include "Block.h"
#include <vector>
#include <string>

class Wall : public Object
{
private:
    std::vector<Block*> blocks;

    float angle = 0.0f;
    bool  falling = false;
    float fallVelY = 0.0f;
    float gravity = 800.0f;

public:
    Wall();
    ~Wall();

    Block* AddBlock(float offsetX, float offsetY,
        float width, float height,
        const char* spriteFile = nullptr,
        const std::string& tag = "");

    void Rotate(float degrees);
    void SetAngle(float degrees);
    float GetAngle() const { return angle; }

    void StartFalling(float initialVelY = 0.0f);
    void StopFalling();

    Block* GetBlockByTag(const std::string& tag);
    const std::vector<Block*>& GetBlocks() const { return blocks; }

    void Update()  override;
    void Draw()    override;
    void OnCollision(Object* obj) override;
    bool IsFalling() const { return falling; }
};

#endif
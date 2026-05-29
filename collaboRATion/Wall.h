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

    float angle    = 0.0f;
    bool  falling  = false;
    float fallVelY = 0.0f;
    float gravity  = 800.0f;

    // ── Escala ────────────────────────────────────────────────────
    float scaleX   = 1.0f;
    float scaleY   = 1.0f;

public:
    Wall();
    ~Wall();

    Block* AddBlock(float offsetX, float offsetY,
                    float width,   float height,
                    const char* spriteFile = nullptr,
                    const std::string& tag = "");

    Block* AddRect1(float offsetX, float offsetY,
        float width, float height,
        const char* spriteFile = nullptr,
        const std::string& tag = "");

    Block* AddRect2(float offsetX, float offsetY,
        float width, float height,
        const char* spriteFile = nullptr,
        const std::string& tag = "");

    Block* AddRect3(float offsetX, float offsetY,
        float width, float height,
        const char* spriteFile = nullptr,
        const std::string& tag = "");

    Block* AddRect1v(float offsetX, float offsetY,
        float width, float height,
        const char* spriteFile = nullptr,
        const std::string& tag = "");

    Block* AddRect2v(float offsetX, float offsetY,
        float width, float height,
        const char* spriteFile = nullptr,
        const std::string& tag = "");

    Block* AddRect3v(float offsetX, float offsetY,
        float width, float height,
        const char* spriteFile = nullptr,
        const std::string& tag = "");

    // ── Remoção ───────────────────────────────────────────────────
    void RemoveBlock(Block* block);
    void RemoveBlock(const std::string& tag);
    void RemoveBlock(int index);
    void RemoveBlocksInRadius(float cx, float cy, float radius);

    // ── Rotação ───────────────────────────────────────────────────
    void  Rotate(float degrees);
    void  SetAngle(float degrees);
    float GetAngle() const { return angle; }

    // ── Escala ────────────────────────────────────────────────────
    // Escala proporcional (X e Y iguais)
    void  SetScale(float s);
    // Escala independente por eixo
    void  SetScale(float sx, float sy);
    float GetScaleX() const { return scaleX; }
    float GetScaleY() const { return scaleY; }

    // ── Queda ─────────────────────────────────────────────────────
    void StartFalling(float initialVelY = 0.0f);
    void StopFalling();
    bool IsFalling() const { return falling; }

    // ── Busca / Acesso ────────────────────────────────────────────
    Block* GetBlockByTag(const std::string& tag);
    const std::vector<Block*>& GetBlocks() const { return blocks; }

    // ── Ciclo ─────────────────────────────────────────────────────
    void Update()                 override;
    void Draw()                   override;
    void OnCollision(Object* obj) override;
};

#endif

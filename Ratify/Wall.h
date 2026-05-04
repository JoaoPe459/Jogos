#ifndef _PACMAN_WALL_H_
#define _PACMAN_WALL_H_

#include "Object.h"
#include "Sprite.h"
#include "Geometry.h"

class Wall : public Object
{
private:
    Sprite* sprite = nullptr;
    float w, h;

public:
    Wall(float px, float py, float width, float height, const char* spriteFile);
    ~Wall();

    void Update();
    void Draw();
};

#endif

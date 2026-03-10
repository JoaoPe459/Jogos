#include "PacMan.h"
#include "Wall.h"

Wall::Wall(float px, float py, float width, float height, const char* spriteFile)
{
    if (spriteFile && strlen(spriteFile) > 0)
        sprite = new Sprite(spriteFile);

    w = width;
    h = height;

    BBox(new Rect(-w/2.0f, -h/2.0f, w/2.0f, h/2.0f));
    MoveTo(px, py);
    type = WALL;
}

Wall::~Wall()
{
    if (sprite)
        delete sprite;
}

void Wall::Update()
{
}

void Wall::Draw()
{
    if (sprite)
        sprite->Draw(X(), Y(), Layer::MIDDLE);
}

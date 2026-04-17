#include "Portal.h"
#include "PacMan.h"

Portal::Portal(float x, float y, int bgIndex) {
    type = PORTAL;
    targetBG = bgIndex;
    sprite = new Sprite("Resources/Portal.png");
    MoveTo(x, y);
    BBox(new Rect(-16, -16, 16, 16));
}

Portal::~Portal() {
    delete sprite;
}

void Portal::Update() {
}

void Portal::Draw() {
    if (sprite) {
        sprite->Draw(x, y, Layer::FRONT);
    }
}

void Portal::Control() {
    // Portal has no control input or AI; empty implementation to satisfy Entity interface
}

void Portal::OnCollision(Object* obj)
{
}

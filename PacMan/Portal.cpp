#include "Portal.h"
#include "PacMan.h"

Portal::Portal(float x, float y, int bgIndex) : Portal(x, y, bgIndex, true, 0.0f) {
}

Portal::Portal(float x, float y, int bgIndex, bool isOpen, float doorRotation) {
    open = isOpen;
    rotation = doorRotation;
    type = open ? PORTAL : WALL;
    targetBG = bgIndex;
    sprite = new Sprite(open ? "Resources/Doors/Porta1.png" : "Resources/Doors/Porta1fechada.png");
    MoveTo(x, y);
    BBox(new Rect(-sprite->Width() / 2.0f, -sprite->Height() / 2.0f, sprite->Width() / 2.0f, sprite->Height() / 2.0f));
}

Portal::~Portal() {
    delete sprite;
}

void Portal::Update() {
}

void Portal::Draw() {
    sprite->Draw(x, y, Layer::FRONT, 1.0f, rotation);
}

void Portal::Control() {
    // Portal has no control input or AI; empty implementation to satisfy Entity interface
}

void Portal::OnCollision(Object* obj)
{
}

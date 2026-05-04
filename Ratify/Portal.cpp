#include "Portal.h"
#include "PacMan.h"

Portal::Portal(float x, float y, int bgIndex) : Portal(x, y, bgIndex, true, 0.0f) {
}

Portal::Portal(float x, float y, int bgIndex, bool isOpen, float doorRotation) {
    open = isOpen;
    rotation = doorRotation;
    // Aberto troca de fase; fechado e apenas visual para nao bloquear o chao.
    type = open ? PORTAL : SPECIAL;
    targetBG = bgIndex;
    // Usa a sprite correspondente ao estado atual da porta.
    sprite = new Sprite(open ? "Resources/Doors/Porta1.png" : "Resources/Doors/Porta1fechada.png");
    MoveTo(x, y);
    // A bounding box acompanha a sprite para debug/visualizacao, mesmo sem colisao quando fechada.
    BBox(new Rect(-sprite->Width() / 2.0f, -sprite->Height() / 2.0f, sprite->Width() / 2.0f, sprite->Height() / 2.0f));
}

Portal::~Portal() {
    delete sprite;
}

void Portal::Update() {
}

void Portal::Draw() {
    // A rotacao aponta a porta para o centro da sala.
    sprite->Draw(x, y, Layer::FRONT, 1.0f, rotation);
}

void Portal::Control() {
    // Portal has no control input or AI; empty implementation to satisfy Entity interface
}

void Portal::OnCollision(Object* obj)
{
}

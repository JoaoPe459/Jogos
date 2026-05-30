#include "MapDoor.h"

MapDoor::MapDoor(float px, float py, int level) {
    levelId = level;
    type = 100;

    // CORREÇÃO: Tamanho ajustado para 64x64!
    tileset = new TileSet("Resources/Portajogo2.png", 96, 128, 2, 1);
    anim = new Animation(tileset, 0.1f, false);

    uint fNormal[1] = { 0 };
    uint fSelected[1] = { 1 };

    anim->Add(0, fNormal, 1);
    anim->Add(1, fSelected, 1);
    anim->Select(0);

    MoveTo(px, py, Layer::FRONT);

    // CORREÇÃO: Caixa de colisão ajustada para ficar quadrada (64x64)
    BBox(new Rect(-40, -20, 40, 64));
}

MapDoor::~MapDoor() {
    delete anim;
    delete tileset;
}

void MapDoor::Select() { anim->Select(1); }
void MapDoor::UnSelect() { anim->Select(0); }
void MapDoor::Update() { anim->NextFrame(); }

void MapDoor::Draw() {
    anim->Draw(x, y, z);
}
#include "Interactables.h"

// ─── ESPINHO ─────────────────────────────────────────────
Spike::Spike(float px, float py, const std::string& tex) {
    type = TYPE_SPIKE;
    spr = new Sprite(tex.c_str());
    MoveTo(px, py);
    // Hitbox mortal um pouco menor que a imagem para ser justo com o jogador
    BBox(new Rect(-12, -12, 12, 12));
}
Spike::~Spike() { delete spr; }
void Spike::Update() {}
void Spike::Draw() { spr->Draw(x, y, Layer::MIDDLE); }


// ─── BOTÃO ───────────────────────────────────────────────
ButtonObj::ButtonObj(float px, float py, const std::string& id) : id(id), pressed(false) {
    type = TYPE_BUTTON;

    // Supondo que sua imagem botão tenha 64x32 pixels inteira (2 colunas, 1 linha)
    // Ajuste o 32, 32 para o tamanho real do frame do seu botão!
    tileset = new TileSet("Resources/botao.png", 32, 32, 2, 1);
    anim = new Animation(tileset, 0.1f, false);

    uint framesNormal[1] = { 0 };
    uint framesPressed[1] = { 1 };
    anim->Add(0, framesNormal, 1);
    anim->Add(1, framesPressed, 1);
    anim->Select(0); // Começa não pressionado

    MoveTo(px, py);
    BBox(new Rect(-14, -8, 14, 8)); // Hitbox achatada
}
ButtonObj::~ButtonObj() { delete anim; delete tileset; }
void ButtonObj::Update() { anim->NextFrame(); }
void ButtonObj::Draw() { anim->Draw(x, y, Layer::MIDDLE); }

void ButtonObj::Press() {
    if (!pressed) {
        pressed = true;
        anim->Select(1); // Muda para a animação do botão afundado
    }
}


// ─── PORTA ───────────────────────────────────────────────
Door::Door(float px, float py, const std::string& tex) {
    type = TYPE_DOOR;
    spr = new Sprite(tex.c_str());
    MoveTo(px, py);
    BBox(new Rect(-16, -32, 16, 32)); // Hitbox alta
}
Door::~Door() { delete spr; }
void Door::Update() {}
void Door::Draw() { spr->Draw(x, y, Layer::BACK); } // Porta fica atrás do rato

DecoObj::DecoObj(float px, float py, const std::string& tex) {
    type = 999; // Tipo neutro, engine ignora reações
    spr = new Sprite(tex.c_str());

    MoveTo(px, py, Layer::BACK); // Joga lá para o fundo da tela
}

DecoObj::~DecoObj() {
    delete spr;
}

void DecoObj::Update() {}

void DecoObj::Draw() {
    // Aplica o mesmo efeito escurecido (tint) do Editor
    spr->Draw(x, y, Layer::BACK);
}
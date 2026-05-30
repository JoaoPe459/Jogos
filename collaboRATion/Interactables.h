#ifndef _INTERACTABLES_H_
#define _INTERACTABLES_H_

#include "Object.h"
#include "Sprite.h"
#include "Animation.h"
#include <string>

// Tipos de identificação para a sua engine de colisão
#define TYPE_SPIKE  101
#define TYPE_BUTTON 102
#define TYPE_DOOR   103

// ─── CLASSE ESPINHO ──────────────────────────────────────
class Spike : public Object {
private:
    Sprite* spr;
public:
    Spike(float px, float py, const std::string& tex);
    ~Spike();
    void Update() override;
    void Draw() override;
};

// ─── CLASSE BOTÃO ────────────────────────────────────────
class ButtonObj : public Object {
private:
    TileSet* tileset;
    Animation* anim;
    bool pressed;
public:
    std::string id;
    ButtonObj(float px, float py, const std::string& id);
    ~ButtonObj();
    void Update() override;
    void Draw() override;

    void Press();
    bool IsPressed() const { return pressed; }
};

// ─── CLASSE PORTA ────────────────────────────────────────
class Door : public Object {
private:
    Sprite* spr;
public:
    Door(float px, float py, const std::string& tex);
    ~Door();
    void Update() override;
    void Draw() override;
};
    
//Classe Bloco
class DecoObj : public Object {
private:
    Sprite* spr;
public:
    DecoObj(float px, float py, const std::string& tex);
    ~DecoObj();
    void Update() override;
    void Draw() override;
};

#endif
#ifndef _PORTAL_H_
#define _PORTAL_H_

#include "Entity.h"

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Portal : public Entity {
public:
    int targetBG;
    Sprite* sprite;
    Direction direction;
    Portal(float x, float y, int bgIndex);

    // Destrutor para limpar o sprite
    ~Portal();

    // IMPLEMENTAÇÃO OBRIGATÓRIA DOS MÉTODOS VIRTUAIS
    void Update() override;
    void Draw() override;
    void Control() override; // implement Control to satisfy Entity's pure virtual
    Direction GetDirection() const { return direction; }
};

#endif
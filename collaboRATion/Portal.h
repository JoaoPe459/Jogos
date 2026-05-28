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
    float rotation = 0.0f;
    bool open = true;
    Portal(float x, float y, int bgIndex);
    Portal(float x, float y, int bgIndex, bool isOpen, float doorRotation);

    // Destrutor para limpar o sprite
    ~Portal();

    // IMPLEMENTAÇÃO OBRIGATÓRIA DOS MÉTODOS VIRTUAIS
    void Update() override;
    void Draw() override;
    void Control() override; // implement Control to satisfy Entity's pure virtual
	void OnCollision(Object* obj) override;
    Direction GetDirection() const { return direction; }
};

#endif

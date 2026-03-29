#ifndef _ENEMY_H_
#define _ENEMY_H

#include "Object.h"
#include "Moves.h"
#include "Sprite.h"

class Enemy : public Object {
protected:
    Moves* moves;
    Sprite* sprite;
    // Possiveis objetos a serem incrementado aqui
    int health;

    void ScreenWrap();

public:
    Enemy();
    virtual ~Enemy();

    // Métodos base que podem ser sobrescritos
    virtual void Update() override;
    virtual void Draw() override;
    virtual void OnCollision(Object* obj) override;

    // IA básica: cada inimigo terá sua própria lógica aqui
    virtual void IA() = 0;
};

#endif
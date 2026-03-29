#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "Object.h"
#include "Moves.h"
#include "Sprite.h"

class Entity : public Object {
protected:
    Moves* moves = nullptr;

public:
    Entity();
    virtual ~Entity();

    bool alive = true;

    // Ciclo de vida comum
    virtual void Update() override;
    virtual void Draw() override;
    virtual void OnCollision(Object* obj) override;

    // Comportamento obrigatório (Teclado para Player, IA para Fantasmas)
    virtual void Control() = 0;

    // Métodos utilitários de física
    void ApplyPhysics();
    void HandleScreenWrap();
};

#endif
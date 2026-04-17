#ifndef _PORTAL_H_
#define _PORTAL_H_

#include "Entity.h"

class Portal : public Entity {
public:
    int targetBG;
    Sprite* sprite;

    // Construtor (já definido no seu .cpp)
    Portal(float x, float y, int bgIndex);

    // Destrutor para limpar o sprite
    ~Portal();

    // IMPLEMENTAÇÃO OBRIGATÓRIA DOS MÉTODOS VIRTUAIS
    void Update() override;
    void Draw() override;
    void Control() override; // implement Control to satisfy Entity's pure virtual
};

#endif
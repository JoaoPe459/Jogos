#ifndef _ATTACK_H_
#define _ATTACK_H_

#include "Entity.h"

class Attack : public Entity {
private:
    float timer;      // Cronômetro de vida
    float duration;   // Duração total
    Entity* owner;    // Quem disparou o ataque

public:
    // Construtor: Criador, tempo de vida, e impulsos iniciais (opcionais)
    Attack(Entity* creator, float lifeTime, float impulseX = 0.0f, float impulseY = 0.0f);

    // Destrutor
    ~Attack();

    // Métodos obrigatórios da Entity
    void Update() override;
    void OnCollision(Object* obj) override;
    void Draw() override;
    void Control() override; // Implementar Control para evitar que Attack seja abstrato

    // Getter para identificar o dono
    Entity* GetOwner() const;
};

#endif
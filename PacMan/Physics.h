#pragma once
#ifndef _PHYSICS_H_
#define _PHYSICS_H_

class Physics {
public:
    static float GravityValue;
    static float Direction;

    // Construtor: permite instanciar se quiser, mas a lógica permanece estática
    Physics();

    // Método para configurar valores iniciais (útil no Init do Level)
    static void Setup(float gravity, float direction = 1.0f) {
        GravityValue = gravity;
        Direction = direction;
    }

    static float GetGravity() {
        return GravityValue * Direction;
    }

    static void Invert() {
        Direction *= -1.0f;
    }
};

#endif
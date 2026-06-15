/**********************************************************************************
// Platform (Arquivo de Cabeçalho)
//
// Criação:     15 Jun 2026
// Compilador:  Visual C++ 2022
//
// Descrição:   Define a classe Platform, que serve como parede/chão no jogo.
//              Player e BaseEnemy não conseguem atravessá-la.
//              A resolução de colisão empurra o objeto para fora com base
//              no eixo de menor penetração (AABB).
//
**********************************************************************************/

#ifndef _GEOWARS_PLATFORM_H_
#define _GEOWARS_PLATFORM_H_

// ---------------------------------------------------------------------------------

#include "Object.h"
#include "Sprite.h"

// ---------------------------------------------------------------------------------

class Platform : public Object
{
private:
    Sprite* sprite;                     // sprite da plataforma
    float   width;                      // largura da plataforma
    float   height;                     // altura da plataforma

    // resolve colisão com objeto que possui bounding box retangular
    void ResolveRectCollision(Rect* platRect, Object* obj, Rect* objRect);

    // resolve colisão com objeto que possui bounding box circular
    void ResolveCircleCollision(Rect* platRect, Object* obj, Circle* objCircle);

public:
    Platform(float px, float py,        // posição (centro)
             float w, float h,          // dimensões
             const string& imgFile = ""); // arquivo de imagem (opcional)

    ~Platform();                        // destrutor

    void Update();                      // atualização (estático)
    void Draw();                        // desenho
    void OnCollision(Object* obj);      // resolução de colisão
};

// ---------------------------------------------------------------------------------

#endif

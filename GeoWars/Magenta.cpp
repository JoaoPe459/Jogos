/**********************************************************************************
// Magenta (Código Fonte)
// 
// Criação:     10 Out 2012
// Atualização: 01 Nov 2021
// Compilador:  Visual C++ 2022
//
// Descrição:   Objeto faz um perseguição direta
//
**********************************************************************************/

#include "GeoWars.h"
#include "Magenta.h"
#include "Random.h" 
#include "IDamageable.h"

// ---------------------------------------------------------------------------------

Magenta::Magenta(Player * p) : player (p)
{
    sprite = new Sprite("Resources/Magenta.png");
    speed  = new Vector(0, 2.0f);
    BBox(new Circle(18.0f));
    
    // move para uma posição aleatória (canto inferior esquerdo)
    Random posX{ 300.0f, 400.0f };
    Random posY{ game->Height() - 400.0f, game->Height() - 300.0f };
    MoveTo(posX.Rand(), posY.Rand());

    type = MAGENTA;
}

// ---------------------------------------------------------------------------------

Magenta::~Magenta()
{
    delete sprite;
    delete speed;
}

// -------------------------------------------------------------------------------

void Magenta::OnCollision(Object * obj)
{
    if (obj->Type() == MISSILE)
        GeoWars::scene->Delete(this, MOVING);
}

// -------------------------------------------------------------------------------

void Magenta::Update()
{
    // ajusta ângulo do vetor 
    speed->RotateTo(Line::Angle(Point(x, y), Point(player->X(), player->Y())));
    Rotate(200 * gameTime);

    // movimenta objeto pelo seu vetor velocidade
    Translate(speed->XComponent() * 50.0f * gameTime, -speed->YComponent() * 50.0f * gameTime);
}

// -------------------------------------------------------------------------------
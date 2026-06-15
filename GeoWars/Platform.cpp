/**********************************************************************************
// Platform (Código Fonte)
//
// Criação:     15 Jun 2026
// Compilador:  Visual C++ 2022
//
// Descrição:   Implementação da classe Platform.
//              Plataforma estática que bloqueia Player e BaseEnemy usando
//              resolução de colisão AABB por eixo de menor penetração.
//
**********************************************************************************/

#include "Platform.h"
#include "Engine.h"
#include "GeoWars.h"
#include "IDamageable.h"
#include <cmath>
#include <algorithm>

using std::min;
using std::max;
using std::clamp;

// -------------------------------------------------------------------------------

Platform::Platform(float px, float py, float w, float h, const string& imgFile)
    : width(w), height(h)
{
    // posiciona a plataforma
    MoveTo(px, py);

    // bounding box retangular centrada no objeto
    // os parâmetros do Rect são coordenadas locais:
    // metade esquerda/superior negativa, metade direita/inferior positiva
    BBox(new Rect(-w / 2.0f, -h / 2.0f, w / 2.0f, h / 2.0f));

    type = PLATFORM;

    // carrega sprite se arquivo foi informado
    if (!imgFile.empty())
        sprite = new Sprite(imgFile);
    else
        sprite = nullptr;
}

// -------------------------------------------------------------------------------

Platform::~Platform()
{
    delete sprite;
}

// -------------------------------------------------------------------------------

void Platform::Update()
{
    // objeto estático — sem atualização necessária
}

// -------------------------------------------------------------------------------

void Platform::Draw()
{
    if (sprite)
    {
        // desenha sprite esticado para cobrir a plataforma
        float texW = (float)sprite->Width();
        float texH = (float)sprite->Height();

        if (texW > 0 && texH > 0)
        {
            float scaleX = width / texW;
            float scaleY = height / texH;
            sprite->Draw(x, y, Layer::LOWER, min(scaleX, scaleY));
        }
        else
        {
            sprite->Draw(x, y, Layer::LOWER);
        }
    }
}

// -------------------------------------------------------------------------------

void Platform::OnCollision(Object* obj)
{
    if (!obj || !obj->BBox())
        return;

    // só bloqueia Player e inimigos
    uint objType = obj->Type();
    if (objType != PLAYER && objType != ENEMY)
        return;

    Rect* platRect = (Rect*)BBox();
    Geometry* objGeom = obj->BBox();

    if (objGeom->Type() == RECTANGLE_T)
    {
        ResolveRectCollision(platRect, obj, (Rect*)objGeom);
    }
    else if (objGeom->Type() == CIRCLE_T)
    {
        ResolveCircleCollision(platRect, obj, (Circle*)objGeom);
    }
    // outros tipos de geometria (Point, Poly, Mixed) são ignorados
    // pois Player e BaseEnemy usam Rect ou Circle
}

// -------------------------------------------------------------------------------
// ResolveRectCollision
//
// Calcula a penetração nos dois eixos entre dois retângulos e empurra
// o objeto para fora pelo eixo de menor penetração.
// -------------------------------------------------------------------------------

void Platform::ResolveRectCollision(Rect* platRect, Object* obj, Rect* objRect)
{
    // calcula penetração em cada direção
    float penLeft   = objRect->Right()  - platRect->Left();
    float penRight  = platRect->Right() - objRect->Left();
    float penTop    = objRect->Bottom() - platRect->Top();
    float penBottom = platRect->Bottom() - objRect->Top();

    // encontra o eixo de menor penetração
    float minPenX = min(penLeft, penRight);
    float minPenY = min(penTop, penBottom);

    if (minPenX < minPenY)
    {
        // empurra horizontalmente
        if (penLeft < penRight)
            obj->Translate(-penLeft, 0.0f);
        else
            obj->Translate(penRight, 0.0f);
    }
    else
    {
        // empurra verticalmente
        if (penTop < penBottom)
            obj->Translate(0.0f, -penTop);
        else
            obj->Translate(0.0f, penBottom);
    }
}

// -------------------------------------------------------------------------------
// ResolveCircleCollision
//
// Encontra o ponto mais próximo do retângulo ao centro do círculo.
// Se a distância for menor que o raio, empurra o círculo para fora.
// -------------------------------------------------------------------------------

void Platform::ResolveCircleCollision(Rect* platRect, Object* obj, Circle* objCircle)
{
    float cx = objCircle->CenterX();
    float cy = objCircle->CenterY();
    float r  = objCircle->Radius();

    // ponto do retângulo mais próximo do centro do círculo
    float closestX = clamp(cx, platRect->Left(), platRect->Right());
    float closestY = clamp(cy, platRect->Top(),  platRect->Bottom());

    float dx = cx - closestX;
    float dy = cy - closestY;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist < r)
    {
        if (dist > 0.001f)
        {
            // centro do círculo está fora do retângulo:
            // empurra na direção do vetor centro→ponto mais próximo
            float pen = r - dist;
            float nx  = dx / dist;   // normal da superfície
            float ny  = dy / dist;
            obj->Translate(nx * pen, ny * pen);
        }
        else
        {
            // centro do círculo está dentro do retângulo:
            // empurra para fora pelo eixo de menor penetração
            float penTop    = cy - platRect->Top();
            float penBottom = platRect->Bottom() - cy;
            float penLeft   = cx - platRect->Left();
            float penRight  = platRect->Right() - cx;

            float minPen = min(min(penTop, penBottom), min(penLeft, penRight));
            r += 1.0f;  // pequena margem

            if (minPen == penTop)
                obj->MoveTo(cx, platRect->Top() - r);
            else if (minPen == penBottom)
                obj->MoveTo(cx, platRect->Bottom() + r);
            else if (minPen == penLeft)
                obj->MoveTo(platRect->Left() - r, cy);
            else
                obj->MoveTo(platRect->Right() + r, cy);
        }
    }
}

// -------------------------------------------------------------------------------

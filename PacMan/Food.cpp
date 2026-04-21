#include "Food.h"
#include "LevelMake.h"

Food::Food()
{
    sprite = foodSprite;
    BBox(new Rect(-5, -5, 5, 5));
    type = FOOD;

    // 1. Define uma margem para a comida não nascer "colada" no canto da tela
    float margin = 40.0f;

    float randomX = (float)(rand() % (int)(window->Width() - (margin * 2))) + margin;
    float randomY = (float)(rand() % (int)(window->Height() - (margin * 2))) + margin;

    // 3. Move o objeto para a posição sorteada
    this->MoveTo(randomX, randomY);

    moves->setSpeed(0.0f);
    moves->setVelX(0.0f);
    moves->setVelY(0.0f);
}

Food::~Food()
{
}

void Food::Draw()
{
    if (sprite) {
        sprite->Draw(x, y, z);
    }
}

void Food::OnCollision(Object* obj) 
{
    if (obj->Type() == PLAYER) {
        LevelMake* lvl = static_cast<LevelMake*>(Engine::game);
        lvl->GetScene()->Delete(this, STATIC); 
    }
}

void Food::Update()
{
    // 3. Totalmente estática: não processa controle nem física de movimento
}

void Food::Control()
{
    // Vazio: sem inteligência de movimento
}

void Food::HandleScreenWrap()
{
    // Vazio: não atravessa bordas
}

void Food::RespawnAtEdge()
{
    // Vazio ou Removido: como ela não deve "renascer", esta função perde o propósito
}
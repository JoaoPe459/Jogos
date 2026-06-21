/**********************************************************************************
// Crawler (Arquivo de Cabeçalho + Código Fonte)
//
// Descrição:   Inimigo básico que patrulha o chão.
//              - Patrulha horizontal, vira ao bater em parede/beira
//              - Ao ver o player (linha de visada) acelera em sua direção
//              - Ataca por contato
//
**********************************************************************************/

#ifndef CRAWLER_H
#define CRAWLER_H

#include "BaseEnemy.h"
#include "Sprite.h"
#include "TileSet.h"
#include "Animation.h"
// -------------------------------------------------------------------------------

class Crawler : public BaseEnemy
{
public:
    Crawler(float startX, float startY);
    ~Crawler();

protected:
    void UpdateAI(float dt) override;
    void DrawSprite()       override;
	void OnCollision(Object* obj) override;
private:

    TileSet* animation = nullptr;
    Animation* anim;


    float   patrolSpeed;
    float   chaseSpeed;
    float   patrolTimer;       // tempo até virar durante patrulha
    float   edgeCheckOffset;   // distância à frente para checar beira
    float   spawnX;

    bool    CheckEdgeAhead() const;   // true se há vazio à frente
};


#endif

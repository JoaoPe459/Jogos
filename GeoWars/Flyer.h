/**********************************************************************************
// Flyer (Arquivo de Cabeçalho + Código Fonte)
//
// Descrição:   Inimigo voador que circula e mergulha sobre o player.
//              - Flutua em órbita acima do player quando longe
//              - Mergulha em linha reta ao entrar em range
//              - Sem gravidade (voa livremente)
//
**********************************************************************************/

#ifndef FLYER_H
#define FLYER_H

#include "BaseEnemy.h"
#include "Sprite.h"
#include "Animation.h"
#include "TileSet.h"

// -------------------------------------------------------------------------------

class Flyer : public BaseEnemy
{
public:
    Flyer(float startX, float startY);
    ~Flyer();

protected:
    void UpdateAI(float dt) override;
    void DrawSprite()       override;
    void ApplyGravity(float dt) {} // sem gravidade

private:

    TileSet* animation;               // folha de sprites do personagem
    Animation* anim;                   // animação do personagem


    float   orbitAngle;        // ângulo atual de órbita
    float   orbitRadius;       // raio da órbita ao redor de um ponto
    float   orbitSpeed;        // velocidade angular
    float   diveSpeed;         // velocidade do mergulho
    float   diveTimer;         // tempo de mergulho ativo
    float   diveCooldown;      // cooldown entre mergulhos
    float   idleY;             // altura de patrulha

    bool    diving;
};


#endif

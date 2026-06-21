/**********************************************************************************
// GeoWars (Arquivo de Cabeçalho)
// 
// Criação:     23 Out 2012
// Atualização: 01 Nov 2021
// Compilador:  Visual C++ 2022
//
// Descrição:   Demonstração da versão final do motor
//
**********************************************************************************/

#ifndef _GEOWARS_H_
#define _GEOWARS_H_

// ------------------------------------------------------------------------------

#include "Game.h"
#include "Audio.h"
#include "Scene.h"
#include "Background.h"
#include "Player.h"
#include "Hud.h"
#include "BaseEnemy.h"




struct EnemyManager {
    BaseEnemy* list[100]; // Vetor primitivo de ponteiros
    int count;            // Quantidade atual de inimigos no mapa

    EnemyManager() : count(0) {}

    bool Add(BaseEnemy* enemy);
    void Remove(BaseEnemy* enemy);
    void Clear();
};

// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------

class GeoWars : public Game
{
private:
    Background * backg = nullptr;   // pano de fundo
    Hud * hud = nullptr;            // painel de informações
    bool viewBBox = false;          // visualização das bouding boxes

public:
    static Player * player;         // nave controlada pela jogador
    static Audio * audio;           // sitema de áudio
    static Scene * scene;           // cena do jogo
    static bool viewHUD;            // visualização do painel
    static EnemyManager enemies;

    void Init();                    // inicialização
    void Update();                  // atualização
    void Draw();                    // desenho
    void Finalize();                // finalização
};

// ---------------------------------------------------------------------------------

#endif
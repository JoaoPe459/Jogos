/**********************************************************************************
// Level1 (Arquivo de Cabeçalho)
//
// Criação:     18 Jan 2013
// Atualização: 04 Mar 2023
// Compilador:  Visual C++ 2022
//
// Descrição:   Nível 1 do jogo PacMan
//
**********************************************************************************/

#ifndef _PACMAN_LEVELMAKE_H_
#define _PACMAN_LEVELMAKE_H_

// ------------------------------------------------------------------------------
// Inclusões

#include "Game.h"
#include "Scene.h"
#include "Entity.h"
#include <vector>
#include <string>
#include "Physics.h"
#include "Player.h"
#include "Engine.h"
#include "Ghost.h"
#include "Food.h"
#include "Wall.h"



// ------------------------------------------------------------------------------
class Home;
class LevelMake : public Game
{
protected:
    Sprite* backg = nullptr;       // background
    Scene* scene = nullptr;        // gerenciador de cena
    Sprite* foodSprite = nullptr;  // Sprite da comida
    bool viewBBox = false;          // habilita visualização da bounding box

    int MAX_FOOD = 0;
    int MAX_GHOSTS = 0;

    Food** foods;            // Array dinâmico de ponteiros para Food
    Entity** entities;       // Array dinâmico para Player e Ghosts
    int foodCount;
    int entityCount;
public:
    void Init() override;
    void Init(float, int, int, std::string);              // inicializa jogo
    void Update() override;                  // atualiza lógica do jogo
    void Draw() override;                    // desenha jogo
    void Finalize() override;                // finaliza jogo

    // Acesso à cena para permitir que outras classes adicionem objetos
    Scene* GetScene() { return scene; }
    void ghostInit(); 
    void foodInit();
    void GenerateMaze(Scene* scene, Window* window, int tileSize);


};


// -----------------------------------------------------------------------------

#endif
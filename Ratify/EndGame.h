#pragma once

#ifndef _PACMAN_ENDGAME_H_
#define _PACMAN_ENDGAME_H_

#include "Game.h"
#include "Sprite.h"
#include "Font.h"
#include "Engine.h"
#include "Home.h"
#include "Player.h"
class Home;
class Player;
class EndGame : public Game
{

protected:
	Sprite* backg = nullptr;
	Sprite* ratoBeta = nullptr;
	Sprite* ratoChad = nullptr;
	Sprite* backg2 = nullptr;
	Font* consolas = nullptr;
	Font* terminal = nullptr;
	int l_damageTaken = 0;
	int l_damageDealt = 0;
	int l_enemiesDefeated = 0;
	int l_levelsVisited = 0;
	string l_status = "";
	int l_stage = 0;
	float l_endGame = 0.0f;

public:
	void Init();                    // inicializa jogo
	void Update() override;          // atualiza lógica do jogo
	void Draw();            // desenha jogo
	void Finalize() override;        // finaliza jogo
	void DrawCentralMessage(const std::string& text, Color color, float x, float y);
};

#endif
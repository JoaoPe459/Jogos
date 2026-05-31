#include "EndGame.h"
#include <fstream>
#include "LevelMake.h"
//Vamos usar

void EndGame::Init()
{
    backg = new Sprite("Resources/vitoria.png");
    consolas = new Font("Resources/consolas12.png");
    terminal = new Font("Resources/terminal.png");

    consolas->Spacing(9);
    terminal->Spacing(30);
    // LER O ARQUIVO
    std::ifstream file("player_stats.txt");
    std::string line;

}

void EndGame::Update()
{

    //Sair do jogo
    if (window->KeyPress(VK_ESCAPE))
        window->Close();

    // passa ao Home com ENTER
    if (window->KeyPress(VK_RETURN))
        Engine::Next<Home>();
}

void EndGame::Draw()
{
    if (backg) backg->Draw(window->CenterX(), window->CenterY(), Layer::BACK);

    if (consolas != nullptr)
    {
      terminal->Draw(100, 700, "BOA GOSTOSÃO", Color(1, 1, 0, 1));
      terminal->Draw(100, 500, "Total de Mortes: " + std::to_string(LevelMake::deathCount), Color(1, 1, 0, 1));

    }
}

void EndGame::Finalize()
{
	delete backg;
	backg = nullptr;
	delete consolas;
	consolas = nullptr;
}

void EndGame::DrawCentralMessage(const std::string& text, Color color, float x, float y) {
    if (text.empty() || consolas == nullptr) return;

    // Se você passar x = -1, a função centraliza automaticamente no eixo X
    float finalX = x;
    if (x == -1.0f) {
        float textWidth = text.length() * 9.0f;
        finalX = window->CenterX() - (textWidth / 2.0f);
    } 

    consolas->Draw(finalX, y, text, color); 
}

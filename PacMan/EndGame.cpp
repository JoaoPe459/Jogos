#include "EndGame.h"
#include <fstream>
void EndGame::Init()
{
    backg = new Sprite("Resources/EndGame.png");
    consolas = new Font("Resources/consolas12.png");

    // LER O ARQUIVO
    std::ifstream file("player_stats.txt");
    std::string line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            // Procura pela label e extrai o valor após os dois pontos ":"
            if (line.find("Total de dano sofrido:") != std::string::npos) {
                l_damageTaken = std::stoi(line.substr(line.find(":") + 1));
            }
            else if (line.find("Total de dano deferido:") != std::string::npos) {
                l_damageDealt = std::stoi(line.substr(line.find(":") + 1));
            }
            else if (line.find("Total de inimigos derrotados:") != std::string::npos) {
                l_enemiesDefeated = std::stoi(line.substr(line.find(":") + 1));
            }
            else if (line.find("Total de fases visitadas:") != std::string::npos) {
                l_levelsVisited = std::stoi(line.substr(line.find(":") + 1));
            }
            else if (line.find("Player morreu no estágio:") != std::string::npos) {
                l_stage = std::stoi(line.substr(line.find(":") + 1));
            }
            else if (line.find("Status final do player:") != std::string::npos) {
                l_status = line.substr(line.find(":") + 2); // +2 para pular ":" e o espaço
            }
        }
        file.close();
    }
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
    if (backg) backg->Draw(window->CenterX(), window->CenterY());

    if (consolas != nullptr)
    {
        // Lógica do Rato baseada no status lido do arquivo
        if (l_status == "VIVO") {
            // Desenha Rato Chad
        }
        else {
            // Desenha Rato Beta
        }

        // Exibe os dados lidos
        DrawCentralMessage("Total de dano sofrido : " + std::to_string(l_damageTaken), Color(1.0f, 0.5f, 0.0f, 1.0f), 0.0f, 40.0f);
        DrawCentralMessage("Total de dano deferido : " + std::to_string(l_damageDealt), Color(1.0f, 0.5f, 0.0f, 1.0f), 0.0f, 80.0f);
        DrawCentralMessage("Inimigos derrotados: " + std::to_string(l_enemiesDefeated), Color(1.0f, 0.5f, 0.0f, 1.0f), 0.0f, 120.0f);
        DrawCentralMessage("Fases visitadas : " + std::to_string(l_levelsVisited), Color(1.0f, 0.5f, 0.0f, 1.0f), 0.0f, 160.0f);
        DrawCentralMessage("Morreu no Estágio: " + std::to_string(l_stage), Color(1.0f, 0.0f, 0.0f, 1.0f), 0.0f, 200.0f);
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
        float textWidth = text.length() * 8.0f;
        finalX = window->CenterX() - (textWidth / 2.0f);
    }

    consolas->Draw(finalX, y, text, color);
}

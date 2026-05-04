#include "EndGame.h"
#include <fstream>
void EndGame::Init()
{
    backg = new Sprite("Resources/Background/EndGame.jpg");
    backg2 = new Sprite("Resources/Background/BlackPainel.png");
    ratoChad = new Sprite("Resources/Player/RatoChad1.png");
    ratoBeta = new Sprite("Resources/Player/RatoTriste.png");
    consolas = new Font("Resources/consolas12.png");
    terminal = new Font("Resources/terminal.png");

    consolas->Spacing(9);
    terminal->Spacing(30);
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
            else if (line.find("Tempo de jogo:") != std::string::npos) {
                l_endGame = std::stof(line.substr(line.find(":") + 1));
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
    if (backg) backg->Draw(window->CenterX(), window->CenterY(), Layer::BACK);
    if (backg2) backg2->Draw(950, window->CenterY(), Layer::MIDDLE, 0.5);

    if (consolas != nullptr)
    {
        // Lógica do Rato baseada no status lido do arquivo
        if (l_status == "VIVO") {
            ratoChad->Draw(350, window->CenterY(), Layer::FRONT, 0.5);
            terminal->Draw(100, 700, "PARABENS VOCE VENCEU!!!", Color(1,1,0,1));
            
        }
        else {
            ratoBeta->Draw(350, window->CenterY(), Layer::FRONT, 0.5);
            terminal->Draw(100, 700, "NAO FOI DESSA VEZ!!!", Color(1, 1, 0, 1));
        }

        // Exibe os dados lidos
        // 1. Defina as cores antes
        Color corTexto = Color(0.95f, 0.93f, 0.88f, 1.0f); // Creme Claro
        Color corMorte = Color(0.85f, 0.0f, 0.00f, 1.0f); // Laranja Queimado / Terracota (banco)
        Color corTempo = Color(0.35f, 0.65f, 0.85f, 1.0f); // Verde Folha Suave (árvore)

        // 2. Desenhe os textos
        DrawCentralMessage("Total de dano sofrido : " + std::to_string(l_damageTaken), corTexto, 820.0f, 240.0f);
        DrawCentralMessage("Total de dano deferido : " + std::to_string(l_damageDealt), corTexto, 820.0f, 260.0f);
        DrawCentralMessage("Inimigos derrotados: " + std::to_string(l_enemiesDefeated), corTexto, 820.0f, 280.0f);
        DrawCentralMessage("Fases visitadas : " + std::to_string(l_levelsVisited), corTexto, 820.0f, 300.0f);
        DrawCentralMessage("Terminou no Estagio: " + std::to_string(l_stage), corMorte, 820.0f, 320.0f);
        DrawCentralMessage("Tempo de jogo: " + std::to_string((int)l_endGame) + " Segundos", corTempo, 820.0f, 340.0f);
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

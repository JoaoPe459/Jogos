#include "Engine.h"
#include "Home.h"
#include "Level2.h"
#include "LevelSelect.h"

// ─────────────────────────────────────────────────────────────────────────────
// Init
// ─────────────────────────────────────────────────────────────────────────────
void Level2::Init()
{
    LevelMake::Init(600.0f, 0, 0, "Resources/Level2.jpg");

    if (!LevelMake::mapasDoNivel.empty() && LevelMake::faseAtual < (int)LevelMake::mapasDoNivel.size())
    {
        LoadLevel2(LevelMake::mapasDoNivel[LevelMake::faseAtual]);
    }
    else
    {
        LoadLevel2("Resources/Level_Editor_Output.txt");
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Update
// ─────────────────────────────────────────────────────────────────────────────
void Level2::Update()
{
    if (window->KeyPress(VK_ESCAPE))
    {
        Engine::Next<Home>();
        return;
    }

    // Debug: girar o spinner manualmente pelo id "spin"
    auto itSpin = parsedWalls.find("spin");
    if (itSpin != parsedWalls.end())
    {
        if (window->KeyDown('R'))  itSpin->second->Rotate(+180.0f * gameTime);
        if (window->KeyDown('E'))  itSpin->second->Rotate(-180.0f * gameTime);
        if (window->KeyPress('1')) itSpin->second->SetAngle(0.0f);
        if (window->KeyPress('2')) itSpin->second->SetAngle(45.0f);
        if (window->KeyPress('3')) itSpin->second->SetAngle(90.0f);
    }

    auto itCeil = parsedWalls.find("ceil");
    if (itCeil != parsedWalls.end())
    {
        if (window->KeyPress('F')) itCeil->second->StartFalling();
        if (window->KeyPress('G')) itCeil->second->StopFalling();
    }

    LevelMake::Update();

    if (LevelMake::avancarFase)
    {
        LevelMake::avancarFase = false;
        LevelMake::faseAtual++;

        if (LevelMake::faseAtual >= (int)LevelMake::mapasDoNivel.size())
        {
            int mundoAtual = 0;
            if (!LevelMake::mapasDoNivel.empty())
            {
                const std::string& primeiro = LevelMake::mapasDoNivel[0];
                // Procura "Mundo" seguido de dígito no caminho
                size_t pos = primeiro.find("Mundo");
                if (pos != std::string::npos && pos + 5 < primeiro.size())
                    mundoAtual = primeiro[pos + 5] - '0'; // '1','2','3'...
            }
  
            LevelMake::nivelDesbloqueado++;
            LevelMake::SalvarProgresso();
            Engine::Next<LevelSelect>();
        }
        else
        {
            Engine::Next<Level2>();
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Finalize
// ─────────────────────────────────────────────────────────────────────────────
void Level2::Finalize()
{
    LevelMake::Finalize();
}

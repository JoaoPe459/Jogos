#include "Engine.h"
#include "Home.h"
#include "Level2.h"

// ─────────────────────────────────────────────────────────────────────────────
// Init
// ─────────────────────────────────────────────────────────────────────────────

void Level2::Init()
{
    // Inicializa a base (gravidade padrão; pode ser sobrescrita pelo GRAVITY do txt)
    LevelMake::Init(600.0f, 0, 0, "Resources/Level2.jpg");

    // Carrega toda a geometria, killzones e mecânicas do arquivo de texto
    LoadLevel2("Resources/Level_Editor_Output.txt");
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

    // Debug: girar o spinner manualmente pelo id "spin" (se existir no txt)
    auto itSpin = parsedWalls.find("spin");
    if (itSpin != parsedWalls.end())
    {
        if (window->KeyDown('R'))  itSpin->second->Rotate(+180.0f * gameTime);
        if (window->KeyDown('E'))  itSpin->second->Rotate(-180.0f * gameTime);
        if (window->KeyPress('1')) itSpin->second->SetAngle(0.0f);
        if (window->KeyPress('2')) itSpin->second->SetAngle(45.0f);
        if (window->KeyPress('3')) itSpin->second->SetAngle(90.0f);
    }

    // Debug: forçar queda/parada da parede "ceil"
    auto itCeil = parsedWalls.find("ceil");
    if (itCeil != parsedWalls.end())
    {
        if (window->KeyPress('F')) itCeil->second->StartFalling();
        if (window->KeyPress('G')) itCeil->second->StopFalling();
    }

    // LevelMake::Update() roda scene, colisões, HUD e UpdateParsedMechanics()
    LevelMake::Update();
}

// ─────────────────────────────────────────────────────────────────────────────
// Finalize
// ─────────────────────────────────────────────────────────────────────────────

void Level2::Finalize()
{
    LevelMake::Finalize();
}

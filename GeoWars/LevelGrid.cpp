/**********************************************************************************
// LevelGrid (Código Fonte)
//
// Criação:     15 Jun 2026
// Compilador:  Visual C++ 2022
//
// Descrição:   Implementação da classe LevelGrid.
//              Editor de nível primitivo baseado em uma matriz 2D.
//              Cada célula pode conter um valor do enum SpawnType,
//              e o método SpawnAll() instancia os objetos correspondentes.
//
//              O nível é carregado de um arquivo .txt onde cada caractere
//              representa o tipo de spawn de uma célula.
//
//              Mapeamento de caracteres:
//                '.' ou '0' = vazio
//                '#' ou '1' = plataforma
//                'P' ou '2' = spawn do jogador
//                'C' ou '3' = Crawler
//                'F' ou '4' = Flyer
//
**********************************************************************************/

#include "LevelGrid.h"
#include "Engine.h"
#include "GeoWars.h"
#include "Platform.h"
#include "Crawler.h"
#include "Flyer.h"

#include <fstream>
#include <algorithm>
using std::ifstream;
using std::max;
using std::min;

// -------------------------------------------------------------------------------
// Construtor — inicializa grade vazia (0×0)
// -------------------------------------------------------------------------------

LevelGrid::LevelGrid()
    : cols(0), rows(0), cellSize(0)
{
}

// -------------------------------------------------------------------------------
// CharToSpawnType — converte caractere do mapa para tipo de spawn
// -------------------------------------------------------------------------------

int LevelGrid::CharToSpawnType(char ch)
{
    switch (ch)
    {
    case '#': case '1':  return SPAWN_PLATFORM;
    case 'P': case '2':  return SPAWN_PLAYER;
    case 'C': case '3':  return SPAWN_CRAWLER;
    case 'F': case '4':  return SPAWN_FLYER;

    // —— novos tipos podem ser adicionados aqui ——

    default:             return SPAWN_EMPTY;   // '.' '0' espaços etc.
    }
}

// -------------------------------------------------------------------------------
// Clear — preenche toda a grade com SPAWN_EMPTY
// -------------------------------------------------------------------------------

void LevelGrid::Clear()
{
    for (auto& row : grid)
        fill(row.begin(), row.end(), SPAWN_EMPTY);
}

// -------------------------------------------------------------------------------
// SetCell — define o tipo de spawn de uma célula
// -------------------------------------------------------------------------------

void LevelGrid::SetCell(int col, int row, SpawnType type)
{
    if (col >= 0 && col < cols && row >= 0 && row < rows)
        grid[row][col] = type;
}

// -------------------------------------------------------------------------------
// GetCell — retorna o tipo de spawn de uma célula
// -------------------------------------------------------------------------------

int LevelGrid::GetCell(int col, int row) const
{
    if (col >= 0 && col < cols && row >= 0 && row < rows)
        return grid[row][col];
    return SPAWN_EMPTY;
}

// -------------------------------------------------------------------------------
// LoadFromFile
//
// Lê o nível de um arquivo .txt onde:
//   - Cada linha corresponde a uma fileira da grade
//   - Cada caractere define o tipo de spawn da célula
//   - Linhas em branco são ignoradas
//   - Linhas mais curtas são completadas com vazio
//
// A grade é redimensionada para caber o conteúdo do arquivo.
// O tamanho da célula é calculado preenchendo a largura do mundo.
// Retorna true em caso de sucesso.
// -------------------------------------------------------------------------------

bool LevelGrid::LoadFromFile(const string& filename)
{
    ifstream file(filename);
    if (!file.is_open())
        return false;

    // —— 1ª passagem: ler todas as linhas, descobrir dimensões ——
    vector<string> lines;
    string line;
    int maxCols = 0;

    while (getline(file, line))
    {
        // ignora linhas completamente vazias
        if (line.empty())
            continue;

        // remove '\r' do Windows (CRLF → LF)
        if (line.back() == '\r')
            line.pop_back();

        lines.push_back(line);
        maxCols = max(maxCols, static_cast<int>(line.size()));
    }
    file.close();

    if (lines.empty())
        return false;

    // —— define dimensões da grade ——
    rows = static_cast<int>(lines.size());
    cols = maxCols;

    // calcula tamanho da célula para preencher a largura do mundo
    cellSize = Engine::game->Width() / static_cast<float>(cols);

    // —— 2ª passagem: preencher a matriz ——
    grid.assign(rows, vector<int>(cols, SPAWN_EMPTY));

    for (int r = 0; r < rows; r++)
    {
        const string& rowStr = lines[r];
        for (int c = 0; c < cols; c++)
        {
            if (c < static_cast<int>(rowStr.size()))
                grid[r][c] = CharToSpawnType(rowStr[c]);
            else
                grid[r][c] = SPAWN_EMPTY;
        }
    }

    return true;
}

// -------------------------------------------------------------------------------
// LoadTestLevel — nível de fallback hardcoded
// -------------------------------------------------------------------------------

void LevelGrid::LoadTestLevel()
{
    // usa tamanhos do mundo para calcular grade
    cols = static_cast<int>(Engine::game->Width() / 120.0f);
    rows = static_cast<int>(Engine::game->Height() / 120.0f);
    cellSize = 120.0f;
    grid.assign(rows, vector<int>(cols, SPAWN_EMPTY));

    Clear();

    // índices das bordas da arena
    int groundRow    = rows - 2;      // linha do chão (próxima à base)
    int ceilingRow   = 2;             // linha do teto
    int leftWallCol  = 2;             // coluna da parede esquerda
    int rightWallCol = cols - 3;      // coluna da parede direita

    // --- chão ---
    for (int c = 0; c < cols; c++)
        SetCell(c, groundRow, SPAWN_PLATFORM);

    // --- teto ---
    for (int c = 0; c < cols; c++)
        SetCell(c, ceilingRow, SPAWN_PLATFORM);

    // --- paredes laterais ---
    for (int r = ceilingRow; r <= groundRow; r++)
    {
        SetCell(leftWallCol,  r, SPAWN_PLATFORM);
        SetCell(rightWallCol, r, SPAWN_PLATFORM);
    }

    // --- spawn do jogador (centro, acima do chão) ---
    SetCell(cols / 2, groundRow - 2, SPAWN_PLAYER);

    // --- inimigos ---
    SetCell(cols / 2 - 5, groundRow - 1, SPAWN_CRAWLER);
    SetCell(cols / 2 + 5, groundRow - 1, SPAWN_FLYER);
}

// -------------------------------------------------------------------------------
// SpawnAll
//
// Percorre todas as células da grade e instancia os objetos correspondentes
// ao tipo de spawn armazenado em cada uma.
// -------------------------------------------------------------------------------

void LevelGrid::SpawnAll()
{
    float halfCell = cellSize / 2.0f;
    float platW    = cellSize;       // largura da plataforma = 1 célula
    float platH    = cellSize;       // altura  da plataforma = 1 célula

    for (int r = 0; r < rows; r++)
    {
        for (int c = 0; c < cols; c++)
        {
            int type = grid[r][c];
            if (type == SPAWN_EMPTY)
                continue;

            // centro da célula no mundo
            float cx = c * cellSize + halfCell;
            float cy = r * cellSize + halfCell;

            switch (type)
            {
            case SPAWN_PLATFORM:
            {
                Platform* p = new Platform(cx, cy, platW, platH, "Resources/Floor.png");
                GeoWars::scene->Add(p, STATIC);
            }
            break;

            case SPAWN_PLAYER:
                GeoWars::player->MoveTo(cx, cy);
                break;

            case SPAWN_CRAWLER:
            {
                Crawler* cr = new Crawler(cx, cy);
                GeoWars::scene->Add(cr, MOVING);
            }
            break;

            case SPAWN_FLYER:
            {
                Flyer* f = new Flyer(cx, cy);
                GeoWars::scene->Add(f, MOVING);
            }
            break;

            // —— novos tipos de spawn podem ser adicionados aqui ——
            }
        }
    }
}

// -------------------------------------------------------------------------------

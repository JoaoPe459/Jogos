/**********************************************************************************
// LevelGrid (Arquivo de Cabeçalho)
//
// Criação:     15 Jun 2026
// Compilador:  Visual C++ 2022
//
// Descrição:   Define o enum SpawnType e a classe LevelGrid, que representa
//              um editor de nível primitivo baseado em uma matriz 2D.
//              Cada célula da grade armazena um tipo de spawn:
//                  0 = vazio, 1 = plataforma, 2 = player, 3 = crawler, ...
//              O tamanho da célula é configurável; a grade é calculada
//              automaticamente a partir do tamanho do mundo (game->Width/Height).
//
**********************************************************************************/

#ifndef _GEOWARS_LEVELGRID_H_
#define _GEOWARS_LEVELGRID_H_

// ---------------------------------------------------------------------------------

#include <vector>
#include <string>
using std::vector;
using std::string;

// ---------------------------------------------------------------------------------

// Tipos de spawn para cada célula da grade de nível
//
// Mapeamento de caracteres para arquivos .txt:
//   '.' ou '0' = SPAWN_EMPTY
//   '#' ou '1' = SPAWN_PLATFORM
//   'P' ou '2' = SPAWN_PLAYER
//   'C' ou '3' = SPAWN_CRAWLER
//   'F' ou '4' = SPAWN_FLYER
//
enum SpawnType
{
    SPAWN_EMPTY    = 0,     // nada
    SPAWN_PLATFORM = 1,     // plataforma / parede
    SPAWN_PLAYER   = 2,     // ponto de spawn do jogador
    SPAWN_CRAWLER  = 3,     // inimigo Crawler
    SPAWN_FLYER    = 4,     // inimigo Flyer

    // ——— aberto para futuras adições ———
    // SPAWN_BOSS, SPAWN_PICKUP, SPAWN_PROJECTILE, etc.
};

// ---------------------------------------------------------------------------------

class LevelGrid
{
private:
    int   cols, rows;              // número de colunas e linhas da grade
    float cellSize;                // tamanho da célula em pixels
    vector<vector<int>> grid;      // matriz [row][col] com valores de SpawnType

    // Converte caractere do mapa para tipo de spawn
    static int CharToSpawnType(char ch);

public:
    // Construtor — inicia grade vazia (dimensões 0×0)
    LevelGrid();

    // Preenche toda a grade com SPAWN_EMPTY
    void Clear();

    // Define o tipo de uma célula
    void SetCell(int col, int row, SpawnType type);

    // Retorna o tipo de uma célula (ou SPAWN_EMPTY se fora dos limites)
    int  GetCell(int col, int row) const;

    // Número de colunas
    int  Cols() const { return cols; }

    // Número de linhas
    int  Rows() const { return rows; }

    // Tamanho da célula em pixels
    float CellSize() const { return cellSize; }

    // Carrega o nível a partir de um arquivo .txt
    // Cada linha = uma fileira da grade, cada caractere = tipo de célula
    // A grade se adapta às dimensões do arquivo
    bool LoadFromFile(const string& filename);

    // Carrega nível de teste hardcoded (fallback)
    void LoadTestLevel();

    // Percorre a grade e instancia todos os objetos na cena
    void SpawnAll();
};

// ---------------------------------------------------------------------------------

#endif

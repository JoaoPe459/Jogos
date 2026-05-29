#ifndef _PACMAN_LEVEL2_H_
#define _PACMAN_LEVEL2_H_

/**********************************************************************************
// Level2 (Arquivo de Cabeçalho)
//
// Descrição:   Fase estilo Level Devil.
//              Toda a geometria, killzones e mecânicas são carregadas de
//              Resources/Level2.txt via LoadLevel2().
//              O código C++ só cuida de lógica específica que o parser
//              não consegue expressar (ex: debug keys).
**********************************************************************************/

#include "LevelMake.h"

class Level2 : public LevelMake
{
public:
    void Init()     override;
    void Update()   override;
    void Finalize() override;
};

#endif

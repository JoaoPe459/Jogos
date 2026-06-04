/**********************************************************************************
// Random (Arquivo de Cabeçalho)
//
// Criação:     03 Ago 2019
// Atualização: 16 Out 2024
// Compilador:  Visual C++ 2022
//
// Descrição:   Gerador de números pseudoaleatórios
//
**********************************************************************************/

#ifndef _PROGJOGOS_RANDOM_H_
#define _PROGJOGOS_RANDOM_H_

// ---------------------------------------------------------------------------------

#include <random>
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;
using std::uniform_real_distribution;
using std::conditional;
using std::is_integral;

template <typename T>
class Random 
{
private:
    mt19937 rng;
    typename conditional<is_integral<T>::value, uniform_int_distribution<T>, uniform_real_distribution<T>>::type dist;

public:
    Random(T min, T max): dist(min, max), rng(random_device{}()) {}

    T Rand() { return dist(rng); }
};

// ---------------------------------------------------------------------------------

#endif

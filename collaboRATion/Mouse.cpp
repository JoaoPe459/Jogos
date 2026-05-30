#include "Mouse.h"
#include "Engine.h" // Garante acesso ao 'window' se necessário

// ---------------------------------------------------------------------------------

Mouse::Mouse()
{
    // Posiciona o objeto inicialmente onde o mouse está
    MoveTo(window->MouseX(), window->MouseY());

    // Cria uma Bounding Box do tipo Ponto para testar colisões com os menus/portas
    BBox(new Point(x, y));
}

// ---------------------------------------------------------------------------------

Mouse::~Mouse()
{
}

// -------------------------------------------------------------------------------

void Mouse::Update()
{
    // Atualiza a posição do ponto a cada frame acompanhando o cursor do Windows
    MoveTo(window->MouseX(), window->MouseY());
}

// -------------------------------------------------------------------------------

void Mouse::Draw()
{
    // Fica vazio porque não desenhamos um sprite por cima do cursor padrão
}

// -------------------------------------------------------------------------------

bool Mouse::Clicked()
{
    // Retorna verdadeiro no frame em que o botão esquerdo é pressionado
    if (window->KeyPress(VK_LBUTTON))
        return true;
    else
        return false;
}

// -------------------------------------------------------------------------------
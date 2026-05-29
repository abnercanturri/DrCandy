/// Project entry point
#include <iostream>
#include "game.h"
#include "../test/test.h"

int main(int argc, const char* argv[])
{
    if (!test())
    {
        std::cout << "Tests NO superados. Los has definido ya?" << std::endl;
    }
    else
    {
        std::cout << "Test superados." << std::endl;
    }

    // The following code runs the graphic part
    Game game;

    // --- CÀRREGA PER LÍNIA DE COMANDES ---
    // Si argc és més gran que 1, vol dir que l'execució inclou un fitxer (argv[1])
    if (argc > 1)
    {
        std::string filePath = argv[1];
        if (game.load(filePath))
        {
            cout << "S'ha carregat l'estat inicial des de: " << filePath << std::endl;
        }
        else
        {
            cout << "No s'ha pogut carregar el fitxer: " << filePath << std::endl;
        }
    }

    // Run until ESC is pressed
    game.run();
    return 0;
}

/**
 * FITXER test.cpp
 * AUTOR Marc Poll y Abner Canturri
 * DATA 22/03/2026
 * VERSIO 1
 * Fitxer amb la classe test
 */
#include "test.h"

#include <iostream>
#include <vector>

#include "../src/board.h"
#include "../src/candy.h"
#include "../src/game.h"
#include "../src/util.h"

 // ---------------------------------------------------------------------------
 // Grups de tests
 // ---------------------------------------------------------------------------

 /**
  * testDimensions
  * Comprova que el constructor inicialitza correctament les dimensions
  * del tauler, tant per defecte com amb valors personalitzats.
  * @return true si tots els casos passen, false en cas contrari.
  */
static bool testDimensions()
{
    Board defaultBoard;
    bool casByDefecte = defaultBoard.getWidth() == DEFAULT_BOARD_WIDTH
        && defaultBoard.getHeight() == DEFAULT_BOARD_HEIGHT;

    Board smallBoard(4, 6);
    bool casPersonalitzat = smallBoard.getWidth() == 4 && smallBoard.getHeight() == 6;

    return casByDefecte && casPersonalitzat;
}

/**
 * testGetSetCell
 * Comprova que tant getCell i setCell funcionen correctament
 * les proves que fa són:
 * * Assignar un valor a una cel·la,
 * * Buidar una cel·la
 * * Sobreescriure una cel·la
 * * Netejar una cel·la
 * * Provar d'assignar un valor a una cel·la fora de rang
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testGetSetCell()
{
    Board board(5, 5);

    Candy* red = new Candy(CandyType::TYPE_RED);
    board.setCell(red, 2, 3);

    bool assignar = board.getCell(2, 3) == red;
    bool buidar = board.getCell(0, 0) == nullptr;
    bool sobrescriure;

    board.setCell(new Candy(CandyType::TYPE_BLUE), 2, 3);
    sobrescriure = board.getCell(2, 3)->getType() == CandyType::TYPE_BLUE;

    board.setCell(nullptr, 2, 3);
    bool netejar = board.getCell(2, 3) == nullptr;
    bool foraDeRang = board.getCell(-1, 0) == nullptr && board.getCell(0, 99) == nullptr;

    return assignar && buidar && sobrescriure && netejar && foraDeRang;
}


/**
 * testShouldExplode
 * Comprova que els caramels no explotin en casos que no haurien d'explotar
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testShouldExplodeNoExplota() {
    Board board(10, 10);
    board.setCell(new Candy(CandyType::TYPE_RED), 0, 9);
    board.setCell(new Candy(CandyType::TYPE_RED), 1, 9);
    bool noExplota = board.shouldExplode(0, 0) == false
        && board.shouldExplode(-1, 0) == false
        && board.shouldExplode(0, 9) == false;

    return noExplota;
}

/**
 * testShouldExplode
 * Comprova que els caramels en horitzontal exploten correctament
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testShouldExplodeHoritzontal()
{
    Board board(10, 10);
    board.setCell(new Candy(CandyType::TYPE_RED), 0, 9);
    board.setCell(new Candy(CandyType::TYPE_RED), 1, 9);
    board.setCell(new Candy(CandyType::TYPE_RED), 2, 9);
    bool horitzontal = board.shouldExplode(0, 9)
        && board.shouldExplode(1, 9)
        && board.shouldExplode(2, 9);
    return horitzontal;
}

/**
 * testShouldExplode
 * Comprova que els caramels en vertical exploten correctament
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testShouldExplodeVertical()
{
    Board board(10, 10);
    board.setCell(new Candy(CandyType::TYPE_GREEN), 5, 7);
    board.setCell(new Candy(CandyType::TYPE_GREEN), 5, 8);
    board.setCell(new Candy(CandyType::TYPE_GREEN), 5, 9);
    bool vertical = board.shouldExplode(5, 7)
        && board.shouldExplode(5, 8)
        && board.shouldExplode(5, 9);
    return vertical;
}

/**
 * testShouldExplode
 * Comprova que els caramels en diagonal '\' exploten correctament
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testShouldExplodeDiagonal1()
{
    Board board(10, 10);
    board.setCell(new Candy(CandyType::TYPE_YELLOW), 0, 7);
    board.setCell(new Candy(CandyType::TYPE_YELLOW), 1, 8);
    board.setCell(new Candy(CandyType::TYPE_YELLOW), 2, 9);
    bool diagonal1 = board.shouldExplode(0, 7) && board.shouldExplode(1, 8);

    return diagonal1;
}

/**
 * testShouldExplode
 * Comprova que els caramels en diagonal '/' exploten correctament
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testShouldExplodeDiagonal2()
{
    Board board(10, 10);
    board.setCell(new Candy(CandyType::TYPE_PURPLE), 4, 9);
    board.setCell(new Candy(CandyType::TYPE_PURPLE), 5, 8);
    board.setCell(new Candy(CandyType::TYPE_PURPLE), 6, 7);
    bool diagonal2 = board.shouldExplode(4, 9) && board.shouldExplode(5, 8);

    return diagonal2;
}
/**
 * testShouldExplode
 * Comprova que els caramels només explotant quan es requereix
 * Els casos que comprova són:
 * * Casos que NO han d'explotar
 * * Horitzontal
 * * Vertical
 * * Diagonal '\'
 * * Diagonal '/'
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testShouldExplode()
{
    bool noExplota = testShouldExplodeNoExplota();
    bool horitzontal = testShouldExplodeHoritzontal();
    bool vertical = testShouldExplodeVertical();
    bool diagonal1 = testShouldExplodeDiagonal1;
    bool diagonal2 = testShouldExplodeDiagonal2;

    return noExplota && horitzontal && vertical && diagonal1 && diagonal2;
}

/**
 * testExplodeAndDropBuit
 * Comprova que la funcio d'explotar caramels funcioni
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testExplodeAndDropBuit()
{
    return Board().explodeAndDrop().empty();
}

/**
 * testExplodeAndDropSimple
 * Comprova que a l'hora de posar 3 caramels
 * del mateix color en fila exploten
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testExplodeAndDropSimple()
{
    Board board(5, 5);
    board.setCell(new Candy(CandyType::TYPE_RED), 0, 4);
    board.setCell(new Candy(CandyType::TYPE_RED), 1, 4);
    board.setCell(new Candy(CandyType::TYPE_RED), 2, 4);

    std::vector<Candy*> exploded = board.explodeAndDrop();
    bool simple = exploded.size() == 3
        && board.getCell(0, 4) == nullptr;

    for (Candy* c : exploded) delete c;

    return simple;
}

/**
 * testExplodeAndDropSimple
 * Comprova que els caramels que estan sobre una explosió cauen
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testExplodeAndDropCaiguda()
{
    Board board(5, 5);
    board.setCell(new Candy(CandyType::TYPE_BLUE), 0, 1);
    board.setCell(new Candy(CandyType::TYPE_RED), 0, 2);
    board.setCell(new Candy(CandyType::TYPE_RED), 1, 2);
    board.setCell(new Candy(CandyType::TYPE_RED), 2, 2);

    std::vector<Candy*> exploded = board.explodeAndDrop();
    bool caiguda = exploded.size() == 3
        && board.getCell(0, 4) != nullptr
        && board.getCell(0, 4)->getType() == CandyType::TYPE_BLUE;

    for (Candy* c : exploded) delete c;

    return caiguda;
}

/**
 * testExplodeAndDropSimple
 * Comprova que les explosions en cadena funcionen correctament
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testExplodeAndDropCadena()
{

    Board board(5, 5);
    for (int x = 0; x < 3; x++)
    {
        board.setCell(new Candy(CandyType::TYPE_GREEN), x, 4);
        board.setCell(new Candy(CandyType::TYPE_ORANGE), x, 3);
    }

    std::vector<Candy*> exploded = board.explodeAndDrop();
    bool cadena = exploded.size() == 6
        && board.getCell(0, 4) == nullptr;

    for (Candy* c : exploded) delete c;

    return cadena;
}

/**
 * testExplodeAndDrop
 * Comprova que a l'hora d'explotar caramels cauen correctament
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testExplodeAndDrop()
{
    // Tauler buit
    bool buit = testExplodeAndDropBuit();
    bool simple = testExplodeAndDropSimple();
    bool caiguda = testExplodeAndDropCaiguda();
    bool cadena = testExplodeAndDropCadena();

    return buit && simple && caiguda && cadena;
}

/**
 * testDumpLoad
 * Comprova que podem guardar i carregar el tauler correctament
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testDumpLoad()
{
    const std::string savePath = getDataDirPath() + "test_board_save.txt";

    Board original(4, 4);
    original.setCell(new Candy(CandyType::TYPE_RED), 0, 3);
    original.setCell(new Candy(CandyType::TYPE_BLUE), 1, 3);
    original.setCell(new Candy(CandyType::TYPE_PURPLE), 0, 2);

    if (!original.dump(savePath))
        return false;

    Board loaded;
    if (!loaded.load(savePath))
        return false;

    if (loaded.getWidth() == 4
        && loaded.getHeight() == 4
        && loaded.getCell(0, 3) != nullptr
        && loaded.getCell(0, 3)->getType() == CandyType::TYPE_RED
        && loaded.getCell(1, 3) != nullptr
        && loaded.getCell(1, 3)->getType() == CandyType::TYPE_BLUE
        && loaded.getCell(0, 2) != nullptr
        && loaded.getCell(0, 2)->getType() == CandyType::TYPE_PURPLE
        && loaded.getCell(0, 0) == nullptr)
        return true;

    else return false;
}

/**
 * testBoardDumpLoad
 * Comprova el cicle complet de guardar i carregar un tauler: les dimensions
 * i el contingut de les cel·les es conserven correctament.
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testBoardDumpLoad()
{
    const std::string savePath = getDataDirPath() + "test_board_save.txt";
 
    Board original(4, 4);
    original.setCell(new Candy(CandyType::TYPE_RED), 0, 3);
    original.setCell(new Candy(CandyType::TYPE_BLUE), 1, 3);
    original.setCell(new Candy(CandyType::TYPE_PURPLE), 0, 2);
 
    if (!original.dump(savePath))
        return false;
 
    Board loaded;
    if (!loaded.load(savePath))
        return false;
 
    return loaded.getWidth() == 4
        && loaded.getHeight() == 4
        && loaded.getCell(0, 3) != nullptr
        && loaded.getCell(0, 3)->getType() == CandyType::TYPE_RED
        && loaded.getCell(1, 3) != nullptr
        && loaded.getCell(1, 3)->getType() == CandyType::TYPE_BLUE
        && loaded.getCell(0, 2) != nullptr
        && loaded.getCell(0, 2)->getType() == CandyType::TYPE_PURPLE
        && loaded.getCell(0, 0) == nullptr;
}

// ---------------------------------------------------------------------------
// Tests de Game
// ---------------------------------------------------------------------------

/**
 * testGameDumpLoad
 * Comprova el cicle complet de guardar i carregar l'estat d'un Game.
 * Un Game guardat i tornat a carregar en un altre Game ha de ser igual
 * a l'original (segons operator==).
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testGameDumpLoad()
{
    const std::string savePath = getDataDirPath() + "test_game_save.txt";

    Game original;
    if (!original.dump(savePath))
        return false;

    Game loaded;
    if (!loaded.load(savePath))
        return false;

    // L'estat carregat ha de coincidir amb l'original
    return original == loaded;
}

/**
 * testGameEquality
 * Comprova que operator== distingeix dos jocs diferents.
 * Dos jocs creats per separat tenen blocs aleatoris diferents, per tant
 * NO haurien de ser iguals; un joc sempre és igual a si mateix.
 * @return true si tots els casos passen, false en cas contrari.
 */
static bool testGameEquality()
{
    Game game1;

    // Un joc sempre és igual a si mateix
    bool igualASiMateix = (game1 == game1);

    // Després de carregar l'estat de game1, game2 ha de ser igual a game1
    const std::string savePath = getDataDirPath() + "test_game_eq.txt";
    bool carregatEsIgual = false;
    if (game1.dump(savePath))
    {
        Game game2;
        if (game2.load(savePath))
            carregatEsIgual = (game1 == game2);
    }

    return igualASiMateix && carregatEsIgual;
}

// ---------------------------------------------------------------------------
// Punt d'entrada principal
// ---------------------------------------------------------------------------

/**
 * test
 * Executa tots els tests unitaris de Board i Game i n'imprimeix el resultat.
 * @return true si tots els tests passen, false si algun falla.
 */
bool test()
{
    std::cout << "\n=== Executant tests unitaris ===\n\n";

    bool dims = testDimensions();
    bool cells = testGetSetCell();
    bool explota = testShouldExplode();
    bool explodrop = testExplodeAndDrop();
    bool boardDump = testDumpLoad();
    bool gameDump = testGameDumpLoad();
    bool gameEq = testGameEquality();

    std::cout << (dims ? "  [PASS]" : "  [FAIL]") << " Board: Dimensions\n";
    std::cout << (cells ? "  [PASS]" : "  [FAIL]") << " Board: GetCell/SetCell\n";
    std::cout << (explota ? "  [PASS]" : "  [FAIL]") << " Board: ShouldExplode\n";
    std::cout << (explodrop ? "  [PASS]" : "  [FAIL]") << " Board: ExplodeAndDrop\n";
    std::cout << (boardDump ? "  [PASS]" : "  [FAIL]") << " Board: Dump/Load\n";
    std::cout << (gameDump ? "  [PASS]" : "  [FAIL]") << " Game: Dump/Load\n";
    std::cout << (gameEq ? "  [PASS]" : "  [FAIL]") << " Game: Equality\n";

    bool allPassed = dims && cells && explota && explodrop
        && boardDump && gameDump && gameEq;

    std::cout << "\n=== "
        << (allPassed ? "TOTS ELS TESTS HAN PASSAT" : "ALGUNS TESTS HAN FALLAT")
        << " ===\n\n";

    return allPassed;
}
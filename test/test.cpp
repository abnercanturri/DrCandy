#include "test.h"

#include <iostream>
#include <string>
#include <vector>

#include "../src/board.h"
#include "../src/candy.h"
#include "../src/util.h"

// ---------------------------------------------------------------------------
// Marc mínim de testing
// ---------------------------------------------------------------------------

/// Imprimeix PASS o FAIL per a un cas de test i retorna si ha passat.
static bool checkTest(const std::string& nomTest, bool condicio)
{
    if (condicio)
    {
        std::cout << "  [PASS] " << nomTest << "\n";
    }
    else
    {
        std::cout << "  [FAIL] " << nomTest << "\n";
    }
    return condicio;
}

// ---------------------------------------------------------------------------
// Helpers: omplen el tauler amb un patró conegut
// ---------------------------------------------------------------------------

/// Omple 'count' cel·les d'una columna amb caramels del tipus donat, des del fons cap amunt.
static void fillColumn(Board& board, int x, CandyType type, int count)
{
    int height = board.getHeight();
    for (int y = height - count; y < height; y++)
    {
        board.setCell(new Candy(type), x, y);
    }
}

/// Omple 'count' cel·les consecutives d'una fila amb caramels del tipus donat.
static void fillRow(Board& board, int y, CandyType type, int count, int startX = 0)
{
    for (int x = startX; x < startX + count; x++)
    {
        board.setCell(new Candy(type), x, y);
    }
}

// ---------------------------------------------------------------------------
// Grups de tests
// ---------------------------------------------------------------------------

static bool testDimensions()
{
    std::cout << "--- Dimensions ---\n";
    bool allPassed = true;

    // Dimensions per defecte
    Board defaultBoard;
    allPassed &= checkTest("L'amplada per defecte és 10",
        defaultBoard.getWidth() == DEFAULT_BOARD_WIDTH);
    allPassed &= checkTest("L'alçada per defecte és 10",
        defaultBoard.getHeight() == DEFAULT_BOARD_HEIGHT);

    // Dimensions personalitzades
    Board smallBoard(4, 6);
    allPassed &= checkTest("L'amplada personalitzada és 4", smallBoard.getWidth() == 4);
    allPassed &= checkTest("L'alçada personalitzada és 6", smallBoard.getHeight() == 6);

    return allPassed;
}

static bool testGetSetCell()
{
    std::cout << "--- getCell / setCell ---\n";
    bool allPassed = true;

    Board board(5, 5);

    // El tauler buit retorna nullptr a tot arreu
    allPassed &= checkTest("Una cel·la buida retorna nullptr",
        board.getCell(0, 0) == nullptr);

    // Assignem un caramel i el recuperem
    Candy* red = new Candy(CandyType::TYPE_RED);
    board.setCell(red, 2, 3);
    allPassed &= checkTest("getCell retorna el caramel que s'ha assignat",
        board.getCell(2, 3) == red);
    allPassed &= checkTest("getCell retorna el tipus correcte després d'assignar",
        board.getCell(2, 3)->getType() == CandyType::TYPE_RED);

    // Les cel·les adjacents segueixen buides
    allPassed &= checkTest("La cel·la adjacent segueix sent nullptr",
        board.getCell(2, 4) == nullptr);

    // Sobreescriure una cel·la: el board elimina l'anterior i posa el nou
    Candy* blue = new Candy(CandyType::TYPE_BLUE);
    board.setCell(blue, 2, 3);
    allPassed &= checkTest("Sobreescriure una cel·la actualitza el tipus",
        board.getCell(2, 3)->getType() == CandyType::TYPE_BLUE);

    // Buidar una cel·la amb nullptr
    board.setCell(nullptr, 2, 3);
    allPassed &= checkTest("Assignar nullptr buida la cel·la",
        board.getCell(2, 3) == nullptr);

    // Coordenades fora de rang retornen nullptr sense petar
    allPassed &= checkTest("getCell fora de rang (x negatiu) retorna nullptr",
        board.getCell(-1, 0) == nullptr);
    allPassed &= checkTest("getCell fora de rang (y massa gran) retorna nullptr",
        board.getCell(0, 99) == nullptr);

    return allPassed;
}

static bool testShouldExplodeNoCandies()
{
    std::cout << "--- shouldExplode: casos límit ---\n";
    bool allPassed = true;

    Board board(5, 5);

    // Una cel·la buida no explota mai
    allPassed &= checkTest("Una cel·la buida no explota",
        board.shouldExplode(2, 2) == false);

    // Fora de rang no explota mai
    allPassed &= checkTest("Fora de rang no explota",
        board.shouldExplode(-1, 0) == false);

    // Un sol caramel: no hi ha línia de 3 possible
    board.setCell(new Candy(CandyType::TYPE_RED), 2, 2);
    allPassed &= checkTest("Un caramel aïllat no explota",
        board.shouldExplode(2, 2) == false);

    // Dos caramels del mateix tipus: encara no és suficient
    board.setCell(new Candy(CandyType::TYPE_RED), 3, 2);
    allPassed &= checkTest("Una línia de 2 no explota",
        board.shouldExplode(2, 2) == false);

    return allPassed;
}

static bool testShouldExplodeHorizontal()
{
    std::cout << "--- shouldExplode: horitzontal ---\n";
    bool allPassed = true;

    Board board(10, 10);
    const int row = 9;

    // Col·loquem exactament 3 rojos en fila
    fillRow(board, row, CandyType::TYPE_RED, 3, 0);

    allPassed &= checkTest("El caramel esquerre d'una horitzontal de 3 explota",
        board.shouldExplode(0, row));
    allPassed &= checkTest("El caramel del mig d'una horitzontal de 3 explota",
        board.shouldExplode(1, row));
    allPassed &= checkTest("El caramel dret d'una horitzontal de 3 explota",
        board.shouldExplode(2, row));

    // Un tipus diferent adjacent a la línia NO ha d'explotar
    board.setCell(new Candy(CandyType::TYPE_BLUE), 3, row);
    allPassed &= checkTest("Un blau adjacent a una línia de rojos no explota",
        board.shouldExplode(3, row) == false);

    return allPassed;
}

static bool testShouldExplodeVertical()
{
    std::cout << "--- shouldExplode: vertical ---\n";
    bool allPassed = true;

    Board board(10, 10);
    const int col = 0;

    // Col·loquem 3 verds apilats al fons de la columna 0
    fillColumn(board, col, CandyType::TYPE_GREEN, 3);

    allPassed &= checkTest("El caramel inferior d'una vertical de 3 explota",
        board.shouldExplode(col, 9));
    allPassed &= checkTest("El caramel del mig d'una vertical de 3 explota",
        board.shouldExplode(col, 8));
    allPassed &= checkTest("El caramel superior d'una vertical de 3 explota",
        board.shouldExplode(col, 7));

    // 4 en columna: tots han d'explotar
    board.setCell(new Candy(CandyType::TYPE_GREEN), col, 6);
    allPassed &= checkTest("El caramel superior d'una vertical de 4 també explota",
        board.shouldExplode(col, 6));

    return allPassed;
}

static bool testShouldExplodeDiagonal()
{
    std::cout << "--- shouldExplode: diagonal ---\n";
    bool allPassed = true;

    // Diagonal de dalt-esquerra a baix-dreta (\)
    {
        Board board(10, 10);
        board.setCell(new Candy(CandyType::TYPE_YELLOW), 0, 7);
        board.setCell(new Candy(CandyType::TYPE_YELLOW), 1, 8);
        board.setCell(new Candy(CandyType::TYPE_YELLOW), 2, 9);

        allPassed &= checkTest("El caramel superior de la diagonal '\\' explota",
            board.shouldExplode(0, 7));
        allPassed &= checkTest("El caramel central de la diagonal '\\' explota",
            board.shouldExplode(1, 8));
        allPassed &= checkTest("El caramel inferior de la diagonal '\\' explota",
            board.shouldExplode(2, 9));
    }

    // Diagonal de dalt-dreta a baix-esquerra (/)
    {
        Board board(10, 10);
        board.setCell(new Candy(CandyType::TYPE_PURPLE), 2, 7);
        board.setCell(new Candy(CandyType::TYPE_PURPLE), 1, 8);
        board.setCell(new Candy(CandyType::TYPE_PURPLE), 0, 9);

        allPassed &= checkTest("El caramel superior de la diagonal '/' explota",
            board.shouldExplode(2, 7));
        allPassed &= checkTest("El caramel central de la diagonal '/' explota",
            board.shouldExplode(1, 8));
        allPassed &= checkTest("El caramel inferior de la diagonal '/' explota",
            board.shouldExplode(0, 9));
    }

    return allPassed;
}

static bool testExplodeAndDropSimple()
{
    std::cout << "--- explodeAndDrop: explosió simple ---\n";
    bool allPassed = true;

    Board board(5, 5);

    // Col·loquem 3 rojos a la fila del fons
    fillRow(board, 4, CandyType::TYPE_RED, 3, 0);

    std::vector<Candy*> exploded = board.explodeAndDrop();

    allPassed &= checkTest("L'explosió simple retorna 3 caramels",
        exploded.size() == 3);
    allPassed &= checkTest("La cel·la (0,4) és buida després de l'explosió",
        board.getCell(0, 4) == nullptr);
    allPassed &= checkTest("La cel·la (1,4) és buida després de l'explosió",
        board.getCell(1, 4) == nullptr);
    allPassed &= checkTest("La cel·la (2,4) és buida després de l'explosió",
        board.getCell(2, 4) == nullptr);

    // El cridant és propietari dels punters retornats
    for (Candy* candy : exploded)
    {
        delete candy;
    }
    return allPassed;
}

static bool testExplodeAndDropFall()
{
    std::cout << "--- explodeAndDrop: caiguda després de l'explosió ---\n";
    bool allPassed = true;

    // Columna 0: un blau a sobre de 3 rojos. Els rojos exploten, el blau ha de caure al fons.
    Board board(5, 5);
    board.setCell(new Candy(CandyType::TYPE_BLUE), 0, 1); // y=1: per sobre dels rojos
    board.setCell(new Candy(CandyType::TYPE_RED),  0, 2);
    board.setCell(new Candy(CandyType::TYPE_RED),  1, 2);
    board.setCell(new Candy(CandyType::TYPE_RED),  2, 2);

    std::vector<Candy*> exploded = board.explodeAndDrop();

    allPassed &= checkTest("Els 3 rojos han explotat",
        exploded.size() == 3);
    allPassed &= checkTest("El caramel blau ha caigut al fons de la columna (y=4)",
        board.getCell(0, 4) != nullptr
        && board.getCell(0, 4)->getType() == CandyType::TYPE_BLUE);
    allPassed &= checkTest("La posició per sobre del fons de la columna 0 és buida (y=3)",
        board.getCell(0, 3) == nullptr);

    for (Candy* candy : exploded)
    {
        delete candy;
    }
    return allPassed;
}

static bool testExplodeAndDropChain()
{
    std::cout << "--- explodeAndDrop: explosió en cadena ---\n";
    bool allPassed = true;

    // 3 taronges a sobre de 3 verds al fons.
    // Els verds exploten, les taronges cauen i formen nova línia -> cadena.
    Board board(5, 5);

    // Fila 4 (fons): verds a les columnes 0, 1, 2
    fillRow(board, 4, CandyType::TYPE_GREEN, 3, 0);
    // Fila 3: taronges a les columnes 0, 1, 2 (directament sobre els verds)
    fillRow(board, 3, CandyType::TYPE_ORANGE, 3, 0);

    std::vector<Candy*> exploded = board.explodeAndDrop();

    // Les dues files han d'explotar: 3 verds + 3 taronges = 6
    allPassed &= checkTest("Cadena: 6 caramels han explotat en total",
        exploded.size() == 6);
    allPassed &= checkTest("Cadena: el fons del tauler és buit després de la cadena",
        board.getCell(0, 4) == nullptr);

    for (Candy* candy : exploded)
    {
        delete candy;
    }
    return allPassed;
}

static bool testExplodeAndDropEmptyBoard()
{
    std::cout << "--- explodeAndDrop: tauler buit ---\n";
    bool allPassed = true;

    Board board;
    std::vector<Candy*> exploded = board.explodeAndDrop();

    allPassed &= checkTest("Un tauler buit retorna un vector buit",
        exploded.empty());

    return allPassed;
}

static bool testDumpLoad()
{
    std::cout << "--- dump / load: round-trip ---\n";
    bool allPassed = true;

    const std::string savePath = getDataDirPath() + "test_board_save.txt";

    // Construïm un tauler amb un patró conegut
    Board original(4, 4);
    original.setCell(new Candy(CandyType::TYPE_RED),    0, 3);
    original.setCell(new Candy(CandyType::TYPE_BLUE),   1, 3);
    original.setCell(new Candy(CandyType::TYPE_GREEN),  2, 3);
    original.setCell(new Candy(CandyType::TYPE_YELLOW), 3, 3);
    original.setCell(new Candy(CandyType::TYPE_PURPLE), 0, 2);

    bool dumpOk = original.dump(savePath);
    allPassed &= checkTest("dump retorna true", dumpOk);

    // Carreguem en un tauler nou
    Board loaded;
    bool loadOk = loaded.load(savePath);
    allPassed &= checkTest("load retorna true", loadOk);

    allPassed &= checkTest("El tauler carregat té l'amplada correcta",
        loaded.getWidth() == 4);
    allPassed &= checkTest("El tauler carregat té l'alçada correcta",
        loaded.getHeight() == 4);

    allPassed &= checkTest("La cel·la (0,3) és VERMELLA després de carregar",
        loaded.getCell(0, 3) != nullptr
        && loaded.getCell(0, 3)->getType() == CandyType::TYPE_RED);
    allPassed &= checkTest("La cel·la (1,3) és BLAVA després de carregar",
        loaded.getCell(1, 3) != nullptr
        && loaded.getCell(1, 3)->getType() == CandyType::TYPE_BLUE);
    allPassed &= checkTest("La cel·la (0,2) és LILA després de carregar",
        loaded.getCell(0, 2) != nullptr
        && loaded.getCell(0, 2)->getType() == CandyType::TYPE_PURPLE);
    allPassed &= checkTest("La cel·la buida (0,0) és nullptr després de carregar",
        loaded.getCell(0, 0) == nullptr);

    return allPassed;
}

// ---------------------------------------------------------------------------
// Punt d'entrada principal
// ---------------------------------------------------------------------------

bool test()
{
    std::cout << "\n=== Executant tests unitaris de Board ===\n\n";

    bool allPassed = true;

    allPassed &= testDimensions();
    allPassed &= testGetSetCell();
    allPassed &= testShouldExplodeNoCandies();
    allPassed &= testShouldExplodeHorizontal();
    allPassed &= testShouldExplodeVertical();
    allPassed &= testShouldExplodeDiagonal();
    allPassed &= testExplodeAndDropSimple();
    allPassed &= testExplodeAndDropFall();
    allPassed &= testExplodeAndDropChain();
    allPassed &= testExplodeAndDropEmptyBoard();
    allPassed &= testDumpLoad();

    std::cout << "\n=== " << (allPassed ? "TOTS ELS TESTS HAN PASSAT" : "ALGUNS TESTS HAN FALLAT") << " ===\n\n";

    return allPassed;
}
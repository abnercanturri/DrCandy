#include "test.h"

#include <iostream>
#include <vector>

#include "../src/board.h"
#include "../src/candy.h"
#include "../src/util.h"

// ---------------------------------------------------------------------------
// Grups de tests
// ---------------------------------------------------------------------------

static bool testDimensions()
{
    Board defaultBoard;
    bool casByDefecte = defaultBoard.getWidth() == DEFAULT_BOARD_WIDTH
                     && defaultBoard.getHeight() == DEFAULT_BOARD_HEIGHT;

    Board smallBoard(4, 6);
    bool casPersonalitzat = smallBoard.getWidth() == 4 && smallBoard.getHeight() == 6;

    return casByDefecte && casPersonalitzat;
}

static bool testGetSetCell()
{
    Board board(5, 5);

    Candy* red = new Candy(CandyType::TYPE_RED);
    board.setCell(red, 2, 3);

    bool assignar    = board.getCell(2, 3) == red;
    bool buidar      = board.getCell(0, 0) == nullptr;
    bool sobrescriure;

    board.setCell(new Candy(CandyType::TYPE_BLUE), 2, 3);
    sobrescriure = board.getCell(2, 3)->getType() == CandyType::TYPE_BLUE;

    board.setCell(nullptr, 2, 3);
    bool netejar     = board.getCell(2, 3) == nullptr;
    bool foraDeRang  = board.getCell(-1, 0) == nullptr && board.getCell(0, 99) == nullptr;

    return assignar && buidar && sobrescriure && netejar && foraDeRang;
}

static bool testShouldExplode()
{
    Board board(10, 10);

    // Casos que NO han d'explotar
    board.setCell(new Candy(CandyType::TYPE_RED), 0, 9);
    board.setCell(new Candy(CandyType::TYPE_RED), 1, 9);
    bool noExplota = board.shouldExplode(0, 0) == false
                  && board.shouldExplode(-1, 0) == false
                  && board.shouldExplode(0, 9)  == false;

    // Horitzontal
    board.setCell(new Candy(CandyType::TYPE_RED), 2, 9);
    bool horitzontal = board.shouldExplode(0, 9)
                    && board.shouldExplode(1, 9)
                    && board.shouldExplode(2, 9);

    // Vertical
    board.setCell(new Candy(CandyType::TYPE_GREEN), 5, 7);
    board.setCell(new Candy(CandyType::TYPE_GREEN), 5, 8);
    board.setCell(new Candy(CandyType::TYPE_GREEN), 5, 9);
    bool vertical = board.shouldExplode(5, 7)
                 && board.shouldExplode(5, 8)
                 && board.shouldExplode(5, 9);

    // Diagonal '\'
    board.setCell(new Candy(CandyType::TYPE_YELLOW), 0, 7);
    board.setCell(new Candy(CandyType::TYPE_YELLOW), 1, 8);
    board.setCell(new Candy(CandyType::TYPE_YELLOW), 2, 9);
    bool diagonal1 = board.shouldExplode(0, 7) && board.shouldExplode(1, 8);

    // Diagonal '/'
    board.setCell(new Candy(CandyType::TYPE_PURPLE), 4, 9);
    board.setCell(new Candy(CandyType::TYPE_PURPLE), 5, 8);
    board.setCell(new Candy(CandyType::TYPE_PURPLE), 6, 7);
    bool diagonal2 = board.shouldExplode(4, 9) && board.shouldExplode(5, 8);

    return noExplota && horitzontal && vertical && diagonal1 && diagonal2;
}

static bool testExplodeAndDrop()
{
    // Tauler buit
    bool buit = Board().explodeAndDrop().empty();

    // Explosio simple
    bool simple;
    {
        Board board(5, 5);
        board.setCell(new Candy(CandyType::TYPE_RED), 0, 4);
        board.setCell(new Candy(CandyType::TYPE_RED), 1, 4);
        board.setCell(new Candy(CandyType::TYPE_RED), 2, 4);

        std::vector<Candy*> exploded = board.explodeAndDrop();
        simple = exploded.size() == 3
              && board.getCell(0, 4) == nullptr;

        for (Candy* c : exploded) delete c;
    }

    // Caiguda despres d'explosio
    bool caiguda;
    {
        Board board(5, 5);
        board.setCell(new Candy(CandyType::TYPE_BLUE), 0, 1);
        board.setCell(new Candy(CandyType::TYPE_RED),  0, 2);
        board.setCell(new Candy(CandyType::TYPE_RED),  1, 2);
        board.setCell(new Candy(CandyType::TYPE_RED),  2, 2);

        std::vector<Candy*> exploded = board.explodeAndDrop();
        caiguda = exploded.size() == 3
               && board.getCell(0, 4) != nullptr
               && board.getCell(0, 4)->getType() == CandyType::TYPE_BLUE;

        for (Candy* c : exploded) delete c;
    }

    // Explosio en cadena
    bool cadena;
    {
        Board board(5, 5);
        for (int x = 0; x < 3; x++)
        {
            board.setCell(new Candy(CandyType::TYPE_GREEN),  x, 4);
            board.setCell(new Candy(CandyType::TYPE_ORANGE), x, 3);
        }

        std::vector<Candy*> exploded = board.explodeAndDrop();
        cadena = exploded.size() == 6
              && board.getCell(0, 4) == nullptr;

        for (Candy* c : exploded) delete c;
    }

    return buit && simple && caiguda && cadena;
}

static bool testDumpLoad()
{
    const std::string savePath = getDataDirPath() + "test_board_save.txt";

    Board original(4, 4);
    original.setCell(new Candy(CandyType::TYPE_RED),    0, 3);
    original.setCell(new Candy(CandyType::TYPE_BLUE),   1, 3);
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
// Punt d'entrada principal
// ---------------------------------------------------------------------------

bool test()
{
    std::cout << "\n=== Executant tests unitaris de Board ===\n\n";

    bool dims      = testDimensions();
    bool cells     = testGetSetCell();
    bool explota   = testShouldExplode();
    bool explodrop = testExplodeAndDrop();
    bool dumpl     = testDumpLoad();
    bool allPassed = dims && cells && explota && explodrop && dumpl;

    if (dims) std::cout << "  [PASS] Dimensions\n";
    else std::cout << "  [FAIL] Dimensions\n";

    if (cells) std::cout << "  [PASS] GetCell/SetCell\n";
    else std::cout << "  [FAIL] GetCell/SetCell\n";

    if (explota) std::cout << "  [PASS] ShouldExplode\n";
    else std::cout << "  [FAIL] ShouldExplode\n";

    if (explodrop) std::cout << "  [PASS] ExplodeAndDrop\n";
    else std::cout << "  [FAIL] ExplodeAndDrop\n";

    if (dumpl) std::cout << "  [PASS] Dump/Load\n";
    else std::cout << "  [FAIL] Dump/Load\n";


    if (dims && cells && explota && explodrop && dumpl) std::cout << "  \n=== TOTS ELS TESTS HAN PASSAT ===\n\n";
    else std::cout << "  \n=== ALGUNS TESTS HAN FALLAT ===\n\n";

    return allPassed;
}
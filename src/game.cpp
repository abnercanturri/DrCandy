#include "game.h"
#include <random>
#include "graphics.h"
#include "candy.h"
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iostream>

Game::Game()
{
    // Implement your code here
    srand(time(nullptr));

    m_blockX = 5;
    m_blockY = -1;
    m_frameCount = 0;
    m_score = 0;
    m_gameOver = false;

    for (int i = 0; i < 3; ++i)
    {
        m_fallingBlock[i] = nullptr;
    }

    spawnNewBlock();
}

Game::~Game()
{
    // Implement your code here
    //Alliberem la memòria dels 3 caramels del bloc actual si encara existeixen
    for (int i = 0; i < 3; ++i)
    {
        if (m_fallingBlock[i] != nullptr)
        {
            delete m_fallingBlock[i];
            m_fallingBlock[i] = nullptr;
        }
    }

    //Alliberem tots els caramels dinàmics que hagin quedat guardats al tauler.
    //D'aquesta manera evitem qualsevol leak de la partida de forma 100% segura.
    for (int y = 0; y < m_board.getHeight(); ++y)
    {
        for (int x = 0; x < m_board.getWidth(); ++x)
        {
            Candy* candy = m_board.getCell(x, y);
            if (candy != nullptr)
            {
                delete candy;
                m_board.setCell(nullptr, x, y); //Deixem la casella neta
            }
        }
    }
}

void Game::update(const Controller& controller)
{
    // Implement your code here
    //Si hem perdut, congelem el joc i no fem res més
    if (m_gameOver) return;

    handleInput(controller);
    saveLoad(controller);
    
    //GRAVETAT I CAIGUDA
    m_frameCount++;

    //Si premem la fletxa avall, el límit de frames baixa de 60 a 5 perquè caigui ràpid
    int speedLimit = controller.isDownPressed() ? 5 : 60;

    if (m_frameCount >= speedLimit)
    {
        m_frameCount = 0; //Tornem a començar a comptar el temps

        applyGravity();
    }
}

void Game::render(GraphicManager& graphics)
{
    //Implement your code here
    drawBoard(graphics);
    drawFallingBlock(graphics);
    drawUI(graphics);
}

void Game::run()
{
    const int screen_width = 750;
    const int screen_height = 750;
    const int bg_red = 255;
    const int bg_green = 255;
    const int bg_blue = 255;
    runGraphicGame(*this, screen_width, screen_height, bg_red, bg_green, bg_blue);
}

bool Game::dump(const std::string& output_path) const
{
    // Implement your code here
    //Obrim el fitxer
    std::ofstream file(output_path);

    if (!file.is_open())
    {
        return false;
    }

    //ESTAT BÀSIC DEL JOC (Salt de línia per separar)
    file << m_score << "\n";
    file << m_gameOver << "\n";
    file << m_frameCount << "\n";
    file << m_blockX << "\n";
    file << m_blockY << "\n";

    //EL BLOC QUE CAU
    for (int i = 0; i < 3; ++i)
    {
        if (m_fallingBlock[i] != nullptr)
        {
            file << static_cast<int>(m_fallingBlock[i]->getType()) << " ";
        }
        else
        {
            file << "-1 "; //-1 significa espai buit
        }
    }
    file << "\n";

    //EL TAULER (Utilitzem els getters en lloc de cridar m_board.dump)
    file << m_board.getWidth() << " " << m_board.getHeight() << "\n";

    for (int y = 0; y < m_board.getHeight(); ++y)
    {
        for (int x = 0; x < m_board.getWidth(); ++x)
        {
            Candy* c = m_board.getCell(x, y);
            if (c != nullptr)
            {
                file << static_cast<int>(c->getType()) << " ";
            }
            else
            {
                file << "-1 ";
            }
        }
        file << "\n";
    }

    return true; //El fitxer es tanca automàticament en destruir-se l'objecte 'file'
}

bool Game::load(const std::string& input_path)
{
    // Implement your code here
    ifstream file(input_path);

    if (!file.is_open())
    {
        return false;
    }

    //LLEGIM L'ESTAT BÀSIC DEL JOC
    file >> m_score >> m_gameOver >> m_frameCount >> m_blockX >> m_blockY;

    //NETEJA I CÀRREGA DEL BLOC QUE CAU
    for (int i = 0; i < 3; ++i)
    {
        if (m_fallingBlock[i] != nullptr)
        {
            delete m_fallingBlock[i];
        }

        int typeVal;
        file >> typeVal;
        m_fallingBlock[i] = (typeVal != -1) ? new Candy(static_cast<CandyType>(typeVal)) : nullptr;
    }

    //NETEJA I CÀRREGA DEL TAULER (Casella per casella)
    int fileWidth, fileHeight;
    file >> fileWidth >> fileHeight;

    for (int y = 0; y < m_board.getHeight(); ++y)
    {
        for (int x = 0; x < m_board.getWidth(); ++x)
        {
            Candy* oldCandy = m_board.getCell(x, y);
            if (oldCandy != nullptr)
            {
                delete oldCandy;
            }

            int candyVal;
            file >> candyVal;
            m_board.setCell((candyVal != -1) ? new Candy(static_cast<CandyType>(candyVal)) : nullptr, x, y);
        }
    }
    return true;
}

bool Game::operator==(const Game& other) const
{
    // Implement your code here
    //Comprovem l'estat bàsic del joc
    bool equal = (m_frameCount == other.m_frameCount &&
        m_score == other.m_score &&
        m_gameOver == other.m_gameOver &&
        m_blockX == other.m_blockX &&
        m_blockY == other.m_blockY);

    //COMPROVEM ELS 3 CARAMELS DEL BLOC QUE CAU
    if (equal)
    {
        for (int i = 0; i < 3 && equal; ++i)
        {
            Candy* c1 = m_fallingBlock[i];
            Candy* c2 = other.m_fallingBlock[i];

            if ((c1 == nullptr && c2 != nullptr) || (c1 != nullptr && c2 == nullptr))
            {
                equal = false;
            }
            else if (c1 != nullptr && c2 != nullptr && c1->getType() != c2->getType())
            {
                equal = false;
            }
        }
    }

    //COMPROVEM LES DIMENSIONS I EL CONTINGUT DEL TAULER
    if (equal)
    {
        if (m_board.getWidth() != other.m_board.getWidth() || m_board.getHeight() != other.m_board.getHeight())
        {
            equal = false;
        }
        else
        {
            for (int y = 0; y < m_board.getHeight() && equal; ++y)
            {
                for (int x = 0; x < m_board.getWidth() && equal; ++x)
                {
                    Candy* c1 = m_board.getCell(x, y);
                    Candy* c2 = other.m_board.getCell(x, y);

                    if ((c1 == nullptr && c2 != nullptr) || (c1 != nullptr && c2 == nullptr))
                    {
                        equal = false;
                    }
                    else if (c1 != nullptr && c2 != nullptr && c1->getType() != c2->getType())
                    {
                        equal = false;
                    }
                }
            }
        }
    }

    return equal;
}

void Game::spawnNewBlock()
{
    //Definim la posició inicial (6a columna = índex 5, i y=0 o per sobre del tauler)
    m_blockX = 5;
    m_blockY = -1; //La peça inferior del bloc estarà a la fila 0 (o com acordeu el sistema de coordenades)

    //Creem 3 caramels aleatoris fent servir 'new' (Memòria Dinàmica)
    for (int i = 0; i < 3; ++i)
    {
        //CandyType::COUNT val 6, així que rand() % 6 ens donarà un número entre 0 i 5
        int randomNum = rand() % static_cast<int>(CandyType::COUNT);
        CandyType randomType = static_cast<CandyType>(randomNum);

        // Guardem el nou caramel a l'array del bloc que cau
        m_fallingBlock[i] = new Candy(randomType);
    }
}

void Game::handleInput(const Controller& controller)
{
    //MOVIMENT HORITZONTAL
    if (controller.isLeftPressed() && m_blockX > 0)
    {
        bool canMove = true;
        //Comprovem si alguna de les 3 peces xocaria a l'esquerra
        for (int i = 0; i < 3; ++i)
        {
            int currentY = m_blockY - (2 - i);
            //Només comprovem si la peça ja ha entrat dins del tauler (currentY >= 0)
            if (currentY >= 0 && m_board.getCell(m_blockX - 1, currentY) != nullptr)
            {
                canMove = false; //Xoc detectat!
                break;
            }
        }
        if (canMove)
            m_blockX--;
    }
    else if (controller.isRightPressed())
    {
        if (m_blockX < m_board.getWidth() - 1)
        {
            bool canMove = true;
            //Comprovem si alguna de les 3 peces xocaria a la dreta
            for (int i = 0; i < 3; ++i)
            {
                int currentY = m_blockY - (2 - i);
                if (currentY >= 0 && m_board.getCell(m_blockX + 1, currentY) != nullptr)
                {
                    canMove = false; //Xoc detectat!
                    break;
                }
            }
            if (canMove)
                m_blockX++;
        }
    }

    //ROTACIÓ (Tecla Q) -> A -> B -> C es converteix en C -> A -> B
    if (controller.isKey1Pressed())
    {
        //Guardem temporalment el caramel de sota de tot
        Candy* temp = m_fallingBlock[2];

        //Fem baixar els altres dos
        m_fallingBlock[2] = m_fallingBlock[1];
        m_fallingBlock[1] = m_fallingBlock[0];

        //El de sota de tot passa a ser el de dalt de tot
        m_fallingBlock[0] = temp;
    }
}

void Game::applyGravity()
{
    //Comprovem si pot baixar: 
    bool canDrop = true;
    if (m_blockY + 1 >= m_board.getHeight())
        canDrop = false; //Toca el terra de la graella
    //També comprovem si xoca amb algun caramel ja col·locat (si estem dins del tauler)
    else if (m_blockY + 1 >= 0 && m_board.getCell(m_blockX, m_blockY + 1) != nullptr)
        canDrop = false; //Toca un altre caramel

    if (canDrop)
        m_blockY++; //Cau una casella lliurement
    else
    {
        //HA ATERRAT! El bloc es congela al seu lloc
        bool outOfBounds = false;

        //Pas A: Traspassar els caramels de l'array temporal al tauler definitiu
        for (int i = 0; i < 3; ++i)
        {
            int currentPieceY = m_blockY - (2 - i);

            if (currentPieceY < 0)
                //Si ha aterrat i alguna peça està flotant fora per dalt, hem perdut
                outOfBounds = true;
            else
                //Li donem l'objecte dinàmic al tauler
                m_board.setCell(m_fallingBlock[i], m_blockX, currentPieceY);
            //Netegem l'array perquè el tauler ara n'és el propietari
            m_fallingBlock[i] = nullptr;
        }

        if (outOfBounds)
            m_gameOver = true; //S'ha acabat la partida!
        else
        {
            //Pas B: Executar les explosions del tauler
            std::vector<Candy*> exploded = m_board.explodeAndDrop();

            //Sumem 10 punts per caramel explotat
            m_score += exploded.size() * 10;

            //Atenció! El tauler ens retorna els caramels esborrats, nosaltres n'hem d'alliberar la memòria
            for (Candy* c : exploded)
            {
                delete c;
            }
            //Pas C: Crear el següent bloc perquè el joc continuï
            spawnNewBlock();
        }
    }
}

void Game::drawBoard(GraphicManager& graphics) const
{
    //MÀRGES I MESURES BASE
    //Aprofitem el padding (marge) de l'esquelet per no dibuixar enganxat a la vora superior esquerra
    const int start_x = CANDY_IMAGE_WIDTH * 3;
    const int start_y = CANDY_IMAGE_HEIGHT * 3;

    //DIBUIXAR EL MARC DEL TAULER
    graphics.drawRectangle(
        start_x, start_y,
        CANDY_IMAGE_WIDTH * m_board.getWidth(),
        CANDY_IMAGE_HEIGHT * m_board.getHeight(),
        5, 150, 150, 150);

    //DIBUIXAR ELS CARAMELS FIXATS AL TAULER
    for (int y = 0; y < m_board.getHeight(); ++y)
    {
        for (int x = 0; x < m_board.getWidth(); ++x)
        {
            Candy* c = m_board.getCell(x, y);
            if (c != nullptr)
            {
                //Calculem on toca dibuixar en píxels (Marge + Coordenada * MidaDelCaramel)
                int drawX = start_x + (x * CANDY_IMAGE_WIDTH);
                int drawY = start_y + (y * CANDY_IMAGE_HEIGHT);
                graphics.drawImage(c->getResourceName(), drawX, drawY);
            }
        }
    }
}

void Game::drawFallingBlock(GraphicManager& graphics) const
{
    const int start_x = CANDY_IMAGE_WIDTH * 3;
    const int start_y = CANDY_IMAGE_HEIGHT * 3;

    //DIBUIXAR EL BLOC QUE ESTÀ CAIENT (Si no hem perdut)
    if (!m_gameOver)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (m_fallingBlock[i] != nullptr)
            {
                //Recordem la nostra convenció: l'índex 2 és a sota de tot (m_blockY)
                //L'índex 1 està al mig (m_blockY - 1)
                //L'índex 0 està a dalt de tot (m_blockY - 2)
                int currentPieceY = m_blockY - (2 - i);

                //Calculem les coordenades en píxels per la pantalla
                int drawX = start_x + (m_blockX * CANDY_IMAGE_WIDTH);
                int drawY = start_y + (currentPieceY * CANDY_IMAGE_HEIGHT);

                graphics.drawImage(m_fallingBlock[i]->getResourceName(), drawX, drawY);
            }
        }
    }
}

void Game::drawUI(GraphicManager& graphics) const
{
    const int start_y = CANDY_IMAGE_HEIGHT * 3;
    
    //INTERFÍCIE D'USUARI
    graphics.drawImage("img/logo_small.png", 10, 10);
    graphics.drawText("Movement: [Up] [Down] [Left] [Right]  --  Buttons: [Q] [W] [E]  --  Exit [ESC]",
        25, 700, 20, 100, 100, 100);

    if (m_gameOver)
        //Si la partida ha acabat, mostrem GAME OVER en vermell gegant
        graphics.drawText("GAME OVER", 230, start_y - 80, 50, 255, 0, 0);
    else
        // Convertim el número m_score a text per poder-lo dibuixar
        graphics.drawText("Score: " + std::to_string(m_score), 450, 25, 40, 125, 200, 125);
}

void Game::saveLoad(const Controller& controller)
{
    //GUARDAR PARTIDA (Tecla W)
    if (controller.isKey2Pressed())
    {
        //Com que l'executable ja està a dins de 'data', només cal posar el nom del fitxer!
        std::string savePath = "save.txt";

        if (dump(savePath)) {
            std::cout << "Partida guardada correctament a: " << savePath << std::endl;

            // --- TEST DE L'OPERADOR == ---
            Game testGame;           // 1. Creem un joc nou i buit
            testGame.load(savePath); // 2. Hi carreguem el fitxer que acabem de crear

            // 3. Comparem el joc actual (*this) amb el que acabem de carregar
            if (*this == testGame) {
                std::cout << "TEST SUPERAT: L'operador == funciona com la seda!" << std::endl;
            }
            else {
                std::cout << "TEST FALLAT: Hi ha alguna dada diferent." << std::endl;
            }
            // -----------------------------
        }
        else {
            std::cout << "Error guardant." << std::endl;
        }
    }

    //CARREGAR PARTIDA (Tecla E)
    if (controller.isKey3Pressed())
    {
        std::string loadPath = "save.txt";

        if (load(loadPath)) {
            std::cout << "Partida carregada correctament!" << std::endl;
        }
        else {
            std::cout << "Error carregant la partida. No s'ha trobat el fitxer." << std::endl;
        }
    }
}

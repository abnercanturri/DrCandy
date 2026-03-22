#include "board.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

Board::Board(int width, int height)
{
    //Inicialitzar les dimensions de la graella
    m_width = width;
    m_height = height;

    //Reservem array principal
    m_grid = new Candy * *[m_height];

    //Creem les columnes de cada fila i omplim de nullptrs (caramels buits)
    for (int y = 0; y < m_height; y++)
    {
        m_grid[y] = new Candy * [m_width];

        for (int x = 0; x < m_width; x++)
        {
            m_grid[y][x] = nullptr;
        }
    }
}

Board::~Board()
{
    // Implement your code here
    
    //Alliberem la memòria de l'estructura
    for (int y = 0; y < m_height; ++y)
    {
        //Només esborrem l'array de punters, no els caramels
        delete[] m_grid[y];
    }

    //Esborrem l'array principal
    delete[] m_grid;
}


Candy* Board::getCell(int x, int y) const
{
    // Implement your code here
    
    //Comprovar els límits de la graella
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
    {
        return nullptr;
    }
    return m_grid[y][x];
}

void Board::setCell(Candy* candy, int x, int y)
{
    // Implement your code here

    //Inserir en cas de que les coordenades siguin vàlides
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
        m_grid[y][x] = candy;
    }
}


int Board::getWidth() const
{
    // Implement your code here
    return m_width;
}


int Board::getHeight() const
{
    // Implement your code here
    return m_height;
}

bool Board::shouldExplode(int x, int y) const
{
    // Implement your code here

    //Comprovar si estem dins del tauler
    if (x < 0 || x >= m_width || y < 0 || y >= m_height)
        return false;

    //Agafem el caramel actual, en cas de buit, no pot ser explotat
    Candy* currentCandy = m_grid[y][x];
    if (currentCandy == nullptr)
        return false;

    CandyType targetType = currentCandy->getType();

    //Definim les 4 direccions que es poden prendre dels eixos
    //Horitzonal - x + 1, y + 0
    //Vertical - x + 0, y + 1
    //Diagonal descendent - x + 1, y + 1
    //Diagonal ascendent - x + 1, y - 1
    int dirX[4] = { 1, 0, 1, 1 };
    int dirY[4] = { 0, 1, 1, -1 };

    //Comprovem tots els eixos
    for (int i = 0; i < 4; i++)
    {
        int count = 1; //Contem el caramel en el que estem

        //Mirem cap al costat positiu
        int dx = dirX[i];
        int dy = dirY[i];
        int cx = x + dx;
        int cy = y + dy;

        //Mentres hi hagi caramel del mateix tipus i no sortim del tauler
        while (cx >= 0 && cx < m_width && cy >= 0 && cy < m_height && m_grid[cy][cx] != nullptr && m_grid[cy][cx]->getType() == targetType)
        {
            //Afegim contador dels caramels, i avancem de casella
            count++;
            cx += dx;
            cy += dy;
        }

        //Ara mirem cap a l'altra banda, el mateix però en negatiu
        dx = -dirX[i];
        dy = -dirY[i];
        cx = x + dx;
        cy = y + dy;

        while (cx >= 0 && cx < m_width && cy >= 0 && cy < m_height && m_grid[cy][cx] != nullptr && m_grid[cy][cx]->getType() == targetType)
        {
            count++;
            cx += dx;
            cy += dy;
        }

        if (count >= SHORTEST_EXPLOSION_LINE)
            return true;
    }
    return false;

}

std::vector<Candy*> Board::explodeAndDrop()
{
    // Implement your code here

    std::vector<Candy*> explodedCandies;
    bool chainReaction; //Bool que ens comunicarà si s'ha de revisar el tauler de nou o no

    do
    {
        chainReaction = false;

        //Creem una mena de "tauler secundari" per a saber quins caramels explotaran
        std::vector<std::vector<bool>> toExplode(m_height, std::vector<bool>(m_width, false));

        for (int y = 0; y < m_height; y++)
        {
            for (int x = 0; x < m_width; x++)
            {
                if (m_grid[y][x] != nullptr && shouldExplode(x, y))
                    toExplode[y][x] = true;
            }
        }

        //Recollim els caramels explotats i buidem
        for (int y = 0; y < m_height; y++)
        {
            for (int x = 0; x < m_width; x++)
            {
                if (toExplode[y][x])
                {
                    explodedCandies.push_back(m_grid[y][x]);    //Guardem per retornar-lo més endavant
                    m_grid[y][x] = nullptr;                     //Buidem la casella
                    chainReaction = true;                       //Hi ha hagut acció, haurem d'aplicar la gravetat 
                }
            }
        }

        //Gravetat
        if (chainReaction)
        {
            //Mirem columna a columna
            for (int x = 0; x < m_width; x++)
            {
                //Mirem d'abaix a dalt
                for (int y = m_height - 1; y >= 0; y--)
                {
                    if (m_grid[y][x] == nullptr)
                    {
                        //Hi ha forat, busquem el primer caramel per sobre que pugui caure
                        for (int z = y - 1; z >= 0; z--)
                        {
                            if (m_grid[z][x] != nullptr)
                            {
                                m_grid[y][x] = m_grid[z][x]; //Fem caure el caramel al forat
                                m_grid[z][x] = nullptr; //Buidem el lloc on estava el caramel
                                break; //No busquem més per sobre del forat
                            }
                        }
                    }
                }
            }
        }
    }
    //Repetir si hi ha hagut alguna explosió
    while (chainReaction);
    
    //Retornem els caramels que s'han anat del tauler
    return explodedCandies;
}

bool Board::dump(const std::string& output_path) const
{
    // Implement your code here

    //Creem objecte per escriure
    std::ofstream out(output_path);

    //En cas d'error amb el fitxer, retornem un false
    if (!out.is_open())
        return false;

    //Escriure dimensions primera línia
    out << m_width << " " << m_height << endl;

    //Reccorer matriu (Amunt-Avall i Esquerra-Dreta)
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            if (m_grid[y][x] != nullptr)
                //Convertir CandyType a número enter i escriure'l
                out << (int)m_grid[y][x]->getType() << " ";
            else
                //En cas de nullptr
                out << "-1 ";
        }
        //Salt de línia per cada fila
        out << endl;
    }
    return true;
}

bool Board::load(const std::string& input_path)
{
    // Implement your code here

    //Creem objecte per llegir
    std::ifstream in(input_path);

    if (!in.is_open())
        return false;

    //Llegir dimensions
    int file_width, file_height;
    in >> file_width >> file_height;

    //Esborrar graella vella
    for (int y = 0; y < file_height; y++)
    {
        //Esborrar files velles
        delete[] m_grid[y];
    }
    //Esborrar estructura principal vella
    delete[] m_grid;

    //Actualitzem dimensions
    m_width = file_width;
    m_height = file_height;

    //Creem la nova graella
    m_grid = new Candy * *[m_height];
    for (int y = 0; y < m_height; y++)
    {
        m_grid[y] = new Candy * [m_width];

        //Llegim i omplim caramels
        for (int x = 0; x < file_width; x++)
        {
            int candy_val;
            in >> candy_val;

            if (candy_val != -1)
                //Si no és buit instanciem un nou caramel i convertim el número a CandyType de nou
                m_grid[y][x] = new Candy((CandyType)candy_val);
            else
                m_grid[y][x] = nullptr;
        }
    }
    return true;
}

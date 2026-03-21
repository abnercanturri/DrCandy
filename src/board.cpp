#include "board.h"
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

Board::Board(int width, int height)
{
    // Implement your code here

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
    return false;
}

std::vector<Candy*> Board::explodeAndDrop()
{
    // Implement your code here
    return {};
}

bool Board::dump(const std::string& output_path) const
{
    // Implement your code here
    //Creem objecte per escriure
    std::ofstream out(output_path);
    
    //En cas d'error amb el fitxer, retornem un false
    if (!out.is_open())
    {
        return false;
    }
    
    //Escriure dimensions primera línia
    out << m_width << " " << m_height << endl;
    
    //Reccorer matriu (Amunt-Avall i Esquerra-Dreta)
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            if (m_grid[y][x] != nullptr)
            {
                //Convertir CandyType a número enter i escriure'l
                out << (int)m_grid[y][x]->getType() << " ";
            }
            else
            {
                //En cas de nullptr
                out << "-1";
            }
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
    {
        return false;
    }
    
    //Llegir dimensions
    int file_width, file_height;
    in >> file_width >> file_height;
    
    //Recorrer el fitxer i omplir el tauler
    for (int y = 0; y < file_height; y++)
    {
        for (int x = 0; x < file_width; x++)
        {
            int candy_val;
            in >> candy_val;
    
            if (candy_val != -1)
            {
                //Si no és buit instanciem un nou caramel i convertim el número a CandyType de nou
                m_grid[y][x] = new Candy((CandyType)candy_val);
            }
            else
            {
                m_grid[y][x] = nullptr;
            }
        }
    }
    return true;
}

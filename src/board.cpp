#include "board.h"

#include <fstream>
#include <iostream>

#include "util.h"

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

Board::Board(int width, int height)
    : m_width(width), m_height(height)
{
    // Inicialitzem la graella amb nullptr (totes les cel·les buides)
    m_cells.assign(m_width, std::vector<Candy*>(m_height, nullptr));
}

Board::~Board()
{
    for (int x = 0; x < m_width; x++)
    {
        for (int y = 0; y < m_height; y++)
        {
            delete m_cells[x][y];
            m_cells[x][y] = nullptr;
        }
    }
}

// ---------------------------------------------------------------------------
// Accessors bàsics
// ---------------------------------------------------------------------------

int Board::getWidth() const
{
    return m_width;
}

int Board::getHeight() const
{
    return m_height;
}

bool Board::isValid(int x, int y) const
{
    return (x >= 0 && x < m_width && y >= 0 && y < m_height);
}

Candy* Board::getCell(int x, int y) const
{
    if (!isValid(x, y))
    {
        return nullptr;
    }
    return m_cells[x][y];
}

void Board::setCell(Candy* candy, int x, int y)
{
    if (!isValid(x, y))
    {
        return;
    }
    // Eliminem el caramel anterior si n'hi havia un
    delete m_cells[x][y];
    m_cells[x][y] = candy;
}

// ---------------------------------------------------------------------------
// Lògica d'explosió
// ---------------------------------------------------------------------------

int Board::countInDirection(int x, int y, int dx, int dy) const
{
    CandyType type = m_cells[x][y]->getType();
    int count = 0;
    int nx = x + dx;
    int ny = y + dy;

    while (isValid(nx, ny) && m_cells[nx][ny] != nullptr
           && m_cells[nx][ny]->getType() == type)
    {
        count++;
        nx += dx;
        ny += dy;
    }
    return count;
}

bool Board::shouldExplode(int x, int y) const
{
    if (!isValid(x, y) || m_cells[x][y] == nullptr)
    {
        return false;
    }

    // Els quatre eixos: horitzontal, vertical, diagonal \, diagonal /
    const int nDirections = 4;
    const int dx[nDirections] = {1, 0, 1,  1};
    const int dy[nDirections] = {0, 1, 1, -1};

    for (int d = 0; d < nDirections; d++)
    {
        // Longitud total = el propi caramel + els de cada costat de l'eix
        int lineLength = 1
            + countInDirection(x, y,  dx[d],  dy[d])
            + countInDirection(x, y, -dx[d], -dy[d]);

        if (lineLength >= SHORTEST_EXPLOSION_LINE)
        {
            return true;
        }
    }
    return false;
}

std::vector<Candy*> Board::removeExplodingCandies()
{
    // Primer marquem tots els que han d'explotar ABANS d'eliminar cap
    // (important: la decisió es pren sobre l'estat original del tauler)
    std::vector<std::vector<bool>> toExplode(
        m_width, std::vector<bool>(m_height, false));

    for (int x = 0; x < m_width; x++)
    {
        for (int y = 0; y < m_height; y++)
        {
            if (shouldExplode(x, y))
            {
                toExplode[x][y] = true;
            }
        }
    }

    // Després eliminem els marcats i els retornem al cridant
    std::vector<Candy*> exploded;
    for (int x = 0; x < m_width; x++)
    {
        for (int y = 0; y < m_height; y++)
        {
            if (toExplode[x][y])
            {
                exploded.push_back(m_cells[x][y]);
                m_cells[x][y] = nullptr;
            }
        }
    }
    return exploded;
}

void Board::dropCandies()
{
    // Per cada columna, compactem els caramels cap al fons preservant l'ordre
    for (int x = 0; x < m_width; x++)
    {
        // Recollim els caramels no nuls de dalt a baix
        std::vector<Candy*> column;
        for (int y = 0; y < m_height; y++)
        {
            if (m_cells[x][y] != nullptr)
            {
                column.push_back(m_cells[x][y]);
            }
        }

        // Reomplim la columna: nullptr a dalt, caramels a baix
        int emptyRows = m_height - static_cast<int>(column.size());
        for (int y = 0; y < m_height; y++)
        {
            if (y < emptyRows)
            {
                m_cells[x][y] = nullptr;
            }
            else
            {
                m_cells[x][y] = column[y - emptyRows];
            }
        }
    }
}

std::vector<Candy*> Board::explodeAndDrop()
{
    std::vector<Candy*> allExploded;

    // Repetim el cicle explotar-caure fins que no hi hagi res més per explotar
    std::vector<Candy*> roundExploded = removeExplodingCandies();
    while (!roundExploded.empty())
    {
        allExploded.insert(allExploded.end(),
                           roundExploded.begin(), roundExploded.end());
        dropCandies();
        roundExploded = removeExplodingCandies();
    }

    return allExploded;
}

// ---------------------------------------------------------------------------
// Serialització
// ---------------------------------------------------------------------------

bool Board::dump(const std::string& output_path) const
{
    std::ofstream file(output_path);
    if (!file.is_open())
    {
        std::cerr << "Board::dump: no s'ha pogut obrir el fitxer '" << output_path << "'\n";
        return false;
    }

    // Primera línia: dimensions del tauler
    file << m_width << " " << m_height << "\n";

    // Resta: una fila per cada y, tipus com enter (-1 = cel·la buida)
    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            if (m_cells[x][y] == nullptr)
            {
                file << -1;
            }
            else
            {
                file << static_cast<int>(m_cells[x][y]->getType());
            }
            if (x < m_width - 1)
            {
                file << " ";
            }
        }
        file << "\n";
    }
    return file.good();
}

bool Board::load(const std::string& input_path)
{
    std::ifstream file(input_path);
    if (!file.is_open())
    {
        std::cerr << "Board::load: no s'ha pogut obrir el fitxer '" << input_path << "'\n";
        return false;
    }

    int newWidth = 0;
    int newHeight = 0;
    file >> newWidth >> newHeight;

    if (!file.good() || newWidth <= 0 || newHeight <= 0)
    {
        std::cerr << "Board::load: dimensions invàlides al fitxer '" << input_path << "'\n";
        return false;
    }

    // Descartem l'estat actual del tauler
    for (int x = 0; x < m_width; x++)
    {
        for (int y = 0; y < m_height; y++)
        {
            delete m_cells[x][y];
        }
    }

    m_width = newWidth;
    m_height = newHeight;
    m_cells.assign(m_width, std::vector<Candy*>(m_height, nullptr));

    for (int y = 0; y < m_height; y++)
    {
        for (int x = 0; x < m_width; x++)
        {
            int typeValue = 0;
            file >> typeValue;
            if (typeValue >= 0 && typeValue < static_cast<int>(CandyType::COUNT))
            {
                m_cells[x][y] = new Candy(static_cast<CandyType>(typeValue));
            }
        }
    }
    return file.good() || file.eof();
}
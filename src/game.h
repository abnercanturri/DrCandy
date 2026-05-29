/**
 * Game logic and rendering implementation. 
 */
#ifndef GAME_H
#define GAME_H

#include "graphics.h"
#include "controller.h"
#include "board.h"

/**
 * Main game class: keep track of the game state it. 
 * When run_graphic_game() is called, the game loop will call
 * update() and render() every frame, in that order.
 */
class Game
{
public:
    Game();

    ~Game();

    /// Run the game loop
    void run();

    /**
     * Update the game state. Called every frame when run().
     * This part is not expected to do any rendering.
     * 
     * @param controller the Controller to use for input handling.
     */
    void update(const Controller& controller);

    /**
     * Draw the next frame. Called once per frame, after update().
     * 
     * @param graphics the GraphicManager to use for rendering.
     */
    void render(GraphicManager& graphics);

    /**
     * Serialize and write the current game state (board and falling block) to a file.
     * @param output_path path where to save the game state.
     * @return true if the dump was successful, false otherwise.
     */
    bool dump(const std::string& output_path) const;

    /**
     * Load a serialized game state (board and falling block) from a file created with dump().
     * @param input_path path from which to load the game state.
     * @return true if the load was successful, false otherwise.
     */
    bool load(const std::string& input_path);

    /// @return true if this game is equal to the other game (same board state and falling block)
    bool operator==(const Game& other) const;

private:
    Board m_board;                                          //tauler instanciat
    Candy* m_fallingBlock[3];                               //Array 3 caramels que cauen
    int m_blockX;                                           //Posició X (columna) del bloc
    int m_blockY;                                           //Posició Y (fila) de la peça inferior del bloc

    int m_frameCount;                                       //Comptador per saber quan ha de baixar el bloc (60 frames)
    int m_score;                                            //Puntuació actual
    bool m_gameOver;                                        //Bool per saber si s'ha perdut o no el joc
    bool m_paused;                                           //Bool per saber si el joc està o no en pausa

    void handleInput(const Controller& controller);         //Gestiona el teclat
    void applyGravity();                                    //Gestiona la caiguda i xocs
    void spawnNewBlock();                                   //Crea el bloc de tres caramels amunt del tauler
    void drawBoard(GraphicManager& graphics) const;         //Dibuixa la graella
    void drawFallingBlock(GraphicManager& graphics) const;  //Dibuixa la fitxa actual
    void drawUI(GraphicManager& graphics) const;            //Dibuixa marcadors i textos
    void saveLoad(const Controller& controller);            //Gestiona guarrdat i càrrega
};
#endif

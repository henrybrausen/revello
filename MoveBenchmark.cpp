#include <iostream>

#include "Board.h"

/* A small benchmarking application to test the performance of the gameboard implementation.
 * Useful for profiling!
 */
int main(int argc, char* argv[])
{
    Board MyBoard = Board();

    unsigned int p;

    std::cout << "Beginning 100M move generation test . . ." << std::endl;
    uint64_t mvs;
    for (unsigned long i = 0; i < 100000000; ++i) {
        mvs = MyBoard.get_moves();
        while ((p = Board::get_next_move(mvs)) != MOVE_PASS);
        MyBoard.move(p);
        MyBoard.undo();
    }
    std::cout << "Move generation complete!" << std::endl;
    
    return 0;
}

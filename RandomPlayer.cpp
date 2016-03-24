#include "RandomPlayer.h"

#include "Board.h"

#include <cstdlib>
#include <ctime>

#include <stdint.h>

#include <iostream>

RandomPlayer::RandomPlayer(Board& brd)
    : Player(brd)
{
    srand(time(NULL));
}

unsigned char RandomPlayer::genmove(bool color)
{
    m_board.set_toPlay(color);
    uint64_t moves = m_board.get_moves();
    
    unsigned char pos;
    unsigned int positions[64];
    
    int move_count = 0;
    
    while ( (pos = Board::get_next_move(moves)) != MOVE_PASS) {
        positions[move_count] = pos;
        ++move_count;
    }
    
    if (move_count == 0) return MOVE_PASS;
    return positions[rand()%move_count];
}

void RandomPlayer::show_info() const
{
    std::cout << "RandomPlayer" << std::endl;
    std::cout << "\tNo statistics." << std::endl;
}

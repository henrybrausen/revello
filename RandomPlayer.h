#ifndef __RANDOM_PLAYER_H__
#define __RANDOM_PLAYER_H__

#include "Player.h"
#include "Board.h"

/* A basic implementation of a player that makes random moves.
 */
class RandomPlayer : public Player {
    public:
        RandomPlayer(Board& brd);
        virtual ~RandomPlayer() {}
        virtual unsigned char genmove(bool color);
        
        virtual void show_info() const;
};

#endif

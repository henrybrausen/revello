#ifndef __AB_PLAYER_H__
#define __AB_PLAYER_H__

#include "Player.h"
#include "Board.h"
#include "TranspositionTable.h"

/* An AI player based on the Alpha-Beta search algorithm.
 * Enhanced with a transposition table and iterative deepening search.
 */

class ABPlayer : public Player {
    public:
        ABPlayer(Board& brd, unsigned int tt_size = 1);
        virtual ~ABPlayer() {}
        
        virtual unsigned char genmove(bool color);
        
        virtual int alphabeta(int depth, int alpha, int beta);
        
        virtual void show_info() const;
        
        virtual void set_mobility_weight(int weight) { m_mobilityWeight = weight; }
        
        virtual void set_mobility_weight(const phenotype& ph);
        
        virtual void set_max_depth(int depth) { m_maxDepth = depth; }
        
    private:
        TranspositionTable m_TT;
        unsigned long int m_nodesExpanded;
        unsigned int m_lastDepth;
        unsigned int m_maxDepth;
        int m_mobilityWeight;
        unsigned char m_iterNum;    // Current searching depth (during iterative deepening)
};

#endif

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <stdint.h>
#include <sys/time.h>
#include "Board.h"

class Player {
    public:
        Player(Board& brd)
            : m_board(brd), m_time(10)
            {}
        virtual ~Player() {}
        
        virtual unsigned char genmove(bool color) = 0;
        virtual bool play(unsigned char pos, bool color);
        
        virtual void show_info() const = 0;
        
        virtual void set_time(unsigned int in_time) { m_time = in_time; }
        virtual unsigned int get_time() const { return m_time; }
        
        virtual void start_time();
        virtual bool timeout() const;
        
    protected:
        Board& m_board;
        struct timeval m_startTime;
        unsigned int m_time;
};

#endif

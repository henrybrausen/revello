#include "Player.h"
#include "Board.h"

#include <sys/time.h>
#include <cstdlib>

bool Player::play(unsigned char pos, bool color)
{
    m_board.set_toPlay(color);
    if (!m_board.is_valid_move(pos, m_board.get_moves()))
        return false;
    m_board.move(pos);
    return true;
}

void Player::start_time()
{
    gettimeofday(&m_startTime, NULL);
}

inline bool Player::timeout() const
{
    static struct timeval current_time;
    gettimeofday(&current_time, NULL);
    return (current_time.tv_sec >= m_startTime.tv_sec+m_time && current_time.tv_usec >= m_startTime.tv_usec);
}

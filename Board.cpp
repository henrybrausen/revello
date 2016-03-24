#include "Board.h"
#include "BitBoard.h"

#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <iostream>

unsigned char Board::get_next_move(uint64_t& moves_rmng)
{
	if (moves_rmng == 0) return MOVE_PASS;
	static const int index64[64] = {
		63,  0, 58,  1, 59, 47, 53,  2,
		60, 39, 48, 27, 54, 33, 42,  3,
		61, 51, 37, 40, 49, 18, 28, 20,
		55, 30, 34, 11, 43, 14, 22,  4,
		62, 57, 46, 52, 38, 26, 32, 41,
		50, 36, 17, 19, 29, 10, 13, 21,
		56, 45, 25, 31, 35, 16,  9, 12,
		44, 24, 15,  8, 23,  7,  6,  5
	};
	static const uint64_t debruijn64 = 0x07EDD5E59A4E28C2ULL;
	static unsigned char p;
	p = index64[((moves_rmng&-moves_rmng) * debruijn64) >> 58];	// magic?!
	moves_rmng &= ~(1ULL << p);	// could use table lookup to speed this up!
	return p;
}

unsigned char Board::num_moves(uint64_t moves_rmng)
{
	unsigned char c;
	for (c = 0; moves_rmng; ++c) {
		moves_rmng &= moves_rmng - 1;	// Clear lsb set
	}
	return c;
}

Board::Board()
{
	// Better generate lookup tables for underlying (ugly) bitboard implementation!
	generate_lookup_tables();
    
    // Generate hashes
    for (int i = 0; i < STACK_SIZE; ++i) {
        m_passHash[i] = 0ULL;
        for (int i = 0; i < 8; ++i) {
            m_passHash[i] |= (uint64_t)((uint8_t)rand()) << 8*i;
        }
    }
    
    for (int player = 0; player < 3; ++player) { // player==2 is empty cell
        for (int pos = 0; pos < 64; ++pos) {
            m_positionHash[player][pos] = 0ULL;
            for (int i = 0; i < 8; ++i)
                m_positionHash[player][pos] |= (uint64_t)((uint8_t)rand()) << 8*i;
        }
    }

    m_emptyHash = 0ULL;
    // Set empty hash AND initialize position values
    for (int pos = 0; pos < 64; ++pos) {
        m_emptyHash ^= m_positionHash[2][pos];
        m_posValues[pos] = 0;
    }
    
	reset();
}

void Board::reset()
{
	m_board = get_empty();
	// Standard initial configuration
	set_pos(m_board, rc_off(4,4), P_WHITE);
	set_pos(m_board, rc_off(3,3), P_WHITE);
	set_pos(m_board, rc_off(3,4), P_BLACK);
	set_pos(m_board, rc_off(4,3), P_BLACK);
    m_hash = m_emptyHash;
    m_hash ^= m_positionHash[P_WHITE][rc_off(4,4)];
    m_hash ^= m_positionHash[P_WHITE][rc_off(3,3)];
    m_hash ^= m_positionHash[P_BLACK][rc_off(3,4)];
    m_hash ^= m_positionHash[P_BLACK][rc_off(4,3)];
	m_stackPos = m_passHashPos = 0;
	m_toPlay = P_BLACK;
}

void Board::show() const
{
    uint64_t moves = get_moves();
    std::cout << std::hex << m_hash << std::dec << std::endl << std::endl;
    printf("   A B C D E F G H\n");
    for (int r = 7; r >= 0; --r) {
        printf("%2.1d ", r+1);
        for (int c = 7; c >= 0; --c) {
            if (m_board.w_r0 & (1ULL<<(r*8+c))) {
                printf("@ ");
            }
            else if (m_board.b_r0 & (1LL<<(8*r+c))) {
                printf("O ");
            }
            else if (is_valid_move(r*8+c, moves))
                printf("* ");
            else
                printf(". ");
        }
        printf("%.1d", r+1);
        if (r == 3)
            printf("\t%s to play.\n", ((m_toPlay==P_BLACK)?("Black (O)"):("White (@)")));
        else if (r == 2)
            printf("\tO: %d\t@: %d\n", num_stones(P_BLACK), num_stones(P_WHITE));
        else
            printf("\n");
    }
    printf("   A B C D E F G H\n\n");
    
}

void Board::show_raw() const
{
    std::cout << m_hash << std::endl << std::endl;
	// Use quick & ugly bitboard display
	show_board(m_board);
}

bool Board::is_valid_move(unsigned char p, uint64_t move_map) const
{
	if (p == MOVE_PASS) {
		return Board::num_moves(move_map) == 0;
	}
	return (((1ULL << p)&move_map) != 0);
}

std::string Board::move_history() const
{
	std::ostringstream out;
	for (int i = 1; i <= m_stackPos; ++i)
        if (m_moveStack[i].p == MOVE_PASS)
            out << "pass ";
        else
            out << (char)((7-(m_moveStack[i].p%8))+'A') << char((m_moveStack[i].p/8) + '1') << ' ';
	return out.str();
}

void Board::move(unsigned char p)
{
	++m_stackPos;
	m_moveStack[m_stackPos].old_board = m_board;
	m_moveStack[m_stackPos].color = m_toPlay;
	m_moveStack[m_stackPos].p = p;
    m_moveStack[m_stackPos].hash = m_hash;
	if (p != MOVE_PASS) {
		m_board = make_move(m_board, p, m_toPlay);
        m_hash ^= m_positionHash[m_toPlay][p];
	} else {
		m_hash ^= m_passHash[m_passHashPos++];
	}
	m_toPlay = !m_toPlay;
}

void Board::undo()
{
	if (m_stackPos == 0) return;
	m_toPlay = m_moveStack[m_stackPos].color;
	m_board = m_moveStack[m_stackPos].old_board;
    m_hash = m_moveStack[m_stackPos].hash;
	if (m_moveStack[m_stackPos].p == MOVE_PASS) {
		--m_passHashPos;
    }
	--m_stackPos;
}

int Board::num_stones(bool color) const
{
	unsigned char c;
	uint64_t temp;
	if (color == P_BLACK)
		temp = m_board.b_r0;
	else
		temp = m_board.w_r0;
	for (c = 0; temp; ++c) {
		temp &= temp - 1;	// Clear lsb set
	}
	return c;
}

int Board::evaluate_territory() const
{
	/*int ret = 0;
    if (m_toPlay == P_BLACK) {
        if (m_board.b_r0 & 0x8000000000000000ULL) ++ret;
        if (m_board.b_r0 & 0x0100000000000000ULL) ++ret;
        if (m_board.b_r0 & 0x80) ++ret;
        if (m_board.b_r0 & 0x01) ++ret;
        if (m_board.w_r0 & 0x8000000000000000ULL) --ret;
        if (m_board.w_r0 & 0x0100000000000000ULL) --ret;
        if (m_board.w_r0 & 0x80) --ret;
        if (m_board.w_r0 & 0x01) --ret;
    } else {
        if (m_board.b_r0 & 0x8000000000000000ULL) --ret;
        if (m_board.b_r0 & 0x0100000000000000ULL) --ret;
        if (m_board.b_r0 & 0x80) --ret;
        if (m_board.b_r0 & 0x01) --ret;
        if (m_board.w_r0 & 0x8000000000000000ULL) ++ret;
        if (m_board.w_r0 & 0x0100000000000000ULL) ++ret;
        if (m_board.w_r0 & 0x80) ++ret;
        if (m_board.w_r0 & 0x01) ++ret;
    }
    return ret;*/
    static unsigned int indices[64] = {
        0, 1, 3, 6, 6, 3, 1, 0,
        1, 2, 4, 7, 7, 4, 2, 1,
        3, 4, 5, 8, 8, 5, 4, 3,
        6, 7, 8, 9, 9, 8, 7, 6,
        6, 7, 8, 9, 9, 8, 7, 6,
        3, 4, 5, 8, 8, 5, 4, 3,
        1, 2, 4, 7, 7, 4, 2, 1,
        0, 1, 3, 6, 6, 3, 1, 0
        
    };
    uint64_t toplay_board = ((m_toPlay == P_BLACK)?(m_board.b_r0):(m_board.w_r0));
    uint64_t opp_board = ((m_toPlay != P_BLACK)?(m_board.b_r0):(m_board.w_r0));
    int ret = 0;
    unsigned char pos = 0;
    while ((pos = Board::get_next_move(toplay_board)) != MOVE_PASS)
        ret += m_posValues[indices[pos]];
    while ((pos = Board::get_next_move(opp_board)) != MOVE_PASS)
        ret -= m_posValues[indices[pos]];
    return ret;
}

bool Board::game_over() const
{
    if (m_stackPos < 2)
        return false;
    if (m_moveStack[m_stackPos].p == MOVE_PASS && m_moveStack[m_stackPos-1].p == MOVE_PASS)
        return true;
    return false;
}

int Board::winner() const
{
    if (num_stones(P_BLACK) < num_stones(P_WHITE))
        return P_WHITE;
    if (num_stones(P_BLACK) > num_stones(P_WHITE))
        return P_BLACK;
    return 2;
}

void Board::load_pos_values(int *values)
{
    for (int i = 0; i < 10; ++i)
        m_posValues[i] = values[i];
}

void Board::save_pos_values(int *dest)
{
    for (int i = 0; i < 10; ++i)
        dest[i] = m_posValues[i];
}

void Board::load_pos_values(const phenotype& ph)
{
    phenotype temp;
    phenotype mask32 = 0xFFFFFFFF;
    for (int i = 0; i < 10; ++i) {
        temp = (ph >> (32*i))&mask32;
        m_posValues[i] = temp.to_ulong()%1000; // Limit size
    }
}

void Board::show_pos_values() const
{
    std::cout << "Position Values" << std::endl;
    for (int i = 0; i < 10; ++i) {
        std::cout << "\t[" << i << "]: " << m_posValues[i] << std::endl;
    }
}

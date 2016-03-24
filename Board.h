#ifndef __BOARD_H__
#define __BOARD_H__

#include <stdint.h>

#include <string>

#include <bitset>

#include "BitBoard.h"

#define MAX_PASSES 64

#define P_BLACK 0
#define P_WHITE 1

#define MOVE_PASS 64
#define MOVE_INVALID 65

#define STACK_SIZE 128	// May as well be safe.

typedef struct OthelloMove_s {
	unsigned char p;
	bool color;
	oboard_t old_board;
	uint64_t hash;
} OthelloMove;

typedef std::bitset<352> phenotype;

// Class to represent an othello board using bitboard techniques.
// Uses a set of 4x2 uint64s for this purpose.
// Also implements some useful and efficient utility functions.
class Board {
public:
	Board();
	virtual ~Board() {}	// Literally nothing to deallocate!

	void reset();
	
    void show() const;
	void show_raw() const;

	unsigned int get_n() const { return 8; }

	bool is_valid_move(unsigned char p, uint64_t move_map) const;
	
	uint64_t get_moves() const { return get_move_map(m_board, m_toPlay); }
	
	unsigned char last_move() const { return m_moveStack[m_stackPos].p; }

	static unsigned char get_next_move(uint64_t& moves_rmng);	// also removes the returned move
	static unsigned char num_moves(uint64_t moves_rmng);
    
    static void mask_away(uint64_t& moves_rmng, unsigned char p) { moves_rmng &= ~(1ULL << p); }

	std::string move_history() const;

	void move(unsigned char p);
	void undo();
    
    uint64_t hash() const { return m_hash; }

	bool game_over() const;

	void set_toPlay(bool color) { m_toPlay = color; }
	bool get_toPlay() const { return m_toPlay; }

	int num_stones(bool color) const;
    
    int stone_differential() const { return num_stones(m_toPlay) - num_stones(!m_toPlay); }
    int winner() const;

	int evaluate_territory() const;
    
    // Takes in length 10 array
    void load_pos_values(int *values);
    
    void save_pos_values(int *dest);
    
    void load_pos_values(const phenotype& ph);
    
    unsigned char num_empties() const { return 64-num_stones(P_BLACK)-num_stones(P_WHITE); }
    
    void show_pos_values() const;

private:
	// Our current board
	oboard_t m_board;

	// Our move stack
	OthelloMove m_moveStack[STACK_SIZE];
	int m_stackPos;

	bool m_toPlay;
    
    // Position values for static evaluation function
    int m_posValues[10];
    
    uint64_t m_positionHash[3][64];
    
	uint64_t m_hash;	// Zobrist-y hash
	uint64_t m_emptyHash;

	uint64_t m_passHash[STACK_SIZE];	// Hashes for subsequent pass moves
	int m_passHashPos;
};

#endif

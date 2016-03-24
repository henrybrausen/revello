#ifndef __BITBOARD_H__
#define __BITBOARD_H__

#include <stdint.h>
#include <cstdio>

typedef struct oboard_s {
	uint64_t b_r0;
	uint64_t w_r0;
	uint64_t b_r90;
	uint64_t w_r90;
	uint64_t b_r45;
	uint64_t w_r45;
	uint64_t b_r45_2;
	uint64_t w_r45_2;
} oboard_t;

inline int rc_off(int r, int c) { return 8*r+c; }

void generate_lookup_tables();
oboard_t get_empty();
void show_move_map(unsigned char pos, unsigned char bconf, unsigned char wconf, bool toplay);
void show_byte(unsigned char byte);
int bitboard_tests();
void show_board(const oboard_t& brd);
void show_bitboard(const uint64_t brd);
void set_pos(oboard_t &brd, unsigned char p, bool color);
uint64_t get_move_map(const oboard_t& board, bool color);
oboard_t make_move(const oboard_t& old, unsigned char p, bool color);
bool is_legal_move(const oboard_t& brd, unsigned char p, bool color);

#endif

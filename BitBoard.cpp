#include "BitBoard.h"

#include <stdint.h>
#include <cstdio>

unsigned char moves[256][256][2];		// Move generation lookup table
unsigned char move_map[8][256][256][2];	// Move result lookup table
unsigned char r90map[64] = {	// convert offset into r90 offset
0, 8, 16, 24, 32, 40, 48, 56,
1, 9, 17, 25, 33, 41, 49, 57,
2, 10, 18, 26, 34, 42, 50, 58,
3, 11, 19, 27, 35, 43, 51, 59,
4, 12, 20, 28, 36, 44, 52, 60,
5, 13, 21, 29, 37, 45, 53, 61,
6, 14, 22, 30, 38, 46, 54, 62,
7, 15, 23, 31, 39, 47, 55, 63 };

unsigned char r45map_backwards[64] = {
	63,
	55, 62,
	47, 54, 61,
	39, 46, 53, 60,
	31, 38, 45, 52, 59,
	23, 30, 37, 44, 51, 58,
	15, 22, 29, 36, 43, 50, 57,
	7, 14, 21, 28, 35, 42, 49, 56,
	6, 13, 20, 27, 34, 41, 48,
	5, 12, 19, 26, 33, 40,
	4, 11, 18, 25, 32,
	3, 10, 17, 24,
	2, 9, 16,
	1, 8,
	0
};
// the "/" diagonals
// convert r45 offset into r0 offset
unsigned char r45index[64] = {
	0,
	1, 8,
	2, 9, 16,
	3, 10, 17, 24,
	4, 11, 18, 25, 32,
	5, 12, 19, 26, 33, 40,
	6, 13, 20, 27, 34, 41, 48,
	7, 14, 21, 28, 35, 42, 49, 56,
	15, 22, 29, 36, 43, 50, 57,
	23, 30, 37, 44, 51, 58,
	31, 38, 45, 52, 59,
	39, 46, 53, 60,
	47, 54, 61,
	55, 62,
	63
};

// map r0 offset to r45 diagonal number.
unsigned char r45_pmap[64] = { 0, 1, 2, 3, 4, 5, 6, 7, 1, 2, 3, 4, 5, 6, 7, 8,
	2, 3, 4, 5, 6, 7, 8, 9, 3, 4, 5, 6, 7, 8, 9, 10, 4, 5, 6, 7, 8, 9, 10, 11,
	5, 6, 7, 8, 9, 10, 11, 12, 6, 7, 8, 9, 10, 11, 12, 13, 7, 8, 9, 10, 11, 12, 13, 14 };

// The "\" diagonals
unsigned char r45_2index[64] = {
	7,
	15, 6,
	23, 14, 5,
	31, 22, 13, 4,
	39, 30, 21, 12, 3,
	47, 38, 29, 20, 11, 2,
	55, 46, 37, 28, 19, 10, 1,
	63, 54, 45, 36, 27, 18, 9, 0,
	62, 53, 44, 35, 26, 17, 8,
	61, 52, 43, 34, 25, 16,
	60, 51, 42, 33, 24,
	59, 50, 41, 32,
	58, 49, 40,
	57, 48,
	56
};

// convert r45_2 offset into r0 offset
/*unsigned char r45_2index[64] = {
	56,
	57, 48,
	58, 49, 40,
	59, 50, 41, 32,
	60, 51, 42, 33, 24,
	61, 52, 43, 34, 25, 16,
	62, 53, 44, 35, 26, 17, 8,
	63, 54, 45, 36, 27, 18, 9, 0,
	55, 46, 37, 28, 19, 10, 1,
	47, 38, 29, 20, 11, 2,
	39, 30, 21, 12, 3,
	31, 22, 13, 4,
	23, 14, 5,
	15, 6,
	7
};*/

// map r0 offset to r45_2 diagonal number.
unsigned char r45_2pmap[64] = { 7, 6, 5, 4, 3, 2, 1, 0, 8, 7, 6, 5, 4, 3, 2, 1, 9, 8, 7, 6, 5, 4, 3, 2, 10, 9, 8, 7, 6, 5, 4, 3, 11, 10, 9, 8, 7, 6, 5, 4,
	12, 11, 10, 9, 8, 7, 6, 5, 13, 12, 11, 10, 9, 8, 7, 6, 14, 13, 12, 11, 10, 9, 8, 7 };

// how far is a piece in r45 into its diagonal?
unsigned char diag_offset[64] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 0,
	2, 2, 2, 2, 2, 2, 1, 0,
	3, 3, 3, 3, 3, 2, 1, 0,
	4, 4, 4, 4, 3, 2, 1, 0,
	5, 5, 5, 4, 3, 2, 1, 0,
	6, 6, 5, 4, 3, 2, 1, 0,
	7, 6, 5, 4, 3, 2, 1, 0
};

// how far is a piece in r45_2 into its diagonal?
unsigned char diag_offset_2[64] = {
	7, 6, 5, 4, 3, 2, 1, 0,
	6, 6, 5, 4, 3, 2, 1, 0,
	5, 5, 5, 4, 3, 2, 1, 0,
	4, 4, 4, 4, 3, 2, 1, 0,
	3, 3, 3, 3, 3, 2, 1, 0,
	2, 2, 2, 2, 2, 2, 1, 0,
	1, 1, 1, 1, 1, 1, 1, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

// convert r0 index into r45 index
unsigned char r45map[64];
unsigned char r45_2map[64];

unsigned char diagmask[15] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};
unsigned char diagshift[15] = { 0x00, 0x01, 0x03, 0x06, 0x0A, 0x0F, 0x15, 0x1C, 0x24, 0x2B, 0x31, 0x36, 0x3A, 0x3D, 0x3F};


// convert "/" diagonal # [0-14] to r0
uint64_t r45_r0[15][256];
// convert "\" diagonal # [0-14] to r0
uint64_t r45_2_r0[15][256];
// convert r90 configuration to r0
uint64_t r90_r0[8][256];
// convert r0 to r90
uint64_t r0_r90[8][256];
// convert r0 ro r45
uint64_t r0_r45[8][256];
// convert r0 to r45_2
uint64_t r0_r45_2[8][256];
// Mask to get only affected areas from a move.
// 0 = row
// 1 = col
// 2 = "/"
// 3 = "\"
uint64_t affectmask[64][4];

oboard_t get_empty()
{
	oboard_t ret;
	ret.b_r0 = 0ULL;
	ret.b_r0 = 0ULL;
	ret.w_r0 = 0ULL;
	ret.b_r90 = 0ULL;
	ret.w_r90 = 0ULL;
	ret.b_r45 = 0ULL;
	ret.w_r45 = 0ULL;
	ret.b_r45_2 = 0ULL;
	ret.w_r45_2 = 0ULL;
	return ret;
}

void show_board(const oboard_t& brd)
{
	printf("   7 6 5 4 3 2 1 0\n");
	for (int r = 7; r >= 0; --r) {
		printf("%2.1d ", r);
		for (int c = 7; c >= 0; --c) {
			if (brd.w_r0 & (1ULL<<(r*8+c))) {
				printf("w ");
			}
			else if (brd.b_r0 & (1LL<<(8*r+c))) {
				printf("b ");
			}
			else
				printf(". ");
		}
		printf("%.1d\n", r);
	}
	printf("   7 6 5 4 3 2 1 0\n");
}

void show_bitboard(const uint64_t brd)
{
	printf("   7 6 5 4 3 2 1 0\n");
	for (int r = 7; r >= 0; --r) {
		printf("%2.1d ", r);
		for (int c = 7; c >= 0; --c) {
			if (brd & (1ULL<<(r*8+c))) {
				printf("* ");
			}
			else
				printf(". ");
		}
		printf("%.1d\n", r);
	}
	printf("   7 6 5 4 3 2 1 0\n");
}

void show_byte(unsigned char byte)
{
	printf(" 7 6 5 4 3 2 1 0\n");
	printf("##################\n#");
	for (int i = 7; i >= 0; --i) {
		if (byte & (1 << i)) {
			printf("* ");
		} else {
			printf(". ");
		}
	}
	printf("#\n##################\n");
}

void show_move_map(unsigned char pos, unsigned char bconf, unsigned char wconf, bool toplay)
{
	show_byte(move_map[pos][bconf][wconf][toplay]);
}

void set_pos(oboard_t &brd, unsigned char p, bool color)
{
	if (color == 0) {
		brd.b_r0 |= 1ULL<<p;
		brd.b_r90 |= 1ULL<<r90map[p];
		brd.b_r45 |= 1ULL<<r45map[p];
		brd.b_r45_2 |= 1ULL<<r45_2map[p];
	}
	else if (color == 1) {
		brd.w_r0 |= 1ULL<<p;
		brd.w_r90 |= 1ULL<<r90map[p];
		brd.w_r45 |= 1ULL<<r45map[p];
		brd.w_r45_2 |= 1ULL<<r45_2map[p];
	}
}

void generate_r90_r0()
{
	uint64_t temp;
	// [column][config] -> r0
	for (unsigned int shift = 0; shift < 8; ++shift) {
		for (unsigned int config = 0; config < 256; ++config) {
			temp = 0ULL;
			for (int i = 0; i < 8; ++i) {
				if ((config >> i)&0x1) {	// ith bit set
					temp |= (1ULL << shift) << (i*8);
				}
			}
			r90_r0[shift][config] = temp;
		}
	}
}

// Actually, r90_r0 == r0_r90! One undoes the other.
void generate_r0_r90()
{	// convert "row" in r0 to correponding repr. in r90
	uint64_t temp;
	for (unsigned int shift = 0; shift < 8; ++shift) {
		for (unsigned int config = 0; config < 256; ++config) {
			temp = 0ULL;
			for (int i = 0; i < 8; ++i) {
				if ((config >> i)&0x1) {
					temp |= (1ULL << i*8) << shift;
				}
			}
			r0_r90[shift][config] = temp;
		}
	}
}

void generate_r45_r0()
{
	uint64_t temp;
	// foreach diagonal: foreach cell configuration: build map[diag][config]->r0
	for (unsigned int diag = 0; diag < 15; ++diag) {
		for (unsigned int config = 0; config < 256; ++config) {
			temp = 0ULL;
			unsigned int masked_config = config & diagmask[diag];
			for (int i = 0; i < 8; ++i) {
				if ((masked_config >> i)&0x1) {
					// convert index into r45 into index into r0
					temp |= (1ULL) << r45index[(diagshift[diag]+i)];
				}
			}
			r45_r0[diag][config] = temp;
		}
	}
}

void generate_r0_r45()
{
	uint64_t temp;
	for (unsigned int shift = 0; shift < 8; ++shift) {	// "row" in r0
		for (unsigned int config = 0; config < 256; ++config) {
			temp = 0ULL;
			for (int i = 0; i < 8; ++i) {
				if ((config >> i)&0x1) {
					temp |= 1ULL << r45map[(shift*8)+i];
				}
			}
			r0_r45[shift][config] = temp;
		}
	}
}

void generate_r45_2_r0()
{
	uint64_t temp;
	// foreach diagonal: foreach cell configuration: build map[diag][config]->r0
	for (unsigned int diag = 0; diag < 15; ++diag) {
		for (unsigned int config = 0; config < 256; ++config) {
			temp = 0ULL;
			unsigned int masked_config = config & diagmask[diag];
			for (int i = 0; i < 8; ++i) {
				if ((masked_config >> i)&0x1) {
					// convert index into r45 into index into r0
					temp |= (1ULL) << r45_2index[(diagshift[diag]+i)];
				}
			}
			r45_2_r0[diag][config] = temp;
		}
	}
}

void generate_r0_r45_2()
{
	uint64_t temp;
	for (unsigned int shift = 0; shift < 8; ++shift) {
		for (unsigned int config = 0; config < 256; ++config) {
			temp = 0ULL;
			for (int i = 0; i < 8; ++i) {
				if ((config >> i)&0x1) {
					temp |= 1ULL << r45_2map[(shift*8)+i];
				}
			}
			r0_r45_2[shift][config] = temp;
		}
	}
}

void generate_affectmask()
{
	unsigned char pos;
	for (int r = 0; r < 8; ++r) {
		for (int c = 0; c < 8; ++c) {
			pos = rc_off(r,c);
			affectmask[pos][0] = 0xFFULL << ((pos/8)*8);	// row in which pos is contained
			affectmask[pos][1] = 0x0101010101010101ULL << (pos%8);	// col in which pos is contained
			affectmask[pos][2] = 0ULL;
			affectmask[pos][3] = 0ULL;
			for (int cr = r, cc = c; cr < 8 && cc < 8; ++cr, ++cc) {	// go up to top left
				affectmask[pos][3] |= 1ULL << rc_off(cr,cc);
			}
			for (int cr = r, cc = c; cr >= 0 && cc >= 0; --cr, --cc) {	// go down to bottom right
				affectmask[pos][3] |= 1ULL << rc_off(cr,cc);
			}
			for (int cr = r, cc = c; cr < 8 && cc >= 0; ++cr, --cc) {	// go up to top right
				affectmask[pos][2] |= 1ULL << rc_off(cr,cc);
			}
			for (int cr = r, cc = c; cr >= 0 && cc < 8; --cr, ++cc) {	// go down to bottom left
				affectmask[pos][2] |= 1ULL << rc_off(cr,cc);
			}
		}
	}
}

void generate_r45map()
{
	for (int i = 0; i < 64; ++i) {
		r45map[r45index[i]] = i;
	}
}

void generate_r45_2map()
{
	for (int i = 0; i < 64; ++i) {
		r45_2map[r45_2index[i]] = i;
	}
}

void generate_lookup_tables()
{
    static bool generated = false;
    if (generated) return;
    generated = true;
	generate_r90_r0();
	generate_r0_r90();
	generate_r45_r0();
	generate_r45map();
	generate_r45_2_r0();
	generate_r45_2map();
	generate_r0_r45();	// This method and the next both depend on generate_r45map(_2)() having been called earlier
	generate_r0_r45_2();
	generate_affectmask();
	// Generate moves[][][] lookup table
	for (unsigned int black_config = 0; black_config < 256; ++ black_config) {
		for (unsigned int white_config = 0; white_config < 256; ++white_config) {
			moves[black_config][white_config][0] = 0;
			moves[black_config][white_config][1] = 0;
			// 7 6 5 4 3 2 1 0
			// Loop through each starting position for a black piece (<--)
			for (int i = 0; i < 6; ++i) {	// 7 and 8 cannot possibly be starting positions!
				if ((black_config >> i)&0x1) {	// We have a starting location
					if ((white_config >> (i+1))&0x1) {	// At least 1 white piece next to it
						int j = i+2;
						while ((j < 8)&&((white_config >> j)&0x1)) ++j;	// j is now the first excluded piece
						if (!(j == 8 || (black_config >> j)&0x1)) {		// We need a hole in which to place piece
							// We have a move.
							moves[black_config][white_config][0] |= 1 << j;
						}
					}
				}
				if ((black_config << i)&0x80) {	// Starting location in opp. direction (-->)
					if ((white_config << (i+1))&0x80) {	// At least 1 white piece next to it
						int j = i+2;
						while ((j<8)&&((white_config <<j)&0x80)) ++j;
						if (!(j == 8 || (black_config << j)&0x80)) {	// Need hole
							// Move found
							moves[black_config][white_config][0] |= 0x80 >> j;
						}
					}
				}
			}
			for (int i = 0; i < 6; ++i) {	// 7 and 8 cannot possibly be starting positions!
				if ((white_config >> i)&0x1) {	// We have a starting location
					if ((black_config >> (i+1))&0x1) {	// At least 1 white piece next to it
						int j = i+2;
						while ((j < 8)&&((black_config >> j)&0x1)) ++j;	// j is now the first excluded piece
						if (!(j == 8 || (white_config >> j)&0x1)) {		// We need a hole in which to place piece
							// We have a move.
							moves[black_config][white_config][1] |= 1 << j;
						}
					}
				}
				if ((white_config << i)&0x80) {	// Starting location in opp. direction (-->)
					if ((black_config << (i+1))&0x80) {	// At least 1 white piece next to it
						int j = i+2;
						while ((j<8)&&((black_config <<j)&0x80)) ++j;
						if (!(j == 8 || (white_config << j)&0x80)) {	// Need hole
							// Move found
							moves[black_config][white_config][1] |= 0x80 >> j;
						}
					}
				}
			}
		}
	}

	// Generate move_map[][][][] lookup table
	for (unsigned int move = 0; move < 8; ++move) {	// For each place where we will place a piece
		// For each configuration along a line
		for (unsigned int black_config = 0; black_config < 256; ++ black_config) {
			for (unsigned int white_config = 0; white_config < 256; ++white_config) {
				move_map[move][black_config][white_config][0] = 0;
				move_map[move][black_config][white_config][1] = 0;
				// If our move is in an occupied spot, skip over it.
				if (((1<<move) & black_config || (1<<move) & white_config)) continue;	// ok

				// Black

				// MUCH cleaner than old code!
				int j;
				// scan >>
				if ( move != 7 ) {	// This scan has no effect in this case!
					j = move;
					do { ++j; }
					while ( (j < 8) && ((white_config >> j)&0x1) );
					if ( (j < 8) && (black_config >> j)&0x1) {	// we have it!
						for (int p = move + 1; p < j; ++p) {
							move_map[move][black_config][white_config][0] |= (1 << p);
						}
					}
				}

				// scan <<
				if ( move != 0 ) {
					j = 7-move;
					do { ++j; }
					while ( (j < 8) && ((white_config << j)&0x80) );
					if ( (j < 8) && (black_config << j)&0x80 ) {
						for (int p = 7 - move + 1; p < j; ++p) {
							move_map[move][black_config][white_config][0] |= 0x80 >> p;
						}
					}
				}

				if ( move != 7 ) {	// This scan has no effect in this case!
					j = move;
					do { ++j; }
					while ( (j < 8) && ((black_config >> j)&0x1) );
					if ( (j < 8) && (white_config >> j)&0x1) {	// we have it!
						for (int p = move + 1; p < j; ++p) {
							move_map[move][black_config][white_config][1] |= 1 << p;
						}
					}
				}

				// scan <<
				if ( move != 0 ) {
					j = 7-move;
					do { ++j; }
					while ( (j < 8) && ((black_config << j)&0x80) );
					if ( (j < 8) && (white_config << j)&0x80 ) {
						for (int p = 7 - move + 1; p < j; ++p) {
							move_map[move][black_config][white_config][1] |= 0x80 >> p;
						}
					}
				}

				/*
				// White

				j = move+1;
				while(!(j == 8 || ((~((black_config >> j)&0x1))&((white_config >> j)&0x1)))) ++j;	// continue while we are sandwitching white pieces
				if ( (j != 8) && ((white_config >> j)&0x1)) {
					for (int p = move+1; p < j; ++p)
						move_map[move][black_config][white_config][1] |= 1<<p;
				}
				// scan <<
				// 0 1 2 3 4 5 6 7
				j = 7-move+1;
				while(!(j == 8 || ((~((black_config << j)&0x80))&((white_config << j)&0x80)))) ++j;	// continue while we are sandwitching white pieces
				if ((j!=8)&&((white_config << j)&0x80)) {
					for (int p = 7-move+1; p < j; ++p)
						move_map[move][black_config][white_config][1] |= 0x80>>p;
				}*/
			}
		}
	}
}

int bitboard_tests()
{
	// Black move generation
	// bww.wbw.
    // Black config: 10000100   = 132
    // White config: 01101010   = 106
	// Expected: 00010001 = 17
	unsigned char test1 = moves[132][106][0];
	if (test1 != 17) return 1;

	// bwwwbwww
	// Expected: 0
	unsigned char test2 = moves[136][119][0];
	if (test2 != 0) return 2;

	// b.w.bww.
	// Expected: 00000001 = 1
	unsigned char test3 = moves[136][38][0];
	if (test3 != 1) return 3;

	// White move generation
	// .bw.wb.w
	// Expected: 10000010 = 130
	unsigned char test4 = moves[68][41][1];
	if (test4 != 130) return 4;

	// Black flips generation
	// bww.wbw.
	// 76543210
	// Move: 4 (from right)
	// Expected: 01101000 = 104
	unsigned char test5 = move_map[4][132][106][0];
	if (test5 != 104) return 5;

	// bw.wwb.w
	// move: 5
	// Expected: 01011000 = 88
	unsigned char test6 = move_map[5][132][89][0];
	if (test6 != 88) return 6;

	// .wwwwbw.
	// move: 7
	// Expected: 01111000 = 120
	unsigned char test7 = move_map[7][4][122][0];
	if (test7 != 120) return 7;

	// White flips generation
	// wb.wbbw.
	// Move: 5
	// Expected: 01000000 = 64
	unsigned char test8 = move_map[5][76][146][1];
	if (test8 != 64) return 8;

	return 0;
}

uint64_t get_move_map(const oboard_t& board, bool color)
{
	// on normal r0 bitboard, each byte is a row
	// first row is the bottom one
	// on r90, each byte is col
	// first col is rightmost
	uint64_t ret = 0ULL;
	// do horizontal and vertical
	unsigned long long int temp;
	for (int i = 0; i < 8; ++i) {
		temp = moves[(board.b_r0 >> (i*8))&0xff][(board.w_r0 >> (i*8))&0xff][color];
		ret |= temp << (8*i);
		temp = moves[(board.b_r90 >> (i*8))&0xff][(board.w_r90 >> (i*8))&0xff][color];
		ret |= r90_r0[i][temp];
	}
	// do diagonals
	for (int diag = 0; diag < 15; ++diag) {
		temp = moves[((board.b_r45 >> diagshift[diag])&diagmask[diag])][((board.w_r45 >> diagshift[diag])&diagmask[diag])][color];
		//printf("r45 moves:\n");
		//show_bitboard(temp);
		ret |= r45_r0[diag][temp];
		//printf("r45_2 moves:\n");
		temp = moves[((board.b_r45_2 >> diagshift[diag])&diagmask[diag])][((board.w_r45_2 >> diagshift[diag])&diagmask[diag])][color];
		//show_bitboard(temp);
		ret |= r45_2_r0[diag][temp];
	}
	return ret;
}

oboard_t make_move(const oboard_t& old, unsigned char p, bool color)
{
	oboard_t ret = get_empty();
	uint64_t mask = (affectmask[p][0] | affectmask[p][1] | affectmask[p][2] | affectmask[p][3]);
	if (color == 0) {	// black
		ret.b_r0 = old.b_r0;
		ret.w_r0 = old.w_r0;
		unsigned long long int flips = 0ULL;
		//show_bitboard(~mask);
		// move_map[position][black_config][white_config][player]
		flips |= (unsigned long long int)move_map[p%8][(old.b_r0 >> ((p/8)*8))&0xFF][(old.w_r0 >> ((p/8)*8))&0xFF][0] << ((p/8)*8);	// add on row change
		flips |= r90_r0[p%8][move_map[p/8][(old.b_r90 >> ((p%8)*8))&0xFF][(old.w_r90 >> ((p%8)*8))&0xFF][0]];	// add on col change
		unsigned char diag = r45_pmap[p];
		unsigned char diag2 = r45_2pmap[p];
		flips |= r45_r0[diag][move_map[diag_offset[p]][(old.b_r45 >> diagshift[diag])&diagmask[diag]][(old.w_r45 >> diagshift[diag])&diagmask[diag]][0]];
		flips |= r45_2_r0[diag2][move_map[diag_offset_2[p]][(old.b_r45_2 >> diagshift[diag2])&diagmask[diag2]][(old.w_r45_2 >> diagshift[diag2])&diagmask[diag2]][0]];
		ret.b_r0 |= flips;
		// remove flipped pieces from white's board
		//show_bitboard(flips);
		ret.w_r0 &= ~(flips);

		// move_map only gives us flipped pieces. We need to add our new piece in as well
		ret.b_r0 |= 1ULL<<p;
	} else {	// white
		ret.b_r0 = old.b_r0;
		ret.w_r0 = old.w_r0;
		unsigned long long int flips = 0ULL;
		//show_bitboard(~mask);
		// move_map[position][black_config][white_config][player]
		flips |= (unsigned long long int)move_map[p%8][(old.b_r0 >> ((p/8)*8))&0xFF][(old.w_r0 >> ((p/8)*8))&0xFF][1] << ((p/8)*8);	// add on row change
		flips |= r90_r0[p%8][move_map[p/8][(old.b_r90 >> ((p%8)*8))&0xFF][(old.w_r90 >> ((p%8)*8))&0xFF][1]];	// add on col change
		unsigned char diag = r45_pmap[p];
		unsigned char diag2 = r45_2pmap[p];
		flips |= r45_r0[diag][move_map[diag_offset[p]][(old.b_r45 >> diagshift[diag])&diagmask[diag]][(old.w_r45 >> diagshift[diag])&diagmask[diag]][1]];
		flips |= r45_2_r0[diag2][move_map[diag_offset_2[p]][(old.b_r45_2 >> diagshift[diag2])&diagmask[diag2]][(old.w_r45_2 >> diagshift[diag2])&diagmask[diag2]][1]];
		//show_bitboard(flips);
		ret.w_r0 |= flips;
		// remove flipped pieces from white's board
		ret.b_r0 &= ~(flips);

		// move_map only gives us flipped pieces. We need to add our new piece in as well
		ret.w_r0 |= 1ULL<<p;
	}
	// finally, we update the rotated boards using lookup tables previously generated
	for (unsigned int shift = 0; shift < 8; ++shift) {
		ret.b_r90 |= r0_r90[shift][(ret.b_r0 >> (8*shift))&0xFF];	// ok
		ret.w_r90 |= r0_r90[shift][(ret.w_r0 >> (8*shift))&0xFF];	// ok
		ret.b_r45 |= r0_r45[shift][(ret.b_r0 >> (8*shift))&0xFF];
		ret.w_r45 |= r0_r45[shift][(ret.w_r0 >> (8*shift))&0xFF];
		ret.b_r45_2 |= r0_r45_2[shift][(ret.b_r0 >> (8*shift))&0xFF];
		ret.w_r45_2 |= r0_r45_2[shift][(ret.w_r0 >> (8*shift))&0xFF];
	}
	return ret;
}

bool is_legal_move(const oboard_t& brd, unsigned char p, bool color)
{
	return ((get_move_map(brd, color)&p) != 0);
}


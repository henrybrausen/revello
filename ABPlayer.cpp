#include "ABPlayer.h"

#include "Board.h"

#include "TranspositionTable.h"

#include <cstdlib>
#include <ctime>
#include <vector>

#include <stdint.h>

#include <algorithm>

#include <iostream>

#define INFINITY 99999999

// Constructor.
ABPlayer::ABPlayer(Board& brd, unsigned int tt_size)
    : Player(brd), m_TT(tt_size)
{
    m_nodesExpanded = 0;
    m_lastDepth = 0;
    m_mobilityWeight = 0;
    m_maxDepth = 99999;
}

// Think and generate a move using Alpha-Beta Search
unsigned char ABPlayer::genmove(bool color)
{
    std::cerr << "ABPlayer: Starting search . . ." << std::endl;
    std::cerr << "ABPlayer: Clearing TT . . . ";
    m_TT.clear();	// Clear transposition table.
    std::cerr << "Done." << std::endl;
    std::cerr << std::endl << "\tDPTH\tPOS\tVAL\tTOTAL NODES" << std::endl;
    m_nodesExpanded = 0;
    
    start_time();	// For benchmarking and timing.
    
    m_board.set_toPlay(color);
    
	// Grab current available moves as a bitstring
    uint64_t all_moves = m_board.get_moves();
    
    unsigned char pos;
    
    int move_count = 0;
    
    unsigned char best_pos = 0;
    int best_val = 0;
    int val;
    bool first;
    
    unsigned char last_best_pos = 0;
    unsigned char num_empties = m_board.num_empties();
    
    uint64_t moves;
    int depth;
	// For each search depth (iterative deepening) . . .
    for (depth = 1; depth <= m_maxDepth;) {
        m_iterNum = depth;
        moves = all_moves;
        first = true;
		// For each available move . . .
        while ((pos = Board::get_next_move(moves)) != MOVE_PASS) {
			// Try the move.
            m_board.move(pos);
			// Compute the value of the move.
            val = -alphabeta(depth, -INFINITY, INFINITY);
			// Undo the move (restore game state)
            m_board.undo();
			// If this was a better move, remember it!
            if (first || (val > best_val)) { best_val = val; best_pos = pos; }
            ++move_count;
            first = false;
            
            if (timeout()) {
				// We ran out of time!
                std::cerr << std::endl << "ABPlayer: Search timed out (" << m_time << "s elapsed)." << std::endl;
                goto finish;
            }
        }
		// If no moves were available, we need to pass.
        if (move_count == 0) {
            std::cerr << "\t" << depth << "\tPASS" << "\tN/A\t" << m_nodesExpanded << std::endl;
            goto finish;
        }
		// Output the best move we found using the current search depth, along with statistics.
        std::cerr << "\t" << depth << "\t" << (char)('A'+(7-best_pos%8)) << best_pos/8+1 << "\t" << best_val << "\t" << m_nodesExpanded << std::endl;
        ++depth;
        last_best_pos = best_pos;
		// Certain win/loss? Quit early!
        if (best_val >= INFINITY || best_val <= -INFINITY)
            break;
    }
    
    finish:	// We ran out of time, we won, or we're doomed!
    m_lastDepth = depth - 1;
    if (move_count == 0) return MOVE_PASS;
    return last_best_pos;
}

int ABPlayer::alphabeta(int depth, int alpha, int beta)
{
    if (timeout())	// Quit if we're out of time.
        return beta;
    ++m_nodesExpanded;
    // Is the current state in the transposition table?
	// If so, use the information that is there!
    const TTEntry *entry;
    if ((entry = m_TT.lookup(m_board.hash(), depth, m_iterNum)) != NULL) {
		// TODO: Some of this seems a bit sketchy, especially with iterative deepening involved. Verify!
        if (entry->type == EXACT)
            return entry->score;
        else if (entry->type == ALPHA)  // Upper bound
            beta = std::min(beta, entry->score);
        else if (entry->type == BETA)   // Lower bound
            alpha = std::max(alpha, entry->score);
        if (beta <= alpha)
            if (entry->type == ALPHA)
                return alpha;
            else
                return beta;
    }
    
	// Grab moves available from this position!
    uint64_t moves = m_board.get_moves();
    unsigned int move_count = m_board.num_moves(moves);
    
    // Quiescence-y improvement
    if (move_count == 1 && depth == 0) depth = 2;   // Search forced chains of moves up until their conclusion.
    
    if (depth <= 0 && m_board.last_move() != MOVE_PASS) {
		// We hit out search depth limit. Eval the board and return!
        int ret = m_mobilityWeight*Board::num_moves(moves) + m_board.evaluate_territory();
        //m_board.set_toPlay(!m_board.get_toPlay());
        //ret += m_mobilityWeight*Board::num_moves(m_board.get_moves());
        //m_board.set_toPlay(!m_board.get_toPlay());
        //int ret = m_board.evaluate_territory();
		// Update the TT with our evaluation.
        m_TT.insert(m_board.hash(), ret, EXACT, depth, m_iterNum, MOVE_PASS);
        return ret;
    }
    
    unsigned char pos = 0;
    int val;
    unsigned char best_move = 0;
    int best_value = -INFINITY-64;
    
    eType htype = ALPHA;
    
    bool first = true;
    
	// Killer move heuristic
    unsigned char killer = (entry == NULL)?(MOVE_PASS):(m_board.is_valid_move(entry->best_move, moves)?(entry->best_move):(MOVE_PASS));
    
    while ((killer != MOVE_PASS) || (pos = Board::get_next_move(moves)) != MOVE_PASS) {
        if (killer != MOVE_PASS) {
			// Use the killer move first if we have it!
            pos = killer;
            Board::mask_away(moves, killer);
            killer = MOVE_PASS;
        }
		// Make the move!
        m_board.move(pos);
        if (htype == EXACT) {
            val = -alphabeta(depth-1, -alpha-1, -alpha);    // Principal Variation Search
            if ((val > alpha) && (val  < beta)) // Must re-search
                val = -alphabeta(depth-1, -beta, -alpha);
        } else {
            val = -alphabeta(depth-1, -beta, -alpha);
        }
		// Un-do the move / restore game state.
        m_board.undo();
		// Did we find a better move?
        if (first || val > best_value) { best_move = pos; best_value = val; }
        first = false;
		
		// Update TT
        if (beta <= val) {
            m_TT.insert(m_board.hash(), beta, BETA, depth, m_iterNum, best_move);
            return beta;
        }
        if (val > alpha) {
            htype = EXACT;
            alpha = val;
        }
    }
    
	// If no moves are available . . .
    if (move_count == 0) {
        // If our opponent just passed . . .
        if (m_board.last_move() == MOVE_PASS) {
            // Return endgame evaluation
            int diff = m_board.stone_differential();
            int ret;
            if (diff > 0)
                ret = INFINITY+diff;
            else if (diff < 0)
                ret = -INFINITY+diff;
            else
                ret = 0;
//             std::cerr << "ENDGAME EVAL: " << ret << std::endl;
            m_TT.insert(m_board.hash(), ret, EXACT, depth, m_iterNum, MOVE_PASS);
            return ret;
        }
        // Otherwise, we pass.
        m_board.move(MOVE_PASS);
//         std::cerr << "TRYING " << depth << std::endl;
		// Continue the search . . .
        val = -alphabeta(depth-1, -beta, -alpha);
//         std::cerr << "EVAL: " << val << std::endl;
		// Restore gamestate.
        m_board.undo();
		
		// Update TT
        if (beta <= val)
            m_TT.insert(m_board.hash(), val, BETA, depth, m_iterNum, MOVE_PASS);
        else if (val > alpha)
            m_TT.insert(m_board.hash(), val, EXACT, depth, m_iterNum, MOVE_PASS);
        else
            m_TT.insert(m_board.hash(), val, ALPHA, depth, m_iterNum, MOVE_PASS);
        return val;
    }
    
	// Update TT after search.
    m_TT.insert(m_board.hash(), alpha, htype, depth, m_iterNum, best_move);
    return alpha;
}

// Dump player configuration to terminal.
void ABPlayer::show_info() const
{
    std::cout << "ABPlayer" << std::endl;
    std::cout << "\tMax Time: " << m_time << std::endl;
    std::cout << "\tMax Depth: " << m_maxDepth << std::endl;
    std::cout << "\tLast Depth: " << m_lastDepth << std::endl;
    std::cout << "\tTT Hits: " << m_TT.hits() << std::endl;
    std::cout << "\tTotal Nodes Expanded: " << m_nodesExpanded << std::endl;
    std::cout << "\tMobility Weight: " << m_mobilityWeight << std::endl;
}

// Configuration for GA Optimization
void ABPlayer::set_mobility_weight(const phenotype& ph)
{
    phenotype mask32 = 0xFFFFFFFF;
    phenotype temp = (ph >> 320)&mask32;
    m_mobilityWeight = temp.to_ulong()%1000;   // Limit size
}

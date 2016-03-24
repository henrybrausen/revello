#ifndef __TRANSPOSITIONTABLE_H__
#define __TRANSPOSITIONTABLE_H__

#include <stdint.h>

typedef enum { EXACT, ALPHA, BETA } eType;

typedef struct {
    int score;
    eType type;
    unsigned char depth;
    unsigned char best_move;
    uint64_t hash;
    unsigned char iter_num;
} TTEntry;

class TranspositionTable {
    public:
        TranspositionTable(unsigned long int size = 10000000);
        virtual ~TranspositionTable();
        
        const TTEntry * lookup(uint64_t hash, int depth, unsigned char iter_num) const;
        void insert(const TTEntry& entry);
        void insert(uint64_t hash, int score, eType type, int depth, unsigned char iter_num, int best_move);
        
        void clear();
        
        unsigned long hits() const { return m_hits; }
        
    private:
        TTEntry *m_table;
        unsigned long int m_tableSize;
        mutable unsigned long int m_hits;
};

#endif

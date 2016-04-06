#include "TranspositionTable.h"

#include <cstdlib>

TranspositionTable::TranspositionTable(unsigned long int size)
{
    m_table = new TTEntry[size];
    m_tableSize = size;
    
    clear();
}

TranspositionTable::~TranspositionTable()
{
    delete[] m_table;
}

const TTEntry * TranspositionTable::lookup(uint64_t hash, int depth, unsigned char iter_num) const
{
    //if(iter_num != m_table[hash%m_tableSize].iter_num || m_table[hash%m_tableSize].hash != hash || m_table[hash%m_tableSize].depth < depth) return NULL;
	if(m_table[hash%m_tableSize].hash != hash || m_table[hash%m_tableSize].depth < depth) return NULL;
    ++m_hits;
    return &m_table[hash%m_tableSize];
}

void TranspositionTable::insert(const TTEntry& entry)
{
    unsigned long int index = entry.hash%m_tableSize;
    if (m_table[index].depth > entry.depth) return;
    m_table[index].hash = entry.hash;
    m_table[index].score = entry.score;
    m_table[index].type = entry.type;
    m_table[index].depth = entry.depth;
    m_table[index].best_move = entry.best_move;
    m_table[index].iter_num = entry.iter_num;
    return;
}

void TranspositionTable::insert(uint64_t hash, int score, eType type, int depth, unsigned char iter_num, int best_move)
{
    unsigned long int index = hash%m_tableSize;
    //if (m_table[index].depth > depth) return;
    m_table[index].hash = hash;
    m_table[index].score = score;
    m_table[index].type = type;
    m_table[index].depth = depth;
    m_table[index].best_move = best_move;
    m_table[index].iter_num = iter_num;
    return;
}

void TranspositionTable::clear()
{
    for (unsigned long int i = 0; i < m_tableSize; ++i) {
        //m_table[i].hash = 0;
        //m_table[i].score = 0;
        //m_table[i].type = EXACT;
        m_table[i].depth = -1;
        //m_table[i].iter_num = 0;
    }
    m_hits = 0;
}

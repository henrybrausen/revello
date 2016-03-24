#ifndef __INDIVIDUAL_H__
#define __INDIVIDUAL_H__

#include "Board.h"
#include "ABPlayer.h"

#include <functional>

class Individual {
    public:
        Individual(phenotype ph = 0)
            : m_player(m_board), m_fitness(0)
        {
            set_phenotype(ph);
            m_player.set_max_depth(2);
        }
        
        ~Individual() {}
        
        void set_phenotype(const phenotype& ph);
        phenotype get_phenotype() const { return m_phenotype; }
        
        int get_fitness() const { return m_fitness; }
        void set_fitness(int fitness) { m_fitness = fitness; }
        
        void play_against(Individual& opponent);
        
        bool operator <(const Individual& rhs) { return (m_fitness < rhs.m_fitness); }
        bool operator >(const Individual& rhs) { return (m_fitness > rhs.m_fitness); }
        
        static Individual * produce_offspring(const Individual& i1, const Individual& i2, int crossover_prob, int mutation_prob);
        
        void show_info() const;
        
    private:
        Board m_board;
        ABPlayer m_player;
        int m_fitness;
        phenotype m_phenotype;
};

#endif

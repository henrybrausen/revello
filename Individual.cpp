#include "Individual.h"

#include <cstdlib>

#include <iostream>

void Individual::set_phenotype(const phenotype& ph)
{
    m_phenotype = ph;
    m_board.load_pos_values(m_phenotype);
    //m_player.set_mobility_weight(m_phenotype);
    m_player.set_mobility_weight(20);
}

void Individual::play_against(Individual& opponent)
{
    unsigned char tempmove;
    bool mycolor = rand()%2;
    bool currentplayer = P_BLACK;
    for (;;) {
        if (currentplayer == mycolor)
            tempmove = m_player.genmove(mycolor);
        else
            tempmove = opponent.m_player.genmove(!mycolor);
        m_player.play(tempmove, currentplayer);
        opponent.m_player.play(tempmove, currentplayer);
        currentplayer = !currentplayer;
        if (m_board.game_over()) {
            if (m_board.winner() == mycolor) {
                ++m_fitness;
                --opponent.m_fitness;
            } else if (m_board.winner() == !mycolor) {
                --m_fitness;
                ++opponent.m_fitness;
            }   // Else is a tie.
            break;
        }
    }
    m_board.reset();
    opponent.m_board.reset();
}

Individual * Individual::produce_offspring(const Individual& i1, const Individual& i2, int crossover_prob, int mutation_prob)
{
    bool crossover = false;
    if ((rand()%1000) < crossover_prob) crossover = true;
    
    int crossover_pos = rand()%352;
    
    phenotype child_ph;
    
    if (!crossover)
        child_ph = i1.m_phenotype;
    else
        for (int i = 0; i < 352; ++i) {
            if (i < crossover_pos)
                child_ph[i] = i1.m_phenotype[i];
            else
                child_ph[i] = i2.m_phenotype[i];
            if (rand()%1000 < mutation_prob)
                child_ph.flip(i);
        }
    return new Individual(child_ph);
}

void Individual::show_info() const
{
    std::cout << "Individual" <<  std::endl << "\tp: " << m_phenotype << std::endl << "\tf: " << m_fitness <<  std::endl;
    m_board.show_pos_values();
    m_player.show_info();
}

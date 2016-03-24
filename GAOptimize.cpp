#include "Individual.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <vector>

#define POP_SIZE 4

void tournament(std::vector<Individual *>& pop)
{
    for (int i = 0; i < pop.size() - 1; ++i) {
        for (int j = i + 1; j < pop.size(); ++j) {
            pop[i]->play_against(*pop[j]);
        }
    }
}

void mk_random_pop(std::vector<Individual *>& pop, int psize)
{
    phenotype temp;
    for (int i = 0; i < psize; ++i) {
        for (int j = 0; j < 352; ++j) {
            temp[j] = rand()%2;
        }
        pop.push_back(new Individual());
        pop[i]->set_phenotype(temp);
    }
}

void free_pop(std::vector<Individual *>& pop)
{
    for (int i = 0; i < pop.size(); ++i)
        delete pop[i];
}

bool cmp(const Individual* lhs, const Individual* rhs)
{
    return lhs->get_fitness() > rhs->get_fitness();
}

void rankify(std::vector<Individual *>& sorted)
{
    for (int i = 0; i < sorted.size(); ++i)
        sorted[i]->set_fitness((sorted.size()-i));
}

int fitness_sum(std::vector<Individual *>& pop)
{
    int ret = 0;
    for (int i = 0; i < pop.size(); ++i)
        ret += pop[i]->get_fitness();
    return ret;
}

int roulette_get(std::vector<Individual *>& pop, int fitsum)
{
    int target = rand()%fitsum;
    int running_sum = 0;
    int i = 0;
    for (;;) {
        running_sum += pop[i]->get_fitness();
        if (running_sum >= target)
            return i;
        ++i;
    }
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    
    // Create initial random population
    std::vector<Individual *> population;
    mk_random_pop(population, POP_SIZE);
    //for (int i = 0; i < POP_SIZE; ++i) population[i]->show_info();
    Individual *p1, *p2;
    
    // Iterate through each generation
    for (int g = 0; g < 600; ++g) {
        tournament(population);
        sort(population.begin(), population.end(), cmp);
        rankify(population);
        int fitsum = fitness_sum(population);
        std::vector<Individual *> new_population;
        new_population.push_back(Individual::produce_offspring(*population[0], *population[0], 0, 0));
        for (int i = 1; i < POP_SIZE; ++i) {
            // Choose parent 1
            int i1,i2;
            i1 = roulette_get(population, fitsum);
            i2 = roulette_get(population, fitsum);
            //std::cout << i1 << " " << i2 << std::endl;
            p1 = population[i1];
            p2 = population[i2];
            new_population.push_back(Individual::produce_offspring(*p1, *p2, 850, 5));
        }
        // Set new generation to current
        free_pop(population);
        population = new_population;
        std::cout << g << " ";
        std::cout.flush();
    }
    std::cout << std::endl;
    tournament(population);
    sort(population.begin(), population.end(), cmp);
    for (int i = 0; i < POP_SIZE; ++i) population[i]->show_info();
    free_pop(population);
    return 0;
}

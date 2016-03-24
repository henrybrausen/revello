#include "Individual.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

phenotype random_phenotype() {
    phenotype ret;
    for (int i = 0; i < 352; ++i)
        ret[i] = rand()%2;
    return ret;
}

phenotype uniform_phenotype(bool fill) {
    phenotype ret;
    for (int i = 0; i < 352; ++i)
        ret[i] = fill;
    return ret;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    Individual i1, i2;
    i1.set_phenotype(uniform_phenotype(0));
    i2.set_phenotype(uniform_phenotype(1));
    //i1.set_phenotype(random_phenotype());
    //i2.set_phenotype(random_phenotype());
    i1.show_info();
    i2.show_info();
    std::cout << "Reproducing . . ." << std::endl;
    Individual *i3 = Individual::produce_offspring(i1, i2, 1000, 1);
    i3->play_against(i1);
    i3->play_against(i2);
    i3->show_info();
    delete i3;
}

#include <iostream>

#include "Board.h"
#include "RandomPlayer.h"
#include "ABPlayer.h"
#include "OTPEngine.h"

// For position evaluation.
int values[10] = { 1000, -100, -200, 0, 0, 0, 0, 0, 0, 0 };

int main(int argc, char *argv[])
{
    Board MyBoard;
    MyBoard.load_pos_values(values);
    ABPlayer MyPlayer = ABPlayer(MyBoard, 10000000);
    MyPlayer.set_mobility_weight(50);
    
    
    OTPEngine MyEngine = OTPEngine();
    
    MyEngine.set_player(&MyPlayer);
    MyEngine.set_board(&MyBoard);
    
    return MyEngine.main_loop();
}

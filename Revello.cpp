#include <iostream>

#include "Board.h"
#include "RandomPlayer.h"
#include "ABPlayer.h"
#include "OTPEngine.h"

//int values[10] = { 400, -100, -100, 50, 10, 10, 30, 5, 5, 20 };
int values[10] = { 987, 617, 141, 540, 269, 946, 992, 459, 272, 440 };

int main(int argc, char *argv[])
{
    Board MyBoard;
    MyBoard.load_pos_values(values);
    ABPlayer MyPlayer = ABPlayer(MyBoard, 10000000);
    MyPlayer.set_mobility_weight(20);
    
    
    OTPEngine MyEngine = OTPEngine();
    
    MyEngine.set_player(&MyPlayer);
    MyEngine.set_board(&MyBoard);
    
    return MyEngine.main_loop();
}

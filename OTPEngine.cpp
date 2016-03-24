#include "OTPEngine.h"
#include "Board.h"

#include <iostream>

#include <cstdlib>
#include <cstring>

OTPEngine::OTPEngine()
{
    m_player = NULL;
    m_board = NULL;
    m_immediate = false;
    m_running = true;
}

void OTPEngine::set_player(Player *player)
{
    m_player = player;
}

void OTPEngine::set_board(Board *brd)
{
    m_board = brd;
}

void OTPEngine::set_immediate(bool state)
{
    m_immediate = state;
}

void OTPEngine::show_info() const
{
    std::cout << "OTPEngine " << ENGINE_VERSION << std::endl;
    std::cout << "\tImmediate Mode: " << ((m_immediate)?("on"):("off")) << std::endl;
    if (m_player == NULL)
        std::cout << "No player selected!" << std::endl;
    else
        m_player->show_info();
}

unsigned char OTPEngine::to_offset(const char *square)
{
    if (strcmp(square, "pass") == 0)
        return MOVE_PASS;
    if (strlen(square) != 2)
        return MOVE_INVALID;
    int col = 7 - (toupper(square[0]) - 'A');
    if (col < 0 || col > 7)
        return MOVE_INVALID;
    int row = atoi(square+1) - 1;
    if (row < 0 || row > 7)
        return MOVE_INVALID;
    return (unsigned char)(8*row+col);
}

bool OTPEngine::from_offset(unsigned char offset, char *dest)
{
    if (offset == MOVE_PASS) {
        strcpy(dest, "pass");
        return true;
    }
    if (offset > MOVE_PASS) {
        return false;
    }
    dest[0] = (char)((7 - (offset%8)) + 'A');
    dest[1] = (char)((offset/8)+'1');
    dest[2] = '\0';
    return true;
}

void OTPEngine::process_command(char *command_str)
{
    if (strcmp(command_str, "") == 0) {
        return;
    }
    char *tok;
    tok = strtok(command_str, " \t");
    
    // play command
    if (strcmp(tok, "play") == 0) {
        if (m_board == NULL || m_player == NULL) {
            std::cerr << "Player not initialized." << std::endl;
            return;
        }
        tok = strtok(NULL, " \t");
        if (tok == NULL) {
            std::cerr << "Error: play requires a color and a position." << std::endl;
            return;
        }
        char color_char = toupper(tok[0]);
        if (color_char == 'O') color_char = 'B';
        else if (color_char == '@') color_char = 'W';
        if (color_char != 'B' && color_char != 'W') {
            std::cerr << "Error: invalid color. Valid colors are B/W or O/@." << std::endl;
            return;
        }
        bool color = ((color_char == 'B')?(P_BLACK):(P_WHITE));
        tok = strtok(NULL, " \t");
        if (tok == NULL) {
            std::cerr << "Error: play requires a color and a position." << std::endl;
            return;
        }
        unsigned char pos;
        if ((pos = to_offset(tok)) == MOVE_INVALID) {
            std::cerr << "Error: position " << tok << " not valid." << std::endl;
            return;
        }
        if (!m_player->play(pos, color)) {
            std::cerr << "Error: illegal move." << std::endl;
            return;
        }
        return;
    }
    
    if (strcmp(tok, "genmove") == 0) {
        if (m_board == NULL || m_player == NULL) {
            std::cerr << "Player not initialized." << std::endl;
            return;
        }
        tok = strtok(NULL, " \t");
        if (tok == NULL) {
            std::cerr << "Error: genmove requires a color." << std::endl;
            return;
        }
        char color_char = toupper(tok[0]);
        if (color_char == 'O') color_char = 'B';
        else if (color_char == '@') color_char = 'W';
        if (color_char != 'B' && color_char != 'W') {
            std::cerr << "Error: invalid color. Valid colors are B/W or O/@." << std::endl;
            return;
        }
        bool color = (color_char == 'B')?(P_BLACK):(P_WHITE);
        unsigned char pos = m_player->genmove(color);
        char buf[5];
        if (!from_offset(pos, buf)) {
            std::cerr << "Error: illegal position generated!" << std::endl;
            return;
        }
        std::cout << "send " << color_char << ' ' << buf << std::endl;
        if (m_immediate)
            m_player->play(pos, color);
        return;
    }
    
    if (strcmp(tok, "evaluate") == 0) {
        if (m_board == NULL || m_player == NULL) {
            std::cerr << "Player not initialized." << std::endl;
            return;
        }
        int val = m_board->evaluate_territory();
        std::cout << "(" << ((m_board->get_toPlay() == P_BLACK)?("B"):("W")) << ") " << val << std::endl;
        return;
    }
    
    if (strcmp(tok, "showboard") == 0) {
        if (m_board == NULL || m_player == NULL) {
            std::cerr << "Player not initialized." << std::endl;
            return;
        }
        m_board->show();
        return;
    }
    
    if (strcmp(tok, "get_time") == 0) {
        if (m_board == NULL || m_player == NULL) {
            std::cerr << "Player not initialized." << std::endl;
            return;
        }
        std::cout << m_player->get_time() << std::endl;
        return;
    }
    
    if (strcmp(tok, "set_time") == 0) {
        if (m_board == NULL || m_player == NULL) {
            std::cerr << "Player not initialized." << std::endl;
            return;
        }
        tok = strtok(NULL, " \t");
        if (tok == NULL) {
            std::cerr << "Error: set_time requires a time." << std::endl;
            return;
        }
        int time = atoi(tok);
        if (time < 1) {
            std::cerr << "Error: time must be positive and > 1." << std::endl;
            return;
        }
        m_player->set_time(time);
        return;
    }
    
    if (strcmp(tok, "immediate") == 0) {
        tok = strtok(NULL, " \t");
        if (tok == NULL) {
            std::cerr << "Error: immediate requires an argument (on/off)." << std::endl;
            return;
        }
        if (!(strcmp(tok, "on") == 0 || strcmp(tok, "off") == 0)) {
            std::cerr << "Error: argument must be one of \"on\" or \"off\"." << std::endl;
            return;
        }
        set_immediate(strcmp(tok, "on") == 0);
        return;
    }
    
    if (strcmp(tok, "quit") == 0) {
        m_running = false;
        return;
    }
    
    if (strcmp(tok, "show_info") == 0) {
        show_info();
        return;
    }
    
    if (strcmp(tok, "undo") == 0) {
        if (m_board == NULL || m_player == NULL) {
            std::cerr << "Player not initialized." << std::endl;
            return;
        }
        m_board->undo();
        return;
    }
    
    if (strcmp(tok, "move_history") == 0) {
        if (m_board == NULL || m_player == NULL) {
            std::cerr << "Player not initialized." << std::endl;
            return;
        }
        std::cout << m_board->move_history() << std::endl;
        return;
    }
    
    if (strcmp(tok, "score") == 0) {
        if (m_board == NULL || m_player == NULL) {
            std::cerr << "Player not initialized." << std::endl;
            return;
        }
        if (!m_board->game_over())
            std::cout << "Warning: game is not over." << std::endl;
        int bstones = m_board->num_stones(P_BLACK);
        int wstones = m_board->num_stones(P_WHITE);
        if (bstones > wstones)
            std::cout << "Black (O) +" << bstones-wstones << std::endl;
        else if (bstones < wstones)
            std::cout << "White (@) +" << wstones-bstones << std::endl;
        else
            std::cout << "Tie" << std::endl;
        return;
    }
    
    std::cerr << "Error: command not recognized." << std::endl;
    return;
}

int OTPEngine::main_loop()
{
    while (m_running) {
        char input[256];
        std::cin.getline(input, 256);
        
        if (std::cin.eof()) {
            m_running = false;
            continue;
        }
        
        process_command(input);
    }
    
    return 0;
}

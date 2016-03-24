#ifndef __OTPENGINE_H__
#define __OTPENGINE_H__

#include "Player.h"
#include "Board.h"

#define ENGINE_VERSION 0.1

/* OTPEngine implements the application's text-based interface.
 */
class OTPEngine {
    public:
        OTPEngine();
        virtual ~OTPEngine() {}
        
        void set_player(Player *player);
        void set_board(Board *brd);
        
        void set_immediate(bool state);
        
        void show_info() const;
        
        void process_command(char *command_str);
        
        int main_loop();
        
    private:
        unsigned char to_offset(const char *square);    // A1 -> 07
        bool from_offset(unsigned char offset, char *dest);
        
        Board *m_board;
        Player *m_player;
        
        bool m_immediate;
        bool m_running;
};

#endif

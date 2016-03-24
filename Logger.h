#ifndef __LOGGER_H__
#define __LOGGER_H__

class RLog {
    public:
        static RLog * instance() {
            static Rlog inst;
            return &inst;
        }
        
        friend ostream &operator<<(ostream &stream, RLog log);
};



#endif

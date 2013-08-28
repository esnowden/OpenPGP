// Public-Key Packet
#include "packet.h"

#ifndef __TAG6__
#define __TAG6__
class Tag6 : public Key{
    protected:
        time_t time;
        uint8_t pka;
        std::vector <integer> mpi;

        // version 3
        uint32_t expire;

        void read_tag6(std::string & data);
        std::string show_tag6();
        std::string raw_tag6();

    public:
        Tag6();
        Tag6(std::string & data);
        virtual void read(std::string & data);
        virtual std::string show();
        virtual std::string raw();

        Tag6 * clone();

        time_t get_time();
        uint8_t get_pka();
        std::vector <integer> get_mpi();

        void set_time(time_t t);
        void set_pka(uint8_t p);
        void set_mpi(std::vector <integer> m);

        std::string get_fingerprint();
        std::string get_keyid();
};
#endif
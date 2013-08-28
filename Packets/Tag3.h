// Symmetric-Key Encrypted Session Key Packet
#include "../cfb.h"
#include "packet.h"

#ifndef __TAG3__
#define __TAG3__
class Tag3 : public Packet{
    private:
        uint8_t sym;
        S2K * s2k;
        std::string * esk; // encrypted session key

    public:
        Tag3();
        Tag3(std::string & data);
        ~Tag3();
        void read(std::string & data);
        std::string show();
        std::string raw();

        Tag3 * clone();

        void set_sym(uint8_t s);
        void set_s2k(S2K * s);
        void set_esk(std::string * s);
        void set_key(std::string pass, std::string sk = "");

        uint8_t get_sym();
        S2K * get_s2k();
        std::string * get_esk();
        std::string get_key(std::string pass);
};
#endif
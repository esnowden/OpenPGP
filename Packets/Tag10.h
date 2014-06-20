// Marker Packet
#ifndef __TAG10__
#define __TAG10__

#include "packet.h"

class Tag10 : public Packet{
    private:
        std::string pgp; // "PGP"

    public:
        typedef std::shared_ptr<Tag10> Ptr;

        Tag10();
        Tag10(std::string & data);
        void read(std::string & data);
        std::string show();
        std::string raw();

        std::string get_pgp();

        void set_pgp(const std::string & s = "PGP");

        Packet::Ptr clone();
};
#endif

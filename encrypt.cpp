#include "encrypt.h"

std::string encrypt(const std::string & data, PGP & pub, bool hash, uint8_t sym_alg){
    std::vector <Packet *> packets = pub.get_packets_pointers();
    Tag6 * public_key = new Tag6;

    // find first key pakcet with encrypting values
    bool found = false;
    for(Packet *& p : packets){
        if ((p -> get_tag() == 5) || (p -> get_tag() == 6)){
            std::string raw = p -> raw();
            public_key -> read(raw);
            if ((public_key -> get_pka() == 1) || (public_key -> get_pka() == 2) || (public_key -> get_pka() == 16)){
                found = true;
                break;
            }
            delete public_key;
            public_key = new Tag6;
        }
    }
    // if no primary key, use subkey
    if (!found){
        for(Packet *& p : packets){
            if ((p -> get_tag() == 7) || (p -> get_tag() == 14)){
                std::string raw = p -> raw();
                public_key -> read(raw);
                if ((public_key -> get_pka() == 1) || (public_key -> get_pka() == 2) || (public_key -> get_pka() == 16)){
                    found = true;
                    break;
                }
                delete public_key;
                public_key = new Tag6;
            }
        }
    }

    if (!found){
        std::cerr << "Error: No public key found";
        exit(1);
    }

    std::vector <integer> mpi = public_key -> get_mpi();

    Tag1 * tag1 = new Tag1;
    tag1 -> set_keyid(public_key -> get_keyid());
    tag1 -> set_pka(public_key -> get_pka());

    // do calculations

    // generate session key
    uint16_t key_len = Symmetric_Algorithm_Key_Length.at(Symmetric_Algorithms.at(sym_alg));
    std::string session_key = integer(BBS((unsigned int) key_len).rand(), 2).str(256, key_len >> 3);
    // get checksum of session key
    uint16_t sum = 0;
    for(char & x : session_key){
        sum += (unsigned char) x;
    }

    integer m(EME_PKCS1_ENCODE(std::string(1, sym_alg) + session_key + unhexlify(makehex(sum, 4)), (mpi[0]).bytes()), 256);
    // encrypt m
    if (public_key -> get_pka() < 3){ // RSA
        tag1 -> set_mpi({POW(m, mpi[1], mpi[0])});
    }
    else if (public_key -> get_pka() == 16){// ElGamal
        integer k = integer(BBS((unsigned int) public_key -> get_mpi()[0].bits()).rand(), 2);
        tag1 -> set_mpi({POW(mpi[1], k, mpi[0]), (m * POW(mpi[2], k, mpi[0])) % mpi[0]});
    }
    else{
        std::cerr << "Error: Unknown or Reserved Public Key Algorithm " << (int) public_key -> get_pka() << std::endl;
        exit(1);
    }

    // Literal Data Packet
    Tag11 tag11;
    tag11.set_format('t');
    tag11.set_filename("");
    tag11.set_time(0);
    tag11.set_literal(data);

    // generate prefix
    uint16_t BS = Symmetric_Algorithm_Block_Length.at(Symmetric_Algorithms.at(sym_alg)) >> 3;
    std::string prefix = unhexlify(makehex(integer(BBS((unsigned int) BS << 3).rand(), 2), BS << 1));

    Packet * encrypted = NULL;

    if (!hash){
        // Symmetrically Encrypted Data Packet
        Tag9 tag9;
        tag9.set_encrypted_data(use_OpenPGP_CFB_encrypt(sym_alg, 9, tag11.write(true), session_key, prefix));
        std::string raw = tag9.raw();
        encrypted = new Tag9;
        encrypted -> read(raw);
    }
    else{
        // Sym. Encrypted Integrity Protected Data Packet
        Tag18 tag18;
        tag18.set_protected_data(tag11.write(true));

        // Modification Detection Code Packet
        Tag19 tag19;
        tag19.set_hash(use_hash(2, prefix + prefix.substr(BS - 2, 2) + tag18.get_protected_data() + "\xd3\x14"));

        // encrypt((literal_data_packet(plain text) + MDC SHA1(20 bytes)))
        tag18.set_protected_data(use_OpenPGP_CFB_encrypt(sym_alg, 18, tag18.get_protected_data() + tag19.write(), session_key, prefix));

        std::string raw = tag18.raw();
        encrypted = new Tag18;
        encrypted -> read(raw);
    }

    // write data to output container
    PGP out;
    out.set_ASCII_Armor(0);
    std::vector <std::pair <std::string, std::string> > header;
    header.push_back(std::pair <std::string, std::string> ("Version", "cc"));
    out.set_Armor_Header(header);
    packets = {tag1, encrypted};
    out.set_packets(packets);

    // erase data
    m = 0;
    session_key = "";
    prefix = "";
    delete public_key;
    delete encrypted;
    return out.write();
}
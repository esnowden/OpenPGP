/*
PGPCleartextSignature.h
OpenPGP Cleartext Signature Framework data structure (RFC 4880 sec 7)

Copyright (c) 2013 - 2017 Jason Lee @ calccrypto at gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef __PGP_CLEARTEXT_SIGNATURE__
#define __PGP_CLEARTEXT_SIGNATURE__

#include "PGP.h"
#include "PGPDetachedSignature.h"
#include "sigcalc.h"

// 7. Cleartext Signature Framework
//
//   It is desirable to be able to sign a textual octet stream without
//   ASCII armoring the stream itself, so the signed text is still
//   readable without special software. In order to bind a signature to
//   such a cleartext, this framework is used. (Note that this framework
//   is not intended to be reversible. RFC 3156 [RFC3156] defines another
//   way to sign cleartext messages for environments that support MIME.)
//
//   The cleartext signed message consists of:
//
//     - The cleartext header ’-----BEGIN PGP SIGNED MESSAGE-----’ on a
//       single line,
//
//     - One or more "Hash" Armor Headers,
//
//     - Exactly one empty line not included into the message digest,
//
//     - The dash-escaped cleartext that is included into the message
//       digest,
//
//     - The ASCII armored signature(s) including the ’-----BEGIN PGP
//       SIGNATURE-----’ Armor Header and Armor Tail Lines.
//
//   If the "Hash" Armor Header is given, the specified message digest
//   algorithm(s) are used for the signature. If there are no such
//   headers, MD5 is used. If MD5 is the only hash used, then an
//   implementation MAY omit this header for improved V2.x compatibility.
//   If more than one message digest is used in the signature, the "Hash"
//   armor header contains a comma-delimited list of used message digests.
//   Current message digest names are described below with the algorithm
//   IDs.
//
//   An implementation SHOULD add a line break after the cleartext, but
//   MAY omit it if the cleartext ends with a line break. This is for
//   visual clarity.

class PGPCleartextSignature {
    private:
        PGP::Armor_Keys hash_armor_header;
        std::string message;
        PGPDetachedSignature sig;

    public:
        typedef std::shared_ptr <PGPCleartextSignature> Ptr;

        PGPCleartextSignature();
        PGPCleartextSignature(const PGPCleartextSignature & copy);
        PGPCleartextSignature(const std::string & data);
        PGPCleartextSignature(std::istream & stream);

        void read(const std::string & data);
        void read(std::istream & stream);
        std::string show(const uint8_t indents = 0, const uint8_t indent_size = 4) const;
        std::string write(uint8_t header = 0) const;

        PGP::Armor_Keys get_hash_armor_header() const;
        std::string get_message() const;
        PGPDetachedSignature get_sig() const;

        void set_hash_armor_header(const PGP::Armor_Keys & keys);
        void set_message(const std::string & data);
        void set_sig(const PGPDetachedSignature & s);

        static std::string dash_escape(const std::string & text);
        static std::string reverse_dash_escape(const std::string & text);
        std::string data_to_text() const;                            // remove trailing whitespace
        static std::string data_to_text(const std::string & text);   // remove trailing whitespace

        bool meaningful() const;

        PGPCleartextSignature & operator=(const PGPCleartextSignature & copy);
        PGPCleartextSignature::Ptr clone() const;
};

#endif

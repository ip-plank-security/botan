#include "tests.h"

#include <botan/libstate.h>
#include <botan/stream_cipher.h>
#include <botan/hex.h>
#include <iostream>
#include <fstream>

using namespace Botan;

namespace {

bool stream_test(const std::string& algo,
                const std::string& key_hex,
                const std::string& in_hex,
                const std::string& out_hex)
   {
   const secure_vector<byte> key = hex_decode_locked(key_hex);
   const secure_vector<byte> pt = hex_decode_locked(in_hex);
   const secure_vector<byte> ct = hex_decode_locked(out_hex);

   Algorithm_Factory& af = global_state().algorithm_factory();

   const auto providers = af.providers_of(algo);
   size_t fails = 0;

   for(auto provider: providers)
      {
      const StreamCipher* proto = af.prototype_stream_cipher(algo, provider);

      if(!proto)
         {
         std::cout << "Unable to get " << algo << " from " << provider << "\n";
         ++fails;
         continue;
         }

      std::unique_ptr<StreamCipher> cipher(proto->clone());
      cipher->set_key(key);
      secure_vector<byte> buf = pt;

      cipher->encrypt(buf);

      if(buf != ct)
         {
         std::cout << algo << " " << provider << " enc " << hex_encode(buf) << " != " << out_hex << "\n";
         ++fails;
         }
      }

   return (fails == 0);
   }

}

size_t test_stream()
   {
   std::ifstream vec("checks/stream.vec");

   return run_tests_bb(vec, "StreamCipher", "Out", true,
             [](std::map<std::string, std::string> m) -> bool
             {
             return stream_test(m["StreamCipher"], m["Key"], m["In"], m["Out"]);
             });
   }

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <boost/filesystem.hpp>

#include "main.cpp"

namespace little_endian_io {
    
    template <typename Word>
    std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word )) {
    for (; size; --size, value >>= 8)
        outs.put( static_cast <char> (value & 0xFF));
    return outs;
  }
}

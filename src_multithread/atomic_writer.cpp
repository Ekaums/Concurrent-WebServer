#include "atomic_writer.h"

Atomic_cout& Atomic_cout::operator<< (std::ostream& (*f)(std::ostream&)){
  stream << f;
  return *this;
}

Atomic_cout::~Atomic_cout(){
  std::cout << stream.str();
}

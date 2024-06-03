#pragma once

#include <iostream>
#include <sstream>

class Atomic_cout{
    private:
    std::ostringstream stream;

    public:
    template <typename T>
    Atomic_cout& operator <<(T const& output);

    Atomic_cout& operator <<(std::ostream& (*f)(std::ostream&));

    ~Atomic_cout();
};


/* 
   Templated function must come along in the header file, to be supplied to source files.
   This is because this isn't a function itself. It can't be linked into a cpp file. Instead, this definition
   is used to create a function at compile time (just like the template of a function)
*/
template <typename T>
Atomic_cout& Atomic_cout::operator<< (T const& output){ // vs T const& t??
    stream << output;
    return *this;
}
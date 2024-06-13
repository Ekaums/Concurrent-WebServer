#pragma once

#include <iostream>
#include <sstream>

/* Needed so that printing lines to terminal can be done atomically.
   A std::cout with multiple parts (i.e. std::cout << "hello " << first_name << " " << last_name << std::endl)
   Will treat each section (in between a '<<' atomically) but not the whole thing!
   The solution: Write entire line into a ostringstream and feed that to std::cout as one string
*/
class Atomic_cout{
    private:
    std::ostringstream stream;

    public:
    // Any value passed to stream operator
    template <typename T>
    Atomic_cout& operator <<(T const &output);

    // Specifically for ostream functions (in this case, just for std::endl)
    Atomic_cout& operator <<(std::ostream& (*f)(std::ostream&));

    ~Atomic_cout();
};


// Must supply templated function in header in order for a function to be created (explained more in journal.md)
template <typename T>
Atomic_cout& Atomic_cout::operator<< (T const &output){
    stream << output;
    return *this;
}
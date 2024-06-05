#pragma once

#include <iostream>
#include <sstream>

class Atomic_cout{
    private:
    std::ostringstream stream;

    public:
    template <typename T>
    Atomic_cout& operator <<(T const &output);

    Atomic_cout& operator <<(std::ostream& (*f)(std::ostream&));

    ~Atomic_cout();
};



template <typename T>
Atomic_cout& Atomic_cout::operator<< (T const &output){
    stream << output;
    return *this;
}
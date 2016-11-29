#pragma once

#include <iostream>

namespace Common {

class Log {
public:
    static std::ostream& i() {return std::cout << "I: ";}
    static std::ostream& d() {return std::cout << "D: ";}
    static std::ostream& w() {return std::cout << "W: ";}
    static std::ostream& e() {return std::cout << "E: ";}
};

}

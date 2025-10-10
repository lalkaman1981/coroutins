#include <string>
#include <iostream>

#include <printf.h>
#include "create_stack.h"

void func2(std::string str1, std::string str2) {
    std::cout << str2 << " " << str1 << std::endl;
}

void func(std::string str1, std::string str2) {
    std::cout << str1 << " " << str2 << std::endl;
    call_coro(func2, str1, str2);
}

int main() {
    std::string str1 = "bibaboba";
    std::string str2 = "hi!";

    call_coro(func, str1, str2);
}
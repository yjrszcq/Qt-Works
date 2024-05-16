#ifndef PASSWORD_H
#define PASSWORD_H
#include <iostream>

class Password{

public:
    Password(std::string, std::string, std::string);
private:
    std::string website;
    std::string username;
    std::string  password;
};

#endif // PASSWORD_H

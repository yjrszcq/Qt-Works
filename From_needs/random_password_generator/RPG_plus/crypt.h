#ifndef CRYPT_H
#define CRYPT_H
#include <Windows.h>
#include <string>

class Crypt
{
public:
    Crypt();
    static std::string encrypt_password(const std::string& password);
    static std::string decrypt_password(const std::string& encrypted_password);
private:
    static std::string base64_encode(const std::string& input);
    static std::string base64_decode(const std::string& input);
    static std::string base32_encode(const std::string& input);
    static std::string base32_decode(const std::string& input);
};

#endif // CRYPT_H

#ifndef CRYPT_H
#define CRYPT_H
#include <Windows.h>
#include <string>

namespace Crypt{
std::string encrypt_password(const std::string& password);
std::string decrypt_password(const std::string& encrypted_password);
std::string base64_encode(const std::string& input);
std::string base64_decode(const std::string& input);
std::string base32_encode(const std::string& input);
std::string base32_decode(const std::string& input);
}

#endif // CRYPT_H

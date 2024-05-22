#ifndef CRYPT_H
#define CRYPT_H

#ifndef B32L
#define B32L 32
#endif

#ifndef B64L
#define B64L 64
#endif

#include <Windows.h>
#include <string>

namespace Crypt{
    //base64_base32
    std::string username_to_base_key(const std::string& username, const DWORD key_length);
    std::string base64_encode(const std::string& input, const std::string& key);
    std::string base64_decode(const std::string& input, const std::string& key);
    std::string base32_encode(const std::string& input, const std::string& key);
    std::string base32_decode(const std::string& input, const std::string& key);
    //xor
    DWORD username_to_xor_key(const std::string& username);
    std::string xor_crypt(const std::string& password, const DWORD& key);
    //md5
    std::string username_to_md5_to_str(const std::string username);
    std::string username_to_md5_to_base_key(const std::string username, const DWORD key_length);
    DWORD username_to_md5_to_xor_key(const std::string username);
    BOOL CalculateHash(BYTE* pData, DWORD dwDataLength, ALG_ID algHashType, BYTE** ppHashData, DWORD* pdwHashDataLength);
    //mix_base64_base32_encrypt
    std::string encrypt_password_base64_base32(const std::string& password, const std::string& base64_key, const std::string& base32_key);
    std::string encrypt_password_base64_base32(const std::string& password, const std::string& username);
    std::string encrypt_password_base64_base32_md5(const std::string& password, const std::string& username);
    //mix_base64_base32_decrypt
    std::string decrypt_password_base64_base32(const std::string& encrypted_password, const std::string& base64_key, const std::string& base32_key);
    std::string decrypt_password_base64_base32(const std::string& encrypted_password, const std::string& username);
    std::string decrypt_password_base64_base32_md5(const std::string& encrypted_password, const std::string& username);
    //mix_base64_xor_encrypt
    std::string encrypt_password_base64_xor(const std::string& password, const std::string& base64_key, const DWORD& xor_key);
    std::string encrypt_password_base64_xor(const std::string& password, const std::string& username);
    std::string encrypt_password_base64_xor_md5(const std::string& password, const std::string& username);
    //mix_base64_xor_decrypt
    std::string decrypt_password_base64_xor(const std::string& encrypted_password, const std::string& base64_key, const DWORD& xor_key);
    std::string decrypt_password_base64_xor(const std::string& encrypted_password, const std::string& username);
    std::string decrypt_password_base64_xor_md5(const std::string& encrypted_password, const std::string& username);
    //mix_base32_xor_encrypt
    std::string encrypt_password_base32_xor(const std::string& password, const std::string& base32_key, const DWORD& xor_key);
    std::string encrypt_password_base32_xor(const std::string& password, const std::string& username);
    std::string encrypt_password_base32_xor_md5(const std::string& password, const std::string& username);
    //mix_base32_xor_decrypt
    std::string decrypt_password_base32_xor(const std::string& encrypted_password, const std::string& base32_key, const DWORD& xor_key);
    std::string decrypt_password_base32_xor(const std::string& encrypted_password, const std::string& username);
    std::string decrypt_password_base32_xor_md5(const std::string& encrypted_password, const std::string& username);
}

#endif // CRYPT_H

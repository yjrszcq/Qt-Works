#include "crypt.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>


//base64_base32
std::string Crypt::username_to_base_key(const std::string& username, const DWORD key_length) {
    if(username.empty()){
        return "";
    }
    const DWORD username_length = username.length();
    int is_use_char[128] = {0};
    is_use_char[61] = 1;
    std::string key;
    if(key_length > username_length){
        int n = key_length / username_length + (key_length % username_length == 0 ? 0 : 1);
        for(DWORD i = 0; i < username_length; i ++){
            for(int j = 0; j < n; j ++){
                char key_char;
                if(j % 2 == 0){
                    key_char = (char)(((int)username[i] + j * 5) % (127 - 33) + 33);
                    while(is_use_char[(int)key_char] != 0){
                        if(key_char == 126){
                            key_char = 33;
                        } else{
                            key_char += 1;
                        }
                    }
                } else{
                    key_char = (char)(abs(((int)username[i] - j * 5)) % (127 - 33) + 33);
                    while(is_use_char[(int)key_char] != 0){
                        if(key_char == 33){
                            key_char = 126;
                        } else{
                            key_char -= 1;
                        }
                    }
                }
                is_use_char[(int)key_char] = 1;
                key += key_char;
                if(key.length() >= key_length){
                    break;
                }
            }
            if(key.length() >= key_length){
                break;
            }
        }
    } else{
        int j = 0;
        for(DWORD i = username_length - 1; i >= username_length - key_length; i --){
            char key_char;
            if(i % 2 == 0){
                key_char = (char)(((int)username[i] + j * 5) % (127 - 33) + 33);
                while(is_use_char[(int)key_char] != 0){
                    if(key_char == 126){
                        key_char = 33;
                    } else{
                        key_char += 1;
                    }
                }
            } else{
                key_char = (char)(abs(((int)username[i] - j * 5)) % (127 - 33) + 33);
                while(is_use_char[(int)key_char] != 0){
                    if(key_char == 33){
                        key_char = 126;
                    } else{
                        key_char -= 1;
                    }
                }
            }
            j ++;
            if(j >= 4){
                j = 0;
            }
            is_use_char[(int)key_char] = 1;
            key += key_char;
            if(key.length() >= key_length){
                break;
            }
        }
    }
    return key;
}

std::string Crypt::base64_encode(const std::string& input, const std::string& key) {
    // Implementation of base64 encoding
    std::string base64_chars = (key == "" ? "z+2O/kFgiStBfXoMlWPu3QpGh450679LJcHYm8UvCaxynIAsrE1eqjVwNdbTKZRD" : key);
    std::string encoded;
    int i = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    for (const auto& c : input) {
        char_array_3[i++] = c;
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (int k = 0; k < 4; k++) {
                encoded += base64_chars[char_array_4[k]];
            }
            i = 0;
        }
    }

    if (i > 0) {
        for (int k = i; k < 3; k++) {
            char_array_3[k] = '\0';
        }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (int k = 0; k < i + 1; k++) {
            encoded += base64_chars[char_array_4[k]];
        }

        while (i++ < 3) {
            encoded += '=';
        }
    }

    return encoded;
}

std::string Crypt::base64_decode(const std::string& input, const std::string& key) {
    // Implementation of base64 decoding
    std::string base64_chars = (key == "" ? "z+2O/kFgiStBfXoMlWPu3QpGh450679LJcHYm8UvCaxynIAsrE1eqjVwNdbTKZRD" : key);
    std::string decoded;
    int i = 0;
    unsigned char char_array_4[4];
    unsigned char char_array_3[3];

    for (const auto& c : input) {
        if (c == '=') {
            break;
        }
        char_array_4[i++] = c;
        if (i == 4) {
            for (int k = 0; k < 4; k++) {
                char_array_4[k] = base64_chars.find(char_array_4[k]);
            }
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (int k = 0; k < 3; k++) {
                decoded += char_array_3[k];
            }
            i = 0;
        }
    }

    if (i > 0) {
        for (int k = 0; k < i; k++) {
            char_array_4[k] = base64_chars.find(char_array_4[k]);
        }
        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

        for (int k = 0; k < i - 1; k++) {
            decoded += char_array_3[k];
        }
    }

    return decoded;
}

std::string Crypt::base32_encode(const std::string& input, const std::string& key){
    // Implementation of base32 encoding
    std::string base32_chars = (key == "" ? "t6wh7EgAcQNdRsIpF4loXMJvBYzU235k" : key);
    std::string encoded;
    int i = 0;
    unsigned char char_array_5[5];
    unsigned char char_array_8[8];

    for (const auto& c : input) {
        char_array_5[i++] = c;
        if (i == 5) {
            char_array_8[0] = (char_array_5[0] & 0xf8) >> 3;
            char_array_8[1] = ((char_array_5[0] & 0x07) << 2) + ((char_array_5[1] & 0xc0) >> 6);
            char_array_8[2] = ((char_array_5[1] & 0x3e) >> 1);
            char_array_8[3] = ((char_array_5[1] & 0x01) << 4) + ((char_array_5[2] & 0xf0) >> 4);
            char_array_8[4] = ((char_array_5[2] & 0x0f) << 1) + ((char_array_5[3] & 0x80) >> 7);
            char_array_8[5] = ((char_array_5[3] & 0x7c) >> 2);
            char_array_8[6] = ((char_array_5[3] & 0x03) << 3) + ((char_array_5[4] & 0xe0) >> 5);
            char_array_8[7] = (char_array_5[4] & 0x1f);

            for (int k = 0; k < 8; k++) {
                encoded += base32_chars[char_array_8[k]];
            }
            i = 0;
        }
    }

    if (i > 0) {
        for (int k = i; k < 5; k++) {
            char_array_5[k] = '\0';
        }

        char_array_8[0] = (char_array_5[0] & 0xf8) >> 3;
        char_array_8[1] = ((char_array_5[0] & 0x07) << 2) + ((char_array_5[1] & 0xc0) >> 6);
        char_array_8[2] = ((char_array_5[1] & 0x3e) >> 1);
        char_array_8[3] = ((char_array_5[1] & 0x01) << 4) + ((char_array_5[2] & 0xf0) >> 4);
        char_array_8[4] = ((char_array_5[2] & 0x0f) << 1) + ((char_array_5[3] & 0x80) >> 7);
        char_array_8[5] = ((char_array_5[3] & 0x7c) >> 2);
        char_array_8[6] = ((char_array_5[3] & 0x03) << 3) + ((char_array_5[4] & 0xe0) >> 5);

        for (int k = 0; k < i + 1; k++) {
            encoded += base32_chars[char_array_8[k]];
        }

        while (i++ < 5) {
            encoded += '=';
        }
    }

    return encoded;
}

std::string Crypt::base32_decode(const std::string& input, const std::string& key){
    // Implementation of base32 decoding
    std::string base32_chars = (key == "" ? "t6wh7EgAcQNdRsIpF4loXMJvBYzU235k" : key);
    std::string decoded;
    int i = 0;
    unsigned char char_array_8[8];
    unsigned char char_array_5[5];

    for (const auto& c : input) {
        if (c == '=') {
            break;
        }
        char_array_8[i++] = c;
        if (i == 8) {
            for (int k = 0; k < 8; k++) {
                char_array_8[k] = base32_chars.find(char_array_8[k]);
            }
            char_array_5[0] = (char_array_8[0] << 3) + ((char_array_8[1] & 0x1c) >> 2);
            char_array_5[1] = ((char_array_8[1] & 0x03) << 6) + (char_array_8[2] << 1) + ((char_array_8[3] & 0x10) >> 4);
            char_array_5[2] = ((char_array_8[3] & 0x0f) << 4) + ((char_array_8[4] & 0x1e) >> 1);
            char_array_5[3] = ((char_array_8[4] & 0x01) << 7) + (char_array_8[5] << 2) + ((char_array_8[6] & 0x18) >> 3);
            char_array_5[4] = ((char_array_8[6] & 0x07) << 5) + char_array_8[7];

            for (int k = 0; k < 5; k++) {
                decoded += char_array_5[k];
            }
            i = 0;
        }
    }

    if (i > 0) {
        for (int k = 0; k < i; k++) {
            char_array_8[k] = base32_chars.find(char_array_8[k]);
        }
        char_array_5[0] = (char_array_8[0] << 3) + ((char_array_8[1] & 0x1c) >> 2);
        char_array_5[1] = ((char_array_8[1] & 0x03) << 6) + (char_array_8[2] << 1) + ((char_array_8[3] & 0x10) >> 4);
        char_array_5[2] = ((char_array_8[3] & 0x0f) << 4) + ((char_array_8[4] & 0x1e) >> 1);

        for (int k = 0; k < i - 1; k++) {
            decoded += char_array_5[k];
        }
    }

    return decoded;
}

//xor
DWORD Crypt::username_to_xor_key(const std::string& username){
    if(username.empty()){
        return 0;
    }
    DWORD xor_key = 0;
    for (DWORD i = 0; i < username.length(); i++)
    {
        if(i % 2 == 0){
            xor_key = xor_key * 3 + username[i] * 4;
        } else{
            long long temp = xor_key * 2 - username[i] * 5;
            if(temp >= 0){
                xor_key = temp;
            } else{
                xor_key = -temp;
            }
        }
    }
    return xor_key;
}

std::string Crypt::xor_crypt(const std::string& password, const DWORD& key){
    DWORD xor_key = (key == 0 ? 745672539 : key);
    char xor_password[password.length() + 1];
    xor_password[password.length()] = '\0';
    for (DWORD i = 0; i < password.length(); i++){
        xor_password[i] = password[i] ^ xor_key;
    }
    return std::string(xor_password);
}

//md5
std::string Crypt::username_to_md5_to_str(const std::string username) {
    BYTE* pHashData = NULL;
    DWORD dwHashDataLength = 0;
    CalculateHash((BYTE *)username.c_str(), username.length(), CALG_MD5, &pHashData, &dwHashDataLength);
    std::string key;
    for(DWORD i = 0; i < dwHashDataLength; i ++){
        key += (char)(((int)pHashData[i]) % (91 - 65) + 65);
    }
    if(pHashData != NULL){
        delete pHashData;
    }
    return key;
}

std::string Crypt::username_to_md5_to_base_key(const std::string username, const DWORD key_length) {
    return username_to_base_key(username_to_md5_to_str(username), key_length);
}

DWORD Crypt::username_to_md5_to_xor_key(const std::string username) {
    return username_to_xor_key(username_to_md5_to_str(username));
}

BOOL Crypt::CalculateHash(BYTE* pData, DWORD dwDataLength, ALG_ID algHashType, BYTE** ppHashData, DWORD* pdwHashDataLength) {
    HCRYPTPROV hCryptProv = NULL;
    HCRYPTHASH hCryptHash = NULL;
    BYTE* pHashData = NULL;
    DWORD dwHashDataLength = 0;
    DWORD dwTemp = 0;
    BOOL bRet = FALSE;
    do
    {
        bRet = CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
        bRet = CryptCreateHash(hCryptProv, algHashType, NULL, NULL, &hCryptHash);
        bRet = CryptHashData(hCryptHash, pData, dwDataLength, 0);
        dwTemp = sizeof(dwHashDataLength);
        bRet = CryptGetHashParam(hCryptHash, HP_HASHSIZE, (BYTE*)(&dwHashDataLength), &dwTemp, 0);
        pHashData = new BYTE[dwHashDataLength];
        RtlZeroMemory(pHashData, dwHashDataLength);
        bRet = ::CryptGetHashParam(hCryptHash, HP_HASHVAL, pHashData, &dwHashDataLength, 0);
        *ppHashData = pHashData;
        *pdwHashDataLength = dwHashDataLength;
    } while (FALSE);

    if (FALSE == bRet)
    {
        if (pHashData)
        {
            delete[]pHashData;
            pHashData = NULL;
        }
    }
    if (hCryptHash || hCryptProv)
    {
        CryptDestroyHash(hCryptHash);
        CryptReleaseContext(hCryptProv, 0);
    }
    return bRet;
}

//mix_base64_base32_encrypt
std::string Crypt::encrypt_password_base64_base32(const std::string& password, const std::string& base64_key, const std::string& base32_key) {
    std::string base64_encoded = base64_encode(password, base64_key);
    std::string base32_encoded = base32_encode(base64_encoded, base32_key);
    return base32_encoded;
}

std::string Crypt::encrypt_password_base64_base32(const std::string& password, const std::string& username) {
    return encrypt_password_base64_base32(password, username_to_base_key(username, B64L), username_to_base_key(username, B32L));
}

std::string Crypt::encrypt_password_base64_base32_md5(const std::string& password, const std::string& username){
    return encrypt_password_base64_base32(password, username_to_md5_to_base_key(username, B64L), username_to_md5_to_base_key(username, B32L));
}

//mix_base64_base32_decrypt
std::string Crypt::decrypt_password_base64_base32(const std::string& encrypted_password, const std::string& base64_key, const std::string& base32_key) {
    std::string base32_decoded = base32_decode(encrypted_password, base64_key);
    std::string base64_decoded = base64_decode(base32_decoded, base32_key);
    return base64_decoded;
}

std::string Crypt::decrypt_password_base64_base32(const std::string& encrypted_password, const std::string& username) {
    return decrypt_password_base64_base32(encrypted_password, username_to_base_key(username, B64L), username_to_base_key(username, B32L));
}

std::string Crypt::decrypt_password_base64_base32_md5(const std::string& encrypted_password, const std::string& username) {
    return decrypt_password_base64_base32(encrypted_password, username_to_md5_to_base_key(username, B64L), username_to_md5_to_base_key(username, B32L));
}

//mix_base64_xor_encrypt
std::string Crypt::encrypt_password_base64_xor(const std::string& password, const std::string& base64_key, const DWORD& xor_key) {
    std::string xor_crypted = xor_crypt(password, xor_key);
    std::string base64_encoded = base64_encode(xor_crypted, base64_key);
    return base64_encoded;
}

std::string Crypt::encrypt_password_base64_xor(const std::string& password, const std::string& username) {
    return encrypt_password_base64_xor(password, username_to_base_key(username, B64L), username_to_xor_key(username));
}

std::string Crypt::encrypt_password_base64_xor_md5(const std::string& password, const std::string& username){
    return encrypt_password_base64_xor(password, username_to_md5_to_base_key(username, B64L), username_to_md5_to_xor_key(username));
}

//mix_base64_xor_decrypt
std::string Crypt::decrypt_password_base64_xor(const std::string& encrypted_password, const std::string& base64_key, const DWORD& xor_key) {
    std::string base64_decoded = base64_decode(encrypted_password, base64_key);
    std::string xor_decrypted = xor_crypt(base64_decoded, xor_key);
    return xor_decrypted;
}

std::string Crypt::decrypt_password_base64_xor(const std::string& encrypted_password, const std::string& username) {
    return decrypt_password_base64_xor(encrypted_password, username_to_base_key(username, B64L), username_to_xor_key(username));
}

std::string Crypt::decrypt_password_base64_xor_md5(const std::string& encrypted_password, const std::string& username){
    return decrypt_password_base64_xor(encrypted_password, username_to_md5_to_base_key(username, B64L), username_to_md5_to_xor_key(username));
}

//mix_base32_xor_encrypt
std::string Crypt::encrypt_password_base32_xor(const std::string& password, const std::string& base32_key, const DWORD& xor_key) {
    std::string xor_crypted = xor_crypt(password, xor_key);
    std::string base32_encoded = base32_encode(xor_crypted, base32_key);
    return base32_encoded;
}

std::string Crypt::encrypt_password_base32_xor(const std::string& password, const std::string& username) {
    return encrypt_password_base32_xor(password, username_to_base_key(username, B32L), username_to_xor_key(username));
}

std::string Crypt::encrypt_password_base32_xor_md5(const std::string& password, const std::string& username) {
    return encrypt_password_base32_xor(password, username_to_md5_to_base_key(username, B32L), username_to_md5_to_xor_key(username));
}

//mix_base32_xor_decrypt
std::string Crypt::decrypt_password_base32_xor(const std::string& encrypted_password, const std::string& base32_key, const DWORD& xor_key) {
    std::string base32_decoded = base32_decode(encrypted_password, base32_key);
    std::string xor_decrypted = xor_crypt(base32_decoded, xor_key);
    return xor_decrypted;
}

std::string Crypt::decrypt_password_base32_xor(const std::string& encrypted_password, const std::string& username) {
    return decrypt_password_base32_xor(encrypted_password, username_to_base_key(username, B32L), username_to_xor_key(username));
}

std::string Crypt::decrypt_password_base32_xor_md5(const std::string& encrypted_password, const std::string& username) {
    return decrypt_password_base32_xor(encrypted_password, username_to_md5_to_base_key(username, B32L), username_to_md5_to_xor_key(username));
}

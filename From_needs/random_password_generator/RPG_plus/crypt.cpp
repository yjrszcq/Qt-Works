#include "crypt.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>

namespace Crypt{

    // Base64 encoding and decoding functions
    std::string base64_encode(const std::string& input) {
        // Implementation of base64 encoding
        std::string base64_chars = "z+2O/kFgiStBfXoMlWPu3QpGh450679LJcHYm8UvCaxynIAsrE1eqjVwNdbTKZRD";
        std::string encoded;
        int i = 0;
        int j = 0;
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

    std::string base64_decode(const std::string& input) {
        // Implementation of base64 decoding
        std::string base64_chars = "z+2O/kFgiStBfXoMlWPu3QpGh450679LJcHYm8UvCaxynIAsrE1eqjVwNdbTKZRD";
        std::string decoded;
        int i = 0;
        int j = 0;
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

    // Base32 encoding and decoding functions
    std::string base32_encode(const std::string& input) {
        // Implementation of base32 encoding
        std::string base32_chars = "t6wh7EgAcQNdRsIpF4loXMJvBYzU235k";
        std::string encoded;
        int i = 0;
        int j = 0;
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

    std::string base32_decode(const std::string& input) {
        // Implementation of base32 decoding
        std::string base32_chars = "t6wh7EgAcQNdRsIpF4loXMJvBYzU235k";
        std::string decoded;
        int i = 0;
        int j = 0;
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

    // Function to encrypt password using base64 and base32
    std::string encrypt_password(const std::string& password) {
        std::string base64_encoded = base64_encode(password);
        std::string base32_encoded = base32_encode(base64_encoded);
        return base32_encoded;
    }

    // Function to decrypt password using base32 and base64
    std::string decrypt_password(const std::string& encrypted_password) {
        std::string base32_decoded = base32_decode(encrypted_password);
        std::string base64_decoded = base64_decode(base32_decoded);
        return base64_decoded;
    }

}

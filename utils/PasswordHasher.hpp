#pragma once

#include <string>
#include <boost/nowide/convert.hpp>

#ifdef _WIN32
#include <windows.h>
#include <dpapi.h>
#include <wincrypt.h>
#include <vector>
#include <string>
#include <stdexcept>
#endif

class PasswordHasher {
public:
    static std::string encrypt(const std::string &plaintext);
    static std::string decrypt(const std::string &ciphertext_b64);

private:
#ifdef _WIN32
    static void secure_clear(std::vector<BYTE>& data);
    static void secure_clear(std::string& data);
    static void secure_clear(std::wstring& data);

    static std::string blob_to_base64(const BYTE* data, DWORD length);
    static std::vector<BYTE> base64_to_blob(const std::string &b64_cipher);

    static std::vector<BYTE> protect_data(const std::vector<BYTE>& data);
    static std::vector<BYTE> unprotect_data(const std::vector<BYTE>& cipher);
#endif
};
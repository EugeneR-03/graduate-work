#include "PasswordHasher.hpp"

#ifdef _WIN32
#include <windows.h>
#include <dpapi.h>
#include <wincrypt.h>
#include <vector>
#include <string>
#include <stdexcept>

void PasswordHasher::secure_clear(std::vector<BYTE>& data)
{
    if (!data.empty()) {
        SecureZeroMemory(data.data(), data.size());
    }
}

void PasswordHasher::secure_clear(std::string& data)
{
    if (!data.empty()) {
        SecureZeroMemory(&data[0], data.size());
    }
}

void PasswordHasher::secure_clear(std::wstring& data)
{
    if (!data.empty()) {
        SecureZeroMemory(&data[0], data.size() * sizeof(wchar_t));
    }
}

std::string PasswordHasher::blob_to_base64(const BYTE* data, DWORD length)
{
    DWORD outLen = 0;
    CryptBinaryToStringA(
        data, 
        length,
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        nullptr, 
        &outLen
    );

    std::string s(outLen, '\0');
    if (!CryptBinaryToStringA(
        data, 
        length,
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        s.data(), 
        &outLen
    )) {
        throw std::runtime_error("CryptBinaryToStringA failed");
    }

    if (!s.empty() && s.back() == '\0') {
        s.pop_back();
    }
    return s;
}

std::vector<BYTE> PasswordHasher::base64_to_blob(const std::string& b64_cipher)
{
    DWORD binLen = 0, skip = 0, flags = 0;
    if (!CryptStringToBinaryA(
        b64_cipher.c_str(), 
        (DWORD)b64_cipher.size(),
        CRYPT_STRING_BASE64, 
        nullptr, 
        &binLen, 
        &skip, 
        &flags
    )) {
        throw std::runtime_error("CryptStringToBinaryA failed");
    };

    std::vector<BYTE> buf(binLen);
    if (!CryptStringToBinaryA(
        b64_cipher.c_str(), 
        (DWORD)b64_cipher.size(),
        CRYPT_STRING_BASE64, 
        buf.data(), 
        &binLen, 
        &skip, 
        &flags
    )) {
        throw std::runtime_error("CryptStringToBinaryA failed");
    }
    return buf;
}

std::vector<BYTE> PasswordHasher::protect_data(const std::vector<BYTE>& data) {
    if (data.empty()) return {};

    DATA_BLOB in = {
        static_cast<DWORD>(data.size()),
        const_cast<BYTE*>(data.data())
    };
    
    DATA_BLOB out = { 0, nullptr };
    LPCWSTR description = L"PasswordHasher_Encrypted";

    if (!CryptProtectData(
        &in,
        description,
        nullptr,
        nullptr,
        nullptr,
        0,
        &out
    )) {
        throw std::runtime_error("CryptProtectData failed");
    }

    // копируем данные в вектор
    std::vector<BYTE> result(out.pbData, out.pbData + out.cbData);
    
    SecureZeroMemory(out.pbData, out.cbData);
    LocalFree(out.pbData);
    
    return result;
}

std::vector<BYTE> PasswordHasher::unprotect_data(const std::vector<BYTE>& cipher) {
    if (cipher.empty()) return {};

    DATA_BLOB in = {
        static_cast<DWORD>(cipher.size()),
        const_cast<BYTE*>(cipher.data())
    };
    
    DATA_BLOB out = { 0, nullptr };

    if (!CryptUnprotectData(
        &in,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        0,
        &out
    )) {
        throw std::runtime_error("CryptUnprotectData failed");
    }

    // копируем данные в вектор
    std::vector<BYTE> result(out.pbData, out.pbData + out.cbData);
    
    SecureZeroMemory(out.pbData, out.cbData);
    LocalFree(out.pbData);
    
    return result;
}

std::string PasswordHasher::encrypt(const std::string &plaintext)
{
    if (plaintext.empty()) return "";

    std::vector<BYTE> data(plaintext.begin(), plaintext.end());
    
    std::vector<BYTE> encrypted = protect_data(data);
    PasswordHasher::secure_clear(data);
    
    std::string result = blob_to_base64(encrypted.data(), 
                                       static_cast<DWORD>(encrypted.size()));
    
    PasswordHasher::secure_clear(encrypted);
    return result;
}

std::string PasswordHasher::decrypt(const std::string &ciphertext_b64)
{
    if (ciphertext_b64.empty()) return "";

    std::vector<BYTE> encrypted = base64_to_blob(ciphertext_b64);
    std::vector<BYTE> decrypted = unprotect_data(encrypted);
    secure_clear(encrypted);
    
    // создаем строку
    std::string result(decrypted.begin(), decrypted.end());

    secure_clear(decrypted);
    return result;
}
#endif

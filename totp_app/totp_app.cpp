#include "cryptlib.h"
#include "hmac.h"
#include "sha.h"
#include <iostream>
#include <sstream>
#include <string>


//  this fucking function outputs wrong fucking HOTP key and i dont fucking know why

std::string GenerateHOTP(const std::string& key, const int counter, const int digits)
{
    using namespace CryptoPP;
    std::stringstream ss;

    uint8_t CounterBytes[8];
    for (int i = 0; i < 8; i++)
    {
        CounterBytes[7 - i] = ((uint64_t)counter >> (i * 8)) & 0xff;
    }

    //  this shit generates HMAC-SHA1
    HMAC<SHA1> hmac_sha1((const uint8_t*)key.data(), key.size());
    uint8_t hmac[HMAC<SHA1>::DIGESTSIZE];
    hmac_sha1.Update(CounterBytes, sizeof(CounterBytes));
    hmac_sha1.Final(hmac);
    
    //  this is for debugging only, to show in terminal
    for (int i = 0; i < HMAC<SHA1>::DIGESTSIZE; i++)
    {
        ss << std::hex << (int)hmac[i];
    }
    std::string hmac_sha_1 = ss.str();
    std::cout << hmac_sha_1 << std::endl;

    //  this fucking shit generates a number, i just copy-pasted algorithm from the doc
    unsigned int offset = hmac[19] & 0xF;
    uint32_t bin_code = (((uint8_t)hmac[offset] & 0x7F) << 24)
        | (((uint8_t)hmac[offset + 1] & 0xFF) << 16)
        | (((uint8_t)hmac[offset + 2] & 0xFF) << 8)
        | ((uint8_t)hmac[offset + 3] & 0xFF);

    //  make it "digit"-digits long (usually, its six)
    int mod = pow(10, digits);
    uint32_t hotp = bin_code % mod;

    std::string result = std::to_string(hotp);

    //  if the number is less than 6
    while (result.length() < digits) {
        result = "0" + result;
    }

    return result;
}

std::string GenerateTOTP(const std::string secret, const int duration, const int digit)
{
    std::time_t current_time = std::time(nullptr);
    std::cout << "UNIX Time: " << current_time << std::endl;
    int counter = current_time / duration;
    std::cout << "Counter: " << counter << std::endl;

    std::string totp = GenerateHOTP(secret, counter, digit);

    return totp;
}


int main()
{
    std::string secret;
    std::cout << "Enter secret here:\n>>";
    std::cin >> secret;
    std::cout << "Secret: " << secret << std::endl;

    std::string totp = GenerateTOTP(secret, 30, 6);
    std::cout << "TOTP: " << totp << std::endl;
    return 0;
}


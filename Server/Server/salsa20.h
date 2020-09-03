#pragma once
#include <Windows.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#define R(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define R2(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

#define quarter(a, b, c, d) \
  do {                      \
    b ^= R(d + a, 7);       \
    c ^= R(a + b, 9);       \
    d ^= R2(b + c, 12);     \
    a ^= R2(c + d, 14);     \
  } while (0)

class Salsa20 {
 public:
  Salsa20(uint8_t key[32]);
  void Salsa20Do(uint8_t* message, uint64_t mlen, uint64_t nonce);

 private:
  void Salsa20Block(uint8_t* out, uint8_t key[32], uint64_t nonce, uint64_t index);
  void Salsa20Words(uint32_t* out, uint32_t in[16]);

  uint8_t key_[32];
};
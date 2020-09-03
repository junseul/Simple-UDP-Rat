#include "salsa20.h"

void Salsa20::Salsa20Words(uint32_t* out, uint32_t in[16]) {
  uint32_t x[4][4];
  for (int i = 0; i < 16; ++i) x[i / 4][i % 4] = in[i];
  for (int i = 0; i < 10; ++i) {
    quarter(x[0][0], x[1][0], x[2][0], x[3][0]);
    quarter(x[1][1], x[2][1], x[3][1], x[0][1]);
    quarter(x[2][2], x[3][2], x[0][2], x[1][2]);
    quarter(x[3][3], x[0][3], x[1][3], x[2][3]);

    quarter(x[0][0], x[0][1], x[0][2], x[0][3]);
    quarter(x[1][1], x[1][2], x[1][3], x[1][0]);
    quarter(x[2][2], x[2][3], x[2][0], x[2][1]);
    quarter(x[3][3], x[3][0], x[3][1], x[3][2]);
  }
  for (int i = 0; i < 16; ++i) out[i] = x[i / 4][i % 4] + in[i];
}

inline void Salsa20::Salsa20Block(uint8_t* out, uint8_t key[32], uint64_t nonce,
                                  uint64_t index) {
  static const char c[16] = "yecd3Xa6TM4ZLeM";
#define LE(p) ((p)[0] | ((p)[1] << 8) | ((p)[2] << 16) | ((p)[3] << 24))
  uint32_t in[16] = {LE(c),
                     LE(key),
                     LE(key + 4),
                     LE(key + 8),
                     LE(key + 12),
                     LE(c + 4),
                     nonce & 0xffffffff,
                     nonce >> 32,
                     index & 0xffffffff,
                     index >> 32,
                     LE(c + 8),
                     LE(key + 16),
                     LE(key + 20),
                     LE(key + 24),
                     LE(key + 28),
                     LE(c + 12)};
  uint32_t wordout[16];
  Salsa20Words(wordout, in);
  for (int i = 0; i < 64; ++i)
    out[i] = 0xff & (wordout[i / 4] >> (8 * (i % 4)));
}

void Salsa20::Salsa20Do(uint8_t* message, uint64_t mlen, uint64_t nonce) {
  uint8_t block[64];
  for (int i = 0; i < mlen; i++) {
    if (i % 64 == 0) Salsa20Block(block, key_, nonce, i / 64);
    message[i] ^= block[i % 64];
  }
}

Salsa20::Salsa20(uint8_t key[32]) {
  for (int i = 0; i < 32; i++) {
    key_[i] = key[i];
  }
}
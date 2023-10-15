#pragma once

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t word_size;
  uint64_t rule;
  size_t epochs;
  size_t neighborhood_size;
  size_t size;
} meca_config;

// Define the MECA_CRYPT macro using _Generic
#define MECA_CRYPT(buffer, len, config)                                        \
  _Generic((buffer),                                                           \
      uint8_t *: meca_crypt_uint8_t,                                           \
      uint16_t *: meca_crypt_uint16_t,                                         \
      uint32_t *: meca_crypt_uint32_t,                                         \
      uint64_t *: meca_crypt_uint64_t)(buffer, len, config)

#define DECLARE_MECA_CRYPT_FOR_TYPE(type)                                      \
  void meca_crypt_##type(type *buffer, size_t len, const meca_config *config)  \
  {                                                                            \
    assert(len % 2 == 0 && "error: state must be divisible by two for "        \
                           "previous timestep (FIXME: pad and remove)");       \
    assert(config->neighborhood_size % 2 != 0 &&                               \
           "error: neighborhood size must be odd");                            \
    const size_t BUFTYPE_BITLEN = config->word_size * CHAR_BIT;                \
    const size_t TIMESTEP = len / 2;                                           \
    const size_t TIMESTEP_BITS = TIMESTEP * BUFTYPE_BITLEN;                    \
    const size_t reach = config->neighborhood_size / 2;                        \
    type *prev = malloc(config->word_size * TIMESTEP),                         \
         *state = malloc(config->word_size * TIMESTEP),                        \
         *next = malloc(config->word_size * TIMESTEP);                         \
    memcpy(prev, buffer, config->word_size *TIMESTEP);                         \
    memcpy(state, buffer + TIMESTEP, config->word_size * TIMESTEP);            \
    for (unsigned epoch = 1; epoch <= config->epochs; epoch++) {               \
      for (size_t target = 0; target < TIMESTEP_BITS; target++) {              \
        uint64_t neighborhood = 0;                                             \
        size_t state_idx = target / BUFTYPE_BITLEN;                            \
        size_t shift = target % BUFTYPE_BITLEN;                                \
        size_t lhs = (target - reach + TIMESTEP_BITS) % TIMESTEP_BITS;         \
        size_t rhs = (target + reach) % TIMESTEP_BITS;                         \
        int nshift = config->neighborhood_size - 1;                            \
                                                                               \
        for (size_t pos = lhs; pos != rhs; pos = (pos + 1) % TIMESTEP_BITS) {  \
          size_t pos_idx = pos / BUFTYPE_BITLEN;                               \
          size_t pos_shift = pos % BUFTYPE_BITLEN;                             \
          neighborhood |= (state[pos_idx] >> pos_shift & 1) << nshift--;       \
        }                                                                      \
                                                                               \
        size_t pos_idx = rhs / BUFTYPE_BITLEN;                                 \
        size_t pos_shift = rhs % BUFTYPE_BITLEN;                               \
        neighborhood |= (state[pos_idx] >> pos_shift & 1) << nshift;           \
                                                                               \
        type first_order = (config->rule >> neighborhood & 1) << shift;        \
        type second_order = prev[state_idx] & (1 << shift);                    \
        next[state_idx] &= ~(1 << shift);                                      \
        next[state_idx] |= first_order ^ second_order;                         \
      }                                                                        \
                                                                               \
      memcpy(prev, state, config->word_size *TIMESTEP);                        \
      memcpy(state, next, config->word_size *TIMESTEP);                        \
    }                                                                          \
                                                                               \
    memcpy(buffer, state, config->word_size *TIMESTEP);                        \
    memcpy(buffer + TIMESTEP, prev, config->word_size * TIMESTEP);             \
    free(prev);                                                                \
    prev = NULL;                                                               \
    free(state);                                                               \
    state = NULL;                                                              \
    free(next);                                                                \
    next = NULL;                                                               \
  }

#define DECLARE_MECA_CRYPT_PROTO_FOR_TYPE(type)                                \
  void meca_crypt_##type(type *buffer, size_t len, const meca_config *config);

DECLARE_MECA_CRYPT_PROTO_FOR_TYPE(uint8_t)
DECLARE_MECA_CRYPT_PROTO_FOR_TYPE(uint16_t)
DECLARE_MECA_CRYPT_PROTO_FOR_TYPE(uint32_t)
DECLARE_MECA_CRYPT_PROTO_FOR_TYPE(uint64_t)
#define CALL_MECA_CRYPT(buffer, len, config) MECA_CRYPT(buffer, len, config)

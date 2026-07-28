#include <libhbcrypto/hbcrypto.h>

#define LIBHBCRYPTO_VERSION_STRING_BASE EMB_STRINGIFY(LIBHBCRYPTO_VERSION_MAJOR) "." EMB_STRINGIFY(LIBHBCRYPTO_VERSION_MINOR) "." EMB_STRINGIFY(LIBHBCRYPTO_VERSION_PATCH)

#if LIBHBCRYPTO_VERSION_IS_RELEASE
#define LIBHBCRYPTO_VERSION_STRING LIBHBCRYPTO_VERSION_STRING_BASE
#else
#define LIBHBCRYPTO_VERSION_STRING LIBHBCRYPTO_VERSION_STRING_BASE "-" LIBHBCRYPTO_VERSION_SUFFIX
#endif

uint32_t libhbcrypto_version(void) {
    return LIBHBCRYPTO_VERSION_HEX;
}

const char* libhbcrypto_version_string(void) {
    return LIBHBCRYPTO_VERSION_STRING;
}

#define DECLARE_IS_ENDIAN \
    const union {         \
        long one;         \
        char little;      \
    } ossl_is_endian = {1}

#define IS_LITTLE_ENDIAN (ossl_is_endian.little != 0)
#define IS_BIG_ENDIAN (ossl_is_endian.little == 0)

typedef union {
    uint32_t u[16];
    uint8_t c[64];
} chacha_buf;

#define ROTATE(v, n) (((v) << (n)) | ((v) >> (32 - (n))))
#define U32TO8_LITTLE(p, v)           \
    do {                              \
        (p)[0] = (uint32_t)(v >> 0);  \
        (p)[1] = (uint32_t)(v >> 8);  \
        (p)[2] = (uint32_t)(v >> 16); \
        (p)[3] = (uint32_t)(v >> 24); \
    } while (0)

/* QUARTERROUND updates a, b, c, d with a ChaCha "quarter" round. */
#define QUARTERROUND(a, b, c, d) (                  \
    x[a] += x[b], x[d] = ROTATE((x[d] ^ x[a]), 16), \
    x[c] += x[d], x[b] = ROTATE((x[b] ^ x[c]), 12), \
    x[a] += x[b], x[d] = ROTATE((x[d] ^ x[a]), 8),  \
    x[c] += x[d], x[b] = ROTATE((x[b] ^ x[c]), 7))

/* chacha_core performs 20 rounds of ChaCha on the input words in
 * |input| and writes the 64 output bytes to |output|. */
static void chacha20_core(chacha_buf *output, const uint32_t input[16]) {
    uint32_t x[16];
    int i;
    DECLARE_IS_ENDIAN;

    for (i = 0; i < 16; i++) {
        x[i] = input[i];
    }

    for (i = 20; i > 0; i -= 2) {
        QUARTERROUND(0, 4, 8, 12);
        QUARTERROUND(1, 5, 9, 13);
        QUARTERROUND(2, 6, 10, 14);
        QUARTERROUND(3, 7, 11, 15);
        QUARTERROUND(0, 5, 10, 15);
        QUARTERROUND(1, 6, 11, 12);
        QUARTERROUND(2, 7, 8, 13);
        QUARTERROUND(3, 4, 9, 14);
    }

    if (IS_LITTLE_ENDIAN) {
        for (i = 0; i < 16; ++i)
            output->u[i] = x[i] + input[i];
    } else {
        for (i = 0; i < 16; ++i)
            U32TO8_LITTLE(output->c + 4 * i, (x[i] + input[i]));
    }
}

static void ChaCha20_ctr32(uint8_t *out, const uint8_t *inp, size_t len,
                           const uint32_t key[8], const uint32_t counter[4]) {
    uint32_t input[16];
    chacha_buf buf;
    size_t todo, i;

    /* sigma constant "expand 32-byte k" in little-endian encoding */
    input[0] = ((uint32_t)('e')) | ((uint32_t)('x') << 8) | ((uint32_t)('p') << 16) | ((uint32_t)('a') << 24);
    input[1] = ((uint32_t)('n')) | ((uint32_t)('d') << 8) | ((uint32_t)(' ') << 16) | ((uint32_t)('3') << 24);
    input[2] = ((uint32_t)('2')) | ((uint32_t)('-') << 8) | ((uint32_t)('b') << 16) | ((uint32_t)('y') << 24);
    input[3] = ((uint32_t)('t')) | ((uint32_t)('e') << 8) | ((uint32_t)(' ') << 16) | ((uint32_t)('k') << 24);

    input[4] = key[0];
    input[5] = key[1];
    input[6] = key[2];
    input[7] = key[3];
    input[8] = key[4];
    input[9] = key[5];
    input[10] = key[6];
    input[11] = key[7];

    input[12] = counter[0];
    input[13] = counter[1];
    input[14] = counter[2];
    input[15] = counter[3];

    while (len > 0) {
        todo = sizeof(buf);
        if (len < todo)
            todo = len;

        chacha20_core(&buf, input);

        for (i = 0; i < todo; i++)
            out[i] = inp[i] ^ buf.c[i];
        out += todo;
        inp += todo;
        len -= todo;

        /*
         * Advance 32-bit counter. Note that as subroutine is so to
         * say nonce-agnostic, this limited counter width doesn't
         * prevent caller from implementing wider counter. It would
         * simply take two calls split on counter overflow...
         */
        input[12]++;
    }
}

void hb_chacha20(uint8_t *out, const uint8_t *input, size_t len, const uint8_t key[32], const uint8_t nonce[12]) {
    uint8_t i;
    uint32_t key32[8], nonce32[4];
    // Convert key and nonce from strings to uint32_t arrays
    for (i = 0; i < 8; i++) {
        key32[i] = (uint32_t)key[i * 4] | ((uint32_t)key[i * 4 + 1] << 8) |
                   ((uint32_t)key[i * 4 + 2] << 16) | ((uint32_t)key[i * 4 + 3] << 24);
    }
    nonce32[0] = 1;  // initial count
    for (i = 0; i < 3; i++) {
        nonce32[i + 1] = (uint32_t)nonce[i * 4] | ((uint32_t)nonce[i * 4 + 1] << 8) |
                         ((uint32_t)nonce[i * 4 + 2] << 16) | ((uint32_t)nonce[i * 4 + 3] << 24);
    }
    // Encrypt the input string
    ChaCha20_ctr32(out, input, len, key32, nonce32);
}

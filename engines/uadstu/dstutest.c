/* =====================================================================
 * Author: Ignat Korchagin <ignat.korchagin@gmail.com>
 * This file is distributed under the same license as OpenSSL
 ==================================================================== */

#include <stdio.h>

#if defined(OPENSSL_NO_ENGINE) || defined(OPENSSL_NO_DSTU)
int main(int argc, char *argv[])
{
    printf("No DSTU support\n");
    return 0;
}
#else

# include <stdlib.h>
# include <string.h>
# include <openssl/conf.h>
# include <openssl/crypto.h>
# include <openssl/engine.h>
# include <openssl/err.h>
# include <openssl/evp.h>
# include <openssl/hmac.h>
# include <openssl/obj_mac.h>
# include <openssl/rand.h>

# include "../engines/uadstu/dstu_params.h"

# define DSTU_ENGINE_ID "dstu"
# define UADSTU_DIR "../engines/uadstu"

static unsigned char hash[] =
    { 0x10, 0xe6, 0x5a, 0x11, 0xac, 0xcc, 0x36, 0x5e, 0x18, 0x46, 0x67, 0x02,
    0xb0, 0x64, 0x6d, 0x74, 0x92, 0x41, 0x9a, 0x9d, 0xa6, 0x57, 0xcf, 0x03,
        0x64, 0x52, 0x7c, 0x33,
    0x65, 0x55, 0x5b, 0xa7
};

static char c163[] = "uacurve0";
static unsigned char e163[] =
    { 0x01, 0x02, 0x5e, 0x40, 0xbd, 0x97, 0xdb, 0x01, 0x2b, 0x7a, 0x1d, 0x79,
    0xde, 0x8e, 0x12, 0x93, 0x2d, 0x24, 0x7f, 0x61, 0xc6
};

static unsigned char d163[] =
    { 0x03, 0xe0, 0x74, 0x8d, 0x62, 0x9e, 0xb5, 0x4a, 0x1d, 0x8f, 0x9a, 0x87,
    0xeb, 0xde, 0x12, 0xca, 0x0e, 0xed, 0x48, 0x54, 0xa5
};

static unsigned char s163[] =
    { 0x04, 0x2a, 0x66, 0xa7, 0x40, 0x07, 0x84, 0xa7, 0x4a, 0x72, 0xcc, 0xa7,
    0x5b, 0x0c, 0x35, 0x8b, 0x4c, 0xdd, 0x6c, 0x2a, 0xa6, 0x36, 0x00, 0x00,
        0xc4, 0x74, 0x66, 0xda,
    0xd4, 0xe1, 0x01, 0x1b, 0x3e, 0x18, 0x95, 0x27, 0x72, 0xc0, 0x80, 0xa9,
        0xa3, 0x1b, 0x31,
    0x01
};

static char c257[] = "uacurve6";
static unsigned char e257[] =
    { 0x00, 0x43, 0x94, 0xe1, 0x9d, 0xcc, 0x1d, 0xef, 0x57, 0xdd, 0xbe, 0x6f,
    0xb7, 0x05, 0x65, 0xc7, 0x49, 0xf2, 0x21, 0x44, 0x62, 0x6f, 0x05, 0x0a,
        0x6d, 0xaa, 0xb4, 0xaf,
    0x00, 0x71, 0xe6, 0xfe, 0xb9
};

static unsigned char d257[] =
    { 0x00, 0x6f, 0x31, 0xc4, 0x44, 0x59, 0x14, 0xfb, 0x34, 0x7b, 0xa9, 0xbd,
    0x27, 0x2b, 0xb5, 0xb7, 0xf5, 0x13, 0x42, 0x2f, 0xe1, 0x16, 0xdf, 0xa4,
        0xf0, 0xdf, 0x34, 0xb1,
    0xce, 0xb1, 0x1a, 0x64, 0x5c
};

static unsigned char s257[] =
    { 0x04, 0x40, 0x6f, 0x77, 0x37, 0xe8, 0x65, 0xd2, 0x50, 0xd7, 0x0a, 0x29,
    0xea, 0xdf, 0xe3, 0xc2, 0x3d, 0x37, 0x0c, 0x3f, 0xa7, 0xb5, 0xc9, 0x43,
        0xcd, 0xed, 0x1b, 0x72,
    0xf3, 0xe1, 0x53, 0x2d, 0x2c, 0x09, 0xe1, 0x8b, 0x17, 0x72, 0x1d, 0xab,
        0xeb, 0xec, 0x39, 0xbd,
    0x4a, 0x87, 0x9a, 0xb0, 0x50, 0x7d, 0x80, 0xe9, 0x99, 0xe1, 0xea, 0x5d,
        0x65, 0xb5, 0x56, 0xd6,
    0xf1, 0x66, 0x44, 0x8b, 0xee, 0x26
};

static char c431[] = "uacurve9";
static unsigned char e431[] =
    { 0x04, 0x57, 0xce, 0x62, 0x0e, 0x2b, 0x71, 0x10, 0x0d, 0x87, 0xf7, 0xe1,
    0xc2, 0x32, 0x38, 0xaa, 0x36, 0x29, 0xba, 0x52, 0xc6, 0xba, 0xb4, 0x2c,
        0x6a, 0x22, 0xdb, 0x43,
    0x78, 0x15, 0xcc, 0xfe, 0xb9, 0xa4, 0x47, 0xb0, 0x2a, 0x82, 0x98, 0xe5,
        0x26, 0x0a, 0x7f, 0xa9,
    0x4a, 0x13, 0xc1, 0x82, 0x2d, 0x88, 0x5e, 0x8c, 0x5c, 0x69
};

static unsigned char d431[] =
    { 0x31, 0x13, 0x1b, 0xec, 0x94, 0x8d, 0xac, 0xbb, 0xaf, 0xad, 0xbf, 0x9b,
    0xb1, 0x50, 0xb0, 0xb7, 0x92, 0xdf, 0x32, 0x24, 0x79, 0x2a, 0x80, 0xf6,
        0x76, 0x1a, 0x0b, 0x03,
    0xb3, 0xb8, 0x2d, 0x11, 0x8b, 0xc3, 0xc2, 0x27, 0x1c, 0x04, 0xa0, 0x4c,
        0x2d, 0x9b, 0x38, 0x76,
    0xc2, 0x33, 0x86, 0x0b, 0x19, 0x0d, 0xcc, 0xea, 0x01, 0x77
};

static unsigned char s431[] =
    { 0x04, 0x6c, 0xc4, 0xe4, 0xf0, 0xb5, 0xea, 0xb8, 0xe7, 0xaa, 0xf4, 0x17,
    0xa6, 0xb5, 0xd1, 0xd7, 0x4f, 0xe5, 0x34, 0x81, 0x64, 0x50, 0xdf, 0x6d,
        0xff, 0xd9, 0x37, 0x7d,
    0xfc, 0xac, 0x5f, 0x25, 0x67, 0x6a, 0x11, 0x9e, 0x5f, 0x70, 0x0f, 0x92,
        0xc2, 0x7c, 0x80, 0x17,
    0xb6, 0xb3, 0xe3, 0xc2, 0x05, 0xf2, 0x74, 0xd5, 0x4c, 0xd3, 0x33, 0x00,
        0xdd, 0x1e, 0xe8, 0x43,
    0x63, 0x85, 0x69, 0xe1, 0x10, 0xe4, 0x15, 0xc4, 0x46, 0x0e, 0x4e, 0xb3,
        0x25, 0x8f, 0xcf, 0xa7,
    0x50, 0x98, 0xa5, 0xce, 0xb5, 0xad, 0x35, 0xb8, 0xfd, 0x28, 0xd1, 0x21,
        0x06, 0xb5, 0x51, 0x14,
    0x87, 0x43, 0x45, 0xef, 0x7d, 0xc1, 0xa1, 0x45, 0xa5, 0xbb, 0x1c, 0xcf,
        0x27, 0x4b, 0x5d, 0x64,
    0x80, 0x11
};

struct dstu_test_vector {
    char *curve;
    int field_byte_size;
    unsigned char *e;
    unsigned char *d;
    unsigned char *s;
};

static struct dstu_test_vector vectors[] = {
    {c163, 21, e163, d163, s163},
    {c257, 33, e257, d257, s257},
    {c431, 54, e431, d431, s431}
};

static unsigned char *rand_source;
static int rand_size;
static RAND_METHOD fake_rand;
static const RAND_METHOD *old_rand;

static int fbytes(unsigned char *buf, int num)
{
    int to_copy = num;

    while (to_copy) {
        memcpy(buf, rand_source, (to_copy > rand_size) ? rand_size : to_copy);
        buf += (to_copy > rand_size) ? rand_size : to_copy;
        to_copy -= (to_copy > rand_size) ? rand_size : to_copy;
    }

    return num;
}

static int change_rand(void)
{
    /* save old rand method */
    if ((old_rand = RAND_get_rand_method()) == NULL)
        return 0;

    fake_rand.seed = old_rand->seed;
    fake_rand.cleanup = old_rand->cleanup;
    fake_rand.add = old_rand->add;
    fake_rand.status = old_rand->status;
    /* use own random function */
    fake_rand.bytes = fbytes;
    fake_rand.pseudorand = old_rand->bytes;
    /* set new RAND_METHOD */
    if (!RAND_set_rand_method(&fake_rand))
        return 0;
    return 1;
}

static int restore_rand(void)
{
    if (!RAND_set_rand_method(old_rand))
        return 0;
    else
        return 1;
}

static ENGINE *load_dstu(void)
{
    ENGINE *ret = 0;
    CONF *pConfig = NCONF_new(NULL);
    BIO *bpConf = NULL;
    long lErrLine;
    char sConf[] = "openssl_conf = openssl_def\n"
        "\n"
        "[openssl_def]\n"
        "engines = engine_section\n"
        "\n"
        "[engine_section]\n"
        "dstu = dstu_section\n"
        "\n" "[dstu_section]\n" "default_algorithms = ALL\n" "\n";

    if (!pConfig)
        return 0;

# ifndef OPENSSL_NO_DYNAMIC_ENGINE
    setenv("OPENSSL_ENGINES", UADSTU_DIR, 1);
# endif
    ERR_load_crypto_strings();
    ENGINE_load_builtin_engines();
    OPENSSL_load_builtin_modules();

    bpConf = BIO_new_mem_buf(sConf, -1);
    if (!bpConf)
        goto err;

    if (!NCONF_load_bio(pConfig, bpConf, &lErrLine)) {
        fflush(NULL);
        fprintf(stderr, "NCONF_load_bio: ErrLine=%ld: %s\n", lErrLine,
                ERR_error_string(ERR_get_error(), NULL));
        goto err;
    }

    if (!CONF_modules_load(pConfig, NULL, 0)) {
        fflush(NULL);
        fprintf(stderr, "CONF_modules_load: %s\n",
                ERR_error_string(ERR_get_error(), NULL));
        goto err;
    }

    ret = ENGINE_by_id(DSTU_ENGINE_ID);
    if (!ret) {
        fflush(NULL);
        fprintf(stderr, "Can't load engine id \"" DSTU_ENGINE_ID "\"\n");
        goto err;
    }

 err:if (bpConf)
        BIO_free(bpConf);

    if (pConfig)
        NCONF_free(pConfig);

    return ret;
}

int main(int argc, char *argv[])
{
    EVP_PKEY_CTX *pkey_ctx = NULL;
    EVP_PKEY *pkey = NULL;
    ENGINE *dstu = load_dstu();
    unsigned char sig[256];
    size_t siglen = sizeof(sig);
    struct dstu_test_vector *vector;
    int ret = 1;

    if (!dstu)
        return 0;

    printf("Testing DSTU 4145-2002\n");
    fflush(NULL);

    if (!change_rand()) {
        fprintf(stderr, "Change random failed\n");
        fflush(NULL);
        return 1;
    }

    pkey = EVP_PKEY_new();
    if (!pkey) {
        fprintf(stderr, "EVP_PKEY_new() failed\n");
        fflush(NULL);
        goto err;
    }

    if (!EVP_PKEY_set_type(pkey, NID_dstu4145le)) {
        fprintf(stderr, "EVP_PKEY_set_type() failed\n");
        fflush(NULL);
        goto err;
    }

    pkey_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!pkey_ctx) {
        fprintf(stderr, "EVP_PKEY_CTX_new() failed\n");
        fflush(NULL);
        goto err;
    }

    for (vector = vectors;
         vector <
         vectors + (sizeof(vectors) / sizeof(struct dstu_test_vector));
         vector++) {
        if (!EVP_PKEY_CTX_ctrl_str(pkey_ctx, "curve", vector->curve)) {
            fprintf(stderr, "EVP_PKEY_CTX_ctrl_str() failed\n");
            fflush(NULL);
            goto err;
        }

        rand_source = vector->d;
        rand_size = vector->field_byte_size;

        if (!EVP_PKEY_keygen_init(pkey_ctx)) {
            fprintf(stderr, "EVP_PKEY_keygen_init() failed\n");
            fflush(NULL);
            goto err;
        }

        if (!EVP_PKEY_keygen(pkey_ctx, &pkey)) {
            fprintf(stderr, "EVP_PKEY_keygen() failed\n");
            fflush(NULL);
            goto err;
        }

        rand_source = vector->e;

        if (!EVP_PKEY_sign_init(pkey_ctx)) {
            fprintf(stderr, "EVP_PKEY_sign_init() failed\n");
            fflush(NULL);
            goto err;
        }

        if (!EVP_PKEY_sign(pkey_ctx, sig, &siglen, hash, sizeof(hash))) {
            fprintf(stderr, "EVP_PKEY_sign() failed\n");
            fflush(NULL);
            goto err;
        }

        if (memcmp(sig, vector->s, siglen)) {
            fprintf(stderr, "%s: test signature mismatch\n", vector->curve);
            fflush(NULL);
            goto err;
        }

        if (!EVP_PKEY_verify_init(pkey_ctx)) {
            fprintf(stderr, "EVP_PKEY_verify_init() failed\n");
            fflush(NULL);
            goto err;
        }

        if (1 != EVP_PKEY_verify(pkey_ctx, sig, siglen, hash, sizeof(hash))) {
            fprintf(stderr, "%s: test signature verification failed\n",
                    vector->curve);
            fflush(NULL);
            goto err;
        }

        siglen = sizeof(sig);
    }

    ret = 0;
    printf("Passed\n");
    fflush(NULL);

 err:
    if (pkey_ctx)
        EVP_PKEY_CTX_free(pkey_ctx);

    if (pkey)
        EVP_PKEY_free(pkey);

    restore_rand();

    return ret;
}
#endif
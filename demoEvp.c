#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

/* 编译时+  -lssl -lcrypto */

#define BUFFER_SIZE 1024
#define KEY_SIZE 32

void handleErrors(void) 
{
    printf("发生错误\n");
    exit(1);
}

void encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext) 
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) handleErrors();

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) handleErrors();
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
}

void decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext) 
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) handleErrors();

    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) handleErrors();
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
}

int main() 
{
    unsigned char *plaintext = (unsigned char *)"这是一条明文"; // 明文
    unsigned char key[KEY_SIZE]; // 密钥
    unsigned char iv[EVP_MAX_IV_LENGTH]; // 初始化向量
    unsigned char ciphertext[BUFFER_SIZE]; // 密文
    unsigned char decryptedtext[BUFFER_SIZE]; // 解密后的明文
    
    // 生成随机密钥和 IV
    RAND_bytes(key, KEY_SIZE);
    RAND_bytes(iv, EVP_MAX_IV_LENGTH);
    
    // 加密明文
    encrypt(plaintext, strlen((char *)plaintext), key, iv, ciphertext);
    printf("加密后的文本: %s\n", ciphertext);
    
    // 解密密文
    decrypt(ciphertext, strlen((char *)ciphertext), key, iv, decryptedtext);
    printf("解密后的文本: %s\n", decryptedtext);
    
    return 0;
}

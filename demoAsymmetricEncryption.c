#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

#define RSA_KEY_BITS 2048

void handleErrors() {
    fprintf(stderr, "错误发生.\n");
    exit(1);
}

int main() {
    // 生成密钥对
    RSA *keypair = RSA_generate_key(RSA_KEY_BITS, RSA_F4, NULL, NULL);
    if (keypair == NULL) handleErrors();

    // 要加密的数据
    const char *plaintext = "这是一条明文!";
    int plaintext_len = strlen(plaintext);

    // 分配存储密文的缓冲区
    unsigned char *ciphertext = (unsigned char *)malloc(RSA_size(keypair));
    if (ciphertext == NULL) handleErrors();

    // 使用公钥加密
    int ciphertext_len = RSA_public_encrypt(plaintext_len, (unsigned char *)plaintext, ciphertext, keypair, RSA_PKCS1_PADDING);
    if (ciphertext_len == -1) handleErrors();
    /* 
    int RSA_public_encrypt(int flen, const unsigned char *from, unsigned char *to,RSA *rsa, int padding);
    
    参数解释如下：
    flen：要加密的数据长度。
    from：指向要加密的数据的指针。
    to：指向存储加密结果的缓冲区的指针。
    rsa：RSA 密钥对象，用于加密。
    padding：填充模式，通常使用 RSA_PKCS1_PADDING，表示使用 PKCS#1 v1.5 填充方案。

    这个函数将 from 指向的数据使用 rsa 指定的 RSA 公钥进行加密，并将加密结果存储到 to 指定的缓冲区中。

    */

    // 输出加密后的密文
    printf("密文: ");
    for (int i = 0; i < ciphertext_len; i++) {
        printf("%02X", ciphertext[i]);
    }
    printf("\n");

    // 使用私钥解密
    unsigned char *decrypted_text = (unsigned char *)malloc(plaintext_len);
    if (decrypted_text == NULL) handleErrors();

    int decrypted_len = RSA_private_decrypt(ciphertext_len, ciphertext, decrypted_text, keypair, RSA_PKCS1_PADDING);
    if (decrypted_len == -1) handleErrors();

    /*
    int RSA_private_decrypt(int flen, const unsigned char *from, unsigned char *to,RSA *rsa, int padding);

    参数解释如下：
    flen：要解密的数据长度。
    from：指向要解密的数据的指针。
    to：指向存储解密结果的缓冲区的指针。
    rsa：RSA 密钥对象，用于解密。
    padding：填充模式，通常使用 RSA_PKCS1_PADDING，表示使用 PKCS#1 v1.5 填充方案。

    这个函数将 from 指向的数据使用 rsa 指定的 RSA 私钥进行解密，并将解密结果存储到 to 指定的缓冲区中。
    */

    // 输出解密后的明文
    printf("解密文本: %s\n", decrypted_text);

    // 释放资源
    RSA_free(keypair);
    free(ciphertext);
    free(decrypted_text);

    return 0;
}

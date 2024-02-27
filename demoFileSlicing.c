#include <stdio.h>
#include <stdlib.h>
#include <openssl/md5.h>

/* 使用openssl库编译时要加 -lssl -lcrypto */
/* MD5好像被弃用了，所以编译时会有警告，加 -Wno-deprecated-declarations 可以解决这个问题 */

#define CHUNK_SIZE (2 * 1024 * 1024) // 2MB

int main() 
{
    /* 打开文件,并按字节读取 */
    FILE *file = fopen("123.txt", "rb");
    if (file == NULL) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    /* 获取文件大小 */
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* 开辟一个2MB的空间，用于切片 */
    char *buffer = (char *)malloc(CHUNK_SIZE);
    if (buffer == NULL) {
        perror("Failed to allocate memory");
        fclose(file);
        return EXIT_FAILURE;
    }

    /* 计算切片数量 */
    int slice_count = (file_size + CHUNK_SIZE - 1) / CHUNK_SIZE;
    printf("File size: %ld bytes, Chunk size: %d bytes, Slice count: %d\n", file_size, CHUNK_SIZE, slice_count);

    /* 声明MD5结构体，并初始化 */
    MD5_CTX md5_context;
    MD5_Init(&md5_context);


    for (int i = 0; i < slice_count; ++i) {
        size_t bytes_to_read = (i == slice_count - 1) ? (file_size % CHUNK_SIZE) : CHUNK_SIZE;      // 判断要读的位置是否是最后一段，是则按文件大小%切片来获取最后一段，不是就按正常切片大小来
        size_t bytes_read = fread(buffer, 1, bytes_to_read, file);                                  // 读取指定大小的数据

        MD5_Update(&md5_context, buffer, bytes_read);

        /* 创建一个无符号字符串来接MD5码 8888888888
        4
        
        
        
        */
        unsigned char md5_hash[MD5_DIGEST_LENGTH];
        MD5_Final(md5_hash, &md5_context);

        printf("Slice %d MD5: ", i + 1);
        for (int j = 0; j < MD5_DIGEST_LENGTH; ++j) {
            printf("%02x", md5_hash[j]);
        }
        printf("\n");
    }

    free(buffer);
    fclose(file);

    return EXIT_SUCCESS;
}

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int ret;
    char path[] = ".cmake/api/v1/query/client-sw4stm32";
    const char filename_codemodel[] = "codemodel-v2";   // contains generated output of cmake
    const char filename_cache[] = "cache-v2";   // contains variables supplied to cmake
    const char *s = "/";
    char *token;
   
    /* equivalent of "mkdir -p" */
    for (token = strtok(path, s); token != NULL; token = strtok(NULL, s)) {
        //printf("## %s\n", token);
#ifdef __WIN32__
        int ret = mkdir(token);
#else
        int ret = mkdir(token, 0777);
#endif
        if (ret < 0 && errno != EEXIST) {
            perror("mkdir");
            printf("ret:%d\r\n", ret);
            return ret;
        }
        ret = chdir(token);
        if (ret < 0) {
            perror("chdir");
            return ret;
        }
    }
    printf("%s created\r\n", path);
    ret = open(filename_codemodel, O_CREAT);
    if (ret < 0) {
        perror("open");
        return ret;
    } else
        printf("query file %s ok\r\n", filename_codemodel);

    close(ret);

    ret = open(filename_cache, O_CREAT);
    if (ret < 0) {
        perror("open");
        return ret;
    } else
        printf("query file %s ok\r\n", filename_cache);

    close(ret);

    return 0;
}

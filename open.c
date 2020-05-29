#include <openssl/ssl.h>

int main(int argc, char *argv[]) {
	printf("OpenSSl version: %s\n", OpenSSL_version(SSLEAY_VERSION));
	return 0;
}


all: cipher_m3

cipher_m3: produce_ciphertext_c3.c
	gcc -o cipher_m3 produce_ciphertext_c3.c -L/usr/local/opt/openssl/lib/ -I/usr/local/opt/openssl/include/ -lcrypto


clean:
	rm cipher_m3

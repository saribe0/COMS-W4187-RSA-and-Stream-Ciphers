#include <openssl/rsa.h>
#include <string.h>
#include <openssl/bn.h>

// Prints out a character array in hex, 2 hex values per byte
void print_hex(char* hex, int length)
{
        // Iterate through the array and print out the characters in hex
        for(int ii = 0; ii < length; ii++)
        {
                printf("%02x", (unsigned char)hex[ii]);
        }
        printf("\n");
}

// Removes colons from character strings
// - If *out is Null, memory will be allocated for it, remember to free it
// - Iterates through the input and adds any non-colon (':') to the output
void remove_colons(char* in, char** const out, int len_in)
{
        // Create new memory if necessary
        if(*out == NULL)
        {
                *out = malloc((len_in+1) * sizeof(char));
        }

        // Copy over characters that aren't colons to the output
        int jj = 0;
        for(int ii = 0; ii < len_in; ii++)
        {
                if(in[ii] != ':')
                {
                        *(*out + jj) = in[ii];
                        jj++;
                }
        }
}

// Validates that the method used to get the cipher c3 using the values given works
// - Randomly creates a key and generates ciphers for m1, m2, and m3
// - Applies the method derived (e->3, encrpyt c2) to get c3, ensure this matches the cipher for c3
void validate_method()
{
        // Prepare message buffers
        char m1[128] = {0};
        m1[127] = 4;
        char m2[128] = {0};
        m2[127] = 2;
        char m3[128] = {0};
        m3[127] = 8;

        // Prepare cipher buffers
        char c1[256]={'\0'};
        char c2[256]={'\0'};
        char c3[256]={'\0'};

        // Create public key
        RSA* key = RSA_generate_key(1024, RSA_F4, NULL, NULL);
        RSA_print_fp(stdout,key,0);

        // Generate ciphers
        int size1 = RSA_public_encrypt(RSA_size(key), (unsigned char*) m1, (unsigned char*) c1, key, RSA_NO_PADDING);
        int size2 = RSA_public_encrypt(RSA_size(key), (unsigned char*) m2, (unsigned char*) c2, key, RSA_NO_PADDING);
        int size3 = RSA_public_encrypt(RSA_size(key), (unsigned char*) m3, (unsigned char*) c3, key, RSA_NO_PADDING);

        // Modify key for validation
        BIGNUM* e = NULL;
        char* e_str = "3";
        int len_e = BN_dec2bn(&e, e_str);
        key->e = e;

        // Prepare cipher buffer for validation
        char c3_new[256] = {'\0'};

        // Preform encryption on c2 to get c3
        int size3_new = RSA_public_encrypt(RSA_size(key), (unsigned char*) c2, (unsigned char*) c3_new, key, RSA_NO_PADDING);

        // Compare c3_new and c3
        char* out = malloc(6 * sizeof(char));
        if(strcmp(c3, c3_new) == 0)
        {
                // If the strings are equal, output is true
                strcpy(out, "True");
        }
        else
        {
                // If they are not equal, output is false
                strcpy(out, "False");
        }

        // Print out m1, m2, m3, c1, c2, c3, and c3_new for visual verification
        printf("===========================  Validation  =======================\n");
        printf("M1: \n");
        print_hex(m1, 128);
        printf("M2: \n");
        print_hex(m2, 128);
        printf("M3: \n");
        print_hex(m3, 128);
        printf("\nC1: \n");
        print_hex(c1, 128);
        printf("C2: \n");
        print_hex(c2, 128);
        printf("C3: \n");
        print_hex(c3, 128);
        printf("\nC3 Generated From C2: \n");
        print_hex(c3_new, 128);
        printf("\nValidation Provs: %s\n", out);
        printf("===========================  Validation Complete  =======================\n");

        // Clean up 
        if(key) 
                RSA_free(key);
        if(out)
                free(out);
}

int main(void)
{
        /* Theory Evaluation:

         Method 1 (the one used):
         c1 = m1^e mod(n)
         c2 = m2^e mod(n)
         c3 = m3^e mod(n)

         We know that m2 = 2 and want m3 = 8
         m3 = m2^3

         c3 = (m2^3)^e mod(n)
         c3 = (m2^e)^3 mod(n)
         c3 = (m2^e mod(n))^3 mod(n)
         c3 = c2^3 mod(n)

         This becomes "Textbook" RSA encryption if we set the public exponent to 3, the modulus to n, and encrypt c2


         Method 2 (not implemented):
         c1 = m1^e mod(n)
         c2 = m2^e mod(n)
         c3 = m3^e mod(n)

         We know that m1 = 4, m2 = 2 and want m3 = 8
         m3 = m1 * m2

         c1 * c2 = m1^e mod(n) * m2^e mod(n)
         (c1 * c2) mod(n) = (m1^e mod(n) * m2^e mod(n)) mod(n)
         (c1 * c2) mod(n) = (m1^e * m2^e) mod(n)
         (c1 * c2) mod(n) = (m1 * m2)^e mod(n)
         (c1 * c2) mod(n) = (m3)^e mod(n)
         (c1 * c2) mod(n) = m3^e mod(n)
         (c1 * c2) mod(n) = c3
         c3 = (c1 * c2) mod(n)
        */

        // Check to see if this method works in practice with the library functions
        validate_method();

        // Create messages m1, m2, and m3 (128 byte big endian representations of 4, 2, and 8) as provided
        char m1[128] = {0};
        m1[127] = 4;
        char m2[128] = {0};
        m2[127] = 2;
        char m3[128] = {0};
        m3[127] = 8;

        // Create the output ciphers of c1 and c2 and a buffer for c3
        char c1[256] = "481526a435772e8c9426a76eab9733bd1a56c8edb15f1431fe8f48734c9b4a1fcdef686e24695ae622bcad76cb94f328a0102b47117b509bcf2086f33a2d12da67016629beb5b17bfad4d8e703211bb508eea588ed565cc00785555717c489b31cdb433be3a37e3462a714d57ff89e61df6e3266374fbb6e7214f41463ad23f5";
        char c2[256] = "830fba3a7606b09aa82b9d21f1a00b706b7096c709283b1bdb60117a3e2a1680406edd751bc4ca86b47bb77fbdda39873a59e7123b3f17e00daf77d023f9bd9e9162dbe288c955d0712eff6c35058a259b9d905bad2a11200452b18e15be59cdcaebe922c52ed90323d258532d2a089bd7daa061915b1a49a22f266b75f843ee";
        char c3_new[256]={'\0'};

        // Print out M1 through M3 for verification
        printf("M1: \n");
        print_hex(m1, 128);
        printf("M2: \n");
        print_hex(m2, 128);
        printf("M3: \n");
        print_hex(m3, 128);

        // Create the exponent and modulus
        BIGNUM* e = NULL;
        char* e_str = "3";
        int len_e = BN_dec2bn(&e, e_str);

        BIGNUM* n = NULL;
        char* n_str_temp = "b2:55:14:6d:77:62:45:2a:d2:3e:4e:7a:0c:b5:af:f4:a8:87:d5:81:b1:87:16:96:d3:34:72:8c:90:3f:56:be:68:4b:a4:7d:e8:af:b5:06:82:ef:3f:bc:cc:26:63:c8:3b:03:9f:00:eb:c8:b7:23:22:10:89:75:57:b8:1c:f3:06:d9:6b:a7:3b:02:86:b6:c3:66:48:83:94:76:1f:00:ca:54:ff:ef:16:f0:f9:79:be:46:94:c5:b8:2e:90:d7:be:31:22:5c:61:6e:c4:bd:19:2f:68:ee:ca:f1:f5:4f:cf:f1:60:80:7f:bc:af:7e:49:d9:ce:3c:39:2e:8a:7d";
        char* n_str = NULL;
        remove_colons(n_str_temp, &n_str, strlen(n_str_temp));
        int len_n = BN_hex2bn(&n, n_str);

        // Create the public key
        RSA *pub_key = RSA_new();
        pub_key->e = e;
        pub_key->n = n;

        // Generate the output cipher and print out the new c3 that was generated using the method above
        int size = RSA_public_encrypt(RSA_size(pub_key), (unsigned char*) c2, (unsigned char*) c3_new, pub_key, RSA_NO_PADDING);
        printf("C3: \n");
        print_hex(c3_new, size);

        // Clean up and exit successfully
        if(pub_key) 
                RSA_free(pub_key);
        if(n_str)
                free(n_str);

        return 0;
}
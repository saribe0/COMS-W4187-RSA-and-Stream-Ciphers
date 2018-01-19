# COMS-W4187-RSA-and-Stream-Ciphers
COMS W4187 Security Architecture and Engineering HW4: RSA and Stream Ciphers

## Folder Organization
Thes directory includes this ReadMe, the assignment descriptor, two output screenshots of my solution, a makefile for my code (not rsa_demo.c), rsa_demo.c code, and my produce_ciphertext_c3.c code.

produce_ciphertext_c3.c is what contains my implementation of the solution. It has three auxiliary functions and the main function. The main function is where I solve for the c3 (the cipher to message m3). The three other functions do the following:

print_hex -> Prints a character array as hex with two hex characters per byte.
remove_colons -> Removes colons from an input character array (needed to deal with the output of data printed from RSA_print_fp)
validate_method -> Takes the rsa_demo code, reorganizes and modifies it to create output ciphers for m1, m2, and m3 using a new public/private key pair. The function then generates the cipher for m3 using the method I use in my solution and compares it against the output from the standard encryption method. This validates that my approach to finding the cipher for m3 given m1 and m2 is appropriate.

#### Note the rsa_demo.c has been modified to do the same as the validate_method function in my code. The validate_method function is cleaner however and more of a validation solution while the rsa_demo is more of a proof of concept.

## Cipher for m3
M3:
0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000008

C3:
7da5ba536eca580d8344d21ec4f8a2ee0651fd01a56e7e66d98bc6b760123a56e27f574222183817edefde0af4903c746822561424749168429bd6dd01a9b6a0ba2f7f7154409aec4cf27f990f6b85871cc56908e987973965417e0f47f2f196141d44dde6b4543c2c732de9acb480814c7b4c0ba57432fef0641f3d9508daf3

## Explanation of the Attack
"Textbook" RSA encryption works by using a public key to encrypt and a private key to decrypt. Encryption takes the form of  `c = m^e mod(n)`. This means that if n is known, certain modifications can be made to c that carry through to when the message is decrypted. Most of these have to do with exponent laws. The two ways that I found to generate the cipher for M3 can be proved in the following way:

```
Method 1:

We know that:
c1 = m1^e mod(n)
c2 = m2^e mod(n)
c3 = m3^e mod(n)

We also know that, for this assignment: m2 = 2 and m3 = 8
Therefore: m3 = m2^3

Plugging this in for m3 in the equation for m3's cipher, we get:
c3 = (m2^3)^e mod(n)
c3 = (m2^e)^3 mod(n)
c3 = (m2^e mod(n))^3 mod(n)
c3 = c2^3 mod(n)

This becomes "Textbook" RSA encryption if we set the public exponent to 3, the modulus to n, and encrypt c2
```
This is the method I used when solving for C3. There is however another method to solve for C3 as well:

```
Method 2:

We know that:
c1 = m1^e mod(n)
c2 = m2^e mod(n)
c3 = m3^e mod(n)

We also know that m1 = 4, m2 = 2 and m3 = 8
m3 = m1 * m2

Using this fact, let us first multiply c1 and c2:
c1 * c2 = m1^e mod(n) * m2^e mod(n)
(c1 * c2) mod(n) = (m1^e mod(n) * m2^e mod(n)) mod(n)
(c1 * c2) mod(n) = (m1^e * m2^e) mod(n)
(c1 * c2) mod(n) = (m1 * m2)^e mod(n)

Since we know m3 = m1 * m2, we can subsitute m3 into the equation:
(c1 * c2) mod(n) = (m3)^e mod(n)
(c1 * c2) mod(n) = m3^e mod(n)
(c1 * c2) mod(n) = c3
c3 = (c1 * c2) mod(n)

Here we find that c3 can be reached by multiplying the ciphers of m1 and m2 and taking the mod n of their product.
```
I did not implement this method though it should work just as well. These attacks work if the plaintext you are trying to find the cipher for is a product of two other plaintexts whose ciphers you have or if the plaintext is to the power of one of the plaintexts whose cipher you have.

## How To Prevent These Attacks
To combat these attacks, a random, fresh value and two hash functions can be added. Instead of encrypting the plaintext message, encrypt the value of the mesage xor'd with a hashed "fresh" random value. Since this random value is "fresh," it is new every time something is encrypted. This means that two ciphers will not be able to be modified in the same way as above because the values that can be multiplied or put to some exponent are the message xor'd with a hashed random value. As a result, doing such multiplication or exponentiating would change the xor'd value and, due to the one way nature of a hash, make it impossible to revert back into a valid plaintext message.






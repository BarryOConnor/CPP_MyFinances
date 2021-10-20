#pragma once
//using namespace std;
#include <string>

/*=========================================
ENCRYPTION CLASS
===========================================*/
class EncryptionClass {
	string encryption_key;
	string encryptDecrypt(string to_encrypt);
public:
	EncryptionClass();
	string encrypt(string);
	string decrypt(string);
};

#pragma once
#include "stdafx.h"
//using namespace std;
#include <string>
#include "EncryptionClass.h"
#include "MyFinancesDefinitions.h"
#include "3rdpartylibs/base64.h"


EncryptionClass::EncryptionClass() {
	encryption_key = APP_ENCRYPTION_KEY;
	for (int loop = 0; loop < (int)encryption_key.length() - 1; ++loop)   // y is 0, the first character of x
	{                              // and as long as y is less than the length of x
		encryption_key[loop] += 10;          // the y of x plus one
	}
}


string EncryptionClass::encryptDecrypt(string input) {
	char key[MAX_KEY_CHARS];
	strncpy_s(key, encryption_key.c_str(), sizeof(key)); //Any chars will work
	string output = input;

	for (int i = 0; i < (int)input.size(); i++)
		output[i] = input[i] ^ key[i % (sizeof(key) / sizeof(char))];

	return output;
}

string EncryptionClass::encrypt(string input) {
	string output = input;
	output = encryptDecrypt(output);
	output = base64_encode(reinterpret_cast<const unsigned char*>(output.c_str()), output.length());
	return output;
}

string EncryptionClass::decrypt(string input) {
	string output = input;
	output = base64_decode(output);
	output = encryptDecrypt(output);
	return output;
}
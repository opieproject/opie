/* C implementation of RC2 encryption algorithm, as described in RFC2268 */
/* By Matthew Palmer <mjp16@uow.edu.au> */
/* $Id: krc2.cpp,v 1.1 2003-07-22 19:23:17 zcarsten Exp $ */

#include "krc2.h"

unsigned char _rc2_pitable[] = { 0xd9, 0x78, 0xf9, 0xc4, 0x19, 0xdd, 0xb5, 0xed,
			    0x28, 0xe9, 0xfd, 0x79, 0x4a, 0xa0, 0xd8, 0x9d,
			    0xc6, 0x7e, 0x37, 0x83, 0x2b, 0x76, 0x53, 0x8e,
			    0x62, 0x4c, 0x64, 0x88, 0x44, 0x8b, 0xfb, 0xa2,
			    0x17, 0x9a, 0x59, 0xf5, 0x87, 0xb3, 0x4f, 0x13,
			    0x61, 0x45, 0x6d, 0x8d, 0x09, 0x81, 0x7d, 0x32,
			    0xbd, 0x8f, 0x40, 0xeb, 0x86, 0xb7, 0x7b, 0x0b,
			    0xf0, 0x95, 0x21, 0x22, 0x5c, 0x6b, 0x4e, 0x82,
			    0x54, 0xd6, 0x65, 0x93, 0xce, 0x60, 0xb2, 0x1c,
			    0x73, 0x56, 0xc0, 0x14, 0xa7, 0x8c, 0xf1, 0xdc,
			    0x12, 0x75, 0xca, 0x1f, 0x3b, 0xbe, 0xe4, 0xd1,
			    0x42, 0x3d, 0xd4, 0x30, 0xa3, 0x3c, 0xb6, 0x26,
			    0x6f, 0xbf, 0x0e, 0xda, 0x46, 0x69, 0x07, 0x57,
			    0x27, 0xf2, 0x1d, 0x9b, 0xbc, 0x94, 0x43, 0x03,
			    0xf8, 0x11, 0xc7, 0xf6, 0x90, 0xef, 0x3e, 0xe7,
			    0x06, 0xc3, 0xd5, 0x2f, 0xc8, 0x66, 0x1e, 0xd7,
			    0x08, 0xe8, 0xea, 0xde, 0x80, 0x52, 0xee, 0xf7,
			    0x84, 0xaa, 0x72, 0xac, 0x35, 0x4d, 0x6a, 0x2a,
			    0x96, 0x1a, 0xd2, 0x71, 0x5a, 0x15, 0x49, 0x74,
			    0x4b, 0x9f, 0xd0, 0x5e, 0x04, 0x18, 0xa4, 0xec,
			    0xc2, 0xe0, 0x41, 0x6e, 0x0f, 0x51, 0xcb, 0xcc,
			    0x24, 0x91, 0xaf, 0x50, 0xa1, 0xf4, 0x70, 0x39,
			    0x99, 0x7c, 0x3a, 0x85, 0x23, 0xb8, 0xb4, 0x7a,
			    0xfc, 0x02, 0x36, 0x5b, 0x25, 0x55, 0x97, 0x31,
			    0x2d, 0x5d, 0xfa, 0x98, 0xe3, 0x8a, 0x92, 0xae,
			    0x05, 0xdf, 0x29, 0x10, 0x67, 0x6c, 0xba, 0xc9,
			    0xd3, 0x00, 0xe6, 0xcf, 0xe1, 0x9e, 0xa8, 0x2c,
			    0x63, 0x16, 0x01, 0x3f, 0x58, 0xe2, 0x89, 0xa9,
			    0x0d, 0x38, 0x34, 0x1b, 0xab, 0x33, 0xff, 0xb0,
			    0xbb, 0x48, 0x0c, 0x5f, 0xb9, 0xb1, 0xcd, 0x2e,
			    0xc5, 0xf3, 0xdb, 0x47, 0xe5, 0xa5, 0x9c, 0x77,
			    0x0a, 0xa6, 0x20, 0x68, 0xfe, 0x7f, 0xc1, 0xad };
	
unsigned char _rc2_expkey[128];	/* Expanded Key */
int _rc2_counter;				/* global integer variable used in mixing */
int _rc2_s[] = {1, 2, 3, 5};

Krc2::Krc2()
{
}

Krc2::~Krc2()
{
}

void Krc2::rc2_expandkey(char key[], int length, int ekl)
{
	int ekl8, keymask, i;
	
	/* Put supplied key into first length - 1 bytes of the key buffer */
	for (i = 0; i < length; i++) {
		_rc2_expkey[i] = key[i];
	}
	
	ekl8 = (ekl + 7) / 8;
	i = _rc2_pow(2, (8 + ekl - 8 * ekl8));
	keymask = 255 % i;
	
	/* First expansion step */
	for (i = length; i < 128; i++) {
		_rc2_expkey[i] = _rc2_pitable[(_rc2_expkey[i - 1] + _rc2_expkey[i - length]) % 256];
	}
	
	/* Expansion intermediate step */
	_rc2_expkey[128 - ekl8] = _rc2_pitable[_rc2_expkey[128 - ekl8] & keymask];
	
	/* Third Expansion step */
	for (i = 127 - ekl8; i >= 0; i--) {
		_rc2_expkey[i] = _rc2_pitable[_rc2_expkey[i + 1] ^ _rc2_expkey[i + ekl8]];
	}
}

void Krc2::rc2_encrypt(unsigned short input[4])
{
	int i;
	
	_rc2_counter = 0;
	for (i = 0; i < 5; i++) {
		_rc2_mix(input);
	}
	_rc2_mash(input);
	for (i = 0; i < 6; i++) {
		_rc2_mix(input);
	}
	_rc2_mash(input);
	for (i = 0; i < 5; i++) {
		_rc2_mix(input);
	}
}

void Krc2::_rc2_mix(unsigned short input[])
{
	unsigned short K, i;
	
	for (i = 0; i < 4; i++) {
		K = _rc2_expkey[_rc2_counter * 2] + 256 * _rc2_expkey[_rc2_counter * 2 + 1];
		input[i] = input[i] + K + (input[(i + 3) % 4] & input[(i + 2) % 4]) + ((~input[(i + 3) % 4]) & input[(i + 1) % 4]);
		_rc2_counter++;
		input[i] = _rc2_rol(input[i], _rc2_s[i]);
	}
}

void Krc2::_rc2_mash(unsigned short input[])
{
	unsigned short K, i, x;
	
	for (i = 0; i < 4; i++) {
		x = input[(i + 3) % 4] & 63;
		K = _rc2_expkey[2 * x] + 256 * _rc2_expkey[2 * x + 1];
		input[i] = input[i] + K;
	}
}

void Krc2::rc2_decrypt(unsigned short input[4])
{
	int i;
	
	_rc2_counter = 63;
	for (i = 0; i < 5; i++) {
		_rc2_rmix(input);
	}
	_rc2_rmash(input);
	for (i = 0; i < 6; i++) {
		_rc2_rmix(input);
	}
	_rc2_rmash(input);
	for (i = 0; i < 5; i++) {
		_rc2_rmix(input);
	}
}

void Krc2::_rc2_rmix(unsigned short input[])
{
	unsigned short K;
	int i;

	for (i = 3; i >= 0; i--) {
		input[i] = _rc2_ror(input[i], _rc2_s[i]);
		K = _rc2_expkey[_rc2_counter * 2] + 256 * _rc2_expkey[_rc2_counter * 2 + 1];
		input[i] = input[i] - K - (input[(i + 3) % 4] & input[(i + 2) % 4]) - ((~input[(i + 3) % 4]) & input[(i + 1) % 4]);
		_rc2_counter--;
	}
}

void Krc2::_rc2_rmash(unsigned short input[])
{
	unsigned short K, x;
	int i;
	
	for (i = 3; i >= 0; i--) {
		x = input[(i + 3) % 4] & 63;
		K = _rc2_expkey[2 * x] + 256 * _rc2_expkey[2 * x + 1];
		input[i] = input[i] - K;
	}
}

int Krc2::_rc2_pow(int base, int exponent)
{
	int i, result;

	if (exponent == 0) {
		return 1;
	}
	result = 1;
	for (i = 0; i < exponent; i++) {
		result = result * base;
	}
	return result;
}

unsigned short Krc2::_rc2_rol(unsigned short input, int places)
{
	unsigned short temp, i;
	
	for (i = 0; i < places; i++) {
		temp = input & 0x8000;
		input = input << 1;
		if (temp) {
			input++;
		}
	}
	return input;
}

unsigned short Krc2::_rc2_ror(unsigned short input, int places)
{
	unsigned short temp, i;
	for (i = 0; i < places; i++) {
		temp = input & 0x1;
		input = input >> 1;
		if (temp) {
			input = input + 0x8000;
		}
	}
	return input;
}


/*
** $Id: krc2.h,v 1.1 2003-07-22 19:23:17 zcarsten Exp $
*/

#ifndef _KRC2_H_
#define _KRC2_H_

/* Header file for rc2 implementation by Matthew Palmer <mjp16@uow.edu.au> */

class Krc2
{
public:
	Krc2();
	~Krc2();

	/* Externally worked functions */
	void rc2_expandkey(char key[], int length, int ekl);
	void rc2_encrypt(unsigned short *input);
	void rc2_decrypt(unsigned short *input);

private:
	/* The internals */
	void _rc2_mix(unsigned short *input);
	void _rc2_mash(unsigned short *input);
	void _rc2_rmix(unsigned short *input);
	void _rc2_rmash(unsigned short *input);
	int _rc2_pow(int base, int exponent);
	unsigned short _rc2_ror(unsigned short input, int places);
	unsigned short _rc2_rol(unsigned short input, int places);

};
#endif // _KRC2_H_




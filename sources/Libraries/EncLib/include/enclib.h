
////////////////////////////////////////
//
//      encryption/decryption
//      by jampe
//      2006/5/26
//
////////////////////////////////////////

#if !defined __ENC_LIB__
#define __ENC_LIB__


typedef unsigned char       __byte;


//  sets the key
//  key is the 8 byte key
extern int SetEncKey(const __byte* key);


//  encrypts
//  buf output buffer
//  len output buffer length
//  pwd the password to encrypt
//  plen the password length
//  returns the length of the encrypted string
extern int Encrypt(__byte* buf, int len, const __byte* pwd, int plen);


//  decrypts
//  buf output buffer
//  len output buffer length
//  enc the key to decrypt with
//  elen the key length
//  returns the length of the encrypted string
extern int Decrypt(__byte* buf, int len, const __byte* enc, int elen);



#if NDEBUG
#pragma comment(lib, "EncLib.lib")
#else
#pragma comment(lib, "EncLib_d.lib")
#endif


#endif  /*__ENC_LIB__*/

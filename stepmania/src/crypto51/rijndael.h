#ifndef CRYPTOPP_RIJNDAEL_H
#define CRYPTOPP_RIJNDAEL_H

/** \file
*/

#include "seckey.h"
#include "secblock.h"

NAMESPACE_BEGIN(CryptoPP)

struct Rijndael_Info : public FixedBlockSize<16>, public VariableKeyLength<16, 16, 32, 8>
{
	static const char *StaticAlgorithmName() {return "Rijndael";}
};

/// <a href="http://www.weidai.com/scan-mirror/cs.html#Rijndael">Rijndael</a>
class Rijndael : public Rijndael_Info, public BlockCipherDocumentation
{
	class Base : public BlockCipherBaseTemplate<Rijndael_Info>
	{
	public:
		void UncheckedSetKey(CipherDir direction, const byte *userKey, unsigned int length);

	protected:
		static const word32 Te0[256];
		static const word32 Te1[256];
		static const word32 Te2[256];
		static const word32 Te3[256];
		static const word32 Te4[256];

		static const word32 Td0[256];
		static const word32 Td1[256];
		static const word32 Td2[256];
		static const word32 Td3[256];
		static const word32 Td4[256];

		static const word32 rcon[];

		unsigned int m_rounds;
		SecBlock<word32> m_key;
	};

	class Enc : public Base
	{
	public:
		void ProcessAndXorBlock(const byte *inBlock, const byte *xorBlock, byte *outBlock) const;
	};

	class Dec : public Base
	{
	public:
		void ProcessAndXorBlock(const byte *inBlock, const byte *xorBlock, byte *outBlock) const;
	};

public:
	typedef BlockCipherTemplate<ENCRYPTION, Enc> Encryption;
	typedef BlockCipherTemplate<DECRYPTION, Dec> Decryption;
};

typedef Rijndael::Encryption RijndaelEncryption;
typedef Rijndael::Decryption RijndaelDecryption;

NAMESPACE_END

#endif

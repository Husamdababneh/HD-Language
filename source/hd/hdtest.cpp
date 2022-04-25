/* ========================================================================
   $File: hdtest.cpp
   $Date: 2022-04-23
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: Random file used for testing
    ========================================================================*/


constexpr U32
djb2(String data)
{
	U32 hash = 5381;
	for (U64 i = 0; i < data.length; i++)
		hash = ((hash << 5) + hash) + (U32)data[i];	
	return hash;
}

U32
djb2(unsigned char *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}



static unsigned long
sdbm(unsigned char *str)
{
	unsigned long hash = 0;
	int c;

	while (c = *str++)
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}

// this is just checksum
unsigned long
loselose(unsigned char *str)
{
	unsigned int hash = 0;
	int c;

	while (c = *str++)
	    hash += c;

	return hash;
}

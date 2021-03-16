int nVal = 0xA156;
int nVal2 = nVal & 0xF000;

int nFlags = 0;

typedef enum { READ_ONLY   = 1,
			   SYSTEM	   = 2,
			   ARCHIV      = 4,
			   BIGGER_1MB  = 8
              };


nFlags = READ_ONLY | BIGGER_1MB;

neu_nFlags = nFlags & ~BIGGER_1MB; // READ_ONLY jetzt weg
0000000000001001 & 1111111111110111 == 0000000000000001

8   0x0008
1   0x0001
0x0008 | 0x0001 = 0x0009

1000 | 0001 == 1001
1001 == 0x0009
1000 | 1000 == 1000


1001 & 0001 == 1000 != 0 ==TRUE

if( nFlags & READ_ONLY)

0x0009 & 0x0001
0x0000
0x0001


0xAD36 & 0xFFF0 = 0xAD30

nID = 0x002D
0x002D & 0xFFF0 == 0x0020

0xFFF0
1111111111110000
0xAAA0
1010101010100000
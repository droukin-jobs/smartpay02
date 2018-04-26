
//shift positions 
#define C_S 0 
#define A_S 2 
//bitmasks when shifted right
#define C_BM 0b00000011
#define A_BM 0b00000011

//note that i % 2 == i & 1

//put even indexed value in the first half of the byte
//put odd indexed value in the second half of the byte
#define PACK_CARD(I,V) (((V) << (((I) & 1) << 2)) << (C_S))
#define PACK_ACCT(I,V) (((V) << (((I) & 1) << 2)) << (A_S))

//extract even fro 0x0f, odd from 0xf0, shift and mask, get the string value from arr
#define UNPACK_CARD(I,V) ((((V) >> (((I) & 1) << 2)) >> (C_S)) & (C_BM))
#define UNPACK_ACCT(I,V) ((((V) >> (((I) & 1) << 2)) >> (A_S)) & (A_BM))

/*
** HDP.H : Encoding and Decoding of High Dynamic Range Pixels
*/

/*
** Encoding and Decoding Types
*/
typedef unsigned char byte;
typedef float real; /* change to "double" if you work with double precision */

typedef byte bytecolor[3];
typedef real realcolor[3];

/*
** Encoding and Decoding Tables
*/
extern byte *EncodeLut;
extern real *DecodeLut;
extern real  LutScale;

/*
** Encoding and Decoding Functions
*/
extern int init_HDP_encode (real,real,int);
extern int init_HDP_decode (real,real,real);
extern void exit_HDP_encode (void);
extern void exit_HDP_decode (void);

/*
** Encoding and Decoding Macros
*/
#define HDP_ENCODE(RealColor,ByteColor) ( \
   ByteColor[0] = EncodeLut [(int) (RealColor[0] * LutScale + 0.5)], \
   ByteColor[1] = EncodeLut [(int) (RealColor[1] * LutScale + 0.5)], \
   ByteColor[2] = EncodeLut [(int) (RealColor[2] * LutScale + 0.5)])

#define HDP_DECODE(ByteColor,RealColor) ( \
   RealColor[0] = DecodeLut [ByteColor[0]], \
   RealColor[1] = DecodeLut [ByteColor[1]], \
   RealColor[2] = DecodeLut [ByteColor[2]])

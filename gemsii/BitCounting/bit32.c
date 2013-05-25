/*
 * bit32on - count the 1 bits in a 32-bit long integer. Three variants are
 *           provided (the second accepts further code revision); one will
 *           be ``best'' based on the nature of the data and the hardware.
 *           Use of "register" directives may also yield further speed-up.
 *           The user is urged to perform comparative analysis for a given
 *           architecture and to study the assembler output, or to hand code.
 *           This code has been thoroughly tested by the second author.
 *
 * Alan W. Paeth
 * David Schilling
 */
 
int bit32on1(a)
  unsigned long a;
  {
  int c;
  c = 0;
  while( a != 0 )       /* until no bits remain, */
    {
    c++;                /* "tally" ho, then */
    a = a &~ -a;        /* clear lowest bit */
    }
  return(c);
  }

int bit32on2(a)
  unsigned long a;                              /* a: 32  1-bit tallies */
  {
  a = (a&0x55555555) + ((a>>1) &(0x55555555));  /* a: 16  2-bit tallies */
  a = (a&0x33333333) + ((a>>2) &(0x33333333));  /* a:  8  4-bit tallies */
  a = (a&0x07070707) + ((a>>4) &(0x07070707));  /* a:  4  8-bit tallies */
/* a %= 255; return(a); may replace what follows */
  a = (a&0x000F000F) + ((a>>8) &(0x000F000F));  /* a:  2 16-bit tallies */
  a = (a&0x0000001F) + ((a>>16)&(0x0000001F));  /* a:  1 32-bit tally */
  return(a);
  }

int bit32on3(a)
    unsigned long a;
    {
    unsigned long mask, sum;
    if (a == 0)                 /* a common case */
        return(0);
    else if (a == 0xffffffff)   /* ditto, but the early return is essential: */
        return(32);             /* it leaves mod 31 (not 33) final states */
    mask = 0x42108421L;
    sum = a & mask;             /* 5x: accumulate through a 1-in-5 sieve */
    sum += (a>>=1) & mask;
    sum += (a>>=1) & mask;
    sum += (a>>=1) & mask;
    sum += (a>>=1) & mask;
    sum %= (mask = 31);         /* casting out mod 31 (save that constant) */
    return(sum ? sum : mask);   /* return bits (zero indicated 31 bits on) */
    }




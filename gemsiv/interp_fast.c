/*
 * C code from the article
 * "Faster Linear Interpolation"
 * by Steven Eker, steve@cs.city.ac.uk
 * in "Graphics Gems IV", Academic Press, 1994
 */

#include <stdio.h>
/*
 *	Test routine:
 *		Read in a, b, c
 *		Compute interpolations using old and new routines
 *		Check for differences
 *		Print interpolation
 */
main()
{
  short o[1000], o2[1000];
  int a, b, c, i;

  while(scanf("%d%d%d", &a, &b, &c) == 3){
    dec_var(a, b, c, o);
    linear(a, b, c, o2);
    printf("\n");
    for(i = 0; i <= a; i++){
      if(o[i] != o2[i]){
	printf("Error\n");
	printf("i = %d, o[i] = %d, o2[i] = %d\n", i, o[i], o2[i]);
	exit(1);
      }
      printf("%d ", o[i]);
    }
    printf("\n\n");
  }
}

/*
 *	Decision variable method
 */
dec_var(a, b, c, o)
int a, b, c;
short *o;
{
  int i1 = b / a, e1 = 2 * (b % a);
  int i2 = i1 + 1, e2 = e1 - 2 * a;
  int t = c, r = e1 - a;

  do{
    *o++ = t;
    if(r < 0){
      t += i1; r += e1;
    }
    else{
      t += i2; r += e2;
    }
  }while(--a >= 0);
}

/*
 *	Fast decision variable method
 */
linear(a, b, c, o)
int a, b, c;
short *o;
{
  int t, i1, i2;

  if(a > b){
    t = 0; i1 = b;
  }
  else{
    t = b / a; i1 = b % a;
  }
  i1 = (i1 << 16) + t;
  i2 = i1 - (a << 16) + 1;
  t = c - t - (((a + 1) >> 1) << 16);
  switch(a & 3){
  do{
  case 3:
    if((t += i1) >= 0) goto pos1;
neg1:
    *o++ = t;
  case 2:
    if((t += i1) >= 0) goto pos2;
neg2:
    *o++ = t;
  case 1:
    if((t += i1) >= 0) goto pos3;
neg3:
    *o++ = t;
  case 0:
    if((t += i1) >= 0) goto pos4;
neg4:
    *o++ = t;
  }while((a -= 4) >= 0);
  }
  return;
  do{
    if((t += i2) < 0) goto neg1;
pos1:
    *o++ = t;
    if((t += i2) < 0) goto neg2;
pos2:
    *o++ = t;
    if((t += i2) < 0) goto neg3;
pos3:
    *o++ = t;
    if((t += i2) < 0) goto neg4;
pos4:
    *o++ = t;
  }while((a -= 4) >= 0);
}

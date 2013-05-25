/*
Median Finding on a 3-by-3 Grid
by Alan Paeth
from "Graphics Gems", Academic Press, 1990
*/

#define s2(a,b) {register int t; if ((t=b-a)<0) {a+=t; b-=t;}}
#define mn3(a,b,c) s2(a,b); s2(a,c);
#define mx3(a,b,c) s2(b,c); s2(a,c);
#define mnmx3(a,b,c) mx3(a,b,c); s2(a,b);
#define mnmx4(a,b,c,d) s2(a,b); s2(c,d); s2(a,c); s2(b,d);
#define mnmx5(a,b,c,d,e) s2(a,b); s2(c,d); mn3(a,c,e); mx3(b,d,e);
#define mnmx6(a,b,c,d,e,f) s2(a,d); s2(b,e); s2(c,f);\
                            mn3(a,b,c); mx3(d,e,f);
med3x3(b1, b2, b3)
    int *b1, *b2, *b3;
/*
 * Find median on a 3x3 input box of integers.
 * b1, b2, b3 are pointers to the left-hand edge of three
 * parallel scan-lines to form a 3x3 spatial median.
 * Rewriting b2 and b3 as b1 yields code which forms median
 * on input presented as a linear array of nine elements.
 */
    {
    register int r1, r2, r3, r4, r5, r6;
    r1 = *b1++; r2 = *b1++; r3 = *b1++;
    r4 = *b2++; r5 = *b2++; r6 = *b2++;
    mnmx6(r1, r2, r3, r4, r5, r6);
    r1 = *b3++;
    mnmx5(r1, r2, r3, r4, r5);
    r1 = *b3++;
    mnmx4(r1, r2, r3, r4);
    r1 = *b3++;
    mnmx3(r1, r2, r3);
    return(r2);
    }


/* t2(i,j) transposes elements in A[] such that A[i] <= A[j] */

#define t2(i, j) s2(A[i-1], A[j-1])


int median25(A)
    int A[25];
    {
/*
 * median25(A) partitions the array A[0..24] such that element
 * A[12] is the median and subarrays A[0..11] and A[13..24]
 * are partitions containing elements of smaller and larger
 * value (rank), respectively.
 *
 * The exchange table lists element indices on the range 1..25,
 * this accounts for the "-1" offsets in the macro t2 and in
 * the final return value used to adjust subscripts to C-code
 * conventions (array indices begin at zero).
 */
    t2( 1, 2); t2( 4, 5); t2( 3, 5); t2( 3, 4); t2( 7, 8);
    t2( 6, 8); t2( 6, 7); t2(10,11); t2( 9,11); t2( 9,10);
    t2(13,14); t2(12,14); t2(12,13); t2(16,17); t2(15,17);
    t2(15,16); t2(19,20); t2(18,20); t2(18,19); t2(22,23);
    t2(21,23); t2(21,22); t2(24,25); t2( 3, 6); t2( 4, 7);
    t2( 1, 7); t2( 1, 4); t2( 5, 8); t2( 2, 8); t2( 2, 5);
    t2(12,15); t2( 9,15); t2( 9,12); t2(13,16); t2(10,16);
    t2(10,13); t2(14,17); t2(11,17); t2(11,14); t2(21,24);
    t2(18,24); t2(18,21); t2(22,25); t2(19,25); t2(19,22);
    t2(20,23); t2( 9,18); t2(10,19); t2( 1,19); t2( 1,10);
    t2(11,20); t2( 2,20); t2( 2,11); t2(12,21); t2( 3,21);
    t2( 3,12); t2(13,22); t2( 4,22); t2( 4,13); t2(14,23);
    t2( 5,23); t2( 5,14); t2(15,24); t2( 6,24); t2( 6,15);
    t2(16,25); t2( 7,25); t2( 7,16); t2( 8,17); t2( 8,20);
    t2(14,22); t2(16,24); t2( 8,14); t2( 8,16); t2( 2,10);
    t2( 4,12); t2( 6,18); t2(12,18); t2(10,18); t2( 5,11);
    t2( 7,13); t2( 8,15); t2( 5, 7); t2( 5, 8); t2(13,15);
    t2(11,15); t2( 7, 8); t2(11,13); t2( 7,11); t2( 7,18);
    t2(13,18); t2( 8,18); t2( 8,11); t2(13,19); t2( 8,13);
    t2(11,19); t2(13,21); t2(11,21); t2(11,13);
    return(A[13-1]);
    }

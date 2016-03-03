/*
 * Graphics Gems III: Fast Generation of Cyclic Sequences
 *
 * programmed by Alan Paeth (awpaeth@alumni.caltech.edu)
 *
 * These functions are implemented as macros which fall into two classes:
 *
 *   sequences -- generate a cyclic set of N arbitrary values
 *   triggers  -- generate a cyclic set of N boolean test conditions.
 *
 * Macros come as a set. The first initializes (declarative), the second steps.
 * These are paired for  VARIABLE / BOOLEAN  triggering use.
 * The first are called  "sequN" such as "getch()" and
 * will not introduce side-effects. A third macro named "testM" provides
 * boolean triggering expressions without altering the current step. Its value
 * is "true" after step "i" if the ith character in string M is non-zero.
 *
 * Note: these macros have been tuned for performance and internal consistency.
 * Unlike the book exposition, a cycle's current value is undefined until
 * *after* the first call to "cyclN" or "stepN". This style facilitates fast
 * testing as in "do { } while(condition)" loops at the expense of additional
 * first-use overhead, total amortized loop time remaining unchanged. The code
 * takes advantage of both register variables and the C "comma" operator.
 * Loops may be cycled symbolically, thereby reordering the initialization or
 * step macros to take advantage of specific compiler or problem optimizations.
 */

/*
 * Sequences
 *
 * The macro "sequN(parm1,...,parmN) initializes a sequence N values;
 * the macro "cyclN()" advances to (and returns in "t1") the next cyclic value.
 */

/* cycle: [a, b]  (eqn 2.2) */

#define sequ2(a,b) register int t1, t2; t1 = (int)(b); t2 = t1^(int)(a)
#define cycl2() (t1 ^= t2)

/* cycle: [a, b, c] (eqn 3.3) */

#define sequ3(a,b,c) register int t1, t2, t3; \
  t1 = (int)(c); t2 = t1^(int)(b); t3 = t2^(int)(a)
#define cycl3() (t2 ^= (t1 ^ t3), t1 ^= t2)


/* cycle: [a, b, c, d, e, f]  (eqn 6.2) 
 *
 * register t1 may be removed if value is not kept; t4 provides only an offset
 * a second table may be easily added to the LHS of the innermost subexpression 
 * values may be cast as ints or the macro rewritten to support floats, etc.  
 */

#define sequ6(a,b,c,d,e,f) register int t1,t2,t3,t4[7],*t5;t2=t3=1;t5= &t4[3];\
   t4[1]=(a); t4[0]=(b); t4[2]=(c); t4[5]=(d); t4[6]=(e); t4[4]=(f)
#define cycl6() (t1=t5[(t2 += t3, t3 += (~t2))])

/* cycle: [1, 2, 3]   (eqn 3.3b, end) */

#define sequ123() register int t1, t2; t1 = 3; t2 = 1
#define cycl123() (t2 ^= t1, t1 ^= t2)


/*
 * Triggers
 *
 * The macro "trigN() initializes a boolean cycle of N states.
 * the macro "stepN()" advances the state
 * the macro "testM" is a true expression at step i if M's ith char is '1'.
 */

/* trigger mod 2 (eqn 2.1) */

#define trig2() register int t1 = 0
#define step2() (t1 = !t1)
#define test01() (!t1)
#define test10() (t1)

/* trigger mod 3 (eqn 3.2) */

#define trig3() register int t1, t2; t1 = t2 = 1
#define step3() (t1 ^= t2, t2 ^= t1)
#define test100() (!t1)
#define test010() (!t2)
#define test001() (t1 == t2)
#define test110() (t1 != t2)

/* trigger mod 6 (eqn 6.3) */

#define trig6() register int t1, t2; t1 = -1; t2 = 1
#define step6() (t1 += t2, t2 += ~t1)
#define test100000() ((t1 == 0) && (t2 == 0))
#define test100100() (!t2)
#define test101000() (t1 == t2)
#define test110000() (!t1)
#define test110100() ((t1 == 0) || (t2 == 0))
#define test010111() (t1 != t2)
#define test011111() ((t1 != 0) || (t2 != 0))

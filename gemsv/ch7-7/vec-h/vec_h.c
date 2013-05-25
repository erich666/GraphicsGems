
/*
 * vec_h.c -- vec.h generator.
 *
 * Outputs a file containing vector macros for dimensions 2..maxdim,
 * where maxdim is specified on the command line.
 *
 * Usage: vec_h <maxdim>
 */

char *intro[] = {
 " vec.h --  Vector macros for %2..d-1, and %d dimensions,",
 "           for any  combination of C scalar types.",
 "",
 " Author:		Don Hatch (hatch@sgi.com)",
 " Last modified:	Fri Sep 30 03:23:02 PDT 1994",
 "",
 " General description:",
 "",
 "	The macro name describes its arguments; e.g.",
 "	    	MXS3 is \"matrix times scalar in 3 dimensions\";",
 "	    	VMV2 is \"vector minus vector in 2 dimensions\".",
 "",
 "	If the result of an operation is a scalar, then the macro \"returns\"",
 "	the value; e.g.",
 "	    	result = DOT3(v,w);",
 "	    	result = DET4(m);",
 "",
 "	If the result of an operation is a vector or matrix, then",
 "	the first argument is the destination; e.g.",
 "	    	SET2(tovec, fromvec);",
 "	    	MXM3(result, m1, m2);",
 "",
 "  WARNING: For the operations that are not done \"componentwise\"",
 "	    (e.g. vector cross products and matrix multiplies)",
 "	    the destination should not be either of the arguments,",
 "	    for obvious reasons.  For example, the following is wrong:",
 "		VXM2(v,v,m);",
 "          For such \"unsafe\" macros, there are safe versions provided,",
 "          but you have to specify a type for the temporary",
 "	    result vector or matrix.  For example, the safe versions",
 "	    of VXM2 are:",
 "              VXM2d(v,v,m)    if v's scalar type is double or float",
 "              VXM2i(v,v,m)    if v's scalar type is int or char",
 "              VXM2l(v,v,m)    if v's scalar type is long",
 "              VXM2r(v,v,m)    if v's scalar type is real",
 "              VXM2safe(type,v,v,m) for other scalar types.",
 "	    These \"safe\" macros do not evaluate to C expressions",
 "	    (so, for example, they can't be used inside the parentheses of",
 "	    a for(...)).",
 "",
 "  Specific descriptions:",
 "",
 "	The \"?\"'s in the following can be 2, 3, or 4.",
 "",
 "	SET?(to,from)			to = from",
 "	SETMAT?(to,from)		to = from",
 "	ROUNDVEC?(to,from)		to = from with entries rounded",
 "							to nearest integer",
 "	ROUNDMAT?(to,from)		to = from with entries rounded",
 "							to nearest integer",
 "	FILLVEC?(v,s)			set each entry of vector v to be s",
 "	FILLMAT?(m,s)			set each entry of matrix m to be s",
 "	ZEROVEC?(v)			v = 0",
 "	ISZEROVEC?(v)			v == 0",
 "	EQVEC?(v,w)			v == w",
 "	EQMAT?(m1,m2)			m1 == m2",
 "	ZEROMAT?(m)			m = 0",
 "	IDENTMAT?(m)			m = 1",
 "	TRANSPOSE?(to,from)		(matrix to) = (transpose of matrix from)",
 "	ADJOINT?(to,from)		(matrix to) = (adjoint of matrix from)",
 "					 i.e. its determinant times its inverse",
 "",
 "	V{P,M}V?(to,v,w)		to = v {+,-} w",
 "	M{P,M}M?(to,m1,m2)		to = m1 {+,-} m2",
 "	SX{V,M}?(to,s,from)		to = s * from",
 "	M{V,M}?(to,from)		to = -from",
 "	{V,M}{X,D}S?(to,from,s)		to = from {*,/} s",
 "	MXM?(to,m1,m2)			to = m1 * m2",
 "	VXM?(to,v,m)			(row vec to) = (row vec v) * m",
 "	MXV?(to,m,v)			(column vec to) = m * (column vec v)",
 "	LERP?(to,v0,v1,t)		to = v0 + t*(v1-v0)",
 "",
 "	DET?(m)				determinant of m",
 "	TRACE?(m)			trace (sum of diagonal entries) of m",
 "	DOT?(v,w)			dot (scalar) product of v and w",
 "	NORMSQRD?(v)			square of |v|",
 "	DISTSQRD?(v,w)			square of |v-w|",
 "",
 "	XV2(to,v)			to = v rotated by 90 degrees",
 "	VXV3(to,v1,v2)			to = cross (vector) product of v1 and v2",
 "	VXVXV4(to,v1,v2,v3)		to = 4-dimensional vector cross product",
 "					 of v1,v2,v3 (a vector orthogonal to",
 "					 v1,v2,v3 whose length equals the",
 "					 volume of the spanned parallelotope)",
 "	VXV2(v0,v1)			determinant of matrix with rows v0,v1",
 "	VXVXV3(v0,v1,v2)		determinant of matrix with rows v0,v1,v2",
 "	VXVXVXV4(v0,v1,v2,v3)		determinant of matrix with rows v0,..,v3",
 "",
 "   The following macros mix objects from different dimensions.",
 "   For example, V3XM4 would be used to apply a composite",
 "   4x4 rotation-and-translation matrix to a 3d vector.",
 "",
 "	SET3from2(to,from,pad)		(3d vec to) = (2d vec from) with pad",
 "	SET4from3(to,from,pad)		(4d vec to) = (3d vec from) with pad",
 "	SETMAT3from2(to,from,pad0,pad1) (3x3 mat to) = (2x2 mat from)",
 "					 padded with pad0 on the sides",
 "					 and pad1 in the corner",
 "	SETMAT4from3(to,from,pad0,pad1) (4x4 mat to) = (3x3 mat from)",
 "					 padded with pad0 on the sides",
 "					 and pad1 in the corner",
 "	V2XM3(to2,v2,m3)       (2d row vec to2) = (2d row vec v2) * (3x3 mat m3)",
 "	V3XM4(to3,v3,m4)       (3d row vec to3) = (3d row vec v2) * (4x4 mat m4)",
 "	M3XV2(to2,m3,v2)       (2d col vec to2) = (3x3 mat m3) * (2d col vec v2)",
 "	M4XV3(to3,m4,v3)       (3d col vec to3) = (4x4 mat m4) * (3d col vec v3)",
 "	M2XM3(to3,m2,m3)       (3x3 mat to3) = (2x2 mat m2) * (3x3 mat m3)",
 "	M3XM4(to4,m3,m4)       (4x4 mat to4) = (3x3 mat m3) * (4x4 mat m4)",
 "	M3XM2(to3,m3,m2)       (3x3 mat to3) = (3x3 mat m3) * (2x2 mat m2)",
 "	M4XM3(to4,m4,m3)       (4x4 mat to4) = (4x4 mat m4) * (3x3 mat m3)",
 "",
 "",
 "   This file is machine-generated and can be regenerated",
 "   for any number of dimensions.",
 "   The program that generated it is available upon request.",
0
};

struct {
    char *abbr, *full;
} types[] = {
    {"d", "double"},
    {"i", "int"},
    {"l", "long"},
    {"r", "real"},
};

struct definition {
    int safety;	/* 0 means already safe, i.e. returns a scalar or is done
		   "componentwise".  1 means unsafe and returns a vector.
				     2 means unsafe and returns a matrix. */
    char *name_and_args, *all_but_last, *sep, *last;
} defs[] = {
    {0, "SET%d(to,from)",     "(to)[%i] = (from)[%i]",			   ","},
    {0, "SETMAT%d(to,from)",  "SET%d((to)[%i], (from)[%i])",		   ","},
    {0, "ROUNDVEC%d(to,from)","(to)[%i] = floor((from)[%i]+.5)",	   ","},
    {0, "ROUNDMAT%d(to,from)","ROUNDVEC%d((to)[%i], (from)[%i])",	   ","},
    {0, "FILLVEC%d(v,s)",     "(v)[%i] = (s)",				   ","},
    {0, "FILLMAT%d(m,s)",     "FILLVEC%d((m)[%i], s)",			   ","},
    {0, "ZEROVEC%d(v)",       "(v)[%i] = 0",				   ","},
    {0, "ISZEROVEC%d(v)",     "(v)[%i] == 0",				 " &&"},
    {0, "EQVEC%d(v,w)",       "(v)[%i] == (w)[%i]",			 " &&"},
    {0, "EQMAT%d(m1,m2)",     "EQVEC%d((m1)[%i], (m2)[%i])",		 " &&"},
    {0, "ZEROMAT%d(m)",       "ZEROVEC%d((m)[%i])",			   ","},
    {0, "IDENTMAT%d(m)",      "ZEROVEC%d((m)[%i]), (m)[%i][%i]=1",	   ","},
    {2, "TRANSPOSE%d(to,from)", "_SETcol%d((to)[%i], from, %i)",	   ","},
    {0, "VPV%d(to,v,w)",      "(to)[%i] = (v)[%i] + (w)[%i]",		   ","},
    {0, "VMV%d(to,v,w)",      "(to)[%i] = (v)[%i] - (w)[%i]",		   ","},
    {0, "MPM%d(to,m1,m2)",    "VPV%d((to)[%i], (m1)[%i], (m2)[%i])",	   ","},
    {0, "MMM%d(to,m1,m2)",    "VMV%d((to)[%i], (m1)[%i], (m2)[%i])",	   ","},
    {0, "SXV%d(to,s,from)",   "(to)[%i] = (s) * (from)[%i]",		   ","},
    {0, "SXM%d(to,s,from)",   "SXV%d((to)[%i], s, (from)[%i])",		   ","},
    {0, "MV%d(to,from)",      "(to)[%i] = -(from)[%i]",			   ","},
    {0, "MM%d(to,from)",      "MV%d((to)[%i], (from)[%i])",		   ","},
    {0, "VXS%d(to,from,s)",   "(to)[%i] = (from)[%i] * (s)",		   ","},
    {0, "VDS%d(to,from,s)",   "(to)[%i] = (from)[%i] / (s)",		   ","},
    {0, "MXS%d(to,from,s)",   "VXS%d((to)[%i], (from)[%i], s)",		   ","},
    {0, "MDS%d(to,from,s)",   "VDS%d((to)[%i], (from)[%i], s)",		   ","},
    {2, "MXM%d(to,m1,m2)",    "VXM%d((to)[%i], (m1)[%i], m2)",		   ","},
    {1, "VXM%d(to,v,m)",      "(to)[%i] = _DOTcol%d(v, m, %i)",		   ","},
    {1, "MXV%d(to,m,v)",      "(to)[%i] = DOT%d((m)[%i], v)",		   ","},
    {0, "LERP%d(to,v0,v1,t)", "(to)[%i]=(v0)[%i]+(t)*((v1)[%i]-(v0)[%i])", ","},
    {0, "TRACE%d(m)",	      "(m)[%i][%i]",				  " +"},
    {0, "DOT%d(v,w)",	      "(v)[%i] * (w)[%i]",			  " +"},
    {0, "NORMSQRD%d(v)",      "(v)[%i] * (v)[%i]",			  " +"},
    {0, "DISTSQRD%d(v,w)",    "((v)[%i]-(w)[%i])*((v)[%i]-(w)[%i])",	  " +"},
    {0, "_DOTcol%d(v,m,j)",   "(v)[%i] * (m)[%i][j]",			  " +"},
    /* following two aren't really "safe", but shouldn't be used anyway */
    {0, "_SETcol%d(v,m,j)",   "(v)[%i] = (m)[%i][j]",			   ","},
    {0, "_MXVcol%d(to,m,M,j)","(to)[%i][j] = _DOTcol%d((m)[%i],M,j)",	   ","},
    {0, "_DET%d(v%0..d-1,i%0..d-1)","(v0)[i%i]*%-_DET%d-1(v%1..d-1,i%~i)"," +"},
    {1, "%XV%d(to,v%1..d-1)",     "(to)[%i] = %-%-_DET%d-1(v%1..d-1, %~i)",","},
    /* careful! don't use v%0..d-1 for the above or the hack utility routines
       for making "safe" macros won't be able to find the first arg. */

    /*
     * dimension-mixing macros for which d (= the dimension of the destination)
     * is the larger of the two dimensions.  This is deduced from
     * the fact that the macro name contains %d-1.
     */
    {0, "SET%dfrom%d-1(to,from,pad)", "(to)[%i] = (from)[%i]", ",",
				      "(to)[%i] = (pad)"},
    {0, "SETMAT%dfrom%d-1(to,from,pad0,pad1)",
			"SET%dfrom%d-1((to)[%i], (from)[%i], pad0)", ",",
			"FILLVEC%d-1((to)[%i], (pad0)), (to)[%i][%i] = (pad1)"},
    {2, "M%d-1XM%d(to%d,m%d-1,m%d)",
       "_MXVcol%d-1(to%d,m%d-1,m%d,%i), (to%d)[%d-1][%i]=(m%d)[%d-1][%i]", ","},
    {2, "M%dXM%d-1(to%d,m%d,m%d-1)",
  "VXM%d-1((to%d)[%i],(m%d)[%i],m%d-1), (to%d)[%i][%d-1]=(m%d)[%i][%d-1]", ","},

    /*
     * dimension-mixing macros for which d (= the dimension of the destination)
     * is the smaller of the two dimensions.  This is deduced from
     * the fact that the macro name contains %d+1.
     */
    {1, "V%dXM%d+1(to%d,v%d,m%d+1)",
	    "(to%d)[%i] = _DOTcol%d(v%d,m%d+1,%i) + (m%d+1)[%d][%i]",	","},
    {1, "M%d+1XV%d(to%d,m%d+1,v%d)",
	    "(to%d)[%i] = DOT%d((m%d+1)[%i],v%d) + (m%d+1)[%i][%d]",	","},

    /*
     * definitions that don't get vector-expanded.
     * This is deduced from the fact that the "sep" field is empty.
     */
    {0, "_DET1(v0,i0)",		"(v0)[i0]"},
    {0, "%VXV%d(v%0..d-1)",	"_DET%d(v%0..d-1,%0..d-1)"},
    {0, "DET%d(m)",		"%VXV%d((m)[%0..d-1])"},

    /*
     * New adjoint stuff.
     */
    {2, "ADJOINT%d(to,m)",	"%__ADJOINTcol%d(to,%i,m,%~i)", ","},
    {0, "_ADJOINTcol%d(to,col,m,i%1..d-1)",
			  "(to)[%i][col] = %-_DET%d-1(m[i%1..d-1], %~i)",  ","},
    {0, "__ADJOINTcol%d(to,col,m,i%1..d-1)",
			  "(to)[%i][col] = %--_DET%d-1(m[i%1..d-1], %~i)", ","},
};

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define numberof(sextoys)  (sizeof(sextoys) / sizeof(*(sextoys)))

/*
 * s is pointing to the last char of the prefix;
 * to a space, ',' or a '(', taking into account nesting parens
 */
char *getprefix(s)
char *s;
{
    static char buf[100];
    char *bufptr = buf + sizeof(buf);
    int i, nest = 0;
    *--bufptr = 0;
    for (i = 0; nest || !strchr(" ,(", s[i]); --i) {
	nest += (s[i] == ')') - (s[i] == '(');
	*--bufptr = s[i];
    }
    return bufptr;
}

/*
 * s is pointing to the beginning of the suffix;
 * get the suffix up to but not including a space, ',' or ')'.
 */
char *getsuffix(s)
char *s;
{
    static char buf[100];
    int i;
    for (i = 0; !strchr(" ,)", s[i]); ++i)
	buf[i] = s[i];
    buf[i] = 0;
    return buf;
}

/*
 * Print the string s with the following substitutions:
 * 	%d turns into d
 *	%i turns into i
 *	%- turns into "-" if i is odd, " " otherwise
 *	%-- turns into "-" if i is even, " " otherwise
 *	%_ turns into "_" if i is odd, " " otherwise
 *	%-%- turns into "-" if d+i is even, " " otherwise (sorry this is stupid)
 *	%i+1 turns into (i+1)%d etc.
 *	%d-1 turns into d-1 etc.
 *	%% turns into %
 *	blah%0..d-1bleh turns into blah0bleh,blah1bleh,...,blahd-1bleh
 *	blah%~ibleh  is same as blah%0..d-1bleh but excluding i
 *	%XV turns into VXVX...XV with d-1 V's (or XV if d==2)
 *	%VXV turns into VXVX...XV with d V's 
 */
void printformatted_to(s, tochar, d, i)
char *s;
int tochar, d, i;
{
    char *prefix, *suffix;
    int num, j, lastj;

    for (; *s != tochar; s++) {
	if (*s == '%') {
	    ++s;

	    prefix = NULL;
	    if (isdigit(*s) && !strncmp(s+1, "..d-1", 5)) {
		prefix = getprefix(s-2);	/* one char before the % */
		suffix = getsuffix(s+6);
	    } else if (!strncmp(s, "~i", 2)) {
		prefix = getprefix(s-2);	/* one char before the % */
		suffix = getsuffix(s+2);
	    }
	    if (prefix) {
		lastj = d-1 - (*s == '~');
		for (j = atoi(s); j <= lastj; ++j) {
		    printf("%d", j + (*s == '~' && j >= i));
		    printf("%s", suffix);
		    if (j < lastj)
			printf(",%s", prefix);
		}
		s += (*s == '~' ? 2 : 6) + strlen(suffix) - 1;
	    } else {
		switch(*s) {
		    case '-': case '_':
			if (!strncmp(s, "-%-", 3)) {
			    printf("%c", (i+d)%2==0 ? *s : ' ');
			    s += 2;
			} else if (!strncmp(s, "--", 2)) {
			    printf("%c", i%2==0 ? *s : ' ');
			    s++;
			} else {
			    printf("%c", i%2==1 ? *s : ' ');
			}
			break;
		    case 'i': case 'd':
			num = (*s == 'i' ? i : d);
			if (strchr("+-", s[1])) {
			    num += (s[1] == '-' ? -1 : 1) * atoi(s+2);
			    if (*s == 'i')
				num = (num+d) % d;
			    s += 2;
			    /* s is now pointing to the first digit */
			    while (isdigit(s[1]))
				s++;
			    /* s is now pointing to the last digit */
			}
			printf("%d", num);
			break;
		    case 'X': case 'V':
			if (*s == 'X' && d == 2)
			    printf("X");
			for (j = 0; j < d-2; ++j)
			    printf("VX");
			if (*s == 'V')
			    printf("V");
			break;
		    case '%':
			    printf("%%");
			break;
		    default:
			assert(0);
		}
	    }
	} else
	    putchar(*s);
    }
}

void printformatted(s, d, i)
char *s;
int d, i;
{
    printformatted_to(s, '\0', d, i);
}

void define(d, name_and_args, all_but_last, sep, last)
int d;
char *name_and_args, *all_but_last, *sep, *last;
{
    int i;

    printf("#define ");
    printformatted(name_and_args, d, 0);
    printf("\t\\\n\t\t");
    printf("(");

	if (sep)
	    for (i = 0; i < d-1; ++i) {		/* loop for all but last */
		printformatted(all_but_last, d, i);
		printformatted(sep, d, i);
		printf(" \\\n\t\t ");
	    }
	printformatted(last, d, i);

    printf(")\n");
}

int is_substring(a,b)
char *a, *b;
{
    for (; *b; b++)
	if (strncmp(a, b, strlen(a)) == 0)
	    return 1;
    return 0;
}


/*
 * Hack utility routines...
 */
void print_name_and_args_with_stuff_inserted(name_and_arg, suff, arg0, d)
char *name_and_arg, *suff, *arg0;
int d;
{
    printformatted_to(name_and_arg, '(', d, 0);
    printformatted(suff, d, 0);
    printf("(");
    if (arg0) {
	printformatted(arg0, d, 0);
	printf(",");
    }
    printformatted(strchr(name_and_arg, '(' /*)*/ ) + 1, d, 0);
}

void print_name_and_args_with_first_arg_changed(name_and_arg, arg0, d)
char *name_and_arg, *arg0;
int d;
{
    printformatted_to(name_and_arg, '(', d, 0);
    printf("(");
    printformatted(arg0, d, 0);
    printformatted(strchr(name_and_arg, ','), d, 0);
}

void print_first_arg(name_and_arg, d)
char *name_and_arg;
int d;
{
    printformatted_to(strchr(name_and_arg, '(' /*)*/ ) + 1, ',', d, 0);
}


#define MINDIM 2

main(argc, argv)
char **argv;
{
    int i, d, maxdim;

    if (argc != 2 || ! (maxdim = atoi(argv[argc-1])))
	fprintf(stderr, "Usage: %s [<maxdim>]\n", argv[0]), exit(1);


    printf("/*\n");
    for (i  = 0; intro[i]; ++i) {
	printf(" *");
	printformatted(intro[i], maxdim, 0);
	printf("\n");
    }
    printf(" */\n\n");

    printf("#ifndef VEC_H\n");
    printf("#define VEC_H %d\n", maxdim);
    printf("#include <math.h>	/* for definition of floor() */\n");

    for (d = MINDIM; d <= maxdim; ++d)
	for (i = 0; i < numberof(defs); ++i) {
	    if (is_substring("%d-1", defs[i].name_and_args) && d-1 < MINDIM
	     || is_substring("%d+1", defs[i].name_and_args) && d+1 > maxdim)
		continue;
	    if (!is_substring("%d", defs[i].name_and_args) && d != MINDIM)
		continue;	/* don't redefine if it's the same */
	    define(d, defs[i].name_and_args,
		      defs[i].all_but_last,
		      defs[i].sep,
		      defs[i].last ? defs[i].last : defs[i].all_but_last);
	}
    for (d = MINDIM; d <= maxdim; ++d)
	for (i = 0; i < numberof(defs); ++i) {
	    if (is_substring("%d-1", defs[i].name_and_args) && d-1 < MINDIM
	     || is_substring("%d+1", defs[i].name_and_args) && d+1 > maxdim)
		continue;
	    if (!is_substring("%d", defs[i].name_and_args) && d != MINDIM)
		continue;	/* don't redefine if it's the same */
	    if (defs[i].safety > 0) {
		int t;
		printf("#define ");
		print_name_and_args_with_stuff_inserted(defs[i].name_and_args,
							    "safe", "type", d);
		printf(" \\\n\t\t");
		printformatted("do {type _vec_h_temp_[%d]", d, 0);
		if (defs[i].safety > 1)
		    printformatted("[%d]", d, 0);
		printf("; \\\n\t\t    ");

		print_name_and_args_with_first_arg_changed(
			defs[i].name_and_args, "_vec_h_temp_", d);
		printf("; \\\n\t\t    ");

		if (defs[i].safety > 1)
		    printformatted("SETMAT%d(", d, 0);
		else
		    printformatted("SET%d(", d, 0);
		print_first_arg(defs[i].name_and_args, d);
		printf(", _vec_h_temp_); \\\n\t\t} while (0)\n");

		for (t = 0; t < numberof(types); ++t) {
		    printf("#define ");
		    print_name_and_args_with_stuff_inserted(
			defs[i].name_and_args, types[t].abbr, (char *)NULL, d);
		    printf(" ");
		    print_name_and_args_with_stuff_inserted(
			defs[i].name_and_args, "safe", types[t].full, d);
		    printf("\n");
		}
	    }
	}

    printf("#endif /* VEC_H */\n");

    return 0;
}

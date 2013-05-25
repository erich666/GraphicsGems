/*
Proper Treatment of Pixels as Integers
by Alan Paeth
from "Graphics Gems", Academic Press, 1990
*/

#define Min     code[2]
#define Med     code[1]
#define Max     code[0]
#define NCODE   3

#include <string.h>	/* or <strings.h> */

/*
 * A call to getplanes of the form:
 * getplanes(&red, &green, &blue, 256, "grb");
 *
 * fills the first three integer pointers with (near) identical
 * values which maximize red*green*blue <= 256. The final parameter
 * string defines tie-break order, here green>=red>=blue (the usual
 * default). The present code procedure calls "err(string, arg)"
 * given bad parameters; it is a simple task to rewrite the code as
 * a function which returns a success/failure code(s), as needed.
 *
 * In the example given above the code fills in the values
 * red = 6, green = 7, blue = 6.
 */

getplanes(r, g, b, n, bias)
    int *r, *g, *b;
    char *bias;
    {
    int i, code[NCODE];
    if(strlen(bias) != NCODE )
        err("bias string \"%s\" wrong length",bias);
    Min = Med = Max = 0;
    *r = *g = *b = 0;
    while(Min*Min*Min <= n) Min++;
    Min--;
    while(Med*Med*Min <= n) Med++;
    Med--;
    Max = n/(Min*Med);
    for( i = 0; i < NCODE; i++ )
	   {
        switch(bias[i])
            {
            case 'r': case 'R': *r = code[i]; break;
            case 'g': case 'G': *g = code[i]; break;
            case 'b': case 'B': *b = code[i]; break;
            default: err("bad bias character: \'%c\'",bias[i]); break;
            }
        }
    if (!(*r && *g && *b)) err("bias string \"%s\" deficient", bias);
    }
    

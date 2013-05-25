#include <stdlib.h>
#include <stdio.h>
#include "chainCode.h"



/*************************************************************/
/*                                                           */
/* Class constructor.                                        */
/*                                                           */
/*************************************************************/

chainCode::chainCode()
{
code = malloc(DEFAULT_CODE_LENGTH * sizeof(char));
code[0] = '\0';
length = DEFAULT_CODE_LENGTH;
}

/*************************************************************/
/*                                                           */
/* Class destructor.                                         */
/*                                                           */
/*************************************************************/

chainCode::~chainCode()
{
free(code);
}


/*************************************************************/
/*                                                           */
/* This method appends a new code to the chain. If there     */
/* is not enough memory left, the function doubles the size  */
/* of the chain code.                                        */
/* It receives as a parameter the new code to be added (c).  */
/*                                                           */
/*************************************************************/

void chainCode::add(char c)
{
int l = strlen(code);

if (l >= length-1){
    length *= 2;
    code = realloc(code, length);
    }
code[l] = c;
code[l+1] = '\0';
}


/*****************************************************************/
/*                                                               */
/* This method post-processes a 4x chain code to generate a 1x   */
/* chain code. A pointer to the 1x code is returned. The method  */
/* uses the 4 following rules:                                   */
/* CCCC ->  C :  reduce to one copy                              */
/*  CCC -> {} :  eliminate                                       */
/*   CC -> CC :  (ignored)                                       */
/*    C ->  C :  identity                                        */
/*                                                               */
/*****************************************************************/

chainCode* chainCode::postProcess()
{
int    i = 0, j;
chainCode *filtCode;
int    trueLength = strlen(code);

filtCode = new chainCode();
while (i<trueLength){
    if (i+SCALE-1 < trueLength){
        for (j=0; j<SCALE-1; j++)
            if (code[i+j] != code[i+j+1])
                break;
        if (j == SCALE-1){
            filtCode->add(code[i]);
            i += SCALE;
            continue;
        }
    }

    if (i+SCALE-2 < trueLength){
        for (j=0; j<SCALE-2; j++)
            if (code[i+j] != code[i+j+1])
                break;
        if (j == SCALE-2){
            i += SCALE-1;
            continue;
            }
        }
    filtCode->add(code[i]);
    i++;
}
return filtCode;
}

/*****************************************************************/
/*                                                               */
/* A utility method to display the chain code                    */
/*                                                               */
/*****************************************************************/

void chainCode::printSelf()
{
printf("\n%s", code);
}

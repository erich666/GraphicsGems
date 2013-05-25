/* TEST FILE vectorize NOT FOR BOOK */

#include <stdio.h>
#include <stdlib.h>
#include "chainCode.h"
#include "pt2.h"

#define MAXPATHLEN 1024




extern chainCode* encode(pt2 *size, char *bitmap);


char *mes1[]={"VECTORIZE",
              "by Jean-Francois Doue",
              " ",
              "This program transforms a bitmap image into a chain code",
              "Bitmap images should be encoded in the following format:",
              " ",
              "x_size y_size",
              "00001111000",
              "00001111000",
              "00001001000",
              "...........",
              "00001111000",
              " ",
              "To convert the image, simply type",
              " ",
              "vectorized fileName",
              " ",
              "The encoded file will be saved under fileName.vec",
              "\n",
              0};

char *mes2[]={"Error !",
              "Your file cannot be opened",
              0};



/*************************************************************/
/*                                                           */
/* A simple function to display messages on the screen       */
/*                                                           */
/*************************************************************/

void printMessage(char** mes)
{
int i;
if (!mes)
    return;
while(mes[i]){
    printf("\n%s",mes[i]);
    i++;
}
}

/*************************************************************/
/*                                                           */
/* This function transforms a text-file containing a bitmap  */
/* image into another text file containing the vectorized    */
/* image.                                                    */
/*                                                           */
/*************************************************************/

main(int argc, char** argv)
{
pt2     size;
int     i, fileN = 0;
unsigned char   c;
char        output_name[MAXPATHLEN],
            *bitmap;
FILE        *input,
            *output;
chainCode   *code;

/* make sure the user uses the right syntax */
if (argc == 1){
    printMessage(mes1);
    exit(0);
}

/* for all the specified files... */
while (fileN < argc -1){
    /* open the data file */
    fileN++;
    if ((input = fopen(argv[fileN], "r")) == NULL){
        printMessage(mes2);
        exit(0);
    }

    /* create the .vec file */
    sprintf(output_name,"%s.vec", argv[fileN]);
        if ((output = fopen(output_name, "w")) == NULL){
        printMessage(mes2);
        exit(0);
    }

    /* read x_size and y_size of the bitmap image*/
    fscanf(input,"%d%d",&size.x, &size.y);
    printf("\nEncoding file:%s (x=%d, y=%d)", argv[fileN], size.x, size.y);

    /* read the data from the bitmap image*/
    i = 0;
    bitmap = malloc(size.x * size.y * sizeof(char));
    while (!feof(input)){
        fscanf(input,"%c", &c);
        if (c == '0' || c == '1'){
            bitmap[i] = c;
            i++;
            }
    }

    /* encode */
    code = encode(&size, bitmap);
    printf("\nThe encoded vector is:");
    code->printSelf();
    fprintf(output,"%s", code->code);
    fclose(output);
    fclose(input);
    }

printf("\n");
return 0;
}


























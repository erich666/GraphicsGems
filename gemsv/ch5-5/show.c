#include <malloc.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <starbase.c.h>

typedef unsigned char icolor[3];

char command[100]="/usr/bin/X11/xwcreate -depth 24 -geometry ";
char screen[100]="/dev/screen/";
int watchmode=0;

main(ac, av)
int ac;
char *av[];
{
    FILE *f;
    int width, height, w, h;
    int fildes;
    icolor *row;
    unsigned char *r, *g, *b;
    register int i, j;
    register long t;
        
    while(av[1]&&*av[1]=='-') {
        switch(av[1][1]) {
          case 'w':
            watchmode=1;
            break;
        }
        ac--;
        av++;
    }
    if(ac!=2) {
        printf("usage: show file\n");
        exit(1);
    }
    if(!(f=fopen(av[1],"rb"))) {
        printf("failed to open file %s\n", av[1]);
        exit(1);
    }
    fread(&width, sizeof(int), 1, f);
    fread(&height, sizeof(int), 1, f);
    row=(icolor*)malloc(width*sizeof(icolor));
    r=(unsigned char*)malloc(width*sizeof(unsigned char));
    g=(unsigned char*)malloc(width*sizeof(unsigned char));
    b=(unsigned char*)malloc(width*sizeof(unsigned char));
    if(!row||!r||!g||!b) {
        printf("not enough memory\n");
        exit(1);
    }
    if(!system(NULL)) {
        printf("command processor not available\n");
        exit(1);
    }
    sprintf(&command[strlen(command)], "%dx%d+10+10 %s", width, height, av[1]);
    system(command);
    if(errno) {
        printf("failed to create window\n");
        exit(1);
    }
    strcat(screen, av[1]);
    if((fildes=gopen(screen,OUTDEV,0,INIT))<0) {
        printf("failed to open screen device\n");
        exit(1);
    }
    shade_mode(fildes, CMAP_FULL|INIT, 0);
    for(;;) {
        for(i=0; i<height; i++) {
            if(fread(row, sizeof(icolor), width, f)!=width)
                break;
            for(j=0; j<width; j++) {
                r[j]=row[j][0];
                g[j]=row[j][1];
                b[j]=row[j][2];
            }
            bank_switch(fildes, 2, 0);
            dcblock_write(fildes, 0, i, width, 1, r, 1);          
            bank_switch(fildes, 1, 0);
            dcblock_write(fildes, 0, i, width, 1, g, 1);          
            bank_switch(fildes, 0, 0);
            dcblock_write(fildes, 0, i, width, 1, b, 1);          
        }
        fclose(f);
        if(!watchmode)
            break;
        for(j=0; j<width; j++)
            r[j]=0;
        for(; i<height; i++) {
            bank_switch(fildes, 2, 0);
            dcblock_write(fildes, 0, i, width, 1, r, 1);          
            bank_switch(fildes, 1, 0);
            dcblock_write(fildes, 0, i, width, 1, r, 1);
            bank_switch(fildes, 0, 0);
            dcblock_write(fildes, 0, i, width, 1, r, 1);
        }
        for(t=clock(); (clock()-t)/CLOCKS_PER_SEC<2L;);
        if(!(f=fopen(av[1],"rb"))) {
            printf("failed to open file %s\n", av[1]);
            exit(1);
        }
        fread(&w, sizeof(int), 1, f);
        fread(&h, sizeof(int), 1, f);
        if(!(w==width&&h==height)) {
            printf("picture size changed\n");
            exit(1);
        }
    }
    exit(0);
}   

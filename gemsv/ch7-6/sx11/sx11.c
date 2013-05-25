/*
 * I don't wanna write the header so only ...
 *
 * (c) 1992, Raul Rivero
 */

#include <lug.h>
#include <lugfnts.h>

/* extern int LUGverbose; */
char *MY_NAME;

main(argc, argv)
int argc;
char **argv;
{
  register int i;

  bitmap_hdr in;
  ifunptr read_file;


  MY_NAME = argv[0];

  /*
   * Get options ( some day I'll build a procedure ).
   */
  if ( argc > 1 ) {             /* else core on SGI */
    while ( argv[1][0] == '-' ) {
      for ( i = 1; argv[1][i]; i++ ) {
        switch ( argv[1][i] ) {
                case 'v':
/*                        LUGverbose++; */
                        break;
                case '!':
                        print_copyright();
                        break;
                default :
                        usage();
                        break;
        }
      }
      argv++;
      argc--;
    }
  }

  if ( argc < 2 ) {
    usage();
  }


  if ( argc > 1 ) {
    /*
     * We have arguments, so read the files ...
     */
    for ( i = 1; i < argc; i++ ) {
      /* Read the file */
      read_file = get_readlug_function( argv[i] );
      read_file( argv[i], &in );
      /* Release the child and show the image */
      show_bitmap_x11( argv[i], &in );
      /* Ok, the child is displaying the image, so the parent continues */
      freebitmap( &in );
    }
  }
  exit( 0 );
}

usage()
{
  char *msg = "\n\
%s: Usage: %s [-v!] <input_file> [<input_file>]\n\n\
Flags:\n\
\t-v: verbose\n\
\t-!: hey!, what about this program ?!\n\n\
The file type is got using its suffix:\n\n\
\t* .gif\t\t\t* .hf\t\t\t* .pbm/.pgm/.ppm\n\
\t* .pcx\t\t\t* .raw\t\t\t* .rgb\n\
\t* .rla\t\t\t* .rle\t\t\t* .sgi\n\
\t* .sun\t\t\t* .tga\t\t\t* .tif/.tiff\n\
\t* .ps\t\t\t* .jpeg/.jpg\t\t* .pix (** default **)\n\n\
The Alias 'pix' format will be used by default.\n\n\
If required, the quantization method is the default process to reduce\n\
the number of colors.\n\n";

  fprintf( stderr, msg, MY_NAME, MY_NAME );
  exit( 1 );
}

print_copyright()
{
  char *msg = "\
sx11 ( v.1.0 ) - show several image file formats\n\n\
This program - (c) 1992, Raul Rivero\n\
LUG  library - (c) 1992, Raul Rivero && Math Dept. ( U. of Oviedo )\n\n\
This software is free and you can get a full copy of original LUG library\n\
via E-mail to rivero@pinon.ccu.uniovi.es or via anonymous ftp to \n\
telva.ccu.uniovi.es ( 156.35.31.31, /uniovi/mathdept/src ).\n\n\
The LUG library includes support for several file formats, viewers on\n\
different architectures and digital image processing.\n\n\
Supported input formats:\n\n\
\t* Pix ( Alias )  *** default ***\n\
\t* TIFF ( needs Sam Leffler's TIFF library )\n\
\t* RLE ( needs Utah Raster Toolkit library )\n\
\t* RLA ( Wavefront )\n\
\t* SGI ( internal Silicon Graphics file format )\n\
\t* Targa ( Truevision )\n\
\t* GIF ( Compuserve )\n\
\t* PCX ( ZSoft )\n\
\t* PBM/PGM/PPM\n\
\t* Postscript\n\
\t* JPEG ( needs Thomas G. Lane's JPEG library )\n";

  fputs( msg, stderr );
  exit( 1 );
}

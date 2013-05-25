#include	<string.h>
/*******************************************************************************
The following code implements motion blur on a HP9000 Series 800 computer, 
including computing on fields and compensation for lack of subpixel positioning.
The anti-flicker filter discussed in Section 4 is not included.
In the interests of brevity and clarity, the code presented here is
not the most efficient possible, but note:

    1. Rather than initializing the image accumulation buffer to 0 before each
       frame in mb_frame(), the buffer can be initialized to the value of the 
       first subframe.

    2. If the number of subframes per field is a power of two, then shifting the
       accumulated result is faster than dividing.

    3. If more processing must be done to the image in output(), such as 
       conversion from RGB to YUV, it is most efficient to do the processing
       within the mb_average() routine, rather than to make another pass over 
       the same memory.  

    4. Results of the averaging are stored in place in a single accumulation
       buffer.  Depending on the additional processing that must be done by the
       output() procedure, it may be advantageous to store the final result in a
       different buffer instead, such as an array of unsigned char rather than
       unsigned short.
*******************************************************************************/

#define FB_STRIDE 2048    /* framebuffer stride -- pixels in a scan line */
#define VID_XRES   640    /* video x resolution */
#define VID_YRES   486    /* video y resolution */

/* framebuffer address -- pixels stored in unsigned ints */
/* we assume frame is initialized to point to the memory-mapped framebuffer */
static unsigned int *frame;                     
#define BB(v)  ( ((v) <<24)>>24 )    /* extract blue byte from fb pixel */
#define BG(v)  ( ((v) <<16)>>24 )    /* extract green byte from fb pixel */
#define BR(v)  ( ((v) << 8)>>24 )    /* extract red byte from fb pixel */

/* accumulation buffer -- pixels stored as r,g,b, r,g,b,... */
static unsigned short acc[VID_XRES*VID_YRES*3]; 

void mb_average(), mb_accumulate();

/*******************************************************************************
Compute and output a motion blurred frame.

External Procedures:

    void shift_image(int x,int y) - adjusts the viewing transformation to
                                    render the image shifted by an integral
                                    number of pixels in x and y 

    void draw(double t)           - renders a subframe at time t

    void output(unsigned short *acc,int w,int h) - outputs an image (e.g. 
                                                   records a frame of the 
                                                   animation onto the output
                                                   device)

Entry:
    t0      - frame start time
    delta   - shutter open time
    n       - number of subframes
*******************************************************************************/
void mb_frame(t0,delta,n)
double t0;
double delta;
int n;
{
    int i;
    int nfield = n/2; /* number of subframes per field */
    int field = 1;    /* current field; first 1, then 0 */

    /* pixel offsets for 16 subframes/field, gives triangle filter in x and y */
    static int x_off[] = {0,1,0,1, -1,0,-1,0,  0, 1, 0,1, -1, 0,-1,0};
    static int y_off[] = {0,1,1,0,  0,1, 1,0,  0,-1,-1,0,  0,-1,-1,0};
#define NOFFS ( sizeof(x_off)/sizeof(x_off[0]) )

    /* clear accumulation buffer */
    memset((void *)acc,0,sizeof(acc));

    for (i = 0; i < n; i++) {
        shift_image(x_off[(i%nfield)%NOFFS],y_off[(i%nfield)%NOFFS]);
        draw(t0 + i*delta/(n-1));
        if (i == nfield-1 || i == n-1) {
            mb_average(nfield,field);
            field = !field;
        } else {  
            mb_accumulate(field);
        }
    }

    /* output final image */
    output(acc,VID_XRES,VID_YRES);
}

/*******************************************************************************
Add another subframe to the accumulation buffer, computing on fields.  

Entry:
    field   - field number (0 or 1)
*******************************************************************************/
void mb_accumulate(field)
int field;
{
    register unsigned int *iptr;
    register unsigned short *optr;
    register int i,j;

    /* shift input and output pointers by one scan line if field 1 */
    if (field) {
        iptr = frame + FB_STRIDE;
        optr = acc + 3*VID_XRES;
    } else {
        iptr = frame;
        optr = acc;
    }

    /* add in field to accumulation buffer */
    for (i = 0; i < VID_YRES; i+=2) {
        for (j = 0; j < VID_XRES; j+=4) {
            /* process 4 pixels, skipping every other one */
            register unsigned int v1 = iptr[0];
            register unsigned int v3 = iptr[2];
            register unsigned int v5 = iptr[4];
            register unsigned int v7 = iptr[6];

            optr[0] += BR(v1); optr[1]  += BG(v1); optr[2]  += BB(v1);
            optr[3] += BR(v3); optr[4]  += BG(v3); optr[5]  += BB(v3);
            optr[6] += BR(v5); optr[7]  += BG(v5); optr[8]  += BB(v5);
            optr[9] += BR(v7); optr[10] += BG(v7); optr[11] += BB(v7);

            iptr += 8;
            optr += 12;
        }
        iptr += 4*FB_STRIDE-VID_XRES*2; /* process every fourth scan line */
        optr += 3*VID_XRES;             /* skip to next field scan line */
    }
}

/*******************************************************************************
Add another subframe and divide by the number of field subframes.
The result is placed back into the accumulation buffer.

Entry:
    n       - number of subframes in a field
    field   - field number (0 or 1)
*******************************************************************************/
void mb_average(n,field)
int n;
int field;
{
    register unsigned int *iptr;
    register unsigned short *optr;
    register int i,j;

    /* shift input and output pointers by one scan line if field 1 */
    if (field) {
        iptr = frame + FB_STRIDE;
        optr = acc + 3*VID_XRES;
    } else {
        iptr = frame;
        optr = acc;
    }

    /* add in field to accumulation buffer and divide */
    for (i = 0; i < VID_YRES; i+=2) {
        for (j = 0; j < VID_XRES; j+=4) {
            /* process 4 pixels, skipping every other one */
            register unsigned int v1 = iptr[0];
            register unsigned int v3 = iptr[2];
            register unsigned int v5 = iptr[4];
            register unsigned int v7 = iptr[6];

            optr[0] = (optr[0]+BR(v1))/n; 
            optr[1] = (optr[1]+BG(v1))/n; 
            optr[2] = (optr[2]+BB(v1))/n;

            optr[3] = (optr[3]+BR(v3))/n; 
            optr[4] = (optr[4]+BG(v3))/n; 
            optr[5] = (optr[5]+BB(v3))/n;

            optr[6] = (optr[6]+BR(v5))/n; 
            optr[7] = (optr[7]+BG(v5))/n; 
            optr[8] = (optr[8]+BB(v5))/n;

            optr[9] = (optr[9]+BR(v7))/n; 
            optr[10] = (optr[10]+BG(v7))/n; 
            optr[11] = (optr[11]+BB(v7))/n;

            iptr += 8;
            optr += 12;
        }
        iptr += 4*FB_STRIDE-VID_XRES*2; /* process every fourth scan line */
        optr += 3*VID_XRES;             /* skip to next field scan line */
    }
}


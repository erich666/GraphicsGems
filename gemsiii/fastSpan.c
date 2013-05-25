/***********
 * movespan()
 *
 *   Move a span of common values into frame buffer memory.  Assume that
 *   the frame buffer is organized as one byte per pixel; consecutive pixels
 *   occupy consecutive bytes.  We assume that the longest span has 16 pixels; 
 *   longer spans may be handled by coding additional cases.  See
 *   movelongspan() for an alternative.
 *
 *   PARAMETERS:
 *      here : pointer to the first pixel to be set
 *      val  : the value (e.g intensity) to be placed at each pixel
 *      n    : the number of pixels to be illuminated
 *
 *   AUTHOR: Thom Grace, CS Dept, Illinois Institute of Technology,
 *           Chicago, IL 60616   (grace@iitmax.iit.edu)
 ***********/

movespan(unsigned char *here, unsigned char val, int n)  {
      /*This is simple: fall into the proper place in the switch*/
   switch(n)   {
      case 16: *(here++) = val;
      case 15: *(here++) = val;
      case 14: *(here++) = val;
      case 13: *(here++) = val;
      case 12: *(here++) = val;
      case 11: *(here++) = val;
      case 10: *(here++) = val;
      case  9: *(here++) = val;
      case  8: *(here++) = val;
      case  7: *(here++) = val;
      case  6: *(here++) = val;
      case  5: *(here++) = val;
      case  4: *(here++) = val;
      case  3: *(here++) = val;
      case  2: *(here++) = val;
      case  1: *(here++) = val;
      case  0: return;
      }
}   /*end of movespan()*/


/***********
 * movelongspan()
 *
 *   This will move an arbitrarily long sequence of constant values into
 *   frame buffer memory.  Loops are partially unrolled, requiring about
 *   1/16 of the overhead of movespan().  Note that the number of switch
 *   cases should be one less than the number of statements in the loop
 *   which should be the same as the constant in the loop condition.
 *
 *   PARAMETERS:  here : pointer to first pixel to be set;
 *                val  : value to be placed at each pixel;
 *                n    : number of pixels to be illuminated.
 *
 *   AUTHOR: Thom Grace, CS Dept, Illinois Institute of Technology,
 *           Chicago, IL 60616   (grace@iitmax.iit.edu)
 ***********/
movelongspan(unsigned char *here, unsigned char val, int n)  {
      /*Illuminate 16 pixels at a time, as long as we are able*/
   while (n >= 16) {
      *(here++)=val;  *(here++)=val;  *(here++)=val;  *(here++)=val;
      *(here++)=val;  *(here++)=val;  *(here++)=val;  *(here++)=val;
      *(here++)=val;  *(here++)=val;  *(here++)=val;  *(here++)=val;
      *(here++)=val;  *(here++)=val;  *(here++)=val;  *(here++)=val;
      n -= 16;   /*count off those pixels*/
      }
      /*Now fall into the proper place in the switch*/
   switch(n)   {
      case 16: *(here++) = val;
      case 15: *(here++) = val;
      case 14: *(here++) = val;
      case 13: *(here++) = val;
      case 12: *(here++) = val;
      case 11: *(here++) = val;
      case 10: *(here++) = val;
      case  9: *(here++) = val;
      case  8: *(here++) = val;
      case  7: *(here++) = val;
      case  6: *(here++) = val;
      case  5: *(here++) = val;
      case  4: *(here++) = val;
      case  3: *(here++) = val;
      case  2: *(here++) = val;
      case  1: *(here++) = val;
      case  0: return;
      }
}   /*end of movespan()*/


/***********
 * shadespan()
 *
 *   This will move a span of shade values into frame buffer memory.
 *   The shades are computed by the addition of a shading constant.  This
 *   assumes that the frame buffer is organized as one byte per pixel and
 *   that consecutive pixels occupy consecutive bytes.  This also assumes
 *   that the longest span is of length 16; see previous code for ideas on
 *   how to alleviate this limitation.
 *
 *   PARAMETERS:
 *      here : pointer to the first pixel to be set
 *      val  : the value (e.g intensity) to be placed at the first
 *             pixel MINUS the parameter disp (see below)
 *      n    : the number of pixels to be illuminated
 *      disp : the shading constant
 *
 *   AUTHOR: Thom Grace, CS Dept, Illinois Institute Of Technology,
 *           Chicago, IL  60616   (grace@iitmax.iit.edu)
 ***********/

shadespan(unsigned char *here, unsigned char val, int n, int disp)
   {
      /*Each pixel is filled with the incremented intensity*/
   switch(n)   {
      case 16: *(here++) = (val += disp);
      case 15: *(here++) = (val += disp);
      case 14: *(here++) = (val += disp);
      case 13: *(here++) = (val += disp);
      case 12: *(here++) = (val += disp);
      case 11: *(here++) = (val += disp);
      case 10: *(here++) = (val += disp);
      case  9: *(here++) = (val += disp);
      case  8: *(here++) = (val += disp);
      case  7: *(here++) = (val += disp);
      case  6: *(here++) = (val += disp);
      case  5: *(here++) = (val += disp);
      case  4: *(here++) = (val += disp);
      case  3: *(here++) = (val += disp);
      case  2: *(here++) = (val += disp);
      case  1: *(here++) = (val += disp);
      case  0: return;
      }
}   /*end of movespan()*/

/* 
A Fast 2D Point-On-Line Test
by Alan Paeth
from "Graphics Gems", Academic Press, 1990
*/

#include "GraphicsGems.h"

int PntOnLine(px,py,qx,qy,tx,ty)
   int px, py, qx, qy, tx, ty;
   {
/*
 * given a line through P:(px,py) Q:(qx,qy) and T:(tx,ty)
 * return 0 if T is not on the line through      <--P--Q-->
 *        1 if T is on the open ray ending at P: <--P
 *        2 if T is on the closed interior along:   P--Q
 *        3 if T is on the open ray beginning at Q:    Q-->
 *
 * Example: consider the line P = (3,2), Q = (17,7). A plot
 * of the test points T(x,y) (with 0 mapped onto '.') yields:
 *
 *     8| . . . . . . . . . . . . . . . . . 3 3
 *  Y  7| . . . . . . . . . . . . . . 2 2 Q 3 3    Q = 2
 *     6| . . . . . . . . . . . 2 2 2 2 2 . . .
 *  a  5| . . . . . . . . 2 2 2 2 2 2 . . . . .
 *  x  4| . . . . . 2 2 2 2 2 2 . . . . . . . .
 *  i  3| . . . 2 2 2 2 2 . . . . . . . . . . .
 *  s  2| 1 1 P 2 2 . . . . . . . . . . . . . .    P = 2
 *     1| 1 1 . . . . . . . . . . . . . . . . .
 *      +--------------------------------------
 *        1 2 3 4 5 X-axis 10        15      19
 *
 * Point-Line distance is normalized with the Infinity Norm
 * avoiding square-root code and tightening the test vs the
 * Manhattan Norm. All math is done on the field of integers.
 * The latter replaces the initial ">= MAX(...)" test with
 * "> (ABS(qx-px) + ABS(qy-py))" loosening both inequality
 * and norm, yielding a broader target line for selection.
 * The tightest test is employed here for best discrimination
 * in merging collinear (to grid coordinates) vertex chains
 * into a larger, spanning vectors within the Lemming editor.
 */

	/* addenda: this first set of tests has been added to detect
	 * the case where the line is of zero length. Remove this if
	 * such a case is impossible.
	 */
	if ((px == qx) && (py == qy))
		if ((tx == px) && (ty == py)) return 2;
		else return 0;


    if ( ABS((qy-py)*(tx-px)-(ty-py)*(qx-px)) >=
        (MAX(ABS(qx-px), ABS(qy-py)))) return(0);
    if (((qx<px)&&(px<tx)) || ((qy<py)&&(py<ty))) return(1);
    if (((tx<px)&&(px<qx)) || ((ty<py)&&(py<qy))) return(1);
    if (((px<qx)&&(qx<tx)) || ((py<qy)&&(qy<ty))) return(3);
    if (((tx<qx)&&(qx<px)) || ((ty<qy)&&(qy<py))) return(3);
    return(2);
    }

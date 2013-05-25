/*
 * C code from the article
 * "Voxel Traversal along a 3D Line"
 * by Daniel Cohen, danny@bengus.bgu.ac.il
 * in "Graphics Gems IV", Academic Press, 1994
 */

/* The following C subroutine visits all voxels along the line
segment from (x, y, z) and (x + dx, y + dy, z + dz) */

Line ( x, y, z, dx, dy, dz )
int x, y, z, dx, dy, dz;
{
    int n, sx, sy, sz, exy, exz, ezy, ax, ay, az, bx, by, bz;

    sx = sgn(dx);  sy = sgn(dy);  sz = sgn(dz);
    ax = abs(dx);  ay = abs(dy);  az = abs(dz);
    bx = 2*ax;	   by = 2*ay;	  bz = 2*az;
    exy = ay-ax;   exz = az-ax;	  ezy = ay-az;
    n = ax+ay+az;
    while ( n-- ) {
	VisitVoxel ( x, y, z );
	if ( exy < 0 ) {
	    if ( exz < 0 ) {
		x += sx;
		exy += by; exz += bz;
	    }
	    else  {
		z += sz;
		exz -= bx; ezy += by;
	    }
	}
	else {
	    if ( ezy < 0 ) {
		z += sz;
		exz -= bx; ezy += by;
	    }
	    else  {
		y += sy;
		exy -= bx; ezy -= bz;
	    }
	}
    }
}

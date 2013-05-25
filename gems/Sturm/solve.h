
/*
 * solve.h
 *
 *	some useful constants and types.
 */
#define	 	MAX_ORDER  		12	
/* maximum order for a polynomial */
	
#define	  	RELERROR	  		1.0e-14
/* smallest relative error we want */

#define	  	MAXPOW	    	32		
/* max power of 10 we wish to search to */

#define	  	MAXIT	     	800		
/* max number of iterations */

/* a coefficient smaller than SMALL_ENOUGH is considered to 
   be zero (0.0). */

#define	  	SMALL_ENOUGH		1.0e-12


/*
 * structure type for representing a polynomial
 */
typedef  	struct	p {
		     int	ord;
		     double	coef[MAX_ORDER+1];
} poly;

extern 	int		modrf();
extern 	int		numroots();
extern 	int		numchanges();
extern 	int		buildsturm();

extern 	double	evalpoly();
	


/****************************************************************************
**    TEST FILE FOR graph (Dynamic Layout Alg)
**
**    HEADER   - 2D VECTOR OPERATIONS
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
*****************************************************************************/
/*
*    VECTOR - 2D GEOMETRIC VECTOR TYPE
*/
class	 vector {
    double x;				     // coordinates
    double y;
public:
    vector()			     { x = 0.0; y = 0.0;	      }
    vector(double x0, double y0)     { x = x0; y = y0;		      }
    void     operator=(vector&	a)   { x = a.x; y = a.y;	      }
    void     operator+=(vector& a)   { x += a.x; y += a.y;	      }
    void     operator/=(double d)    { if (d != 0.0) {x /= d; y /= d;}}
    void     operator*=(double d)    { x *= d; y *= d;		      }
    double	  X()		     { return x;		      }
    double	  Y()		     { return y;		      }
    double     Size();

// FRIENDS
    friend vector operator+(vector&, vector&);
    friend vector operator-(vector&, vector&);
    friend vector operator-(vector&);
    friend vector operator*(vector&, double);
    friend vector operator*(double, vector&);
};

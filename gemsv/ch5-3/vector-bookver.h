/***********************************************************
* vector.h - a vector class written in c++                 *
* functions for +, -, dotproduct, crossproduct, scaling,   *
* length & normalizing, many of these are operators        *
* By Tomas Moller                                          *
***********************************************************/
#ifndef VECTOR_H
#define VECTOR_H

#include <stream.h>
#include <string.h>
#include <math.h>

#define Xi 0            // indices into vector
#define Yi 1
#define Zi 2

class Vector
{
    protected:
        float fx,fy,fz;
    public:
        Vector() {fx=0.0;fy=0.0;fz=0.0;}   // constructor with no argument
        Vector(float x,float y,float z);   // constructor with coords
        Vector(Vector& a);                 // constructor with vector
        void Set(float x,float y,float z); // assign new values to vector
        void SetX(float x);                // set x
        void SetY(float y);                // set y
        void SetZ(float z);                // set z;
        void SetIndex(int index,float value);
        // set x,y or z to value depending on index
        float X(void);                     // return fx
        float Y(void);                     // return fy
        float Z(void);                     // return fz
        void Add(float x,float y,float z); // addition to this vector
        void Sub(float x,float y,float z); // subtraction
        void Scale(float a);               // scaling of vector
        float Length(void);                // length of vector
        void Normalize(void);              // normalize vector

        void operator=(Vector& a);         // operator: assignment
        Vector operator*(float t);         // operator: scaling
        Vector operator+(Vector& a);       // operator: addition 
        Vector operator-(Vector& a);       // operator: subtraction
        Vector operator+(void);            // unary +
        Vector operator-(void);            // unary -
        void operator+=(Vector& a);        // operator: +=
        void operator-=(Vector& a);        // operator: -=
        void operator*=(float t);          // operator: *= (scaling)
        float operator*(Vector& a);        // operator: dot product
        Vector operator%(Vector& a);       // operator: cross product
        float operator[](short index);     
        // if short=0 then X, short=1 then Y, else Z, see constants above
};

/* here follows the inline functions and operators */

inline Vector::Vector(float x,float y,float z)
{ fx=x; fy=y; fz=z; }

inline Vector::Vector(Vector& a)
{ fx=a.fx; fy=a.fy; fz=a.fz; }

inline void Vector::Set(float x,float y,float z)
{ fx=x; fy=y; fz=z; }

inline void Vector::SetX(float x)
{ fx=x;}

inline void Vector::SetY(float y)
{ fy=y; }

inline void Vector::SetZ(float z)
{ fz=z; }

inline void Vector::SetIndex(int index,float value)
{
    switch(index)
    {
        case Xi: fx=value;
        case Yi: fy=value;
        case Zi: fz=value;
    }
}

inline float Vector::X(void)
{ return fx; }

inline float Vector::Y(void)
{ return fy; }

inline float Vector::Z(void)
{ return fz; }

inline void Vector::Add(float x,float y,float z)
{ fx+=x; fy+=y; fz+=z; }
 
inline void Vector::Sub(float x,float y,float z)
{ fx-=x; fy-=y; fz-=z; }

inline void Vector::Scale(float a)
{ fx*=a; fy*=a; fz*=a; }

inline float Vector::Length(void)
{ return sqrt((*this)*(*this)); // square root of Dot(this,this)
}

inline void Vector::Normalize(void)
{ 
    if(Length()==0.0) cout<<"Error:normalize\n";
    else Scale(1.0/Length());
}

/****************** Operators *********************/
inline void Vector::operator=(Vector& a)    // assignment
{ fx=a.fx; fy=a.fy; fz=a.fz; }

inline Vector Vector::operator+(void)       // unary +
{ return *this; }

inline Vector Vector::operator*(float t)        // scaling
{ Vector temp; temp.Set(fx*t,fy*t,fz*t); return temp; }

inline Vector Vector::operator+(Vector& a)
{ Vector sum; sum.Set(fx+a.fx,fy+a.fy,fz+a.fz); return sum; }

inline Vector Vector::operator-(Vector& a)
{ Vector sum; sum.Set(fx-a.fx,fy-a.fy,fz-a.fz); return sum; }

inline Vector Vector::operator-(void)           // unary -
{ Vector neg; neg.Set(-fx,-fy,-fz); return neg; }

inline void Vector::operator+=(Vector& a)
{ Set(fx+a.fx,fy+a.fy,fz+a.fz); }

inline void Vector::operator-=(Vector& a)
{ Set(fx-a.fx,fy-a.fy,fz-a.fz); }

inline void Vector::operator*=(float t)     // scaling
{ Set(fx*t,fy*t,fz*t); }

inline float Vector::operator*(Vector& a)       // dot product
{ return fx*a.fx+fy*a.fy+fz*a.fz; }

inline Vector Vector::operator%(Vector& a)      // cross product
{
    Vector cross;
    cross.Set(fy*a.fz-fz*a.fy,fz*a.fx-fx*a.fz,fx*a.fy-fy*a.fx);
    return cross;
}

inline float Vector::operator[](short index)
{
    switch(index)
    {
        case Xi: return fx;
        case Yi: return fy;
        case Zi: return fz;
    }
    return 0.0;                 // if invalid index
}
/*************** End of Operators *****************/
#endif

/***********************************************************************
*  The following macros handle most vector operations, the exceptions
*  usually being complex equations with four or more vectors.
*
*  An alternate form for the multiple-statement macros is the
*  "if (1) <macro_body> else" form.  This allows for temporary variable
*  declaration and control-flow constructs, but cannot be used
*  everywhere a function call could, as with the form used below.
*
*  Note that since the vector arguments are not enclosed in parentheses
*  in the macro body, you can scale the vector arguments in the macro
*  calls, e.g. Vec2Op(vec1,=,scalar*vec2).
*
*  Here are some example uses of the following macros:
*
*      printf ("Vector = <%lg %lg %lg>\n", VecList(vector))
*      vector_dot = VecDot (vec1, vec2)
*      norm = VecNorm (vector)
*      VecScalar (vector, /=, norm)
*      VecScalar (vector, *=, scale)
*      Vec3Scalar (Xaxis, =, 1.0, 0.0, 0.0)
*      Vec3Scalar (vector, *=, Xshear, Yshear, Zshear)
*      Vec2Op (vector, =, Xaxis)
*      Vec2Op (vector, +=, norm * Xaxis)
*      Vec3Op (vec1, =, vec2, =, Xaxis)
*      Vec3Op (vec1, =, vec2, -, vec3)
*      Vec3Op (vec1, +=, scale2 * vec2, -, scale3 * vec3)
*      VecCross (vec1, -=, vec2, X, vec3)
*      VecCrossSafe (vec1, =, vec1, X, Xaxis)
***********************************************************************/

#include <math.h>       /* Needed for sqrt() definition. */

    /* Vector type definition.  If you define colors in the same manner,
    ** you can also use these macros for color vector operations.  */

typedef long float  Vector[3];
typedef Vector      Point;              /* For readability. */

    /* VecList enumerates the vector fields for function calls. */

#define VecList(V)      V[0], V[1], V[2]

    /* This macro computes the dot product of two vectors. */

#define VecDot(A,B)     ((A[0]*B[0]) + (A[1]*B[1]) + (A[2]*B[2]))

    /* The VecNorm macro computes the norm of the vector. */

#define VecNorm(V)      sqrt(VecDot(V,V))

    /* VecScalar provides for scalar operations on a vector. */

#define VecScalar(V,assign_op,k)        \
(   V[0] assign_op k,   \
    V[1] assign_op k,   \
    V[2] assign_op k    \
)

    /* Vec3Scalar provides for vector operations that involve three
    ** distinct scalar factors. */

#define Vec3Scalar(V,assign_op,a,b,c)   \
(   V[0] assign_op a,   \
    V[1] assign_op b,   \
    V[2] assign_op c    \
)

    /* Vec2Op provides for operations with two vectors. */

#define Vec2Op(A,assign_op,B)   \
(   A[0] assign_op B[0],        \
    A[1] assign_op B[1],        \
    A[2] assign_op B[2]         \
)

    /* Vec3op handles vector operations with three vectors. */

#define Vec3Op(A,assign_op,B,op,C)      \
(   A[0] assign_op B[0] op C[0],        \
    A[1] assign_op B[1] op C[1],        \
    A[2] assign_op B[2] op C[2]         \
)

    /* The cross product macros come in two flavors.  VecCross() requires
    ** that all three vectors are distinct.  With the VecCrossSafe()
    ** macro, it's OK to do A <- A X B, but this requires a temporary
    ** vector for storage, which in turn requires the "if (1) ... else"
    ** form.  As an alternative, a global temporary vector could be used.
    */

#define VecCross(A,assign_op,B,dummy_op,C)              \
(   A[0] assign_op (B[1] * C[2]) - (B[2] * C[1]),       \
    A[1] assign_op (B[2] * C[0]) - (B[0] * C[2]),       \
    A[2] assign_op (B[0] * C[1]) - (B[1] * C[0])        \
)

#define VecCrossSafe(A,assign_op,B,dummy_op,C)  \
if (1)                          \
{   auto Vector result;         \
    VecCross (result,=,B,X,C);  \
    Vec2Op (A,=,result);        \
} else


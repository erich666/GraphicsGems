//
// ExtHit minimal tester
//
// Len Wanger - Fri Aug 30 10:07:01 PDT 1991
//

#include "iostream.h"
#include "exthit.h"

void func_tst (Ptr data, Ptr p1, Ptr p2 )
{
  cout << "Objects " << (Ptr) p1 << " and " << (Ptr) p2 << " intersect.\n";
}

main()
{
  ExtHit eh(5);
  Extent ext;
  Ptr ptr;

  ext.min[0] = 1; ext.max[0] = 3;
  ext.min[1] = 2; ext.max[1] = 4;
  ext.min[2] = 4; ext.max[2] = 6;
  ext.min[3] = 2; ext.max[3] = 8;

  ptr = (Ptr) 0x1;

  if ( ! eh.add ( ext, ptr ) )
    cout << "Fillnext failed\n";;

  ext.min[0] = 5; ext.max[0] = 7;
  ext.min[1] = 6; ext.max[1] = 8;
  ext.min[2] = 2; ext.max[2] = 8;
  ext.min[3] = 1; ext.max[3] = 3;

  ptr = (Ptr) 0x2;

  if ( ! eh.add ( ext, ptr ) )
    cout << "Fillnext failed\n";;

  ext.min[0] = 2; ext.max[0] = 6;
  ext.min[1] = 3; ext.max[1] = 7;
  ext.min[2] = 7; ext.max[2] = 7;
  ext.min[3] = 2; ext.max[3] = 4;

  ptr = (Ptr) 0x3;

  if ( ! eh.add ( ext, ptr ) )
    cout << "Fillnext failed\n";;

  ext.min[0] = 2; ext.max[0] = 6;
  ext.min[1] = 5; ext.max[1] = 5;
  ext.min[2] = 1; ext.max[2] = 5;
  ext.min[3] = 1; ext.max[3] = 5;

  ptr = (Ptr) 0x4;

  if ( ! eh.add ( ext, ptr ) )
    cout << "Fillnext failed\n";;

  eh.test ( func_tst, (Ptr) 0x5);
}


// -*- C++ -*-
// list.h by George Vanecek Jr. June 1994

#ifndef _LIST_H_
#define _LIST_H_

#ifndef __STDLIB_H__
#include <stdlib.h>
#endif

#ifndef _BASIC_H_
#include "basic.h"
#endif

class ListNode {
public:
  ListNode( void* const t, ListNode* const n ) : val(t), nxt(n) {}
  void*          value ( ) const { return val; }
  ListNode*      next  ( ) { return nxt; }
  void* operator new   ( size_t );
  void  operator delete( void* );
  
private:
  void* const val;
  ListNode*   nxt;
  static ListNode* freeList;
};

template <class T>
class List {
public:
  List( ) : anchor(NULL), nNodes(0) { }
  Boolean   empty      ( ) const { return Boolean( anchor == NULL ); }
  T*        first      ( ) const { return (T*)(anchor->value()); }
  ListNode* head       ( ) const { return anchor; }
  Counter   size       ( ) const { return nNodes; }
  void      operator <<( T* const ); // insert (i.e. push)
  Boolean   operator >>( T*& );	     // remove (i.e. pop)

private:
  ListNode* anchor;		// Start of List
  Counter   nNodes;		// Number of Nodes on list
};

template <class T>
void List<T>::operator <<( T* const t )
{
  anchor = new ListNode( t, anchor );
  ++nNodes;
}

template <class T>
Boolean List<T>::operator >>( T*& t )
{
  if( empty() ) {
    t = NULL;
    return FALSE;
  }
  t = first();
  ListNode* f = anchor;
  anchor = f->next();
  delete f;
  --nNodes;
  return TRUE;
}

#define forEachItemOnList(p) \
     for( ListNode* pp = p.head(); pp != NULL; pp = pp->next() )

#define getItem(Type) \
     (Type*)(pp->value())

#endif

// -*- C++ -*-
//
// list.cc
// by George Vanecek, 1994
// This List template is used frequently, and consequently, we
// want to improve its performance.  Since memory allocation and
// dealocation is very time consuming, we keep a stack of free ListNodes,
// and reuse them.

#include "list.h"

ListNode* ListNode::freeList = NULL;

// Allocate ListNode first from our free list, then from system heap.
void* ListNode::operator new( size_t s )
{
  if( freeList ) {
    ListNode* const n = freeList;
    freeList = freeList->nxt;
    return n;
  }
  return ::operator new(s);
}

// Delete node by placing it on our free list.
void ListNode::operator delete( void* p )
{
  ListNode* const n = (ListNode*)p;
  n->nxt   = freeList;
  freeList = n;
}


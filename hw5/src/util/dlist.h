/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node -> _data; }
      T& operator * () { return _node -> _data; }
      iterator& operator ++ () { 
         _node = _node -> _next;
         return *(this); 
      }
      iterator operator ++ (int) { 
         iterator temp = *(this);
         (this -> _node) = (this -> _node) -> _next;
         return temp; 
      }
      iterator& operator -- () { 
         _node = _node -> _prev;
         return *(this);   
      }
      iterator operator -- (int) {
         iterator temp = *(this);
         (this -> _node) = (this -> _node) -> _prev;
         return temp; 
      }

      iterator& operator = (const iterator& i) { this -> _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return this -> _node != i._node; }
      bool operator == (const iterator& i) const { return this -> _node == i._node; }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const {  return iterator(_head->_next); }
   iterator end() const { return iterator(_head); }
   bool empty() const { return (_head -> _next) == (_head);}
   size_t size() const {  
      DListNode<T>* cur = _head -> _next;
      int num = 0;
      while(cur != _head){
         num++;
      }
      return num; 
   }

   void push_back(const T& x) {
      DListNode<T>* temp = new DListNode<T>(x, _head -> _prev, _head);
      if(empty())
         _head -> _next = _head -> _prev = temp;
      else{
         (_head -> _prev) -> _next = temp;
         _head -> _prev = temp;
      }
   }
   void pop_front() { 
      if(empty()) return;
      DListNode<T>* temp = _head -> _next;
      _head -> _next = temp -> _next;
      (temp -> _next) -> _prev = _head;
      delete temp;
   }
   void pop_back() {
      if(empty()) return;
      DListNode<T>* temp = _head -> _prev;
      _head -> _prev = temp -> _prev;
      (temp -> _prev) -> _next = _head;
      delete temp;
      
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      for(iterator it = begin(); it != end(); it++){
         if(it == pos){
            it--;
            (it._node) -> _next = (pos._node) -> _next;
            ((pos._node) -> _next) -> _prev = it._node;
            delete pos._node;
            return true;
         }
      } 
      return false; 
   }
   bool erase(const T& x) { 
      DListNode<T>* cur = _head -> _next;
      while(cur != _head){
         if(cur -> _data == x){
            (cur -> _prev) -> _next = cur -> _next;
            (cur -> _next) -> _prev = cur -> _prev;
            delete cur;
            return true;
         }
         cur = cur -> _next;
      }
      return false;
   }

   void clear() { 
      while(!empty())
         pop_back();
   }  // delete all nodes except for the dummy node

   void sort() const { }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // DLIST_H

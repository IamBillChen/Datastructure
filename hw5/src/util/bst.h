/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;
	
   BSTreeNode(T data, BSTreeNode* parent=NULL, BSTreeNode* left=NULL, BSTreeNode* right=NULL):
   _data(data), _parent(parent), _left(left), _right(right){}

   ~BSTreeNode(){}  

	T           _data;
	BSTreeNode<T>* _left;
	BSTreeNode<T>* _right;
	BSTreeNode<T>* _parent;
};


template <class T>
class BSTree
{	

   // TODO: design your own class!!
public:
	BSTree(){
		_begin = _end = _head = new BSTreeNode<T>(T());
	}
	~BSTree(){
		clear();
		delete _head;
	}
	class iterator {
		friend class BSTree; 
	public:
		iterator(BSTreeNode<T>* n=0): _node(n){}
		iterator(const iterator& i): _node(i._node){}
		~iterator(){}

		const T& operator * () const {return _node -> _data;}
		T& operator * () {return _node -> _data;}
		iterator& operator ++ () {
			T temp = *(*(this));
			if(_node -> _right){
				_node = _node -> _right;
				while(_node -> _left) _node = _node -> _left;
			}
			else{
				while(_node -> _data <= temp) _node = _node -> _parent;
			}
			return *(this);
		}

		iterator operator ++ (int) {
			iterator temp(*(this));
			++(*(this));
			return temp;
		}

		iterator& operator -- () {
			T temp = *(*(this));
			if(_node -> _left){
				_node = _node -> _left;
				while(_node -> _right) _node = _node -> _right;
			}
			else{
				_node = _node -> _parent;
				while(_node -> _data > temp) _node = _node -> _parent;
			}
			return *(this);
		}

		iterator operator -- (int) {
			iterator temp(*(this));
			--(*this);
			return temp;
		}

		iterator& operator = (iterator i) {this -> _node = i._node; return *(this);}

		bool operator == (const iterator& i) {return this -> _node == i._node;}
		bool operator != (const iterator& i) {return this -> _node != i._node;}

	private:
		BSTreeNode<T>* _node;
	};


	iterator begin() const { return _size == 0 ?  iterator(NULL) : iterator(_begin);}  //the leftmost element, retrun end() if it's empty
	iterator end() const { return _size == 0 ? iterator(NULL) : iterator(_end);}
	bool empty() const {return _size == 0;}
	size_t size() const {return _size;}
	void pop_front(){erase(begin());} //remove the leftmost one
	void pop_back(){erase(--end());}	 //remove the rightmost one, no error if it is empty

	void insert(const T& x){
		if(empty()){
			_head -> _data = x;
			_end = _head -> _right = new BSTreeNode<T>(x, _head);
			++_size;
			return;
		}
		BSTreeNode<T>* cur = _head;
		while(true){
			if(x >= cur -> _data)
				if(cur -> _right && cur -> _right != _end) cur = cur -> _right;
				else{
					cur -> _right = new BSTreeNode<T>(x, cur);
					++_size;
					if(x >= _end -> _data){
						_end -> _data = x;
						_end -> _parent = cur -> _right;
						cur -> _right -> _right = _end;
					}
					return;
				}
			else{
				if(cur -> _left) cur = cur -> _left;
				else{
					cur -> _left = new BSTreeNode<T>(x, cur);
					++_size;
					if(x < _begin -> _data)
						_begin = cur -> _left;
					return;
				}
			}
		}
	}

	bool erase(const T& x){
		if(_size == 1){
			clear();
			return true;
		}
		BSTreeNode<T>* it = _head;
		while(it){
			if(x > it -> _data) it = it -> _right;
			else if(x < it -> _data) it = it -> _left;
			else
			{
				if(it -> _right && it != _end -> _parent){
					BSTreeNode<T>* cur = it -> _right;
					if(cur -> _left){
						while(cur -> _left) cur = cur -> _left;
						cur -> _parent -> _left = NULL;
						if(cur -> _right){
							cur -> _parent -> _left = cur -> _right;
							cur -> _right -> _parent = cur -> _parent;
						}
					} 
					else if(cur -> _right){
						cur -> _parent -> _right = cur -> _right;
						cur -> _right -> _parent = cur -> _parent;
					}
					else{ cur -> _parent -> _right = NULL; }
					it -> _data = cur -> _data;
					delete cur;
					--_size;
					return true;
				} else{
					BSTreeNode<T>* pre = it -> _parent;
					if(pre){
						if(x > pre -> _data) pre ->  _right = it -> _left; 
						else pre ->  _left = it -> _left; 
					} 
					else _head = it -> _left;

					if(it -> _left)
							it -> _left -> _parent = pre;
					if(it == _end -> _parent) {
						_end ->  _parent = it -> _left == NULL ? it -> _parent : it -> _left;
						while(_end -> _parent -> _right) _end -> _parent = _end -> _parent -> _right;				
						_end -> _data = _end -> _parent -> _data;
						_end -> _parent -> _right = _end;
					}
					else if(it == _begin)
						_begin = it -> _parent;
					delete it;
					--_size;
					return true;
				}
			}
		}
		return 0;
	} 

	bool erase(iterator pos){
		if(_size == 1){
			clear();
			return true;
		}
		iterator i = begin();
		for(; i != end(); ++i){
			if(i == pos){
				BSTreeNode<T>* it = i._node;
				if(it -> _right && it != _end -> _parent){
					BSTreeNode<T>* cur = it -> _right;
					if(cur -> _left){
						while(cur -> _left) cur = cur -> _left;
						cur -> _parent -> _left = NULL;
						if(cur -> _right){
							cur -> _parent -> _left = cur -> _right;
							cur -> _right -> _parent = cur -> _parent;
						}
					} 
					else if(cur -> _right){
						cur -> _parent -> _right = cur -> _right;
						cur -> _right -> _parent = cur -> _parent;
					}
					else{ cur -> _parent -> _right = NULL; }
					it -> _data = cur -> _data;
					delete cur;
					--_size;
					return true;
				} 
				else{
					BSTreeNode<T>* pre = it -> _parent;
					if(pre){
						if(it -> _data > pre -> _data) pre ->  _right = it -> _left; 
						else pre ->  _left = it -> _left; 
					} 
					else _head = it -> _left;

					if(it -> _left)
							it -> _left -> _parent = pre;
					if(it == _end -> _parent) {
						_end ->  _parent = it -> _left == NULL ? it -> _parent : it -> _left;
						while(_end -> _parent -> _right) _end -> _parent = _end -> _parent -> _right;				
						_end -> _data = _end -> _parent -> _data;
						_end -> _parent -> _right = _end;
					}
					else if(it == _begin)
						_begin = it -> _parent;
					delete it;
					--_size;
					return true;
				}
			}
		}
		return false;
	}

	void clear(){
		if(_head -> _left) clear(_head -> _left);
		if(_head -> _right) clear(_head -> _right);
		_head -> _left = _head -> _right = NULL;
		_end = _begin = _head;
		_size = 0;
	}

	void print() const {
		iterator it = begin();
		for(; it != end(); it++)
			cout << *(it) << endl;
	}
	void sort() const {} //dummy function
private:
	BSTreeNode<T>* _head; //dummy node
	BSTreeNode<T>* _begin;
	BSTreeNode<T>* _end;
	size_t _size = 0;

	void clear(BSTreeNode<T>* root){
		if(root -> _left) clear(root -> _left);
		if(root -> _right) clear(root -> _right);
		delete root;
	}
};

#endif // BST_H

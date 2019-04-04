#pragma once
#include <iostream>
#include <memory>
#include <cstdint>
#include <iterator>


//-------------------------------NODE_AD-------------------------------


template < class T >
class NodeList {
public:
	typedef T value_type;
	typedef const T& const_reference;
	typedef T&& rvalue_reference;

	value_type _data;
	NodeList<T>* _xor_ptr;
	
	explicit NodeList(const_reference data):
		_data(data), _xor_ptr(nullptr) {};

	explicit NodeList(rvalue_reference data):
		_data(std::move(data)), _xor_ptr(nullptr) {};

	explicit NodeList<T>(value_type data, NodeList<T>* previous):
		_data(data), _xor_ptr(previous) {};

	const NodeList<T>& operator =(const NodeList<T>& rep) {
		_data = rep._data;
		_xor_ptr = rep._xor_ptr;
		return *this;
	}

	const NodeList<T>& operator =(NodeList<T>&& rep) {
		_data = std::move(rep._data);
		_xor_ptr = rep._xor_ptr;
		return *this;
	}
	~NodeList() {
		_xor_ptr = nullptr;
	}
};


//-------------------------------ITERATOR_AD-------------------------------


template < class ListType, class Container >
class ListIterator : public std::iterator<std::bidirectional_iterator_tag, ListType, std::ptrdiff_t, ListType*, ListType&> {
private:
	NodeList<ListType>* current;
	NodeList<ListType>* previous;
public:
	typedef typename ListIterator::iterator_category iterator_category;
	typedef typename ListIterator::difference_type difference_type;
	typedef typename ListIterator::value_type value_type;
	typedef typename ListIterator::reference reference;
	typedef typename ListIterator::pointer pointer;
	typedef const ListIterator& const_reference_it;
	typedef ListIterator& reference_it;
	typedef ListType& reference_value;
	typedef ListType* pointer_value;

	ListIterator(const_reference_it it) :
		current(it.current), previous(it.previous) {};

	explicit ListIterator(NodeList<ListType>* Node, NodeList<ListType>* prev = nullptr) :
		current(Node), previous(prev) {};

	const_reference_it operator = (const_reference_it it);

	bool operator ==(const_reference_it it);
	bool operator !=(const_reference_it it);

	const_reference_it operator++();
	reference_it operator++(int);
	const_reference_it operator--();
	reference_it operator--(int);

	reference_value operator*() const;
	pointer_value operator->() const;

	NodeList<ListType>* get_previous() {return this->previous;}
	NodeList<ListType>* get_current() { return this->current; }
};


//-------------------------------XORLIST_AD-------------------------------


template < class ListType, class Allocator = std::allocator<ListType> >
class XorList {

public:

	typedef NodeList< ListType> Node;
	typedef NodeList< ListType>* Node_ptr;
	typedef ListIterator<ListType, XorList<ListType, Allocator> > iter;
	typedef std::size_t size_type;
	typedef ListType value_type;

	explicit XorList(const Allocator& _allocator = Allocator()) :
		first(nullptr), last(nullptr), size_(0), allocator(_allocator) {};
	explicit XorList(const XorList& list_);
	explicit XorList(XorList&& list_);

	const XorList& operator =(const XorList& list_);
	const XorList& operator =(XorList&& list_);

	size_type size() const;
	iter begin() const;
	iter end() const;

	template <class U>
	void insert_before(iter it, U&& data);

	template <class U>
	void insert_after(iter it, U&& data);

	template <class U>
	void push_back(U&& data_);

	template <class U>
	void push_front(U&& data_);


	const ListType& pop_back();
	const ListType& pop_front();

	void erase(iter it);

	~XorList();
private:


	Node_ptr first;
	Node_ptr last;
	size_type size_;
	typename Allocator::template rebind< NodeList <ListType> >::other allocator;


};


namespace xorPtr {
	typedef std::size_t ptr_cast;

	template < class T >
	NodeList<T>* xor_ptr(NodeList<T>* first, NodeList<T>* second) {
		ptr_cast first_ = reinterpret_cast<ptr_cast>(first);
		ptr_cast second_ = reinterpret_cast<ptr_cast>(second);
		NodeList<T>* result = reinterpret_cast<NodeList<T>*>(first_ ^ second_);
		return result;
	}

	template < class T >
	NodeList<T>* get_xor(NodeList<T>* ptr) {
		return (ptr ? ptr->_xor_ptr : nullptr);
	}
}


//-------------------------------ITERATOR_IM-------------------------------


template<class ListType, class Container>
inline const ListIterator<ListType, Container>& ListIterator<ListType, Container>::operator=(const_reference_it it) {
	return ListIterator(*this);
}

template<class ListType, class Container>
inline bool ListIterator<ListType, Container>::operator==(const_reference_it it) {
	if (it.current == current && it.previous == previous)
		return true;
	return false;
}


template<class ListType, class Container>
inline bool ListIterator<ListType, Container>::operator!=(const_reference_it it){
	return !(it == *this);
}

template<class ListType, class Container>
inline const ListIterator<ListType, Container>& ListIterator<ListType, Container>::operator++() {
	ListIterator<ListType, Container> result = *this;
	NodeList<ListType>* newCur = xorPtr::xor_ptr<ListType>(previous, current->_xor_ptr);
	previous = current;
	current = newCur;
	return result;
}

template<class ListType, class Container>
inline ListIterator<ListType, Container>& ListIterator<ListType, Container>::operator++(int) {
	NodeList<ListType>* newCur = xorPtr::xor_ptr<ListType>(previous, current->_xor_ptr);
	previous = current;
	current = newCur;
	return *this;
}

template<class ListType, class Container>
inline const ListIterator<ListType, Container>& ListIterator<ListType, Container>::operator--() {
	ListIterator<ListType, Container> result = *this;
	NodeList<ListType>* newCur = xorPtr::xor_ptr<ListType>(previous->_xor_ptr, current);
	current = previous;
	previous = newCur;
	return result;
}

template<class ListType, class Container>
inline ListIterator<ListType, Container>& ListIterator<ListType, Container>::operator--(int) {
	NodeList<ListType>* newCur = xorPtr::xor_ptr<ListType>(previous->_xor_ptr, current);
	current = previous;
	previous = newCur;
	return *this;
}

template<class ListType, class Container>
inline ListType& ListIterator<ListType, Container>::operator *() const {
	return current->_data;
}

template<class ListType, class Container>
inline ListType* ListIterator<ListType, Container>::operator ->() const {
	return &(current->_data);
}


//-------------------------------XORLIST_IM-------------------------------


template < class ListType, class Allocator = std::allocator<ListType> >
XorList<ListType, Allocator>::XorList(const XorList& list_) {
	*this = list_;
	return *this;
}


/////////-----------------------------------------------------------------


template < class ListType, class Allocator = std::allocator<ListType> >
XorList<ListType, Allocator>::XorList(XorList&& list_){
	*this = std::move(_list);
	return *this;
}

template < class ListType, class Allocator = std::allocator<ListType> >
const XorList<ListType, Allocator>& XorList<ListType, Allocator>::operator =(const XorList& list_) {
	NodeList<ListType>* cur = first;
	NodeList<ListType>* temp = nullptr;
	while (cur != nullptr) {
		NodeList<ListType>* newNode = xorPtr::xor_ptr<ListType>(cur->_xor_ptr, temp);
		temp = cur;
		allocator.destroy(ñur);
		allocator.deallocate(cur, 1);
		cur = newNode;
	}
	this->size = 0;
	first = nullptr;
	last = nullptr;
	cur = list_.first;
	temp = nullptr;
	int cnt = 0;
	while (cnt < size) {
		this->push_back(cur->_data);
		NodeList<ListType>* next = xorPtr::xor_ptr<ListType>(temp, cur->_xor_ptr);
		temp = cur;
		cur = next;
		cnt++;
	}
	return *this;
}


template < class ListType, class Allocator = std::allocator<ListType> >
const XorList<ListType, Allocator>& XorList<ListType, Allocator>::operator=(XorList&& list_) {
	this->first = list_->first;
	this->last = list_->last;
	this->size = list_->size;
	list_->first = nullptr;
	list_->last = nullptr;
};

template <class ListType, class Allocator = std::allocator<ListType > >
std::size_t XorList<ListType, Allocator>::size() const {
	return size_;
}


template <class ListType, class Allocator = std::allocator<ListType> >
typename XorList<ListType, Allocator>::iter XorList<ListType, Allocator>::begin() const {
	return iter(first);
}

template <class ListType, class Allocator = std::allocator<ListType> >
typename XorList<ListType, Allocator>::iter XorList<ListType, Allocator>::end() const {
	return iter(nullptr, last);
}

template <class ListType, class Allocator = std::allocator<ListType> >
template <class U>
void XorList<ListType, Allocator>::insert_before(typename XorList<ListType, Allocator>::iter it, U&& data) {
	NodeList<ListType>* cur = it.get_current();
	NodeList<ListType>* temp = it.get_previous();
	size_++;
	NodeList<ListType>* newNode = allocator.allocate(1);
	newNode->_data = std::forward<U>(data);
	newNode->_xor_ptr = xorPtr::xor_ptr<ListType>(temp, cur);
	if (temp) {
		temp->_xor_ptr = xorPtr::xor_ptr<ListType>(newNode, xorPtr::xor_ptr<ListType>(xorPtr::get_xor(temp), cur));
	}
	if (cur)
		cur->_xor_ptr = xorPtr::xor_ptr<ListType>(newNode, xorPtr::xor_ptr<ListType>(xorPtr::get_xor(cur), temp));
	if (!temp) {
		first = newNode;
	}
	if (!cur) {
		last = newNode;
	}
}
 
template <class ListType, class Allocator = std::allocator<ListType> >
template <class U>
void XorList<ListType, Allocator>::insert_after(typename XorList<ListType, Allocator>::iter it, U&& data) {
	NodeList<ListType>* cur = it.get_current();
	if (cur == nullptr) {
		this->push_back(std::forward<U>(data));
	}
	else {
		it++;
		this->insert_before(it, std::forward<U>(data));
	}
}


template <class ListType, class Allocator = std::allocator<ListType> >
template <class U>
void XorList<ListType, Allocator>::push_back(U&& data) {
	NodeList<ListType>* newNode = allocator.allocate(1);
	newNode->_data = std::forward<U>(data);
	newNode->_xor_ptr = nullptr;
	++size_;
	if (size_ == 1) {
		last = newNode;
		first = last;
		return;
	}
	last->_xor_ptr = xorPtr::xor_ptr<ListType>(newNode, last->_xor_ptr);
	newNode->_xor_ptr = last;
	last = newNode;
}

template <class ListType, class Allocator>
template <class U>
void XorList<ListType, Allocator>::push_front(U&& data) {
	insert_before(this->begin(), std::forward<U>(data));
}

template <class ListType, class Allocator>
const ListType& XorList<ListType, Allocator>::pop_back() {
	this->erase(this->end()--);
}


template <class ListType, class Allocator = std::allocator<ListType> >
const ListType& XorList<ListType, Allocator>::pop_front() {
	this->erase(this->begin());
}

template <class ListType, class Allocator = std::allocator<ListType> >
void XorList<ListType, Allocator>::erase(typename XorList<ListType, Allocator>::iter it) {
	NodeList<ListType>* cur = it.get_current();
	NodeList<ListType>* temp = it.get_previous();
	--size_;
	auto newNode = xorPtr::xor_ptr<ListType>(xorPtr::get_xor(cur), temp);
	if (temp) 
		temp->_xor_ptr = xorPtr::xor_ptr<ListType>(newNode, xorPtr::xor_ptr<ListType>(temp->_xor_ptr, cur));
	if (newNode) 
		newNode->_xor_ptr = xorPtr::xor_ptr<ListType>(temp, xorPtr::xor_ptr<ListType>(newNode->_xor_ptr, cur));
	allocator.destroy(cur);
	allocator.deallocate(cur, 1);
	if (!temp) {
		first = newNode;
	}
	if (!newNode) {
		last = temp;
	}
}

template <class ListType, class Allocator = std::allocator<ListType> >
XorList<ListType, Allocator>::~XorList() {
	NodeList<ListType>* cur = first;
	NodeList<ListType>* temp = nullptr;
	while (cur != nullptr) {
		NodeList<ListType>* newNode = xorPtr::xor_ptr<ListType>(cur->_xor_ptr, temp);
		temp = cur;
		allocator.destroy(cur);
		allocator.deallocate(cur, 1);
		cur = newNode;
	}
} 



// DList.hpp
#ifndef DList_hpp
#define DList_hpp

#include "DListNode.hpp"
template <typename ItemType>
class DList {

public:
    /// constructor
    DList();

    /// copy constructor
    DList(const DList& source);

    /// assignment operator
    DList& operator=(const DList& source);
    /// returns the number of items in the list
    size_t length() const { return _size; }

    /// item at index specified by position
    /// @param position index of item to return
    /// @return item at index specified by position
    ItemType operator[](long position) const;

    /// reference to item at index specified by position
    /// @param position index of item to return
    /// @return reference to item at index specified by position
    ItemType& operator[](long position);

    /// removes all elements from the list
    void clear();

    /// adds the value x onto the end of the list
    /// @param x value to add to the end of the list
    void append(const ItemType& x);

    /// inserts x at the index (negative or non-negative) at the specified poition; note if
    /// position is beyond the end, it adds to the end of the list or if position is beyond
    /// the beginning it inserts at the beginning
    /// @param position index to insert at
    /// @param x value to insert at specified position
    void insert(long position, const ItemType& x);

    /// remove and return element at index specified by position
    /// @param position index of element to remove
    ItemType pop(long position = -1);

    /// removes element from the list
    /// @param x element to remove
    void remove(ItemType x);

    /// returns non-negative index of x starting at index start
    /// @param x value to find the index of
    /// @return non-negative index of x or -1 if not found
    size_t index(ItemType x, size_t start = 0) const;

    /// returns number of copies of x in the list
    /// @param x value to count
    /// @return number of copies of x in the list
    int count(ItemType x) const;

    /// adds each element of otherList onto this list
    /// @param otherList list to add the elements of
    void extend(const DList& otherList);

private:

    /// helper function for copy constructor and operator=
    /// @param source existing DList to make a copy of its nodes for and store in this
    void _copy(const DList& source);

    /// returns node at specified index
    /// @param position index from -length() to length()
    /// @return node at specified position or nullptr if position is out of range
    std::shared_ptr<DListNode<ItemType>> _find(long position) const;

    /// remove and return the element at the specified index
    /// if index is invalid, it does nothing
    /// @param position index of element to remove
    ItemType _delete(long position);

    // pointers to the head, and tail nodes
    std::shared_ptr<DListNode<ItemType>> _head, _tail;

    // number of items in the list
    long _size;
};


template <typename ItemType>
DList<ItemType>::DList() {
	_head = nullptr;
	_tail = nullptr;
	_size = 0;
}

template <typename ItemType>
DList<ItemType>::DList(const DList& source) {
	_copy(source);
}

template <typename ItemType>
DList<ItemType>& DList<ItemType>::operator=(const DList& source) {
	if (this != &source) {
		clear();
		_copy(source);
	}
	return *this;
}

template <typename ItemType>
ItemType DList<ItemType>::operator[](long position) const {
	return _find(position)->_item;
}

template <typename ItemType>
ItemType& DList<ItemType>::operator[](long position) {
	return _find(position)->_item;
}

template <typename ItemType>
void DList<ItemType>::clear() {
	_head = nullptr;
	_tail = nullptr;
	_size = 0;
}

template <typename ItemType>
void DList<ItemType>::append(const ItemType& x) {
	auto newNode = std::make_shared<DListNode<ItemType>>(x);
	if (_size == 0) {
		_head = newNode;
		_tail = newNode;
	}
	else {
		_tail->_next = newNode;
		newNode->_prev = _tail;
		_tail = newNode;
	}
	_size++;
}

template <typename ItemType>
void DList<ItemType>::insert(long position, const ItemType& x) {

	if (position < 0) { // convert negative position to positive to insert at index
		position += _size;
	}
	if (position < 0) { // if still negative, set to 0 so we can insert at front
		position = 0;
	}
	if (position > _size) { // if beyond end, set to end so we can append
		position = _size;
	}

	if (_size == 0 || position == _size) {
		append(x);
		return;
	}

	auto current = _find(position);
	auto previous = current->_prev.lock();
	auto newNode = std::make_shared<DListNode<ItemType>>(x, previous, current);

	if (previous) {
		previous->_next = newNode;
	}
	else {
		_head = newNode;
	}
	current->_prev = newNode;
	++_size;
}

template <typename ItemType>
ItemType DList<ItemType>::pop(long position) {
	return _delete(position);
}

template <typename ItemType>
void DList<ItemType>::remove(ItemType x) {
	auto node = _head;
	while (node != nullptr) {
		if (node->_item == x) {
			auto previous = node->_prev.lock();
			auto next = node->_next;
			if (previous) {
				previous->_next = next;
			}
			else {
				_head = next;
			}
			if (next) {
				next->_prev = previous;
			}
			else {
				_tail = previous;
			}
			--_size;
			return;
		}
		node = node->_next;
	}
}

template <typename ItemType>
size_t DList<ItemType>::index(ItemType x, size_t start) const {
	auto node = _find(start);
	auto index = start;
	while (node != nullptr) {
		if (node->_item == x) {
			return index;
		}
		node = node->_next;
		++index;
	}
	return -1;
}

template <typename ItemType>
int DList<ItemType>::count(ItemType x) const {
	int count = 0;
	auto node = _head;
	while (node != nullptr) {
		if (node->_item == x) {
			++count;
		}
		node = node->_next;
	}
	return count;
}

template <typename ItemType>
void DList<ItemType>::extend(const DList& otherList) {
	if (&otherList == this) {
		long n = _size;
		auto node = _head;
		for (long i = 0; i < n; ++i) {
			append(node->_item);
			node = node->_next;
		}
		return;
	}

	// Normal case
	auto node = otherList._head;
	while (node != nullptr) {
		append(node->_item);
		node = node->_next;
	}
}

template <typename ItemType>
void DList<ItemType>::_copy(const DList& source) {
	std::shared_ptr<DListNode<ItemType>> sourceNode, newNode;
	sourceNode = source._head;
	if (sourceNode) {
		newNode = _head = std::make_shared<DListNode<ItemType>>(sourceNode->_item);
		sourceNode = sourceNode->_next;
	}
	else {
		_head.reset();
	}
	while (sourceNode) {
		newNode->_next = std::make_shared<DListNode<ItemType>>(sourceNode->_item, newNode);
		newNode = newNode->_next;
		sourceNode = sourceNode->_next;
	}
	_tail = newNode;
	_size = source._size;
}

template <typename ItemType>
std::shared_ptr<DListNode<ItemType>> DList<ItemType>::_find(long position) const {
	if (position >= _size || position < -_size) {
		return nullptr;
	}
	if (position >= 0) {
		auto current = _head;
		for (long i = 0; i < position; i++) {
			current = current->_next;
		}
		return current;
	}
	else {
		auto current = _tail;
		for (long i = -1; i > position; i--) {
			current = current->_prev.lock();
		}
		return current;
	}
}

template <typename ItemType>
ItemType DList<ItemType>::_delete(long position) {
	// normalize negative indices
	if (position < 0) position += _size;

	// invalid index -> no exceptions allowed, so return default value
	if (position < 0 || position >= _size) {
		return ItemType{};
	}

	auto current = _find(position);        // guaranteed non-null now
	auto previous = current->_prev.lock();
	auto next = current->_next;

	if (previous) {
		previous->_next = next;
	}
	else {
		_head = next;
	}
	if (next) {
		next->_prev = previous;
	}
	else {
		_tail = previous;
	}

	--_size;
	return current->_item;
}

#endif /* DList_hpp */
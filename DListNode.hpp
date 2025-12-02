// DListNode.hpp

#ifndef DListNode_h
#define DListNode_h

#ifdef DEBUG
#include <iostream>
#endif

#include <memory>

// typedef int ItemType;

template<typename ItemType>
class DListNode {
    template <typename> friend class DList;

public:
    DListNode(ItemType item, std::shared_ptr<DListNode> prev = nullptr, std::shared_ptr<DListNode> next = nullptr);

#ifdef DEBUG
    // ~DListNode() { std::cerr << "deallocate DListNode " << _item << std::endl; }
#endif

private:
    ItemType _item;
    std::shared_ptr<DListNode> _next;
    std::weak_ptr<DListNode> _prev;
};
template<typename ItemType>
inline DListNode<ItemType>::DListNode(ItemType item, std::shared_ptr<DListNode> prev, std::shared_ptr<DListNode> next) {
    _item = item;
    _prev = prev;
    _next = next;
}

#endif /* DListNode_h */
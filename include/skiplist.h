#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <random>
#include <memory>
#include <list>
#include <utility>
#include <initializer_list>
#include <type_traits>
#include <iostream>
#include "memorytools.h"

 template<typename T, size_t max_layer = 10, typename Compare = std::less<T>>
 class SkipList {

    // we use a separate struct for the head of the list, since it will not hold any values, and T might not be default constructable
    // pointers to following nodes are stored in succ
    struct LinkHead {
        LinkHead* succ[max_layer]{};

        LinkHead() = default;
    };

    struct Link : LinkHead {
        using LinkHead::succ;
        T value;

        template<typename... Args>
        Link(Args&&... arguments) : LinkHead(), value(std::forward<Args>(arguments)...) {}
    };

    size_t chunks_per_block_ = 64;

    PoolAllocator pool_{sizeof(Link), chunks_per_block_};

    Compare comp_{};

    // this might actually not always correspond to the current maximum layer, since it does not get updated in remove
    // in the vast majority of cases, this barely impacts performance
    size_t current_max = 1;
    size_t elements = 0;

    LinkHead head_{};

    int CoinToss();

    public:
    SkipList() = default;

    SkipList(std::initializer_list<T> lst);
    SkipList& operator=(std::initializer_list<T> lst);

    SkipList(const SkipList&);
    SkipList& operator=(const SkipList&);

    SkipList(SkipList&&);
    SkipList& operator=(SkipList&&);

    ~SkipList();

    class Iterator {
        LinkHead* current_;

        public:
        Iterator(LinkHead* p) : current_{p} {}

        Iterator& operator++() {current_ = current_->succ[0]; return *this;}
        const T& operator*() const {return static_cast<const Link*>(current_)->value;}
        const T* operator->() const {return &(static_cast<const Link*>(current_)->value);}

        bool operator==(const Iterator& b) const {return current_ == b.current_;}
        bool operator!=(const Iterator& b) const {return current_ != b.current_;}
    };

    Iterator begin()  {return Iterator(head_.succ[0]);}
    Iterator begin() const  {return Iterator(head_.succ[0]);}

    Iterator end() {return Iterator(nullptr);}
    Iterator end() const {return Iterator(nullptr);}

    template<typename... Args>
    void insert(Args&&... arguments);

    bool find(const T&) const;
    void remove(const T&);

    size_t size();

    void print(std::ostream& os = std::cout) const;
 };

 #include "skiplist.tpp"
 #endif
#ifndef SKIPLIST_H
#include "skiplist.h"
#endif

template<typename T, size_t max_layer, typename Compare>
SkipList<T, max_layer, Compare>::SkipList(std::initializer_list<T> lst) : SkipList() {
    for (const T& element : lst) {
        insert(element);
    }
}

template<typename T, size_t max_layer, typename Compare>
SkipList<T, max_layer, Compare>& SkipList<T, max_layer, Compare>::operator=(std::initializer_list<T> lst) {
    LinkHead* current = head_.succ[0];
    while (current) {
        LinkHead* next = current->succ[0];
        static_cast<Link*>(current)->~Link();
        pool_.Deallocate(current);
        current = next;
    }
    elements = 0;

    for (const T& element : lst) {
        insert(element);
    }

    return *this;
}



template<typename T, size_t max_layer, typename Compare>
SkipList<T, max_layer, Compare>::SkipList(const SkipList& slist) {
    for (auto element : slist) {
        insert(element);
    }
}

template<typename T, size_t max_layer, typename Compare>
SkipList<T, max_layer, Compare>& SkipList<T, max_layer, Compare>::operator=(const SkipList& slist) {
    if (this != &slist) {
        LinkHead* current = head_.succ[0];
        while (current) {
            LinkHead* next = current->succ[0];
            static_cast<Link*>(current)->~Link();
            pool_.Deallocate(current);
            current = next;
        }
        current_max = 1;
        elements = 0;
        head_ = LinkHead{};

        for (auto element : slist) {
            insert(element);
        }
    }

    return *this;
}



template<typename T, size_t max_layer, typename Compare>
SkipList<T, max_layer, Compare>::SkipList(SkipList&& other) : pool_{std::move(other.pool_)}, comp_{std::move(other.comp_)}, current_max{other.current_max}, elements{other.elements}, head_{other.head_} {
    other.head_ = LinkHead{};
}

template<typename T, size_t max_layer, typename Compare>
SkipList<T, max_layer, Compare>& SkipList<T, max_layer, Compare>::operator=(SkipList&& other) {
    if (this != &other) {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            LinkHead* current = head_.succ[0];
            while (current) {
                LinkHead* next = current->succ[0];
                static_cast<Link*>(current)->~Link();
                current = next;
            }
        }

        pool_ = std::move(other.pool_);
        comp_ = std::move(other.comp_);
        current_max = other.current_max;
        elements = other.elements;
        head_ = other.head_;

        other.head_ = LinkHead{};
    }

    return *this;
}



template<typename T, size_t max_layer, typename Compare>
SkipList<T, max_layer, Compare>::~SkipList() {
    if constexpr (!std::is_trivially_destructible_v<T>) {
        LinkHead* current = head_.succ[0];
        while (current) {
            LinkHead* next = current->succ[0];
            static_cast<Link*>(current)->~Link();
            current = next;
        }
    }
}



template<typename T, size_t max_layer, typename Compare>
int SkipList<T, max_layer, Compare>::CoinToss() {
    thread_local std::random_device rd;
    thread_local std::mt19937 gen(rd());
    std::geometric_distribution<int> dist(0.25);
    return dist(gen);
}



template<typename T, size_t max_layer, typename Compare>
template<typename... Args>
void SkipList<T, max_layer, Compare>::insert(Args&&... arguments) {
    int layer = CoinToss() + 1;
    if (layer > max_layer) {layer = max_layer;}
    if (layer > current_max) {current_max = layer;}
    ++elements;

    void* mem = pool_.Allocate();
    Link* newcomer = new (mem) Link(std::forward<Args>(arguments)...);

    LinkHead* current = &head_;
    for (int i = layer - 1; i >= 0; i--) {
        while (current->succ[i] && comp_(static_cast<Link*>(current->succ[i])->value, newcomer->value)) {
            current = current->succ[i];
        }
        newcomer->succ[i] = current->succ[i];
        current->succ[i] = newcomer;
    }
}

template<typename T, size_t max_layer, typename Compare>
bool SkipList<T, max_layer, Compare>::find(const T& find_value) const {
    const LinkHead* current = &head_;

    for (int i = current_max - 1; i >= 0; i--) {
        for (; current->succ[i]; current = current->succ[i]) {
            if (!comp_(static_cast<Link*>(current->succ[i])->value, find_value)) {
                break;
            }
        }
    }

    if (current->succ[0] && !comp_(find_value, static_cast<Link*>(current->succ[0])->value)) {
        return true;
    }

    return false;
}

template<typename T, size_t max_layer, typename Compare>
void SkipList<T, max_layer, Compare>::remove(const T& remove_value) {
    LinkHead* current = &head_;
    for (int i = current_max - 1; i >= 0; i--) {
        for (; current->succ[i]; current = current->succ[i]) {
            if (comp_(remove_value, static_cast<Link*>(current->succ[i])->value)) {
                break;
            }
            if (!comp_(static_cast<Link*>(current->succ[i])->value, remove_value)) {
                LinkHead* to_delete = current->succ[i];
                for (; i >= 0; i--) {
                    for (; current->succ[i]; current = current->succ[i]) {
                        if (current->succ[i] == to_delete) {
                            current->succ[i] = to_delete->succ[i];
                            break;
                        }
                    }
                }
                static_cast<Link*>(to_delete)->~Link();
                pool_.Deallocate(to_delete);
                --elements;
                return;
            }
        }
    }
}



template<typename T, size_t max_layer, typename Compare>
size_t SkipList<T, max_layer, Compare>::size() {
    return elements;
}



template<typename T, size_t max_layer, typename Compare>
void SkipList<T, max_layer, Compare>::print(std::ostream& os) const {
    int high = current_max - 1;

    for (; high > 0; high--) {
        if (head_.succ[high]) {
            break;
        }
    }

    for (int i = high; i >= 0; i--) {
        os << "layer " << i << ":    ";
        const LinkHead* current = &(head_);

        while (current->succ[i]) {
            os << static_cast<Link*>(current->succ[i])->value << " ";
            current = current->succ[i];
        }
        os << "\n";
    }
}
#include<functional>
#include<list>
#include<stdexcept>
#include<utility>
#include<vector>
#include<iterator>
#include<cmath>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> > class HashMap {
private:
    class table_element {
        public:
        std::pair<const KeyType, ValueType> value;
        typename std::list<std::pair<const KeyType, ValueType>*>::iterator it;
        table_element() {}
        table_element(std::pair<const KeyType, ValueType> new_value) : value(new_value) {}
    };
    ValueType _default = ValueType();
    size_t cnt_elements = 0;
    size_t array_length = 64;
    std::vector<std::list<table_element>> container;
    std::list<std::pair<const KeyType, ValueType>*> all_values;
    
    Hash hasher;
    size_t get_hash(const KeyType& key) const {
        return hasher(key);
    }


public:
    class const_iterator;
    class iterator {
    public:
        typename std::list<std::pair<const KeyType, ValueType>*>::iterator value;
        iterator() {}
        iterator(typename std::list<std::pair<const KeyType, ValueType>*>::iterator new_value) : value(new_value) {}
        iterator(const const_iterator &obj): value(obj.value){}
        iterator(const iterator &obj) : value(obj.value){}
        std::pair<const KeyType, ValueType> &operator*() { return (**value); }
        std::pair<const KeyType, ValueType> *operator->() { return &(**value); }
        bool operator==(const iterator &obj) const { return value == obj.value; }
        bool operator!=(const iterator &obj) const { return value != obj.value; }
        iterator &operator++() { ++value; return *this; }
        iterator operator++(int) { typename std::list<std::pair<const KeyType, ValueType>*>::iterator temp = value; ++value; return temp; }
        iterator &operator--() { --value; return *this; }
        iterator operator--(int) { typename std::list<std::pair<const KeyType, ValueType>*>::iterator temp = value; --value; return temp; }
    };

    class const_iterator {
    public:
        typename std::list<std::pair<const KeyType, ValueType>*>::const_iterator value;
        const_iterator() {}
        const_iterator(const iterator& iter) : value(iter.value) {}
        const_iterator(typename std::list<std::pair<const KeyType, ValueType>*>::const_iterator new_value) : value(new_value) {}
        const_iterator(typename std::list<std::pair<const KeyType, ValueType>*>::iterator new_value) : value(new_value) {}
        const_iterator(const const_iterator &obj): value(obj.value) {}
        const std::pair<const KeyType, ValueType> &operator*() const { return (**value); }
        const std::pair<const KeyType, ValueType> *operator->() const { return &(**value); }
        bool operator==(const const_iterator &obj) const { return value == obj.value; }
        bool operator!=(const const_iterator &obj) const { return value != obj.value; }
        const_iterator &operator++() { ++value; return *this; }
        const_iterator operator++(int) { typename std::list<std::pair<const KeyType, ValueType>*>::const_iterator temp = value; ++value; return temp; }
        const_iterator &operator--() { --value; return *this; }
        const_iterator operator--(int) { typename std::list<std::pair<const KeyType, ValueType>*>::const_iterator temp = value; --value; return temp; }
    };
    
    HashMap(Hash new_hasher = Hash()) : hasher(new_hasher) {
        container.resize(array_length);
    }

    template<typename It>
    HashMap(It _begin, It _end, Hash new_hasher = Hash()) : hasher(new_hasher) {
        container.resize(array_length);
        while (_begin != _end) {
            insert(*_begin);
            ++_begin;
        }
    }

    HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> init, Hash new_hasher = Hash()) : hasher(new_hasher) {
        auto _begin = init.begin();
        auto _end = init.end();
        container.resize(array_length);
        while (_begin != _end) {
            insert(*_begin);
            ++_begin;
        }
    }

    Hash hash_function() const {
        return hasher;
    }

    iterator begin() {
        return iterator(all_values.begin());
    }

    iterator end() {
        return iterator(all_values.end());
    }
    
    const_iterator begin() const {
        return const_iterator(all_values.begin());
    }

    const_iterator end() const {
        return const_iterator(all_values.end());
    }

    iterator find(const KeyType& key) {
        size_t hashed_key = get_hash(key);
        hashed_key %= array_length;
        auto it = container[hashed_key].begin();
        while (it != container[hashed_key].end()) {
            if ((*it).value.first == key) {
                return (*it).it;
            }
            ++it;
        }
        return iterator(all_values.end());
    }

    const_iterator find(const KeyType& key) const {
        size_t hashed_key = get_hash(key);
        hashed_key %= array_length;
        auto it = container[hashed_key].begin();
        while (it != container[hashed_key].end()) {
            if ((*it).value.first == key) {
                return (*it).it;
            }
            ++it;
        }
        return const_iterator(all_values.end());
    }

    void insert(std::pair<const KeyType, ValueType> value) {
        if (find(value.first) != all_values.end()) {
            return;
        }
        size_t hashed_key = get_hash(value.first);
        hashed_key %= array_length;
        table_element current_element(value);
        container[hashed_key].push_back(current_element);
        ++cnt_elements;
        all_values.push_back(&(container[hashed_key].back().value));
        container[hashed_key].back().it = all_values.end();
        --container[hashed_key].back().it;
        if (cnt_elements * 4 > array_length) {
            rebuild(array_length * 4);
        }
    }

    void erase(const KeyType& key) {
        iterator current_it = find(key);
        if (current_it == all_values.end()) {
            return;
        }
        --cnt_elements;
        size_t hashed_key = get_hash(key);
        hashed_key %= array_length;
        auto it = container[hashed_key].begin();
        while (it != container[hashed_key].end()) {
            if ((*it).value.first == key) {
                all_values.erase((*it).it);
                container[hashed_key].erase(it);
                return;
            }
            ++it;
        }
        if ((cnt_elements + 10) * 10 < array_length) {
            rebuild(array_length / 4);
        }
    }

    ValueType& operator[] (KeyType key) {
        if (find(key) == all_values.end()) {
            insert({key, ValueType()});
        }
        auto it = find(key);
        return (it->second);
    }
    
    const ValueType& at(const KeyType key) const {
        if (find(key) == all_values.end()) {
            throw std::out_of_range("Key out of range");
        }
        const_iterator it = find(key);
        return (it->second);
    }

    void clear(size_t new_size = 64) {
        all_values.clear();
        container.clear();
        cnt_elements = 0;
        array_length = new_size;
        container.resize(new_size);
    }

    void rebuild(size_t new_size) {
        std::vector<std::pair<const KeyType, ValueType>> temp;
        iterator it = begin();
        while (it != end()) {
            temp.push_back(*it);
            ++it;
        }
        clear(new_size);
        for (auto& elem : temp) {
            insert(elem);
        }
    }

    int size() const {
        return cnt_elements;
    }

    bool empty() const {
        return cnt_elements == 0;
    }

    HashMap(const HashMap& to_cpy) {
        clear();
        auto it = to_cpy.begin();
        while (it != to_cpy.end()) {
            insert(*it);
            ++it;
        }
    }

    HashMap& operator=(const HashMap to_cpy) {
        clear();
        const_iterator it = to_cpy.begin();
        const_iterator _end = to_cpy.end();
        while (it != _end) {
            auto elem = *it;
            insert(elem);
            ++it;
        }
        return *this;
    }
};


#ifndef HASHINDEX_H
#define HASHINDEX_H

#include <optional>
#include <QtContainerFwd>
#include <QList>
#include <QHash>
#include <QStack>
#include <QVector>
#include <type_traits>

/**
 * @class HashIndex
 * @brief Sequence + hash-index container with stable integer indices.
 *
 * Provides O(1) lookup by key, recycled slots, default-constructing operator[],
 * and range-iteration returning (key,value) pairs.
 */
template<typename Key, typename T>
class HashIndex
{
public:
    using Index = int;
    static constexpr Index INVALID = -1;

    struct Entry {
        const Key&  key;
        T& value;
        const Index idx;
    };

    struct ConstEntry {
        const Key&   key;
        const T&     value;
        const Index  idx;
    };

    HashIndex() = default;
    ~HashIndex() = default;

    // Emplace value; returns index or INVALID if key exists
    template<typename K, typename... Args>
    Index emplace(K&& key, Args&&... args) {
        static_assert(std::is_convertible_v<std::decay_t<K>, Key>, "Key type mismatch in emplace");

        if (keyToIndex.contains(key)) {
            return INVALID;
        }

        Key keyCopy = key;
        Index idx = !freeIndices.isEmpty() ? freeIndices.pop() : elements.size();

        if (idx >= elements.size()) {
            elements.emplace_back(std::in_place, std::forward<Args>(args)...);
        } else {
            elements[idx].emplace(std::forward<Args>(args)...);
        }

        if (idx >= indexToKey.size()) {
            indexToKey.resize(idx + 1);
        }
        indexToKey[idx] = std::move(keyCopy);
        keyToIndex.emplace(indexToKey[idx], idx);

        return idx;
    }

    Index insert(const Key& key, const T& value) {
        return emplace(key, value);
    }

    inline Index indexOf(const Key& key) const {
        auto it = keyToIndex.find(key);
        return it != keyToIndex.end() ? it.value() : INVALID;
    }

    inline const Key& keyAt(const Index idx) const {
        static const Key emptyKey = Key{};
        return isValidIndex(idx) ? indexToKey[idx] : emptyKey;
    }

    inline T* at(const Index idx) {
        return isValidIndex(idx) ? &(*elements[idx]) : nullptr;
    }
    inline const T* at(const Index idx) const {
        return isValidIndex(idx) ? &(*elements[idx]) : nullptr;
    }

    // operator[] returns Proxy, inserting default if missing
    inline T& operator[](const Key& key) {
        Index idx = indexOf(key);
        if (idx == INVALID) {
            idx = insert(key, T{});
            if (idx == INVALID) {
                throw std::runtime_error("Insertion failed");
            }
        }
        return *elements[idx];
    }

    // (optional) constant version:
    inline const T& operator[](const Key& key) const {
        Index i = indexOf(key);
        static const T dummy{};
        return i != INVALID ? *elements[i] : dummy;
    }

    bool remove(const Key& key) {
        const Index idx = indexOf(key);
        if (idx == INVALID) {
            return false;
        }

        keyToIndex.remove(key);
        elements[idx].reset();
        freeIndices.push(idx);
        return true;
    }

    inline void clear() {
        elements.clear();
        keyToIndex.clear();
        indexToKey.clear();
        freeIndices.clear();
    }

    inline void reserve(const int n, const bool reserveFree = false) {
        elements.reserve(n);
        keyToIndex.reserve(n);
        indexToKey.reserve(n);
        if (reserveFree) {
            freeIndices.reserve(n);
        }
    }

    inline bool isValidIndex(Index idx) const {
        return idx >= 0 && idx < elements.size() && elements[idx].has_value();
    }

    inline int size() const { return keyToIndex.size(); }
    inline bool isEmpty() const { return keyToIndex.isEmpty(); }

    class iterator {
    public:
        using value_type        = Entry;
        using reference         = Entry;
        using pointer           = Entry*;
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = qsizetype;

        iterator(HashIndex& c, qsizetype i): cont(c), idx(i) { skip(); }
        iterator& operator=(const iterator& o) { cont = o.cont; idx = o.idx; return *this; }
        reference operator*() const {
            return Entry {
                cont.indexToKey[idx],
                *cont.elements[idx],
                static_cast<Index>(idx)
            };
        }
        iterator& operator++() { ++idx; skip(); return *this; }
        bool operator!=(const iterator& o) const { return idx != o.idx; }
        bool operator==(const iterator& o) const { return idx == o.idx; }
        qsizetype getIndex() const { return idx; }
    private:
        void skip() { while (idx < cont.elements.size() && !cont.isValidIndex(idx)) ++idx; }
        HashIndex& cont;
        qsizetype idx;
    };
    // generators begin/end
    iterator begin() { return { *this, 0 }; }
    iterator end()   { return { *this, elements.size() }; }

    class const_iterator {
    public:
        using value_type        = ConstEntry;
        using reference         = ConstEntry;
        using pointer           = const ConstEntry*;
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = qsizetype;

        const_iterator(const HashIndex& c, qsizetype i): cont(c), idx(i) { skip(); }
        reference operator*() const {
            return ConstEntry{
                cont.indexToKey[idx],
                *cont.elements[idx],        // const T&
                static_cast<Index>(idx)
            };
        }
        const_iterator& operator++() { ++idx; skip(); return *this; }
        bool operator!=(const const_iterator& o) const { return idx != o.idx; }
        bool operator==(const const_iterator& o) const { return idx == o.idx; }
        qsizetype getIndex() const { return idx; }
    private:
        void skip() { while (idx < cont.elements.size() && !cont.isValidIndex(idx)) ++idx; }
        const HashIndex& cont;
        qsizetype idx;
    };

    // generators begin/end
    const_iterator begin() const { return { *this, 0 }; }
    const_iterator end()   const { return { *this, elements.size() }; }
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }

    // Find returns iterator to element or end()
    iterator find(const Key& key) {
        Index idx = indexOf(key);
        return idx != INVALID ? iterator(*this, idx) : end();
    }
    const_iterator find(const Key& key) const {
        Index idx = indexOf(key);
        return idx != INVALID ? const_iterator(*this, idx) : end();
    }

    void compact() {
        QList<std::optional<T>> newElements;
        newElements.reserve(size());

        QList<Key> newIndexToKey;
        newIndexToKey.reserve(size());

        QHash<Key, Index> newKeyToIndex;
        newKeyToIndex.reserve(size());

        for (Index i = 0; i < elements.size(); ++i) {
            if (elements[i].has_value()) {
                newKeyToIndex.emplace(indexToKey[i], newElements.size());
                newIndexToKey.emplace_back(indexToKey[i]);
                newElements.emplace_back(std::move(elements[i]));
            }
        }

        elements = std::move(newElements);
        indexToKey = std::move(newIndexToKey);
        keyToIndex = std::move(newKeyToIndex);
        freeIndices.clear();
    }

public:
    const auto& getElements() const { return elements; }
private:
    QList<std::optional<T>> elements;
    QHash<Key, Index>       keyToIndex;
    QList<Key>              indexToKey;
    QStack<Index>           freeIndices;
};

#endif // HASHINDEX_H

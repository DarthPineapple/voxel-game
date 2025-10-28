#ifndef TUPLE_HASH_H
#define TUPLE_HASH_H

#include <tuple>
#include <functional>

// Hash function for std::tuple<int, int, int> to use with unordered_map
// This allows efficient O(1) lookups for chunk coordinates
struct TupleHash {
    std::size_t operator()(const std::tuple<int, int, int>& t) const {
        // Combine hash values of the three integers using bit shifting
        auto h1 = std::hash<int>{}(std::get<0>(t));
        auto h2 = std::hash<int>{}(std::get<1>(t));
        auto h3 = std::hash<int>{}(std::get<2>(t));
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

#endif // TUPLE_HASH_H

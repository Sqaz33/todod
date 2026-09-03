#pragma once

#include <cstdint>
#include <vector>


namespace todod::domain {

template <class Item>
struct Page {
    std::vector<Iteim> items;
    struct {
        std::int32_t total;
        std::int32_t offset;
        std::int32_t limit;
    } meta;
};

}
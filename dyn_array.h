#pragma once

#include <cstdint>

class DynArray
{
  public:
    DynArray(std::uint32_t element_size, std::uint32_t capacity = 2u);

    auto begin() const -> void *;

    auto end() const -> void *;

    auto size() const -> std::uint32_t;

    auto push_back(void *data) -> void *;

    auto erase(void *data) -> void;

  private:
    std::uint32_t element_size_;
    std::uint8_t *begin_;
    std::uint8_t *end_;
    std::uint8_t *capacity_;
};

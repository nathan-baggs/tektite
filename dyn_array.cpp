#include "dyn_array.h"

#include <cstdint>

#include "clib.h"
#include "log.h"

DynArray::DynArray(std::uint32_t element_size, std::uint32_t capacity)
    : element_size_(element_size)
    , begin_(reinterpret_cast<std::uint8_t *>(malloc(element_size_ * capacity)))
    , end_(begin_)
    , capacity_(reinterpret_cast<std::uint8_t *>(begin_) + (element_size_ * capacity))
{
}

auto DynArray::begin() const -> void *
{
    return begin_;
}

auto DynArray::end() const -> void *
{
    return end_;
}

auto DynArray::size() const -> std::uint32_t
{
    return (end_ - begin_) / element_size_;
}

auto DynArray::push_back(void *data) -> void *
{
    if (end_ == capacity_)
    {
        const auto size = (end_ - begin_) / element_size_;
        const auto total_size = (capacity_ - begin_) / element_size_;

        auto new_begin = reinterpret_cast<std::uint8_t *>(malloc(total_size * 2u * element_size_));
        memcpy(new_begin, begin_, size * element_size_);

        free(begin_);
        begin_ = new_begin;
        end_ = begin_ + (size * element_size_);
        capacity_ = begin_ + (total_size * 2u * element_size_);
    }

    memcpy(end_, data, element_size_);

    auto *new_element = end_;
    end_ += element_size_;

    return new_element;
}

auto DynArray::erase(void *data) -> void
{
    for (auto *cursor = begin_; cursor != end_; cursor += element_size_)
    {
        if (memcmp(cursor, data, element_size_) == 0)
        {
            memcpy(cursor, cursor + element_size_, end_ - cursor + element_size_);
            end_ -= element_size_;
            break;
        }
    }
}

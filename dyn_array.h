#pragma once

#include <cstdint>

/**
 * A terrible dynamic array class.
 *
 * It is not exception safe or type safe and doesn't do any cleanup.
 */
class DynArray
{
  public:
    /**
     * Construct a new dynamic array.
     *
     * @param element_size
     *   The size of the elements in the array.
     *
     * @param capacity
     *   Initial capacity of the array.
     */
    DynArray(std::uint32_t element_size, std::uint32_t capacity = 2u);

    /**
     * Get a pointer to the first element in the array.
     *
     * @return
     *   A pointer to the first element in the array.
     */
    auto begin() const -> void *;

    /**
     * Get a pointer to one past the last element of the array.
     *
     * @return
     *  A pointer to the end of the array.
     */
    auto end() const -> void *;

    /**
     * Get the size of an element in the array.
     *
     * @return
     *  The size of an element in the array.
     */
    auto element_size() const -> std::uint32_t;

    /**
     * Get the number of elements in the array.
     *
     * @return
     * The number of elements in the array.
     */
    auto size() const -> std::uint32_t;

    /**
     * Add an element to the end of the array, resizing if necessary.
     *
     * @param data
     *   The data to add to the array, hope it's the right size.
     *
     * @return
     *  A pointer to the newly added element.
     */
    auto push_back(void *data) -> void *;

    /**
     * Erase an element from the array.
     *
     * @param data
     *  The element to erase, hope it's the right size.
     *
     *  @return
     *  A pointer to the element after the erased element.
     */
    auto erase(void *data) -> void *;

  private:
    /** The size of the elements in the array. */
    std::uint32_t element_size_;

    /** The beginning of the array. */
    std::uint8_t *begin_;

    /** The end of the elements in the array. */
    std::uint8_t *end_;

    /** The end of the allocated memory. */
    std::uint8_t *capacity_;
};

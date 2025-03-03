#pragma once

#include <cstdint>

#include "clib.h"
#include "log.h"
#include "vertex_data.h"

static constexpr VertexData g_cube_vertices[] = {
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}};

static constexpr std::uint32_t g_cube_indices[] = {0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,
                                                   8,  9,  10, 10, 11, 8,  12, 13, 14, 14, 15, 12,
                                                   16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20};

inline void generate_sphere(
    std::uint32_t sector_count,
    std::uint32_t stack_count,
    VertexData **vertices,
    std::uint32_t *vertex_count,
    std::uint32_t **indices,
    std::uint32_t *index_count)
{
    log("generating sphere");
    const auto sector_step = static_cast<float>(2.0f * M_PI / sector_count);
    const auto stack_step = static_cast<float>(M_PI / stack_count);

    *vertex_count = (stack_count + 1) * (sector_count + 1);
    *index_count = stack_count * sector_count * 6;

    *vertices = static_cast<VertexData *>(malloc(*vertex_count * sizeof(VertexData)));
    *indices = static_cast<std::uint32_t *>(malloc(*index_count * sizeof(std::uint32_t)));

    log("created vertices");

    auto vertex_cursor = 0u;
    for (auto i = 0u; i <= stack_count; ++i)
    {
        const auto theta = i * stack_step;
        const auto sin_theta = sin(theta);
        const auto cos_theta = cos(theta);

        for (auto j = 0u; j <= sector_count; ++j)
        {
            const auto phi = j * sector_step;
            const auto sin_phi = sin(phi);
            const auto cos_phi = cos(phi);

            auto tangent_x = -sin_theta * sin_phi;
            auto tangent_y = sin_theta * cos_phi;
            auto tangent_z = 0.0f;

            if ((sin_theta < 1e-6f) && (sin_theta > -1e-6f))
            {
                tangent_x = 1.0f;
                tangent_y = 0.0f;
                tangent_z = 0.0f;
            }

            (*vertices)[vertex_cursor++] = {
                {sin_theta * cos_phi, sin_theta * sin_phi, cos_theta},
                {sin_theta * cos_phi, sin_theta * sin_phi, cos_theta},
                {tangent_x, tangent_y, tangent_z},
                {static_cast<float>(j) / sector_count, static_cast<float>(i) / stack_count}};
        }
    }

    log("creating indices");

    auto index_cursor = 0u;

    for (auto i = 0u; i < stack_count; ++i)
    {
        for (auto j = 0u; j < sector_count; ++j)
        {
            const auto k1 = i * (sector_count + 1) + j;
            const auto k2 = k1 + sector_count + 1;

            (*indices)[index_cursor++] = k1;
            (*indices)[index_cursor++] = k2;
            (*indices)[index_cursor++] = k1 + 1;

            (*indices)[index_cursor++] = k1 + 1;
            (*indices)[index_cursor++] = k2;
            (*indices)[index_cursor++] = k2 + 1;
        }
    }

    log("created sphere");
}

inline void generate_cylinder(
    std::uint32_t sector_count,
    VertexData **vertices,
    std::uint32_t *vertex_count,
    std::uint32_t **indices,
    std::uint32_t *index_count)
{
    const auto sector_step = static_cast<float>(2.0f * M_PI / sector_count);
    const auto half_height = 1.0f;

    *vertex_count = (sector_count + 1) * 2 + (sector_count + 2) * 2;
    *index_count = sector_count * 6 + sector_count * 6;

    *vertices = static_cast<VertexData *>(malloc(*vertex_count * sizeof(VertexData)));
    *indices = static_cast<std::uint32_t *>(malloc(*index_count * sizeof(std::uint32_t)));

    auto vertex_index = 0u;
    auto index = 0u;

    for (auto i = 0u; i <= sector_count; ++i)
    {
        const auto sector_angle = i * sector_step;
        const auto x = cos(sector_angle);
        const auto y = sin(sector_angle);

        (*vertices)[vertex_index++] = {
            {x, y, -half_height}, {x, y, 0.0f}, {-y, x, 0.0f}, {static_cast<float>(i) / sector_count, 0.0f}};
        (*vertices)[vertex_index++] = {
            {x, y, half_height}, {x, y, 0.0f}, {-y, x, 0.0f}, {static_cast<float>(i) / sector_count, 1.0f}};
    }

    for (auto i = 0u; i < sector_count; ++i)
    {
        const auto cur = i * 2;
        const auto next = cur + 2;

        (*indices)[index++] = cur;
        (*indices)[index++] = cur + 1;
        (*indices)[index++] = next;

        (*indices)[index++] = cur + 1;
        (*indices)[index++] = next + 1;
        (*indices)[index++] = next;
    }

    const auto top_centre_index = vertex_index;

    (*vertices)[vertex_index++] = {{0.0f, 0.0f, half_height}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.5f, 0.5f}};

    for (auto i = 0u; i <= sector_count; ++i)
    {
        const auto sector_angle = i * sector_step;
        const auto x = cos(sector_angle);
        const auto y = sin(sector_angle);

        (*vertices)[vertex_index++] = {
            {x, y, half_height},
            {0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 0.0f},
            {static_cast<float>(x * 0.5f + 0.5f), static_cast<float>(y * 0.5f + 0.5f)}};
    }

    const auto bottom_centre_index = vertex_index;
    (*vertices)[vertex_index++] = {{0.0f, 0.0f, -half_height}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.5f, 0.5f}};

    for (auto i = 0u; i <= sector_count; ++i)
    {
        const auto sector_angle = i * sector_step;
        const auto x = cos(sector_angle);
        const auto y = sin(sector_angle);

        (*vertices)[vertex_index++] = {
            {x, y, -half_height},
            {0.0f, 0.0f, -1.0f},
            {1.0f, 0.0f, 0.0f},
            {static_cast<float>(x * 0.5f + 0.5f), static_cast<float>(y * 0.5f + 0.5f)}};
    }

    for (auto i = 0u; i < sector_count; ++i)
    {
        const auto cur = top_centre_index + 1 + i;
        (*indices)[index++] = top_centre_index;
        (*indices)[index++] = cur;
        (*indices)[index++] = cur + 1;
    }

    for (auto i = 0u; i < sector_count; ++i)
    {
        const auto cur = bottom_centre_index + 1 + i;
        (*indices)[index++] = bottom_centre_index;
        (*indices)[index++] = cur + 1;
        (*indices)[index++] = cur;
    }
}

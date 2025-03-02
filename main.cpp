#include <cstdint>

#include <Windows.h>

#include "window.h"

#include "buffer.h"
#include "camera.h"
#include "event.h"
#include "log.h"
#include "material.h"
#include "matrix4.h"
#include "mesh.h"
#include "opengl.h"
#include "quaternion.h"
#include "shader.h"
#include "shapes.h"
#include "vector3.h"
#include "vertex_data.h"

// https://stackoverflow.com/a/1583220
EXTERN_C int _fltused = 0;

const auto *vertex_shader_src = R"(
    #version 460 core

    layout(std140, binding = 0) uniform camera
    {
        mat4 view;
        mat4 projection;
        vec3 eye;
    };

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aCol;

    out vec3 vCol;

    uniform mat4 model;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        vCol = aCol;
    }
)";

const auto *fragment_shader_src = R"(
    #version 460 core

    in vec3 vCol;

    out vec4 FragColor;

    void main()
    {
        FragColor = vec4(vCol, 1.0);
    }
)";

auto main() -> int
{
    static const auto width = 1920u;
    static const auto height = 1080u;

    log("starting");

    auto window = Window{width, height};

    auto vertex_shader = Shader{vertex_shader_src, ShaderType::VERTEX};
    auto fragment_shader = Shader{fragment_shader_src, ShaderType::FRAGMENT};

    auto material = Material{vertex_shader, fragment_shader};

    const auto cube_mesh = Mesh{
        g_cube_vertices,
        sizeof(g_cube_vertices) / sizeof(VertexData),
        g_cube_indices,
        sizeof(g_cube_indices) / sizeof(std::uint32_t)};

    const auto sphere_mesh = Mesh{
        g_sphere_vertices,
        sizeof(g_sphere_vertices) / sizeof(VertexData),
        g_sphere_indices,
        sizeof(g_sphere_indices) / sizeof(std::uint32_t)};

    const auto cylinder_mesh = Mesh{
        g_cylinder_vertices,
        sizeof(g_cylinder_vertices) / sizeof(VertexData),
        g_cylinder_indices,
        sizeof(g_cylinder_indices) / sizeof(std::uint32_t)};

    auto camera =
        Camera{{0.0f, 0.0f, 15.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, M_PI / 4.0f, width, height, 0.1f, 100.0f};
    const auto camera_buffer = Buffer{sizeof(Matrix4) * 2 + sizeof(Vector3)};

    auto cube_model = Matrix4{{5.0f, 5.0f, 5.0f}, Matrix4::Scale{}};
    auto sphere_model = Matrix4{{6.0f, 0.0f, 0.0f}, {3.0f, 3.0f, 3.0f}};
    auto cylinder_model = Matrix4{{-6.0f, 0.0f, 0.0f}, {3.0f, 3.0f, 3.0f}};

    auto move_forward = false;
    auto move_backward = false;
    auto move_left = false;
    auto move_right = false;

    while (window.running())
    {
        Event evt{};
        auto has_event = window.pump_message(&evt);
        while (has_event)
        {
            switch (evt.type)
            {
                using enum EventType;
                case KEY_DOWN:
                {
                    switch (evt.data.key)
                    {
                        case 'W': move_forward = true; break;
                        case 'S': move_backward = true; break;
                        case 'A': move_left = true; break;
                        case 'D': move_right = true; break;
                    }
                    break;
                }
                case KEY_UP:
                {
                    switch (evt.data.key)
                    {
                        case 'W': move_forward = false; break;
                        case 'S': move_backward = false; break;
                        case 'A': move_left = false; break;
                        case 'D': move_right = false; break;
                    }
                    break;
                }
                case MOUSE_MOVE:
                {
                    static constexpr auto sensitivity = float{0.002f};
                    const auto delta_x = evt.data.mouse_move.delta_x * sensitivity;
                    const auto delta_y = evt.data.mouse_move.delta_y * sensitivity;
                    camera.adjust_yaw(delta_x);
                    camera.adjust_pitch(-delta_y);
                    break;
                }
            }

            has_event = window.pump_message(&evt);
        }

        auto walk_direction = Vector3{};
        if (move_forward)
        {
            walk_direction += camera.direction();
        }
        if (move_backward)
        {
            walk_direction -= camera.direction();
        }
        if (move_left)
        {
            walk_direction -= camera.right();
        }
        if (move_right)
        {
            walk_direction += camera.right();
        }

        const auto speed = 0.4f;
        if (walk_direction != Vector3{})
        {
            camera.translate(Vector3::normalise(walk_direction) * speed);
        }

        ::glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        material.use();

        const auto camera_pos = camera.position();

        camera_buffer.write(reinterpret_cast<const std::uint8_t *>(camera.view()), sizeof(Matrix4), 0);
        camera_buffer.write(
            reinterpret_cast<const std::uint8_t *>(camera.projection()), sizeof(Matrix4), sizeof(Matrix4));
        camera_buffer.write(reinterpret_cast<const std::uint8_t *>(&camera_pos), sizeof(Vector3), sizeof(Matrix4) * 2);
        ::glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_buffer.native_handle());

        material.set_uniform("model", cube_model);

        cube_mesh.bind();
        ::glDrawElements(
            GL_TRIANGLES, cube_mesh.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(cube_mesh.index_offset()));
        cube_mesh.unbind();

        material.set_uniform("model", sphere_model);

        sphere_mesh.bind();
        ::glDrawElements(
            GL_TRIANGLES,
            sphere_mesh.index_count(),
            GL_UNSIGNED_INT,
            reinterpret_cast<void *>(sphere_mesh.index_offset()));
        sphere_mesh.unbind();

        material.set_uniform("model", cylinder_model);

        cylinder_mesh.bind();
        ::glDrawElements(
            GL_TRIANGLES,
            cylinder_mesh.index_count(),
            GL_UNSIGNED_INT,
            reinterpret_cast<void *>(cylinder_mesh.index_offset()));
        cylinder_mesh.unbind();

        window.swap();
    }

    log("stopping");

    // avoid cleanup, just die
    ::ExitProcess(0);
}

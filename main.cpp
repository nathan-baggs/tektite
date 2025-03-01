#include <cstdint>

#include <Windows.h>

#include "window.h"

#include "buffer.h"
#include "camera.h"
#include "log.h"
#include "material.h"
#include "matrix4.h"
#include "mesh.h"
#include "opengl.h"
#include "quaternion.h"
#include "shader.h"
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

    const VertexData vertices[] = {
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},

        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}},
        {{0.5f, 0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}}};

    const std::uint32_t indices[] = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 0, 3, 7, 7, 4, 0,
                                     1, 5, 6, 6, 2, 1, 0, 1, 5, 5, 4, 0, 3, 2, 6, 6, 7, 3};

    auto vertex_shader = Shader{vertex_shader_src, ShaderType::VERTEX};
    auto fragment_shader = Shader{fragment_shader_src, ShaderType::FRAGMENT};

    auto material = Material{vertex_shader, fragment_shader};

    const auto mesh =
        Mesh{vertices, sizeof(vertices) / sizeof(vertices[0]), indices, sizeof(indices) / sizeof(indices[0])};

    auto camera =
        Camera{{0.0f, 0.0f, 15.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, M_PI / 4.0f, width, height, 0.1f, 100.0f};
    const auto camera_buffer = Buffer{sizeof(Matrix4) * 2 + sizeof(Vector3)};

    auto view = Matrix4{
        {1.00,
         0.00,
         -4.37114E-08,
         2.18557E-06,
         0.00,
         1.00,
         0.00,
         0.00,
         4.37114E-08,
         0.00,
         1.00,
         0.00,
         0.00,
         0.00,
         -5.00,
         1.00}};

    auto cube_model = Matrix4{{5.0f, 5.0f, 5.0f}, Matrix4::Scale{}};

    while (window.running())
    {
        window.pump_message();

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

        mesh.bind();
        ::glDrawElements(
            GL_TRIANGLES, mesh.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(mesh.index_offset()));
        mesh.unbind();

        window.swap();
    }

    log("stopping");

    // avoid cleanup, just die
    ::ExitProcess(0);
}

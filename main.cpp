#include <cstdint>

#include <Windows.h>

#include "window.h"

#include "log.h"
#include "material.h"
#include "mesh.h"
#include "opengl.h"
#include "shader.h"
#include "vertex_data.h"

// https://stackoverflow.com/a/1583220
EXTERN_C int _fltused = 0;

// Vertex Shader source code
const char *vertex_shader_src = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aCol;

    out vec3 vCol;

    void main() {
        gl_Position = vec4(aPos, 1.0);
        vCol = aCol;
    }
)";

// Fragment Shader source code
const char *fragment_shader_src = R"(
    #version 330 core
    in vec3 vCol;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vCol, 1.0);
    }
)";

auto main() -> int
{
    static const auto width = 1920u;
    static const auto height = 1080u;

    log("starting");

    auto window = Window{width, height};

    VertexData vertices[] = {
        {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}};

    std::uint32_t indices[] = {0, 1, 2};

    auto vertex_shader = Shader{vertex_shader_src, ShaderType::VERTEX};
    auto fragment_shader = Shader{fragment_shader_src, ShaderType::FRAGMENT};

    auto material = Material{vertex_shader, fragment_shader};

    auto mesh = Mesh{vertices, 3, indices, 3};

    while (window.running())
    {
        window.pump_message();

        ::glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        material.use();
        mesh.bind();
        ::glDrawElements(
            GL_TRIANGLES, mesh.index_count(), GL_UNSIGNED_INT, reinterpret_cast<void *>(mesh.index_offset()));

        window.swap();
    }

    log("stopping");

    // avoid cleanup, just die
    ::ExitProcess(0);
}

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

#pragma warning(push)
#pragma warning(disable : 4324)
struct PointLightBuffer
{
    alignas(16) Vector3 position;
    alignas(16) Vector3 colour;
    alignas(16) Vector3 attenuation;
};
#pragma warning(pop)

const auto *vertex_shader_src = R"(
    #version 460 core

    layout(std140, binding = 0) uniform camera
    {
        mat4 view;
        mat4 projection;
        vec3 eye;
    };

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec3 aTangent;
    layout (location = 3) in vec3 aUv;

    out vec3 vNormal;
    out vec3 vUv;
    out vec4 vPos;
    out mat3 tbn;

    uniform mat4 model;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        vNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
        vUv = aUv;

        vec3 t = normalize(vec3(model * vec4(aTangent, 0.0)));
        vec3 n = normalize(vec3(model * vec4(aNormal, 0.0)));
        vec3 b = cross(n, t);
        tbn = mat3(t, b, n);

        vPos = model * vec4(aPos, 1.0);
    }
)";

const auto *fragment_shader_src = R"(
    #version 460 core

    layout(std140, binding = 0) uniform camera
    {
        mat4 view;
        mat4 projection;
        vec3 eye;
    };

    struct PointLight
    {
        vec3 point;
        vec3 point_colour;
        vec3 attenuation;
    };

    layout(std430, binding = 1) readonly buffer lights
    {
        int num_points;
        PointLight points[];
    };

    in vec3 vNormal;
    in vec3 vUv;
    in vec4 vPos;
    in mat3 tbn;

    out vec4 FragColor;

    vec3 calc_point(int index)
    {
        vec3 point = points[index].point;
        vec3 point_colour = points[index].point_colour;
        vec3 attenuation = points[index].attenuation;

        vec3 n = normalize(vNormal);
        n = normalize(tbn * n);

        float distance = length(point - vPos.xyz);
        float att = 1.0 / (attenuation.x + (attenuation.y * distance) + (attenuation.z * (distance * distance)));

        vec3 light_dir = normalize(point - vPos.xyz);
        float diff = max(dot(n, light_dir), 0.0);

        vec3 reflect_dir = reflect(-light_dir, n);
        float spec = pow(max(dot(normalize(eye - vPos.xyz), reflect_dir), 0.0), 32);

        return ((diff + spec) * att) * point_colour;
    }


    void main()
    {
        vec4 albedo = vec4(1.0, 0.0, 0.0, 1.0);
        vec3 colour = vec3(0.5);

        for (int i = 0; i < num_points; ++i)
        {
            colour += calc_point(i);
        }

        FragColor = vec4(colour * albedo.rgb, 1.0);
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

    VertexData *sphere_vertices{};
    auto sphere_vertex_count = std::uint32_t{};
    std::uint32_t *sphere_indices{};
    auto sphere_index_count = std::uint32_t{};
    generate_sphere(10, 10, &sphere_vertices, &sphere_vertex_count, &sphere_indices, &sphere_index_count);

    const auto sphere_mesh = Mesh{sphere_vertices, sphere_vertex_count, sphere_indices, sphere_index_count};

    VertexData *cylinder_vertices{};
    auto cylinder_vertex_count = std::uint32_t{};
    std::uint32_t *cylinder_indices{};
    auto cylinder_index_count = std::uint32_t{};
    generate_cylinder(10, &cylinder_vertices, &cylinder_vertex_count, &cylinder_indices, &cylinder_index_count);

    auto cylinder_mesh = Mesh{cylinder_vertices, cylinder_vertex_count, cylinder_indices, cylinder_index_count};

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

    auto light_buffer = Buffer{10240u};
    auto player_light = PointLightBuffer{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.09f, 0.032f}};

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

        player_light.position = camera_pos;

        camera_buffer.write(reinterpret_cast<const std::uint8_t *>(camera.view()), sizeof(Matrix4), 0);
        camera_buffer.write(
            reinterpret_cast<const std::uint8_t *>(camera.projection()), sizeof(Matrix4), sizeof(Matrix4));
        camera_buffer.write(reinterpret_cast<const std::uint8_t *>(&camera_pos), sizeof(Vector3), sizeof(Matrix4) * 2);
        ::glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_buffer.native_handle());

        const auto light_count = 1;
        light_buffer.write(reinterpret_cast<const std::uint8_t *>(&light_count), sizeof(int), 0);
        light_buffer.write(reinterpret_cast<const std::uint8_t *>(&player_light), sizeof(PointLightBuffer), 16);
        ::glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, light_buffer.native_handle());

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

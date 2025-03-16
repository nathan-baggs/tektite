#include <cstdint>

#include <Windows.h>

#include "window.h"

#include "buffer.h"
#include "camera.h"
#include "dyn_array.h"
#include "event.h"
#include "func.h"
#include "log.h"
#include "material.h"
#include "matrix4.h"
#include "mesh.h"
#include "opengl.h"
#include "padding.h"
#include "quaternion.h"
#include "scene.h"
#include "shader.h"
#include "shapes.h"
#include "sound_player.h"
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

struct Entity
{
    std::uint32_t sphere_start;
    std::uint32_t sphere_end;
    std::uint32_t cylinder_start;
    std::uint32_t cylinder_end;
    std::uint32_t cube_start;
    std::uint32_t cube_end;
};

struct Bullet
{
    Vector3 position;
    Vector3 velocity;
};

const auto *vertex_shader_src = R"(
    #version 460 core

    layout(std140, binding = 0) uniform camera
    {
        mat4 view;
        mat4 projection;
        vec3 eye;
    };

    struct ModelData
    {
        mat4 model;
        vec3 checker_colour1;
        vec3 checker_colour2;
        vec3 wood_colour1;
        vec3 wood_colour2;
        vec3 wood_colour3;
        float wood_scale;
        vec3 metal_colour;
        vec3 water_colour1;
        vec3 water_colour2;
        float normal_scale;
    };

    layout(std430, binding = 2) buffer model_data
    {
        ModelData data[];
    };

    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec3 aTangent;
    layout (location = 3) in vec2 aUv;

    out vec3 vNormal;
    out vec2 vUv;
    out vec4 vPos;
    out mat3 tbn;
    out flat int instance_id;

    void main()
    {
        mat4 model = data[gl_InstanceID + gl_BaseInstance].model;

        gl_Position = projection * view * model * vec4(aPos, 1.0);
        vNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
        vUv = aUv;

        vec3 t = normalize(vec3(model * vec4(aTangent, 0.0)));
        vec3 n = normalize(vec3(model * vec4(aNormal, 0.0)));
        vec3 b = cross(n, t);
        tbn = mat3(t, b, n);

        vPos = model * vec4(aPos, 1.0);
    
        instance_id = gl_InstanceID + gl_BaseInstance;
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

    struct ModelData
    {
        mat4 model;
        vec3 checker_colour1;
        vec3 checker_colour2;
        vec3 wood_colour1;
        vec3 wood_colour2;
        vec3 wood_colour3;
        float wood_scale;
        vec3 metal_colour;
        vec3 water_colour1;
        vec3 water_colour2;
        float normal_scale;
    };

    layout(std430, binding = 2) buffer model_data
    {
        ModelData data[];
    };

    in vec3 vNormal;
    in vec2 vUv;
    in vec4 vPos;
    in mat3 tbn;
    in flat int instance_id;

    out vec4 FragColor;

    uniform float time;

    float random(vec2 st)   
    {
        return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
    }

    float noise(vec2 st)
    {
        vec2 i = floor(st);
        vec2 f = fract(st);

        float a = random(i);
        float b = random(i + vec2(1.0, 0.0));
        float c = random(i + vec2(0.0, 1.0));
        float d = random(i + vec2(1.0, 1.0));

        vec2 u = f * f * (3.0 - 2.0 * f);

        return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
    }    

    float fbm(vec2 st)
    {
        float value = 0.0;
        float amplitud = .5;
        float frequency = 0.;

        for (int i = 0; i < 6; i++)
        {   
            value += amplitud * noise(st);
            st *= 2.;
            amplitud *= .5;
        }

        return value;
    }

    vec3 calc_point(int index, vec3 normal)
    {
        vec3 point = points[index].point;
        vec3 point_colour = points[index].point_colour;
        vec3 attenuation = points[index].attenuation;

        vec3 n = normalize(normal);
        n = normalize(tbn * n);

        float distance = length(point - vPos.xyz);
        float att = 1.0 / (attenuation.x + (attenuation.y * distance) + (attenuation.z * (distance * distance)));

        vec3 light_dir = normalize(point - vPos.xyz);
        float diff = max(dot(n, light_dir), 0.0);

        vec3 reflect_dir = reflect(-light_dir, n);
        float spec = pow(max(dot(normalize(eye - vPos.xyz), reflect_dir), 0.0), 32);

        return ((diff + spec) * att) * point_colour;
    }

    float sdf_lens(vec2 p, float width, float height)
    {
        float d = height / width - width / 4.0;
        float r = width / 2.0 + d;
        
        p = abs(p);

        float b = sqrt(r * r - d * d);
        vec4 par = p.xyxy - vec4(0.0, b, -d, 0.0);
        return (par.y * d > p.x * b) ? length(par.xy) : length(par.zw) - r;
    }

    vec3 tile_weave(vec2 pos, vec2 scale, float count, float width, float smoothness)
    {
        vec2 i = floor(pos * scale);    
        float c = mod(i.x + i.y, 2.0);
        
        vec2 p = fract(pos.st * scale);
        p = mix(p.st, p.ts, c);
        p = fract(p * vec2(count, 1.0));
        
        width *= 2.0;
        p = p * 2.0 - 1.0;
        float d = sdf_lens(p, width, 1.0);
        vec2 grad = vec2(dFdx(d), dFdy(d));

        float s = 1.0 - smoothstep(0.0, dot(abs(grad), vec2(1.0)) + smoothness, -d);
        return vec3(s, normalize(grad) * smoothstep(1.0, 0.99, s) * smoothstep(0.0, 0.01, s)); 
    }

    vec3 checker_pattern(vec3 colour1, vec3 colour2)
    {
        vec2 uv = vUv * 10.0;
        bool c = (mod(floor(uv.x) + floor(uv.y), 2.0) > 0.0);
        return c ? colour1 : colour2;
    }

    vec3 wood_pattern(vec3 colour1, vec3 colour2, vec3 colour3)
    {
        vec2 st = vUv;
        float v0 = smoothstep(-1.0, 1.0, sin(st.x * 14.0 + fbm(st.xx * vec2(100.0, 12.0)) * 8.0));
        float v1 = random(st);
        float v2 = noise(st * vec2(200.0, 14.0)) - noise(st * vec2(1000.0, 64.0));

        vec3 col = colour1;
        col = mix(col, colour2, v0);
        col = mix(col, colour3, v1 * 0.5);
        col -= v2 * 0.2;
        
        return col;
    }

    vec3 metal_pattern(vec3 colour)
    {
        vec2 uv = vUv * 15.0;

        float n = fbm(uv);

        float streaks = sin(uv.y * 50.0 + n * 10.0);
        streaks = smoothstep(0.4, 0.6, streaks);

        return colour * (0.5 + 0.5 * n) * (0.8 + 0.2 * streaks);
    }

    vec3 water_pattern(vec3 colour1, vec3 colour2)
    {
        vec2 uv = vUv * 5.0;

        float wave = fbm(uv + vec2(0.0, time * 0.1)) - fbm(uv - vec2(0.0, time * 0.1));
        return colour1 + colour2 * wave;
    }

    vec3 metal_bump_normal_pattern(float scale)
    {
        return vec3(tile_weave(fract(vUv * 2), vec2(8.0), 3.0, 0.75, 1).yz, 1.0) * scale;
    }
    
    void main()
    {
        vec3 albedo = checker_pattern(data[instance_id].checker_colour1, data[instance_id].checker_colour2);
        albedo += water_pattern(data[instance_id].water_colour1, data[instance_id].water_colour2);
        
        vec3 colour = vec3(0.3);

        for (int i = 0; i < num_points; ++i)
        {
            colour += calc_point(i, vNormal + metal_bump_normal_pattern(data[instance_id].normal_scale));
        }

        FragColor = vec4(colour * albedo, 1.0);
    }
)";

SECTION("main");
auto loop_audio() -> void
{
    PADDING_START
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    RAW_PADDING_LINE_GREY
    PADDING_END

    auto sound_player = SoundPlayer{};

    for (;;)
    {
        Note notes[] = {
            {293.66, 0.1}, // D4
            {261.63, 0.1}, // C4
            {349.23, 0.3}, // F4
            {329.63, 0.1}, // E4
            {440.00, 0.1}, // A4
            {392.00, 0.1}, // G4
            {493.88, 0.2}, // B4
            {523.25, 0.1}, // C5
        };

        sound_player.play(notes, sizeof(notes) / sizeof(Note));
    }
}

SECTION("main") auto main() -> int
{
    PADDING_LINE_QUARTER;
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
        Camera{{-2.0f, 1.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, M_PI / 4.0f, width, height, 0.1f, 100.0f};
    const auto camera_buffer = Buffer{sizeof(Matrix4) * 2 + sizeof(Vector3)};

    auto model_data_buffer = Buffer{sizeof(ModelData) * max_models_per_type * 3u};
    model_data_buffer.write(
        reinterpret_cast<const std::uint8_t *>(cube_models), sizeof(ModelData) * cube_model_count, 0);
    model_data_buffer.write(
        reinterpret_cast<const std::uint8_t *>(sphere_models),
        sizeof(ModelData) * sphere_model_count,
        sizeof(ModelData) * max_models_per_type);
    model_data_buffer.write(
        reinterpret_cast<const std::uint8_t *>(cylinder_models),
        sizeof(ModelData) * cylinder_model_count,
        sizeof(ModelData) * max_models_per_type * 2u);

    auto move_forward = false;
    auto move_backward = false;
    auto move_left = false;
    auto move_right = false;

    auto light_buffer = Buffer{10240u};
    auto player_light = PointLightBuffer{{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.09f, 0.032f}};

    auto bullets = DynArray{sizeof(Bullet)};

    auto material_params_buffer = Buffer{1024u};

    auto time = 0.0f;

    auto player = Entity{0u, 0u, 0u, 10u, 0u, 6u};

    ::CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(loop_audio), nullptr, 0, nullptr);

    while (window.running())
    {
        Event evt{};
        auto has_event = window.pump_message(&evt);

        auto delta_x = 0.0f;
        auto delta_y = 0.0f;

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
                    delta_x += evt.data.mouse_move.delta_x * sensitivity;
                    break;
                }
                case LEFT_MOUSE_CLICK:
                {
                    auto bullet = Bullet{camera.position() + camera.direction() * 2.0f, camera.direction() * 2.0f};
                    bullets.push_back(&bullet);
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

        auto *mapped_model_data =
            reinterpret_cast<ModelData *>(::glMapNamedBuffer(model_data_buffer.native_handle(), GL_WRITE_ONLY));

        auto *enemy = &mapped_model_data[max_models_per_type];
        const auto enemy_position = Vector3{enemy->model[12], enemy->model[13], enemy->model[14]};

        const auto speed = 0.4f;
        if (walk_direction != Vector3{})
        {
            const auto translation = Vector3::normalise(walk_direction) * speed;
            camera.translate(translation);

            const auto translation_matrix = Matrix4{translation};
            // no spheres

            for (auto i = player.cube_start; i < player.cube_end; ++i)
            {
                mapped_model_data[i].model = translation_matrix * mapped_model_data[i].model;
            }
            for (auto i = player.cylinder_start; i < player.cylinder_end; ++i)
            {
                mapped_model_data[i + (max_models_per_type * 2u)].model =
                    translation_matrix * mapped_model_data[i + (max_models_per_type * 2u)].model;
            }
        }

        if (delta_x != 0.0f || delta_y != 0.0f)
        {
            camera.adjust_yaw(delta_x);
            camera.adjust_pitch(-delta_y);

            for (auto i = player.cube_start; i < player.cube_end; ++i)
            {
                mapped_model_data[i].model = Matrix4{camera.position()} * Matrix4{Quaternion{0.0f, -delta_x, 0.0f}} *
                                             Matrix4{-camera.position()} * mapped_model_data[i].model;
            }
            for (auto i = player.cylinder_start; i < player.cylinder_end; ++i)
            {
                mapped_model_data[i + (max_models_per_type * 2u)].model =
                    Matrix4{camera.position()} * Matrix4{Quaternion{0.0f, -delta_x, -0.0f}} *
                    Matrix4{-camera.position()} * mapped_model_data[i + (max_models_per_type * 2u)].model;
            }
        }

        ::glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        material.use();

        time += 1.0f / 30.0f;
        material.set_uniform("time", time);

        const auto camera_pos = camera.position();

        player_light.position = camera_pos;

        camera_buffer.write(reinterpret_cast<const std::uint8_t *>(camera.view()), sizeof(Matrix4), 0);
        camera_buffer.write(
            reinterpret_cast<const std::uint8_t *>(camera.projection()), sizeof(Matrix4), sizeof(Matrix4));
        camera_buffer.write(reinterpret_cast<const std::uint8_t *>(&camera_pos), sizeof(Vector3), sizeof(Matrix4) * 2);
        ::glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera_buffer.native_handle());

        auto *cursor = reinterpret_cast<std::uint8_t *>(bullets.begin());

        while (cursor != bullets.end())
        {
            const auto *bullet = reinterpret_cast<Bullet *>(cursor);
            if (Vector3::distance(bullet->position, camera_pos) > 200.0f)
            {
                bullets.erase(cursor);
                continue;
            }

            cursor += bullets.element_size();
        }

        const auto light_count = bullets.size() + 1;
        light_buffer.write(reinterpret_cast<const std::uint8_t *>(&light_count), sizeof(int), 0);
        light_buffer.write(reinterpret_cast<const std::uint8_t *>(&player_light), sizeof(PointLightBuffer), 16);
        for (auto i = 0u; i < bullets.size(); ++i)
        {
            auto *bullet =
                reinterpret_cast<Bullet *>(reinterpret_cast<std::uint8_t *>(bullets.begin()) + (i * sizeof(Bullet)));
            bullet->position += bullet->velocity;
            auto bullet_light = PointLightBuffer{{bullet->position}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.01f, 0.032f}};
            light_buffer.write(
                reinterpret_cast<const std::uint8_t *>(&bullet_light),
                sizeof(PointLightBuffer),
                16 + (sizeof(PointLightBuffer) * (i + 1)));

            auto *model_data = mapped_model_data + max_models_per_type + sphere_model_count + i;
            model_data->model = Matrix4{bullet->position, {0.1f, 0.1f, 0.1f}};
            model_data->checker_colour1 = {1.0f, 0.0f, 0.0f};
            model_data->checker_colour2 = {1.0f, 0.0f, 0.0f};

            if (Vector3::distance(bullet->position, enemy_position) < 3.0f)
            {
                const auto random_float = [](float min, float max) -> float
                { return min + static_cast<float>(rand()) / (static_cast<float>(0xFFFFFFFF / (max - min))); };

                enemy->model[12] = random_float(-20.0f, 20.0f);
                enemy->model[14] = random_float(-20.0f, 20.0f);

                log("hit");
            }
        }
        ::glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, light_buffer.native_handle());

        ::glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, model_data_buffer.native_handle());

        ::glUnmapNamedBuffer(model_data_buffer.native_handle());

        cube_mesh.bind();
        ::glDrawElementsInstancedBaseInstance(
            GL_TRIANGLES,
            cube_mesh.index_count(),
            GL_UNSIGNED_INT,
            reinterpret_cast<void *>(cube_mesh.index_offset()),
            cube_model_count,
            0u);
        cube_mesh.unbind();

        sphere_mesh.bind();
        ::glDrawElementsInstancedBaseInstance(
            GL_TRIANGLES,
            sphere_mesh.index_count(),
            GL_UNSIGNED_INT,
            reinterpret_cast<void *>(sphere_mesh.index_offset()),
            sphere_model_count + bullets.size(),
            max_models_per_type);
        sphere_mesh.unbind();

        cylinder_mesh.bind();
        ::glDrawElementsInstancedBaseInstance(
            GL_TRIANGLES,
            cylinder_mesh.index_count(),
            GL_UNSIGNED_INT,
            reinterpret_cast<void *>(cylinder_mesh.index_offset()),
            cylinder_model_count,
            max_models_per_type * 2u);
        cylinder_mesh.unbind();

        window.swap();
    }

    log("stopping");

    // avoid cleanup, just die
    ::ExitProcess(0);
}

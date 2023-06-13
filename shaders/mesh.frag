#version 310 es

#extension GL_GOOGLE_include_directive: enable

#define m_max_direction_light_count 16

struct DirectionalLight
{
    highp vec4 color;
    highp vec3 direction;
    highp float intensity;
};

layout (set = 0, binding = 0, row_major) uniform _per_frame_ubo_data
{
    highp mat4 proj_view;
    highp vec3 camera_pos;
    highp int directional_light_number;
};

layout (set = 0, binding = 2) uniform _directional_light
{
    DirectionalLight directional_light[m_max_direction_light_count];
};


layout (set = 1, binding = 0) uniform sampler2D base_color_texture_sampler;

layout (location = 0) in highp vec3 world_pos;
layout (location = 1) in highp vec3 normal;
layout (location = 2) in highp vec4 tangent;
layout (location = 3) in highp vec2 texcoord;

layout (location = 0) out highp vec4 out_color;

void main()
{
    highp vec4 ambient_color = 0.1*vec4(0.3, 0.2, 0.1, 1.0);
    highp vec4 diffuse_color = vec4(0.0, 0.0, 0.0, 1.0);
    highp vec4 specular_color = vec4(0.0, 0.0, 0.0, 1.0);

    highp vec4 diffuse_texture = texture(base_color_texture_sampler, texcoord);

    for (highp int i = 0; i < directional_light_number; ++i)
    {
        DirectionalLight light = directional_light[i];
        highp vec3 light_dir = normalize(light.direction);
        highp vec3 view_dir = normalize(camera_pos - world_pos);
        highp vec3 half_dir = normalize(light_dir + view_dir);
        highp float NdotL = max(dot(normal, light_dir), 0.0);

        diffuse_color += diffuse_texture * light.color * light.intensity * NdotL;
        specular_color += 0.5*light.color * light.intensity * pow(max(dot(normal, half_dir), 0.0), 8.0);
    }

    diffuse_color/=float(directional_light_number);
    specular_color/=float(directional_light_number);

    out_color = vec4((ambient_color+diffuse_color+specular_color).xyz, 1.0);
}

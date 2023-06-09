#version 450

#define m_max_direction_light_count 16

struct DirectionalLight
{
    highp vec4 color;
    highp vec3 direction;
    highp float intensity;
};

layout (set = 0, binding = 0, row_major) uniform _per_frame_ubo_data
{
    highp mat4 camera_proj_view;
    highp vec3 camera_pos;
    highp int directional_light_number;
};

layout (set = 0, binding = 1) uniform _directional_light
{
    DirectionalLight directional_light[m_max_direction_light_count];
};

layout (set = 0, binding = 2, row_major) uniform _directional_light_projection
{
    highp mat4 directional_light_proj[m_max_direction_light_count];
};

layout (input_attachment_index = 0, set = 1, binding = 0) uniform highp subpassInput gbuffer_color;
layout (input_attachment_index = 1, set = 1, binding = 1) uniform highp subpassInput gbuffer_normal;
layout (input_attachment_index = 2, set = 1, binding = 2) uniform highp subpassInput gbuffer_position;

layout (set = 2, binding = 0) uniform highp sampler2DArray directional_light_shadowmap_array;

layout (location = 0) out highp vec4 out_color;

highp vec3 DecodeNormal(highp vec3 enc)
{
    return enc * 2.0f - 1.0f;
}

highp float calculate_visibility(highp vec3 world_pos, highp int light_index)
{
    highp vec4 light_space_pos = directional_light_proj[light_index] * vec4(world_pos, 1.0);
    highp vec3 light_space_pos_ndc = light_space_pos.xyz / light_space_pos.w;

    if(light_space_pos_ndc.z >= 1.0f || light_space_pos_ndc.z <= 0.0f)
    {
        return 1.0f;
    }

    highp vec3 light_space_pos_uv = light_space_pos_ndc * 0.5 + 0.5;
    highp vec3 light_space_pos_uv_y_inverted = vec3(light_space_pos_uv.x, light_space_pos_uv.y, float(light_index));
    highp float light_space_depth = texture(directional_light_shadowmap_array, light_space_pos_uv_y_inverted).r;
    if(light_space_depth < light_space_pos_ndc.z - 0.005)
    {
        return 0.0f;
    }
    else
    {
        return 1.0f;
    }
}

void main()
{
    vec3 color = subpassLoad(gbuffer_color).xyz;
    vec3 normal = DecodeNormal(subpassLoad(gbuffer_normal).xyz);
    vec3 position = subpassLoad(gbuffer_position).xyz;

    highp vec3 ambient_color = 0.2*color;
    highp vec3 diffuse_color = vec3(0.0, 0.0, 0.0);
    highp vec3 specular_color = vec3(0.0, 0.0, 0.0);

    highp float visibility;
    for (highp int i = 0; i < directional_light_number; ++i)
    {
        DirectionalLight light = directional_light[i];
        highp vec3 light_dir = normalize(light.direction);
        highp vec3 view_dir = normalize(camera_pos - position);
        highp vec3 half_dir = normalize(light_dir + view_dir);
        highp float NdotL = max(dot(normalize(normal), light_dir), 0.0);

        if(NdotL <= 0.0)
        {
            visibility = 0.0f;
        }
        else
        {
            visibility = calculate_visibility(position, i);
        }

        diffuse_color += 0.6*visibility*color *light.color.xyz * light.intensity* NdotL;
        specular_color += 0.4*visibility*light.color.xyz*light.intensity * pow(max(dot(normalize(normal), half_dir), 0.0), 8.0);
    }

    diffuse_color /= float(directional_light_number);
    specular_color/=float(directional_light_number);

    out_color = vec4(ambient_color+diffuse_color+specular_color, 1.0);

//    out_color = vec4(normal.rgb*0.5f+0.5f, 1.0);
}
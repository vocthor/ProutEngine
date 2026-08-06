#version 410 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;

layout(std140) uniform CameraUBO {
	mat4 view;
	mat4 projection;
	vec4 viewPos;
} camera;

void main()
{
	gl_Position = camera.projection * camera.view * model * vec4(aPos, 1.0f);
}
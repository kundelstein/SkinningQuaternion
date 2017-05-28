#version 150


#define MAX_BONES 33


uniform mat4 u_world;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform vec3 u_skinPos[MAX_BONES];
uniform vec4 u_skinRot[MAX_BONES];


in vec3 in_position;
in vec2 in_texCoord;
in vec4 in_indices;
in vec4 in_weights;
out vec2 v_texCoord;


vec3 trans_for_f (vec3 v, vec3 pos, vec4 rot)
{
	return v + 2.0 * cross (rot.xyz, cross (rot.xyz, v) + rot.w * v) + pos;
}


void main ()
{
	int id0 = int (in_indices.x);
	int id1 = int (in_indices.y);
	int id2 = int (in_indices.z);
	int id3 = int (in_indices.w);

	vec3 result = trans_for_f (in_position, u_skinPos[id0], u_skinRot[id0]) * in_weights.x;
	result += trans_for_f (in_position, u_skinPos[id1], u_skinRot[id1]) * in_weights.y;
	result += trans_for_f (in_position, u_skinPos[id2], u_skinRot[id2]) * in_weights.z;
	result += trans_for_f (in_position, u_skinPos[id3], u_skinRot[id3]) * in_weights.w;

	gl_Position = u_world * vec4 (result, 1.0);
	gl_Position = u_view * gl_Position;
	gl_Position = u_proj * gl_Position;

	v_texCoord = in_texCoord;
}


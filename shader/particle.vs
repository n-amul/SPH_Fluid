layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;
layout(location=2) in mat4 ModelMtx;

out vec3 fragPosition;
out vec3 fragNormal;

uniform mat4 viewProjMtx=mat4(1);

void main() {
	gl_Position=viewProjMtx * ModelMtx * vec4(Position,1);
	fragPosition=vec3(ModelMtx * vec4(Position,1));
	fragNormal=vec3(ModelMtx * vec4(Normal,0));
}
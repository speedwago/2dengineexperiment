// Input vertex data, different for all executions of this shader.
attribute vec3 vertexPosition_modelspace;
attribute vec2 vertexUV;
varying vec2 UV;
varying float alfa;
uniform mat4 Proj;

void main()
{
	//float s = sin(vertexPosition_modelspace.w);
	//float c = cos(vertexPosition_modelspace.w);
	//vec4 tem= vertexPosition_modelspace;
	UV = vertexUV;
	//tem.x = vertexPosition_modelspace.x*c - vertexPosition_modelspace.y*s;
	//tem.y = vertexPosition_modelspace.y*c + vertexPosition_modelspace.x*s;
	alfa = vertexPosition_modelspace.z;
	gl_Position = Proj * vec4(vertexPosition_modelspace, 1.0);

}

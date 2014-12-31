uniform float zscale;

void main()
{
	mat4 mvmat = gl_ModelViewMatrix;
	mvmat[0][0] = mvmat[1][1] = mvmat[2][2] = 1.0;
	mvmat[0][1] = mvmat[0][2] = mvmat[1][2] = mvmat[1][0] = mvmat[2][0] = mvmat[2][1] = 0.0;

	vec4 vertex = gl_Vertex;
	gl_Position = gl_ProjectionMatrix * mvmat * vertex;
	gl_TexCoord[0].xyz = gl_NormalMatrix * (vertex.xyz * vec3(1.0, 1.0, -1.0) * 1.732051) * vec3(1.0, 1.0, zscale) * 0.5 + 0.5;
}

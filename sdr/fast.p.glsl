uniform sampler3D vol_tex;
uniform sampler1D xfer_tex;

const vec3 light_dir[3] = {
	vec3(0.5, 0.5, 1.0),
	vec3(-1.0, 0.2, 0.5),
	vec3(-0.2, 0.0, -7.0)
};
const vec3 light_color[3] = {
	vec3(0.9, 0.7, 0.68),
	vec3(0.5, 0.6, 0.9),
	vec3(0.3, 0.3, 0.3)
};	

const vec3 ambient = vec3(0.1, 0.1, 0.1);
const vec3 vdir = vec3(0.0, 0.0, 1.0);
const float dimmer = 0.7;

void main()
{
	vec3 tc = gl_TexCoord[0].xyz;
	float max_manh_dist = max(tc.x, max(tc.y, tc.z));
	float min_manh_dist = min(tc.x, min(tc.y, tc.z));
	float border = step(0.0, min_manh_dist) * (1.0 - step(1.0, max_manh_dist));

	vec4 voxel = texture3D(vol_tex, gl_TexCoord[0].xyz);
	vec4 color = texture1D(xfer_tex, voxel.a);

	float alpha = color.a * border;
	if(alpha < 0.001) discard;

	vec3 norm = normalize(voxel.rgb * 2.0 - 1.0);

	vec3 diffuse = vec3(0.0, 0.0, 0.0);
	vec3 specular = vec3(0.0, 0.0, 0.0);

	for(int i=0; i<3; i++) {
		vec3 ldir = normalize(light_dir[i]);
		vec3 hdir = normalize(vdir + ldir);

		float ndotl = max(dot(norm, ldir), 0.0);
		float ndoth = max(dot(norm, hdir), 0.0);

		diffuse += light_color[i] * color.rgb * ndotl;
		specular += light_color[i] * vec3(0.6, 0.6, 0.6) * pow(ndoth, 50.0);
	}

	gl_FragColor.rgb = ambient + (diffuse + specular) * dimmer;
	gl_FragColor.a = alpha;
}

uniform sampler3D vol_tex;
uniform sampler1D xfer_tex;

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

	gl_FragColor.rgb = voxel.rgb;
	gl_FragColor.a = alpha;
}

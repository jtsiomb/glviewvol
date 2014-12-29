uniform sampler3D vol_tex;
uniform sampler1D xfer_tex;

void main()
{
	vec4 voxel = texture3D(vol_tex, gl_TexCoord[0].xyz);
	vec4 color = texture1D(xfer_tex, voxel.a);

	gl_FragColor.rgb = color.rgb;
	gl_FragColor.a = 1.0;
}

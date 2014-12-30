#ifndef OPT_H_
#define OPT_H_

enum RendererType {
	REND_NONE,
	REND_FAST
};

struct Options {
	char *fname;
	RendererType rend_type;
	int xsz, ysz;
};

extern Options opt;

int parse_args(int argc, char **argv);

#endif	// OPT_H_

#ifndef XFER_VIEW_H_
#define XFER_VIEW_H_

#include "xfermap.h"

bool xfview_init(TransferFunc *xfer);
void xfview_destroy();

void xfview_draw();

void xfview_button(int bn, int press, int x, int y);
void xfview_motion(int x, int y);

#endif	// XFER_VIEW_H_

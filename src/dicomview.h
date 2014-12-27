#ifndef DICOMVIEW_H_
#define DICOMVIEW_H_

#ifdef __cplusplus
extern "C" {
#endif

int init();
void cleanup();

void ev_display();
void ev_reshape(int x, int y);
void ev_keyboard(int key, int press, int x, int y);
void ev_mouse_button(int bn, int press, int x, int y);
void ev_mouse_motion(int x, int y);

// functions provided by the frontend
void swap_buffers();
void redisplay();
void quit();

#ifdef __cplusplus
}
#endif

#endif	// DICOMVIEW_H_

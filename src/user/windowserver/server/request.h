#include "windowserver/client.h"

#ifndef _WINDOWSERVER_REQUEST_H
#define _WINDOWSERVER_REQUEST_H

void handle_request(unsigned action, unsigned sender, void* raw);

void create_window_handler(unsigned sender, CreateWindowRequest* createwindow);
void destroy_window_handler(unsigned sender, DestroyWindowRequest* req);
void render_window_handler(unsigned sender, WindowRenderRequest* req);
void update_element_handler(unsigned sender, WindowServerElementUpdateRequest* req);

WindowServerElementUpdateResponse create_element_label_handler(unsigned sender, WindowServerLabelUpdateRequest* labelreq);
void update_element_label_handler(unsigned sender, WindowServerLabelUpdateRequest* labelreq);

WindowServerElementUpdateResponse create_element_button_handler(unsigned sender, WindowServerButtonUpdateRequest* buttonreq);
void update_element_rectangle_handler(unsigned sender, WindowServerRectangleUpdateRequest* rectanglereq);

WindowServerElementUpdateResponse create_element_rectangle_handler(unsigned sender, WindowServerRectangleUpdateRequest* rectanglereq);

WindowServerElementUpdateResponse create_element_image_handler(unsigned sender, WindowServerImageUpdateRequest* imagereq);

void load_image_handler(unsigned sender, ImageLoadRequest* req);

#endif
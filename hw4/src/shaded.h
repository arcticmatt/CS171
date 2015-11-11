#ifndef SHADED_H
#define SHADED_H

#include "parser.h"

// Old from HW3

// Variables
Scene *s;
Eigen::Quaternionf current_rotation = Eigen::Quaternionf::Identity();
Eigen::Quaternionf last_rotation = Eigen::Quaternionf::Identity();
int mouse_x, mouse_y;
float mouse_scale_x, mouse_scale_y;

const float step_size = 0.2;
const float x_view_step = 90.0, y_view_step = 90.0;
float x_view_angle = 0, y_view_angle = 0;

bool is_pressed = false;
bool wireframe_mode = false;

// Functions
void init(void);
void reshape(int width, int height);
void display(void);

void init_lights();
void set_lights();
void draw_objects();

void mouse_pressed(int button, int state, int x, int y);
void mouse_moved(int x, int y);
void key_pressed(unsigned char key, int x, int y);

/* Arcball */
Eigen::Quaternionf get_current_rotation();
float *get_rotation_matrix();

#endif

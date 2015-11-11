#include <Eigen/Dense>
#include "framework.h"

using namespace Eigen;
using namespace std;

Eigen::Quaternionf compute_rotation_quaternion(int x0, int y0, int x1, int y1,
        int xres, int yres);
Vector3f get_unit_rotation_axis(int x0, int y0, int x1, int y1, int xres,
        int yres);
float get_rotation_angle(int x0, int y0, int x1, int y1, int xres, int yres);
float screen_to_ndc(int screen_coord, int screen_res);

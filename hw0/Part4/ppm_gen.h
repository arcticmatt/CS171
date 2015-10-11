#include <stdio.h>
#include <vector>
#include <string>

const int CIRCLE_R = 100;
const int CIRCLE_G = 23;
const int CIRCLE_B = 232;

const int BG_R = 45;
const int BG_G = 18;
const int BG_B = 10;

using namespace std;

struct ppm {
    vector<string> lines;
    int xres;
    int yres;
    ppm(int x, int y) : xres(x), yres(y) {}
};

ppm create_ppm(int x, int y);
string get_circle_line();
string get_bg_line();

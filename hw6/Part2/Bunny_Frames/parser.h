#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>

using namespace std;

/*
 * Struct for vectors of size 3 (translation/scale).
 */
struct Vec3f
{
	float x, y, z;
    Vec3f(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3f() {}
    string to_string_precise() {
        ostringstream vec_string;
        vec_string << std::setprecision(6) << std::fixed;
        vec_string << x << " " << y << " " << z;
        return vec_string.str();
    }

    string to_string() {
        ostringstream vec_string;
        vec_string << x << " " << y << " " << z;
        return vec_string.str();
    }
};

/*
 * Keyframe struct holds all relevant info for each keyframe.
 */
struct Keyframe {
    int frame_num;
    vector<Vec3f> vertices;
    vector<Vec3f> faces;

    string to_string() {
        ostringstream frame_string;

        for (Vec3f v : vertices)
            frame_string << "v " << v.to_string_precise() << endl;

        for (Vec3f f :faces)
            frame_string << "f " << f.to_string() << endl;

        return frame_string.str();
    }

    void print() {
        cout << to_string();
    }
};

/*
 * Animation struct stores vector of keyframes and the maximum frame number.
 */
struct Animation {
    int max_frames;
    vector<Keyframe> keyframes;
};

Keyframe parse_frame(ifstream &infile);
Animation parse_animation();

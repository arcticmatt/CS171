#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

struct Vec3f
{
	float x, y, z;
    Vec3f(float x, float y, float z) : x(x), y(y), z(z) {}
    Vec3f() {}
    string to_string() {
        ostringstream vec_string;
        vec_string << x << " " << y << " " << z;
        return vec_string.str();
    }
};

struct Vec4f
{
	float x, y, z, alpha;
    Vec4f(float x, float y, float z, float alpha) : x(x), y(y), z(z), alpha(alpha) {}
    Vec4f() {}
    string to_string() {
        ostringstream vec_string;
        vec_string << x << " " << y << " " << z << " " << alpha;
        return vec_string.str();
    }
};

struct Keyframe {
    int frame_num;
    Vec3f translation;
    Vec3f scale;
    Vec4f rotation;
};

struct Animation {
    int num_frames;
    vector<Keyframe> keyframes;

    void print() {
        cout << num_frames << endl;
        for (Keyframe keyframe : keyframes) {
            cout << "Frame " << keyframe.frame_num << endl;
            cout << "translation " << keyframe.translation.to_string() << endl;
            cout << "scale " << keyframe.scale.to_string() << endl;
            cout << "rotation " << keyframe.rotation.to_string() << endl;
        }
    }
};

Animation parse_frames(ifstream &infile);

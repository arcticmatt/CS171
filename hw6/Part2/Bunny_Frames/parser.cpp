#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>
#include <string>
#include "parser.h"

using namespace std;

const string directory = "keyframes/";
const string filenames[] = {"bunny00.obj", "bunny05.obj", "bunny10.obj", "bunny15.obj",
    "bunny20.obj"};
const int frame_nums[] = {0, 5, 10, 15, 20};
const int NUM_FILES = 5;

/*
 * Parse the passed-in file into a Keyframe struct.
 */
Keyframe parse_frame(ifstream &infile, int frame_num) {
    string line;
    vector<Vec3f> vertices;
    vector<Vec3f> faces;
    float x, y, z;
    char label;

    while (getline(infile, line)) {
        istringstream iss(line);

        (iss >> label >> x >> y >> z);

        assert (label == 'v' || label == 'f');

        Vec3f triple(x, y, z);

        if (label == 'v') {
            vertices.push_back(triple);
        } else if (label == 'f') {
            faces.push_back(triple);
        }
    }

    Keyframe keyframe;
    keyframe.vertices = vertices;
    keyframe.faces = faces;
    keyframe.frame_num = frame_num;
    return keyframe;
}

/*
 * Create animation struct from keyframe files specified at top of this file.
 */
Animation parse_animation() {
    vector<Keyframe> keyframes;
    for (int i = 0; i < NUM_FILES; i++) {
        string filename = directory;
        filename.append(filenames[i]);
        ifstream infile(filename);
        int frame_num = frame_nums[i];
        Keyframe frame = parse_frame(infile, frame_num);
        keyframes.push_back(frame);
    }

    Animation a;
    a.keyframes = keyframes;
    a.max_frames = frame_nums[NUM_FILES - 1];
    return a;
}

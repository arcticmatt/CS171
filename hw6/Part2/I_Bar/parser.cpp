#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "parser.h"

using namespace std;

const int FRAME_LINES = 4;

/*
 * Parse input file into an Animation struct.
 *
 * - Parse number of frames.
 * - Parse all keyframes into a vector of Keyframes.
 */
Animation parse_frames(ifstream &infile) {
    string line;

    int max_frames = 0;
    string label;
    int frame_num, x, y, z, alpha;
    vector<Keyframe> frames;

    while (getline(infile, line)) {
        istringstream iss(line);

        if (max_frames == 0) {
            (iss >> max_frames);
            continue;
        }

        if (line.compare("") == 0)
            continue;

        Keyframe frame;
        for (int i = 0; i < FRAME_LINES; i++) {
            if (i == 0) {
                // Frame number line
                (iss >> label >> frame_num);
                frame.frame_num = frame_num;
            } else if (i == 1) {
                // Translation line
                (iss >> label >> x >> y >> z);
                Vec3f translation(x, y, z);
                frame.translation = translation;
            } else if (i == 2) {
                // Scale line
                (iss >> label >> x >> y >> z);
                Vec3f scale(x, y, z);
                frame.scale = scale;
            } else if (i == 3) {
                // Rotation line
                (iss >> label >> x >> y >> z >> alpha);
                Vec4f rotation(x, y, z, alpha);
                frame.rotation = rotation;
            }

            if (i < 3) {
                getline(infile, line);
                iss.clear();
                iss.str(line);
            }
        }
        frames.push_back(frame);
    }

    Animation a;
    a.max_frames = max_frames;
    a.keyframes = frames;

    return a;
}

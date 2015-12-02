#include <stdio.h>
#include <cmath>
#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include "parser.h"
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

const string OUTPUT_DIR = "output/";

/* Keyframe methods */
void init_keyframes();
void update_keyframes();
Keyframe interpolate_vertices();
float interpolate_points(Vector4f u_vec, MatrixXf B, Vector4f p_vec);
void write_files();
vector<Keyframe> get_frames();

/* Keyframe vars */
Animation animation;
int frame_num = 0;
int keyframe_num = 0;
Keyframe prev_frame;
Keyframe curr_frame;
Keyframe next_frame;
Keyframe next_next_frame;

int main() {
    animation = parse_animation();
    cout << "max frames = " << animation.max_frames << endl;
    //animation.keyframes[0].print();

    init_keyframes();
    write_files();
}

/*
 * Write every .obj file (00 - 20) to the output directory. The output directory
 * should already exist.
 */
void write_files() {
    vector<Keyframe> frames = get_frames();
    cout << "frames.size() = " << frames.size() << endl;
    for (int i = 0; i <= animation.max_frames; i++) {
        string filename = OUTPUT_DIR;
        if (i < 10)
            filename += "bunny0" + to_string(i) + ".obj";
        else
            filename += "bunny" + to_string(i) + ".obj";

        cout << "filename = " << filename << endl;

        ofstream out(filename);
        out << frames[i].to_string();
        out.close();
    }
}

/*
 * Get vector of all bunny frames. Includes keyframes and interpolated frames.
 * That is, returns a vector of frames 00 - 20.
 */
vector<Keyframe> get_frames() {
    vector<Keyframe> frames;
    for (int i = 0; i <= animation.max_frames; i++) {
        cout << "Getting frame #" << i << endl;
        Keyframe frame = interpolate_vertices();
        frames.push_back(frame);
        if (i < animation.max_frames - 1)
            update_keyframes();
    }
    return frames;
}

/*
 * Initialize Keyframe vars.
 */
void init_keyframes() {
    vector<Keyframe> keyframes = animation.keyframes;
    // For frames before 5, use keyframe 0 for first and second control points
    prev_frame = keyframes[0];
    curr_frame = keyframes[0];
    next_frame = keyframes[1];
    next_next_frame = keyframes[2];
}

/*
 * Update the Keyframe vars needed to do Catmull-Rom interpolation.
 */
void update_keyframes() {
    // Don't need to worry about looping
    frame_num += 1;

    if (frame_num == next_frame.frame_num) {
        vector<Keyframe> keyframes = animation.keyframes;

        keyframe_num += 1;

        prev_frame = keyframes[keyframe_num - 1];
        curr_frame = keyframes[keyframe_num];
        next_frame = keyframes[keyframe_num + 1];
        int next_next_index = keyframe_num + 2;
        if (next_next_index > (int) keyframes.size() - 1)
            next_next_index -= 1;
        next_next_frame = keyframes[next_next_index];
    }
}

/*
 * Use Catmull-Rom to interpolate current vertices.
 */
Keyframe interpolate_vertices() {
    vector<Vec3f> vertices;

    if (frame_num == curr_frame.frame_num) {
        vertices = curr_frame.vertices;
    } else {
        MatrixXf B(4, 4);
        B << 0, 2, 0, 0,
             -1, 0, 1, 0,
             2, -5, 4, -1,
             -1, 3, -3, 1;
        B *= 0.5;
        float frame_window = (next_frame.frame_num > curr_frame.frame_num) ?
                                (next_frame.frame_num - curr_frame.frame_num) :
                                (animation.max_frames - curr_frame.frame_num);
        float u = (frame_num - curr_frame.frame_num) / frame_window;
        Vector4f u_vec(1, u, u * u, u * u * u);

        for (int i = 0; i < (int) curr_frame.vertices.size(); i++) {
            // Interpolate vertex
            Vec3f inter_v;

            Vector4f vx_points(prev_frame.vertices[i].x, curr_frame.vertices[i].x,
                    next_frame.vertices[i].x, next_next_frame.vertices[i].x);
            inter_v.x = interpolate_points(u_vec, B, vx_points);

            Vector4f vy_points(prev_frame.vertices[i].y, curr_frame.vertices[i].y,
                    next_frame.vertices[i].y, next_next_frame.vertices[i].y);
            inter_v.y = interpolate_points(u_vec, B, vy_points);

            Vector4f vz_points(prev_frame.vertices[i].z, curr_frame.vertices[i].z,
                    next_frame.vertices[i].z, next_next_frame.vertices[i].z);
            inter_v.z = interpolate_points(u_vec, B, vz_points);

            vertices.push_back(inter_v);
        }
    }

    Keyframe frame;
    frame.vertices = vertices;
    frame.faces = curr_frame.faces;
    return frame;
}

/*
 * Given a vector of points (p_vec), find the interpolated value
 * with the passed-in u_vec and B matrix.
 *
 * Equivalent to evaluating
 * f(u) = uBp
 * at some u.
 */
float interpolate_points(Vector4f u_vec, MatrixXf B, Vector4f p_vec) {
    float interpolated = u_vec.dot(B * p_vec);
    return interpolated;
}

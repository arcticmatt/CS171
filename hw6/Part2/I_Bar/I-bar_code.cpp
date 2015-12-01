/* This is a code snippet for drawing the "I-bar". The 'quadratic' object can be made
 * into a global variable in your program if you want. Line 13, where 'quadratic' gets
 * initialized should be done in your 'init' function or somewhere close to the start
 * of your program.
 *
 * The I-bar is an object that Prof. Al Barr once used in one of his papers to demonstrate
 * an interpolation technique. You might call it "Al Barr's I-Bar" ;)
 */

#include <stdio.h>
#include <iostream>
#include <cassert>
#include <fstream>
#include "parser.h"

#include <GL/glew.h>
#include <GL/glut.h>

using namespace std;

/* Needed to draw the cylinders using glu */
GLUquadricObj *quadratic;

/* Method declarations */
void init();
void display();
void drawIBar();

/* Camera and perspective params */
const float cam_position[] = {0, 0, 7};
const float near_param = 1.0, far_param = 60.0,
            left_param = -1.0, right_param = 1.0,
            bottom_param = -1.0, top_param = 1.0;

/* Arbitrary light */
const float light_color[3] = {1, 1, 1};
const float light_position[3] = {0, 0, -2};

/* Arbitrary material properties for our pendulum */
const float ambient_reflect[3] = {0.3, 0.2, 0.4};
const float diffuse_reflect[3] = {0.7, 0.2, 0.8};
const float specular_reflect[3] = {0.3, 0.3, 0.3};
const float shininess = 0.1;

int main(int argc, char* argv[]) {
    // Should be 3 arguments: the keyframe description file, xres, and yres.
    assert (argc == 4);

    ifstream infile(argv[1]);
    int xres = atoi(argv[2]);
    int yres = atoi(argv[3]);

    Animation animation = parse_frames(infile);
    animation.print();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(xres, yres);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Test");

    /* Call our 'init' function...
     */
    init();

    glutDisplayFunc(display);
    //glutReshapeFunc(reshape);
    //glutMouseFunc(mouse_pressed);
    //glutMotionFunc(mouse_moved);
    //glutKeyboardFunc(key_pressed);
    glutMainLoop();
}

void init()
{
    quadratic = gluNewQuadric();

    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustum(left_param, right_param,
              bottom_param, top_param,
              near_param, far_param);

    glMatrixMode(GL_MODELVIEW);

    // Enable lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_color);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_color);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Set material properties
    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_reflect);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_reflect);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_reflect);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(-cam_position[0], -cam_position[1], -cam_position[2]);
    drawIBar();
    glutSwapBuffers();
}

void drawIBar()
{
    /* Parameters for drawing the cylinders */
    float cyRad = 0.2, cyHeight = 1.0;
    int quadStacks = 4, quadSlices = 4;

    glPushMatrix();
    glColor3f(0, 0, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(90, 1, 0, 0);
    gluCylinder(quadratic, cyRad, cyRad, 2.0 * cyHeight, quadSlices, quadStacks);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0, 1, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1, 0, 1);
    glTranslatef(0, cyHeight, 0);
    glRotatef(-90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();

    glPushMatrix();
    glColor3f(1, 1, 0);
    glTranslatef(0, -cyHeight, 0);
    glRotatef(-90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();

    glPushMatrix();
    glColor3f(0, 1, 0);
    glTranslatef(0, -cyHeight, 0);
    glRotatef(90, 0, 1, 0);
    gluCylinder(quadratic, cyRad, cyRad, cyHeight, quadSlices, quadStacks);
    glPopMatrix();
}

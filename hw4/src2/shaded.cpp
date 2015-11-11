#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <cstdlib>
#include <vector>
#include <cassert>

#define GL_GLEXT_PROTOTYPES 1
#define _USE_MATH_DEFINES

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "shaded.h"
#include "rotation.h"

static const int WIDTH = 800;
static const int HEIGHT = 800;

void initGL();
void reshape(int x, int y);
void readShaders();
extern GLenum readpng(const char *filename);
char *tex_name;
char *normal_name;
GLenum shaderProgram;
string vertProgFileName, fragProgFileName;
GLenum colorTex, normalTex;
GLint colorUniformPos, normalUniformPos;
GLint numLightsPos;
GLint shadingToggle;

using namespace std;

int main(int argc, char* argv[]) {
    // Should be 2 arguments: the color texture and the normal map
    assert (argc == 3);

    tex_name = argv[1];
    normal_name = argv[2];

    /* 'glutInit' intializes the GLUT (Graphics Library Utility Toolkit) library.
     * This is necessary, since a lot of the functions we used above and below
     * are from the GLUT library.
     *
     * 'glutInit' takes the 'main' function arguments as parameters. This is not
     * too important for us, but it is possible to give command line specifications
     * to 'glutInit' by putting them with the 'main' function arguments.
     */
    glutInit(&argc, argv);
    /* The following line of code tells OpenGL that we need a double buffer,
     * a RGB pixel buffer, and a depth buffer.
     */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    /* The following line tells OpenGL to create a program window of size
     * 'xres' by 'yres'.
     */
    glutInitWindowSize(WIDTH, HEIGHT);
    /* The following line tells OpenGL to set the program window in the top-left
     * corner of the computer screen (0, 0).
     */
    glutInitWindowPosition(0, 0);
    /* The following line tells OpenGL to name the program window "Test".
     */
    glutCreateWindow("Test");

    /* Call our 'init' function...
     */
    initGL();

    vertProgFileName = "vertexProgram.glsl";
    fragProgFileName = "fragmentProgram.glsl";
    readShaders();

    /* Specify to OpenGL our display function.
     */
    glutDisplayFunc(display);
    /* Specify to OpenGL our reshape function.
     */
    glutReshapeFunc(reshape);
    /* Specify to OpenGL our function for handling mouse presses.
     */
    glutMouseFunc(mouse_pressed);
    /* Specify to OpenGL our function for handling mouse movement.
     */
    glutMotionFunc(mouse_moved);
    /* The following line tells OpenGL to start the "event processing loop". This
     * is an infinite loop where OpenGL will continuously use our display, reshape,
     * mouse, and keyboard functions to essentially run our program.
     */
    glutMainLoop();
}

void readShaders() {
    string vertProgramSource, fragProgramSource;

    ifstream vertProgFile(vertProgFileName.c_str());
    if (! vertProgFile)
        cerr << "Error opening vertex shader program\n";
    ifstream fragProgFile(fragProgFileName.c_str());
    if (! fragProgFile)
        cerr << "Error opening fragment shader program\n";

    getline(vertProgFile, vertProgramSource, '\0');
    const char* vertShaderSource = vertProgramSource.c_str();

    getline(fragProgFile, fragProgramSource, '\0');
    const char* fragShaderSource = fragProgramSource.c_str();

    char buf[1024];
    GLsizei blah;

    // Initialize shaders
    GLenum vertShader, fragShader;

    shaderProgram = glCreateProgram();

    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertShaderSource, NULL);
    glCompileShader(vertShader);

    GLint isCompiled = 0;
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(vertShader, maxLength, &maxLength, &errorLog[0]);

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        for (int i = 0; i < (int) errorLog.size(); i++)
            cout << errorLog[i];
        glDeleteShader(vertShader); // Don't leak the shader.
        return;
    }

    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderSource, NULL);
    glCompileShader(fragShader);

    isCompiled = 0;
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(fragShader, maxLength, &maxLength, &errorLog[0]);

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        for (int i = 0; i < (int) errorLog.size(); i++)
            cout << errorLog[i];
        glDeleteShader(fragShader); // Don't leak the shader.
        return;
    }

    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    cerr << "Enabling fragment program: " << gluErrorString(glGetError()) << endl;
    glGetProgramInfoLog(shaderProgram, 1024, &blah, buf);
    cerr << buf;

    cerr << "Enabling program object" << endl;
    glUseProgram(shaderProgram);

    colorUniformPos = glGetUniformLocation(shaderProgram, "colorTex");
    normalUniformPos = glGetUniformLocation(shaderProgram, "normalTex");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glUniform1i(colorUniformPos, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTex);
    glUniform1i(normalUniformPos, 1);
}

void initGL() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.5, 0.5,
              -0.5, 0.5,
              1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glRotatef(0, 0, 1, 0);
    glTranslatef(0, -1, 0);



    GLfloat pos[] = {7.0, 2.0, 3.0, 1.0};
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT,GL_DIFFUSE);
    GLfloat black[4] = {0,0,0,0};
    glMaterialfv(GL_FRONT,GL_AMBIENT,black);
    glMaterialfv(GL_FRONT,GL_SPECULAR,black);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    glEnable(GL_DEPTH_TEST);

    if (!(colorTex = readpng(tex_name)))
        exit(1);
    if (!(normalTex = readpng(normal_name)))
        exit(1);
}

void display() {
    glPushMatrix();

    float *m = get_rotation_matrix();
    glMultMatrixf(m);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glColor3f(0, 1, 0);

    glBegin(GL_POLYGON);
    glTexCoord2f(0, 0);
    glVertex3f(-5.0, -5.0, 0.0);

    glTexCoord2f(1, 0);
    glVertex3f(5.0, -5.0, 0.0);

    glTexCoord2f(1, 1);
    glVertex3f(5.0, 5.0, 0.0);

    glTexCoord2f(0, 1);
    glVertex3f(-5.0, 5.0, 0.0);
    glEnd();


    glPopMatrix();
    glutSwapBuffers();
}

void reshape(int x, int y) {
    const double aspect = x / (double)y;

    glViewport(0, 0, x, y);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, aspect, 1, 50);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-10);
}


/* COPIED FROM DEMO
 * 'mouse_pressed' function:
 *
 * This function is meant to respond to mouse clicks and releases. The
 * parameters are:
 *
 * - int button: the button on the mouse that got clicked or released,
 *               represented by an enum
 * - int state: either 'GLUT_DOWN' or 'GLUT_UP' for specifying whether the
 *              button was pressed down or released up respectively
 * - int x: the x screen coordinate of where the mouse was clicked or released
 * - int y: the y screen coordinate of where the mouse was clicked or released
 *
 * The function doesn't really do too much besides set some variables that
 * we need for the 'mouse_moved' function.
 */
void mouse_pressed(int button, int state, int x, int y)
{
    /* If the left-mouse button was clicked down, then...
     */
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        /* Store the mouse position in our global variables.
         */
        mouse_x = x;
        mouse_y = y;

        /* Since the mouse is being pressed down, we set our 'is_pressed"
         * boolean indicator to true.
         */
        is_pressed = true;
    }
    /* If the left-mouse button was released up, then...
     */
    else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        last_rotation = current_rotation * last_rotation;
        current_rotation = Eigen::Quaternionf::Identity();
        /* Mouse is no longer being pressed, so set our indicator to false.
         */
        is_pressed = false;
    }
}

/* COPIED FROM DEMO
 * 'mouse_moved' function:
 *
 * This function is meant to respond to when the mouse is being moved. There
 * are just two parameters to this function:
 *
 * - int x: the x screen coordinate of where the mouse was clicked or released
 * - int y: the y screen coordinate of where the mouse was clicked or released
 *
 * We compute our camera rotation angles based on the mouse movement in this
 * function.
 */
void mouse_moved(int x, int y)
{
    /* If the left-mouse button is being clicked down...
     */
    if(is_pressed)
    {
        current_rotation = compute_rotation_quaternion(mouse_x, mouse_y,
                x, y, WIDTH, HEIGHT);

        /* Tell OpenGL that it needs to re-render our scene with the new camera
         * angles.
         */
        glutPostRedisplay();
    }
}

/*
 * Gets the current rotation.
 */
Eigen::Quaternionf get_current_rotation() {
    return current_rotation * last_rotation;
}

/*
 * Gets the current rotation matrix.
 *
 * Returns a pointer to the consecutive values that are used as the elements
 * of a rotation matrix. The elements are specified in column order. That is,
 * if we have 16 elements and we are specifying a 4 x 4 matrix, then the first
 * 4 elements represent the first column, and so on.
 *
 * The actual form of the rotation matrix is specified in HW3 notes. All we
 * need to do is get the current rotation quaternion and translate it to
 * matrix form.
 */
float *get_rotation_matrix() {
    Eigen::Quaternionf q = get_current_rotation();
    float qs = q.w();
    float qx = q.x();
    float qy = q.y();
    float qz = q.z();

    float *matrix = new float[16];

    MatrixXf m(4, 4);
    m <<
        1 - 2 * qy * qy - 2 * qz * qz, 2 * (qx * qy - qz * qs),
            2 * (qx * qz + qy * qs), 0,
        2 * (qx * qy + qz * qs), 1 - 2 * qx * qx - 2 * qz * qz,
            2 * (qy * qz - qx * qs), 0,
        2 * (qx * qz - qy * qs), 2 * (qy * qz + qx * qs),
            1 - 2 * qx * qx - 2 * qy * qy, 0,
        0, 0, 0, 1;

    // Manually copy eigen data into float array, otherwise we run into
    // memory issues
    int count = 0;
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            matrix[count] = m(row, col);
            count++;
        }
    }
    return matrix;
}

/* COPIED FROM DEMO
 * 'deg2rad' function:
 *
 * Converts given angle in degrees to radians.
 */
float deg2rad(float angle)
{
    return angle * M_PI / 180.0;
}

#include "homework1.h"
using namespace std;

/**
 * In the homework1, we have an object and a camera
 * we are to use the perspective transorm to convert all 3d points onto a plane
 */
homework1 hk1;

/**
 * The 2d result will store all the 2d final points
 */
vector<pair<double,double>> points_2d;

/**
 * Main work is done here
 */
void render_scene();

/*******  OpenGL GDI helper, to draw lines on a 2d window  *******/

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/**
 * make sure the 2d scene is always proportional
 * @param width
 * @param height
 */
void prevent_resize(int width, int height);

/**
 * Callback function for dispalying
 */
void glut_display();

/**
 * This is for test
 */
void keyboardFunc( unsigned char key, int x, int y );

/**
 * Use opengl to draw 2d lines on the screen
 */
void glut_helper(int argn, char** arguments);


/******* Helper end *******/

/**
 * Usage:   built-folder$ ./graphic [object file.d]
 * Notes:   The code implements perspective transform from 3d points to 2d points, back face removed.
 *          The covered front faces are not removed
 *          The object local coordinate is considered the same as the world coordinate
 */
int main( int argn, char** arguments) {

    if(argn <= 1) {
        cout << "Argument file name missing: ./graphic [object file.d]";
        return 2;
    }

    // Set object in the world - Read object data from the file, the function code locate at homework1.cpp
    if(!hk1.set_object_position(arguments[1]))
        return 11;

    // Set camera in the world - Read camera data from the file, the function code locate at homework1.cpp
    hk1.set_camera_position("assets/camera_position.txt");

    // Main work is done here
    render_scene();

    // Render work has done, since I have no power to write a GDI, let opengl help me
    glut_helper(argn, arguments);

    return 0;
}

void render_scene() {

    // Calculate N vector
    hk1.N = (hk1.viewing_point - hk1.camera_position) / (hk1.viewing_point - hk1.camera_position).mold();

    // Calculate U vector
    hk1.U = (hk1.N * hk1.camera_up_direction) / (hk1.N * hk1.camera_up_direction).mold();

    // Calculate V vector
    hk1.V =  hk1.U * hk1.N;

    // Find back face
    hk1.denote_back_face();

    // convert world coordinates into the camera coordinates, using R and T matrix
    matrix<double> R(
            {{hk1.U.x, hk1.U.y, hk1.U.z, 0},
             {hk1.V.x, hk1.V.y, hk1.V.z, 0},
             {hk1.N.x, hk1.N.y, hk1.N.z, 0},
             {0, 0, 0, 1},
            });

    matrix<double> T(
            {{1, 0, 0, -hk1.camera_position.x},
             {0, 1, 0, -hk1.camera_position.y},
             {0, 0, 1, -hk1.camera_position.z},
             {0, 0, 0, 1}
            });

    matrix<double> Mview(R.dot(T));

    // simply igone front cliping plane and back clipping plane, and set focus plane d to be 1
    matrix<double> Mpers(
            {{1,0,0,0},
             {0,1,0,0},
             {0,0,1,0},
             {0,0,1,0}
            });

    points_2d = {};
    // Perspective transfomation for each point
    for(auto each:hk1.object.points){
        matrix<double> each_point_matrix(each.to_matrix());
        each_point_matrix.push_back({1});
        each_point_matrix = Mpers * Mview * each_point_matrix;
        // xs = X / W, ys = Y / W, zs = Z / W, while we only concern xs and ys
        points_2d.push_back({each_point_matrix[0][0]/each_point_matrix[3][0], each_point_matrix[1][0]/each_point_matrix[3][0]});
    }
}

void prevent_resize(int width, int height)  {
    glutReshapeWindow( width, height);
}

void glut_display()  {

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0, 0.5, 0.5);

    for(int i = 0 ; i < hk1.object.faces.size() ; ++i) {
        if(hk1.back_face_indexs.find(i) != hk1.back_face_indexs.end()) continue;
        glBegin(GL_LINE_LOOP);
        for(auto &each_point:hk1.object.faces[i]) {
            glVertex2f(points_2d[each_point].first, points_2d[each_point].second);
        }
        glEnd();
    }
    glFlush();
}

void keyboardFunc(unsigned char key, int x, int y ) {
    double x0 = hk1.camera_position.x, y0 = hk1.camera_position.y, z0 = hk1.camera_position.z;

    switch ( key )
    {
        case 27: // Escape key
            exit(0);
            break;
        case 'w':
            hk1.camera_position.x /= 2;
            hk1.camera_position.y /= 2;
            hk1.camera_position.z /= 2;
            break;
        case 's':
            hk1.camera_position.x *= 2;
            hk1.camera_position.y *= 2;
            hk1.camera_position.z *= 2;
            break;
        case 'a':
            hk1.camera_position.x =  x0* cos(0.1) +  y0* sin(0.1);
            hk1.camera_position.y = -x0 * sin(0.1) + y0 * cos(0.1);
            break;
        case 'e':
            hk1.camera_position.z --;
            break;
        case 'r':
            hk1.camera_position.z ++;
            break;
    }

    render_scene();
    glutPostRedisplay();
}

void glut_helper(int argn, char **arguments)  {

    glutInit(&argn, arguments);

    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    glutInitWindowPosition(100, 100);

    glutInitWindowSize(1000, 1000);

    glutCreateWindow("Graphics II - Assignment 1");

    glutReshapeFunc(prevent_resize);

    glutKeyboardFunc(keyboardFunc);

    glutDisplayFunc(&glut_display);

    glutMainLoop();

}


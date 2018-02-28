#include "homework1.h"
#include "homework2.h"
using namespace std;

/**
 * In the homework1, we have an object and a camera
 * we are to use the perspective transorm to convert all 3d points onto a plane
 */
//homework1 hk;

/**
 * In the homework2, implement z-buffer and scan-conversion algorithm to color the faces
 * and un-display the covered front faces
 */
homework2 hk;

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

    // Set object in the world - Read object data from the file, the function code locate at homework1.cpp
    hk.set_object_position("assets/D/knight.d.txt",{0,0,0});

//    hk.set_object_position("assets/D/bench.d.txt",{0,1,2});

    /* add other object you want */

    // ...

    // Set camera in the world - Read camera data from the file, the function code locate at homework1.cpp
    hk.set_camera_position("assets/camera_position.txt");

    hk.set_single_light_position({1,1,1});

//    hk.set_single_light_position({-1,-1,-1});

    // Main work is done here
    hk.object_points_to_screen_points();

    // show the screen points;
    hk.scan_conversion(true);

    // Render work has done, since I have no power to write a GDI, let opengl help me
    glut_helper(argn, arguments);

    return 0;
}

void prevent_resize(int width, int height)  {
    glutReshapeWindow( width, height);
}

void glut_display()  {

    /**
     * * display for homework 1
     * */

//    glClearColor(0.0, 0.0, 0.0, 0.0);
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    glColor3f(1.0, 0.5, 0.5);
//
//    for(int i = 0 ; i < hk.object.faces.size() ; ++i) {
//        if(hk.back_face_indexs.find(i) != hk.back_face_indexs.end()) continue;
//        glBegin(GL_LINE_LOOP); // GL_LINE_LOOP  GL_POINTS
//        for(auto &each_point:hk.object.faces[i]) {
//            glVertex2f(hk.screen_points[each_point].x, hk.screen_points[each_point].y);
//        }
//        glEnd();
//    }
//    glFlush();

    /*
     * display for homework 2, draw the pixels directly
     * */
    glClear(GL_COLOR_BUFFER_BIT);

    glDrawPixels(hk.window_y,hk.window_y, GL_RGB, GL_UNSIGNED_BYTE, hk.pixel_buffer);
    glFlush();
}

void keyboardFunc(unsigned char key, int x, int y ) {
    double x0 = hk.camera_position.x, y0 = hk.camera_position.y, z0 = hk.camera_position.z;

    switch ( key )
    {
        case 27: // Escape key
            exit(0);
            break;
        case 'w':
            hk.camera_position.x /= 2;
            hk.camera_position.y /= 2;
            hk.camera_position.z /= 2;
            break;
        case 's':
            hk.camera_position.x *= 2;
            hk.camera_position.y *= 2;
            hk.camera_position.z *= 2;
            break;
        case 'z':
            hk.camera_position.x =  x0* cos(0.1) +  y0* sin(0.1);
            hk.camera_position.y = -x0 * sin(0.1) + y0 * cos(0.1);
            break;
        case 'x':
            hk.camera_position.y =  y0* cos(0.1) +  z0* sin(0.1);
            hk.camera_position.z = -y0 * sin(0.1) + z0 * cos(0.1);
            break;
        case 'y':
            hk.camera_position.x =  x0* cos(0.1) +  z0* sin(0.1);
            hk.camera_position.z = -x0 * sin(0.1) + z0 * cos(0.1);
            break;
        case 'e':
            hk.camera_position.x --;
            break;
        case 'r':
            hk.camera_position.x ++;
            break;
        case 'd':
            hk.camera_position.y --;
            break;
        case 'f':
            hk.camera_position.y ++;
            break;
        case 'c':
            hk.camera_position.z --;
            break;
        case 'v':
            hk.camera_position.z ++;
            break;


    }

    /* Re-render the scene */
    hk.object_points_to_screen_points();
    hk.scan_conversion();

    /* Refresh displaying window*/
    glutPostRedisplay();
}

void glut_helper(int argn, char **arguments)  {

    glutInit(&argn, arguments);

    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    glutInitWindowPosition(100, 100);

    glutInitWindowSize(1000, 1000);

    glutCreateWindow("Graphics II - Assignment 1,2");

    glutReshapeFunc(prevent_resize);

    glutKeyboardFunc(keyboardFunc);

    glutDisplayFunc(&glut_display);

    glutMainLoop();

}


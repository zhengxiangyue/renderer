
#include "engine.h"

engine _engine;

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

int main( int argn, char** arguments ) {


    _engine.set_object_position("assets/D/knight.d.txt",{3,3,0}, "assets/cola.bmp");
    _engine.set_object_position("assets/D/king.d.txt",{-3,-3,0}, "assets/KFC.bmp");
    _engine.set_object_position("assets/plane.d.txt",{-3,-3,0}, "const-black");
    _engine.set_object_position("assets/plane.d.txt",{3,3,0}, "const-black");
    _engine.set_object_position("assets/plane.d.txt",{-3,3,0}, "const-white");
    _engine.set_object_position("assets/plane.d.txt",{3,-3,0}, "const-white");

    /* add other object you want */

    // Set camera in the world - Read camera data from the file, the function code locate at homework1.cpp
    _engine.set_camera_position("assets/camera_position.txt");

    _engine.object_points_to_screen_points();

    _engine.scan_conversion();

    glut_helper(argn, arguments);

}


void prevent_resize(int width, int height)  {
    glutReshapeWindow( width, height );
}

void glut_display()  {

    /**
     * * display for homework 1
     * */

//    glClearColor(255.0, 255.0, 255.0, 0.0);
//    glClear(GL_COLOR_BUFFER_BIT);
//
//    glColor3f(1.0, 0.5, 0.5);
//
//    for(int i = 0 ; i < _engine._scene.objects.size() ; ++i) {
//        for(int j = 0 ; j < _engine._scene.objects[i].faces.size() ; ++j) {
//            if(_engine.back_face_indexs.find({i,j}) != _engine.back_face_indexs.end())
//                continue;
//            glBegin(GL_LINE_LOOP);
//
//            for(auto &each_point:_engine._scene.objects[i].faces[j])
//                glVertex2f(_engine._scene.objects[i].points[each_point].screen_point.x, _engine._scene.objects[i].points[each_point].screen_point.y);
//            glEnd();
//        }
//    }
//    glFlush();

    /*
     * display for homework 2, draw the pixels
     * */
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels( WINDOW_X , WINDOW_Y , GL_RGB, GL_UNSIGNED_BYTE, _engine.pixel_buffer);
    glFlush();
}

void keyboardFunc(unsigned char key, int x, int y ) {
    double x0 = _engine._scene.camera_position.x, y0 = _engine._scene.camera_position.y, z0 = _engine._scene.camera_position.z;

    switch ( key )
    {
        case 27: // Escape key
            exit(0);
            break;
        case 'w':
            _engine._scene.camera_position.x /= 1.1;
            _engine._scene.camera_position.y /= 1.1;
            _engine._scene.camera_position.z /= 1.1;
            break;
        case 's':
            _engine._scene.camera_position.x *= 1.3;
            _engine._scene.camera_position.y *= 1.3;
            _engine._scene.camera_position.z *= 1.3;
            break;
        case 'z':
            _engine._scene.camera_position.x =  x0* cos(0.05) +  y0* sin(0.05);
            _engine._scene.camera_position.y = -x0 * sin(0.05) + y0 * cos(0.05);
            break;
        case 'x':
            _engine._scene.camera_position.y =  y0* cos(0.1) +  z0* sin(0.1);
            _engine._scene.camera_position.z = -y0 * sin(0.1) + z0 * cos(0.1);
            break;
        case 'y':
            _engine._scene.camera_position.x =  x0* cos(0.1) +  z0* sin(0.1);
            _engine._scene.camera_position.z = -x0 * sin(0.1) + z0 * cos(0.1);
            break;
        case 'e':
            _engine._scene.camera_position.x --;
            break;
        case 'r':
            _engine._scene.camera_position.x ++;
            break;
        case 'd':
            _engine._scene.camera_position.y --;
            break;
        case 'f':
            _engine._scene.camera_position.y ++;
            break;
        case 'c':
            _engine._scene.camera_position.z --;
            break;
        case 'v':
            _engine._scene.camera_position.z ++;
            break;
    }

    /* Re-render the scene */
    _engine.object_points_to_screen_points();
    _engine.scan_conversion();

    /* Refresh displaying window*/
    glutPostRedisplay();
}

void glut_helper(int argn, char **arguments)  {

    glutInit(&argn, arguments);

    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

    glutInitWindowPosition(100, 100);

    glutInitWindowSize(1000, 1000);

    glutCreateWindow("Graphics II - XiangYue Zheng");

    glutReshapeFunc(prevent_resize);

    glutKeyboardFunc(keyboardFunc);

    glutDisplayFunc(&glut_display);

    glutMainLoop();

}
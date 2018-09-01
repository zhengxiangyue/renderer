//
// Created by Zheng XiangYue on 18/4/2018.
//

#ifndef GRAPHIC_ENGINE_H
#define GRAPHIC_ENGINE_H

#include "lib/myGraphics.h"
#include "lib/matrix.h"
#include "lib/illumination.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>

#define WINDOW_X                    1000
#define WINDOW_Y                    1000

#define DEBUG_MODE                  false

#define CLOCK_WISE_FACE             false

#define SHADING_CONSTANT            1
#define SHADING_GOURAUD             2
#define SHADING_PHONG               3

#define ILLUMINATION_DIFFUSE        1
#define ILLUMINATION_MULTIPLE       2

#define oops(m,x){perror(m); exit(x);}

using namespace std;


struct type_object_texture{

};

struct type_lights{

};


struct pairhash{
    size_t operator()(const pair<int,int> &a) const {
        return hash<int>()(a.first) ^ hash<int>()(a.second);
    }
};

struct type_screen_point : public point3d{
    pair<int,int> original_point;
    type_screen_point(point3d point) : point3d(point.x,point.y,point.z){}
};

struct object_point : public point3d{
    /**
     * coordinate in the screen space, with depth
     */
    point3d screen_point;

    /**
     * point normal
     */
    vector3d normal;

    /**
     * ammbiant term parameter ka for r,g,b
     *
     */
    double ka_r, ka_g, ka_b, kd_r, kd_g, kd_b, ks_r, ks_g, ks_b;

    /**
     * Coordinate of the texture map coordinate
     */
    pair<double,double> texture_point;

    object_point(point3d point) : point3d(point.x,point.y,point.z){}
};

struct type_polygonal_object{

    /**
     * points in 3d world
     */
    vector<object_point> points;

    /**
     * [[0,1,2],[2,3,4]] means there are two polygons in the object
     * the first face is made by 3 points: index 0, index 1, and index2
     */
    vector<vector<int>> faces;

    /**
     * Polygon's normal
     */
    vector<vector3d> face_normal;

    /**
     *
     */
    vector<vector<vector<uint8_t>>> texture_map;

    /**
     * the face_index-th face's normal vector, for the purpose of calculating  face_normal
     * @param int face_index
     * @return vector3d
     */
    vector3d normal(vector<int> &face, int& face_index);
};

class my_scene{
public:
    /**
     * C -> camera position denoted by a 3d point
     */
    point3d camera_position;

    /**
     * Pref -> camera is looking at this point
     */
    point3d viewing_point;

    /**
     * V' -> the up vector of the camera
     */
    vector3d camera_up_direction;

    /**
     * Use 3 vectors to denote the camera coordinates
     */
    vector3d N, U, V;

    /************************ Camera parameter end ***************************/

    /**
     * There may be some objects in the scene
     */
    vector<type_polygonal_object> objects;

    /**
     * Each object may have a texture
     */
    vector<type_object_texture> object_textures;

    /**
     * Several lights may be added to the scene
     */
    vector<type_lights> scene_lights;

    /************************ Scene physical properties end ***************************/
};

class engine {
public:
    engine(){
        /* Initialize refresh buffer, 3 channels*/

        /* Initialize z buffer, the value is between 0 ~ 1, double */
        z_buffer = vector<vector<double>>(WINDOW_X, vector<double>(WINDOW_Y,1));

        edge_table = vector<vector<edge_table_element>>(WINDOW_Y);

        for(int i = 0 ; i < WINDOW_X ; ++i)
            for(int j = 0 ; j < WINDOW_Y ; ++j)
                pixel_buffer[i][j][0] = 0, pixel_buffer[i][j][1] = 0, pixel_buffer[i][j][2] = 0;
    }

    /**
     *
     */
    struct render_configuration{
        /**
         * Background color
         */
        uint8_t _back_r = 255, _back_g = 255, _back_b = 255;

        /**
         * SHADING_CONSTANT
         * SHADING_GOUROUD
         * SHADING_PHONG
         */
        int _shading_model;

        /**
         * ILLUMINATION_DIFFUSE
         * ILLUMINATION_SPECULAR
         * ILLUMINATION_
         */
        int _illumination_model;

    };

    render_configuration _config;

    /**
     * Physical scene
     */
    my_scene _scene;

    /**** Helper structure , one the scene is set, these could be calculated *****/

    /**
     * Back face indexs are stored and the faces are not shown when drawing
     * {i,j} the i-th object's j-th polygon is now a back_face
     */
    unordered_set<pair<int,int>, pairhash> back_face_indexs;

    /**
     * refresh buffer R G B
     */
    uint8_t pixel_buffer[WINDOW_X][WINDOW_Y][3];

    /**
     * Depth buffer
     */
    vector<vector<double>> z_buffer;

    /**
     * the object has an edge table
     */
    vector<vector<edge_table_element>> edge_table;

    /**
     * use active edge table to update the buffers
     */
    vector<edge_table_element> active_edge_table;

    /************************ Helper structure end ***************************/

    /**
     * Read from a ".d" file which contain the information of the object
     */
    bool set_object_position(const char*, const vector<double>, const char* texture = "");

    /**
     * Read from a ".txt" file which contain 9 numbers to denote the pattern of the camera
     */
    void set_camera_position(const char*);

    /**
     * Calculate the back faces base on the current scene
     */
    void denote_back_face();

    /**
     * Perspective transform, after the transform, x is [-1,1], y is [-1,1], z is [0,1]
     */
    void object_points_to_screen_points();

    /**
     * scan conversion with z-buffer
     */
    void scan_conversion();

    /**
     * try depth field
     * I known I'm kiding myself, todo: read books
     */
    void apply_depth_field() {

        uint8_t layer[WINDOW_X][WINDOW_Y][3], blured_layer[WINDOW_X][WINDOW_Y][3];

        for(int i = 0 ; i < WINDOW_X ; ++i) {
            for(int j = 0 ;j < WINDOW_Y ; ++j) {
                blured_layer[i][j][0] = pixel_buffer[i][j][0];
                blured_layer[i][j][1] = pixel_buffer[i][j][1];
                blured_layer[i][j][2] = pixel_buffer[i][j][2];
            }
        }

        double far_depth = 0.940, current_depth = far_depth;

        for(int lay = 0; lay < 9 ; ++lay) {
            current_depth -= 0.004;
            int current_color = rand() % 256;

            for(int i = 0 ; i < WINDOW_X ; ++i) {
                for (int j = 0; j < WINDOW_Y; ++j) {
                    if(z_buffer[j][i] >= current_depth) {
                        layer[i][j][0] = blured_layer[i][j][0];
                        layer[i][j][1] = blured_layer[i][j][1];
                        layer[i][j][2] = blured_layer[i][j][2];
                    }
                }
            }
//
//            // blur the current layer
            for(int i = 1 ; i < WINDOW_X - 1 ; ++i) {
                for (int j = 1; j < WINDOW_Y - 1; ++j) {
                    if(z_buffer[j][i] > current_depth && z_buffer[j-1][i] > current_depth && z_buffer[j+1][i] > current_depth
                            &&z_buffer[j][i-1] > current_depth && z_buffer[j-1][i-1] > current_depth && z_buffer[j+1][i-1] > current_depth
                            && z_buffer[j][i+1] > current_depth && z_buffer[j-1][i+1] > current_depth && z_buffer[j+1][i+1] > current_depth) {
                        blured_layer[i][j][0] =
                                (layer[i - 1][j - 1][0] + layer[i - 1][j][0] + layer[i - 1][j + 1][0] +
                                        layer[i][j - 1][0] +
                                        layer[i][j + 1][0] + layer[i + 1][j - 1][0] + layer[i + 1][j][0] +
                                        layer[i + 1][j + 1][0]) / 8;
                        blured_layer[i][j][1] =
                                (layer[i - 1][j - 1][1] + layer[i - 1][j][1] + layer[i - 1][j + 1][1] +
                                        layer[i][j - 1][1] +
                                        layer[i][j + 1][1] + layer[i + 1][j - 1][1] + layer[i + 1][j][1] +
                                        layer[i + 1][j + 1][1]) / 8;
                        blured_layer[i][j][2] =
                                (layer[i - 1][j - 1][2] + layer[i - 1][j][2] + layer[i - 1][j + 1][2] +
                                        layer[i][j - 1][2] +
                                        layer[i][j + 1][2] + layer[i + 1][j - 1][2] + layer[i + 1][j][2] +
                                        layer[i + 1][j + 1][2]) / 8;
                    }
                }
            }

        }

        for(int i = 0 ; i < WINDOW_X ; ++i) {
            for(int j = 0 ;j < WINDOW_Y ; ++j) {
                pixel_buffer[i][j][0] = blured_layer[i][j][0];
                pixel_buffer[i][j][1] = blured_layer[i][j][1];
                pixel_buffer[i][j][2] = blured_layer[i][j][2];
            }
        }
    }
};

#endif //GRAPHIC_ENGINE_H

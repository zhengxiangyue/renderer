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

#define DEBUG_MODE                  true

#define CLOCK_WISE_FACE             false

#define SHADING_CONSTANT            1
#define SHADING_GOURAUD             2
#define SHADING_PHONG               3

#define ILLUMINATION_DIFFUSE        1
#define ILLUMINATION_MULTIPLE       2

#define oops(m,x){perror(m); exit(x);}

using namespace std;

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
    vector3d normal(vector<int> &face, int& face_index) {

        point3d first_point = points[face[0]], second_point = points[face[1]], third_point = points[face[2]];

        vector3d first_vector = second_point - first_point, second_vector = third_point - second_point;

        vector3d nor = first_vector * second_vector;

        // things happen
        auto nor_mold = nor.mold();

        auto result = nor_mold ? nor / nor.mold() : vector3d({0,0,1});

        return result;

    }
};

struct type_object_texture{

};

struct type_lights{

};


struct pairhash{
    size_t operator()(const pair<int,int> &a) const {
        return hash<int>()(a.first) ^ hash<int>()(a.second);
    }
};

/** Helper function **/

inline int to_pixel(double &value, bool shortten = false) {
    // [-1, 1] is transformed to [0, WINDOW_X]
    return floor((value+1)*WINDOW_Y/2) - shortten;
}

inline double to_double_pixel(double &value) {
    return (value+1)*WINDOW_Y/2.0;
}

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
        uint8_t _back_r = 80, _back_g = 80, _back_b = 80;

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
};

/**
 * read data from .d file, only compatable for .d file
 * @param file_name
 * @return bool
 */
bool engine::set_object_position(const char* file_name, const vector<double> position, const char* texture) {

    type_polygonal_object new_object;

    int points_number, faces_number, point_index, face_points_number;

    double z_min = INT_MAX, z_max = INT_MIN, x_min = INT_MAX, x_max = INT_MIN, y_min = INT_MAX, y_max = INT_MIN;

    char buffer[128];
    ifstream data_stream;
    string each_line;

    data_stream.open(file_name);

    // read the first line to get points' number and faces' number
    getline(data_stream, each_line);
    istringstream each_line_in(each_line);
    each_line_in >> buffer;

    // some file is missing the first string, gosh
    if(buffer[0] != 'd') {
        cout << "data string is missing";
        exit(1);
    }

    // read how many points and faces are in the object
    each_line_in >> points_number >> faces_number;

    // save all points;
    for(int i = 0 ; i < points_number ; ++i) {
        point3d new_point;
        getline(data_stream, each_line);
        istringstream each_line_in(each_line);
        each_line_in >> new_point.x >> new_point.y >> new_point.z;
        x_min = min(x_min, new_point.x), x_max = max(x_max, new_point.x);
        y_min = min(y_min, new_point.y), y_max = max(y_max, new_point.y);
        z_min = min(z_min, new_point.z), z_max = max(z_max, new_point.z);
        new_point.x += position[0], new_point.y += position[1], new_point.z += position[2];
        new_object.points.push_back(new_point);
    }

    double z_range = z_max - z_min, y_range = y_max - y_min, x_range = x_max - x_min;

    // texture map
    for(int i = 0 ; i < points_number ; ++i) {
        double _x = (new_object.points[i].x - position[0] - x_min) / (x_range + 0.0001),
                _y = (new_object.points[i].y - position[1] - y_min) / (y_range + 0.0001),
                _z = (new_object.points[i].z - position[2] - z_min) / (z_range + 0.0001);
        double theta_degree = co_to_degree(_x / sqrt(_x * _x + _y * _y), _y / sqrt(_x * _x + _y * _y));
        new_object.points[i].texture_point.first = _x;
        new_object.points[i].texture_point.second = _z;

//        if(new_object.points[i].texture_point.first < 0.3) {
//            cout << "(" << new_object.points[i].x << "," << new_object.points[i].y << "," << new_object.points[i].z
//                 << ")";
//            cout << new_object.points[i].texture_point.first << "," << new_object.points[i].texture_point.second
//                 << endl;
//        }
    }

    // save all faces denoted by the point index clockwise
    for(int i = 0 ; i < faces_number ; ++i) {
        vector<int> new_face;
        getline(data_stream, each_line);
        istringstream each_line_in(each_line);
        each_line_in >> face_points_number;

        for(int j = 0 ; j < face_points_number ; ++j) {
            each_line_in >> point_index;
            new_face.push_back(point_index - 1);
        }

        new_object.faces.push_back(new_face);
        new_object.face_normal.push_back(new_object.normal(new_face, i));
    }

    // compute point normal
    for(int i = 0 ; i < new_object.face_normal.size() ; ++i)
        for(int j = 0 ; j < new_object.faces[i].size() ; ++j)
            new_object.points[new_object.faces[i][j]].normal = new_object.points[new_object.faces[i][j]].normal + new_object.face_normal[i];

    for(int i = 0 ; i < new_object.points.size() ; ++i)
        if(new_object.points[i].normal.mold())
            new_object.points[i].normal = new_object.points[i].normal /new_object.points[i].normal.mold();

    // save texture map
    if(texture == "const-white")
        new_object.texture_map = vector<vector<vector<uint8_t>>>(1, vector<vector<uint8_t>>(1, vector<uint8_t>(3,255)));
    else if (texture == "const-black")
        new_object.texture_map = vector<vector<vector<uint8_t>>>(1, vector<vector<uint8_t>>(1, vector<uint8_t>(3,0)));
    else
        new_object.texture_map = read_bmp_to_buffer(texture);

    _scene.objects.push_back(new_object);

    return true;
}

/**
 * The file contains content like "5 5 5 0 0 0 0 1 0", meaning the camera is at (5, 5, 5), looking at (0,0,0), the up direction is vector(0,1,0)
 * @param file_name
 */
void engine::set_camera_position(const char* file_name) {
    ifstream data_stream;
    data_stream.open(file_name);

    string each_line;

    getline(data_stream, each_line);
    istringstream each_line_in(each_line);
    each_line_in >> _scene.camera_position.x >> _scene.camera_position.y >> _scene.camera_position.z
                 >> _scene.viewing_point.x >> _scene.viewing_point.y >> _scene.viewing_point.z
                 >> _scene.camera_up_direction.x >> _scene.camera_up_direction.y >> _scene.camera_up_direction.z;
}

void engine::denote_back_face() {
//    back_face_indexs.clear();

    for(int i = 0 ; i < _scene.objects.size() ; ++i) {
        for(int j = 0 ; j < _scene.objects[i].faces.size() ; ++j) {
            // Some the polygons are denoted in anti-clockwise order

            auto view = _scene.camera_position - _scene.objects[i].points[_scene.objects[i].faces[j][2]];

            view = view / view.mold();

            double cos = _scene.objects[i].face_normal[j].dot(view / view.mold());

            // slience the warning
            if((CLOCK_WISE_FACE && cos >= 0) || (!CLOCK_WISE_FACE && cos <= 0)) {
                if(back_face_indexs.find({i,j}) == back_face_indexs.end())
                    back_face_indexs.insert({i,j});
            }else{
                if(back_face_indexs.find({i,j}) != back_face_indexs.end())
                    back_face_indexs.erase({i,j});
            }
        }
    }
}

void engine::object_points_to_screen_points() {
    // Calculate N vector
    _scene.N = (_scene.viewing_point - _scene.camera_position) / (_scene.viewing_point - _scene.camera_position).mold();

    // Calculate U vector
    _scene.U = (_scene.N * _scene.camera_up_direction) / (_scene.N * _scene.camera_up_direction).mold();

    // Calculate V vector
    _scene.V =  _scene.U * _scene.N;

    // Find back face
    denote_back_face();

    // convert world coordinates into the camera coordinates, using R and T matrix
    matrix<double> R(
            {{_scene.U.x, _scene.U.y, _scene.U.z, 0},
             {_scene.V.x, _scene.V.y, _scene.V.z, 0},
             {_scene.N.x, _scene.N.y, _scene.N.z, 0},
             {0, 0, 0, 1},
            });

    matrix<double> T(
            {{1, 0, 0, -_scene.camera_position.x},
             {0, 1, 0, -_scene.camera_position.y},
             {0, 0, 1, -_scene.camera_position.z},
             {0, 0, 0, 1}
            });

    matrix<double> Mview(R.dot(T));

    // simply igone front cliping plane and back clipping plane, and set focus plane d to be 1

    double d = 1.0, h = 1.0, f = 1000;

    // [d,f] maps to [0,1]
    matrix<double> Mpers(
            {{d/h,     0,       0,       0},
             {0,       d/h,     0,       0},
             {0,       0,       f/(f-d), -d*f/(f-d)},
             {0,       0,       1,       0}
            });


    for(int i = 0 ; i < _scene.objects.size() ; ++i) {
        for(int j = 0 ; j < _scene.objects[i].points.size() ; ++j) {
            auto each = _scene.objects[i].points[j];
            matrix<double> each_point_matrix(each.to_matrix());
            each_point_matrix.push_back({1});
            each_point_matrix = Mpers * Mview * each_point_matrix;
            point3d transformed_point(each_point_matrix[0][0]/each_point_matrix[3][0], each_point_matrix[1][0]/each_point_matrix[3][0], each_point_matrix[2][0]/each_point_matrix[3][0]);
            _scene.objects[i].points[j].screen_point = transformed_point;
        }
    }
}

/**
 * For each polygon do scan conversion
 * Step 1: Add all newly intersect edges
 * Step 2: Remove all leaving edges
 * Step 3: Update all intersection information
 */
void engine::scan_conversion() {

    /* each loop handel one face*/
    z_buffer = vector<vector<double>>(WINDOW_X, vector<double>(WINDOW_Y,1));

    /* May render several times, initialize the buffers with background color */
    for(int i = 0 ; i < WINDOW_X ; ++i)
        for(int j = 0 ; j < WINDOW_Y ; ++j)
            pixel_buffer[i][j][0] = _config._back_r, pixel_buffer[i][j][1] = _config._back_g, pixel_buffer[i][j][2] = _config._back_b;

    for(int o_k = 0 ; o_k < _scene.objects.size() ; ++o_k) {
        type_polygonal_object each_object = _scene.objects[o_k];
        for(int i = 0 ; i < each_object.faces.size() ; ++i) {
            if(back_face_indexs.find({o_k,i}) != back_face_indexs.end())
                continue;
            vector<int> each_face = each_object.faces[i];
            int point_number = each_face.size(), _gray = 0;

            for(auto& each:edge_table)
                if(each.size()) each.clear();

            for(int j = 0 ; j < point_number ; ++j) {
                // add each edge to the edge table
                // two point indexs are each_face[j], each_face[(j+1)%each_face.size()]
                int index_start = each_face[j], index_end = each_face[(j+1)%point_number];
                auto lower_point = each_object.points[index_start].screen_point, upper_point = each_object.points[index_end].screen_point;

                // if out of bound, exit, TODO: not exit...
                if(lower_point.y > 1 || lower_point.y < -1 || lower_point.x > 1 || lower_point.x < -1 ||
                   upper_point.y > 1 || upper_point.y < -1 || upper_point.x > 1 || upper_point.x < -1)
                    exit(12);

                // if it is a horizontal edge, don't worry
                if(to_pixel(lower_point.y) == to_pixel(upper_point.y))
                    continue;

                if(lower_point.y > upper_point.y) {
                    swap(lower_point, upper_point);
                    swap(index_start, index_end);
                }

                auto texture_delta = pair<double,double>({
                                                                 (each_object.points[index_end].texture_point.first - each_object.points[index_start].texture_point.first) / ((double)(to_pixel(upper_point.y, true) - to_pixel(lower_point.y, true))) ,
                                                                 (each_object.points[index_end].texture_point.second - each_object.points[index_start].texture_point.second) / ((double)(to_pixel(upper_point.y, true) - to_pixel(lower_point.y, true)))
                });

                edge_table_element new_element(to_pixel(lower_point.y, true),
                                               to_pixel(upper_point.y, true),
                                               to_double_pixel(lower_point.x),
                                               (double)(lower_point.x - upper_point.x) / (double)(lower_point.y - upper_point.y),
                                               lower_point.z,
                                               (double)(lower_point.z - upper_point.z) / (double)(to_pixel(lower_point.y, true) - to_pixel(upper_point.y, true)),
                                               each_object.points[index_start].normal,
                                               each_object.points[index_end].normal,
                                               each_object.points[index_start].texture_point,
                                               texture_delta
                );

                edge_table[to_pixel(lower_point.y, true)].push_back(new_element);
            }

            /* the edge table has been built, use scan line to decide the pixels' color */

            // In the edge table, x, y are from 0 to WINDOW_SIZE

            // ei is the current scan line's y coordinate
            int ei = 0;

            // find the first y place to start TODO: edge_table as data structure
            for(; ei < WINDOW_Y ; ++ei) if(edge_table[ei].size()) break;

            // each loop scan each line
            for(; ei < WINDOW_Y ; ++ei) {
                // move edge(s) from ET bucket y whose ymin = y (entering edges) to ATE,
                for(int j = 0 ; j < edge_table[ei].size() ; ++j)
                    active_edge_table.push_back(edge_table[ei][j]);

                edge_table[ei].clear();

                // Remove the leaving edge
                vector<edge_table_element>::iterator it = active_edge_table.begin();
                while(it != active_edge_table.end()) {
                    if(ei == it->y_max)
                        // Remove from AET entries for which y = ymax (leave edges)
                        active_edge_table.erase(it);
                    else
                        // replace x by x + increment
                        // This places next scan-line intersection into each entry in AET
                        it++;
                }

                sort(active_edge_table.begin(), active_edge_table.end(), act_cmp());

                //  Fill in desired pixel values on scan line y by using pairs of xcoordinates from the AET
                for(double k = 0 ; k < active_edge_table.size() ; k += 2) {

                    edge_table_element *span_left = &active_edge_table[k], *span_right = &active_edge_table[k+1];
                    double z_current = span_left->z_start, z_delta_to_x = (span_left->z_start - span_right->z_start) / (span_left->x_start - span_right->x_start);

                    // any time span_left->y_max >= span_left->y_start
                    vector3d
                            la = span_left->normal_end * (double)(ei - span_left->y_start)/(double)(span_left->y_max - span_left->y_start)
                                 + span_left->normal_start * (double)(span_left->y_max - ei)/(double)(span_left->y_max - span_left->y_start),

                            lb = span_right->normal_end * (double)(ei - span_right->y_start)/(double)(span_right->y_max - span_right->y_start)
                                 + span_right->normal_start * (double)(span_right->y_max - ei)/(double)(span_right->y_max - span_right->y_start);

//                if((int)span_left->x_start == (int)span_right->x_start)
//                    cout << "Single point" << endl;
//
//
//                    cout << "Line start , left : " << span_left->texture_start.first << " , right : " << span_right->texture_start.first << endl;

//                    cout << "line start: left(" << span_left->texture_start.first << "), right(" << span_right->texture_start.first << ")" << endl;

                    // pixel scaned horizontally
                    for(int l = int(span_left->x_start)  ; l < int(span_right->x_start) ; ++l) {

                        if(z_current < z_buffer[l][ei]){

                            // update depth buffer
                            z_buffer[l][ei] = z_current;

                            // Phong illumination is based on normal
                            vector3d cur_normal;

                            // texture map coordinate
                            double real_texture_x = l == (int)span_left->x_start
                                                    ? span_left->texture_start.first
                                                    : span_left->texture_start.first * ((int)span_right->x_start - l) / ((int)span_right->x_start - (int)span_left->x_start)
                                                      + span_right->texture_start.first * (l - (int)span_left->x_start) / ((int)span_right->x_start - (int)span_left->x_start);
                            double real_texture_y = l == (int)span_left->x_start
                                                    ? span_left->texture_start.second
                                                    : span_left->texture_start.second * ((int)span_right->x_start - l) / ((int)span_right->x_start - (int)span_left->x_start)
                                                      + span_right->texture_start.second * (l - (int)span_left->x_start) / ((int)span_right->x_start - (int)span_left->x_start);

//                            cout << "(" << span_left->texture_start.first << "," << span_right->texture_start.first << ")" <<
//                                 ((int)span_right->x_start - l) << "," << (l - (int)span_left->x_start) << "," << ((int)span_right->x_start - (int)span_left->x_start) << "," <<
//                                 span_left->texture_start.first * ((int)span_right->x_start - l) / ((int)span_right->x_start - (int)span_left->x_start)
//                                    + span_right->texture_start.first *  (l - (int)span_left->x_start) / ((int)span_right->x_start - (int)span_left->x_start)
//                                 << endl;


                            if(l == (int)span_left->x_start) {
                                real_texture_x = span_left->texture_start.first, real_texture_y = span_left->texture_start.second;
                                if(ei == span_left->y_start)
                                    cur_normal = span_left->normal_start;
                                else
                                    cur_normal = la;
                            }else {
                                cur_normal = la * (span_right->x_start - l) + lb * (l - span_left->x_start);
                            }

                            // Normalizing is so important
                            cur_normal = cur_normal / cur_normal.mold();

                            vector3d light_vector = point3d(10,10,10) - point3d(0,0,0);
                            light_vector = light_vector / light_vector.mold();

                            int gray = diffuse_term(1, 100, cur_normal, light_vector, 2);

                            if(gray < 0) gray = 0;

                            if(gray > 255) gray = 255;

                            // calculate texture coordinate

//                            cout << real_texture_x << endl;

                            double inten = (double)gray / 100.0;

//                            real_texture_x = real_texture_x < 0 ? 0 : real_texture_x;
//                            real_texture_x = real_texture_x >= 1 ? 0.99 : real_texture_x;
//
//                            real_texture_y = real_texture_y < 0 ? 0 : real_texture_y;
//                            real_texture_y = real_texture_y >= 1 ? 0.99 : real_texture_y;

//                            cout << real_texture_x << "," << real_texture_y << endl;

                            int texture_x = real_texture_x * each_object.texture_map.size(),
                            texture_y = real_texture_y * each_object.texture_map[0].size();

//                            cout << texture_x << "," << texture_y << endl;

//                            if(span_left->texture_start.first < 0.3)
//                                cout << span_left->texture_start.first << endl;

                            auto __r = max(min(each_object.texture_map[texture_x][texture_y][0] * inten  + 30,255.0), 0.0);
                            auto __g = max(min(each_object.texture_map[texture_x][texture_y][1] * inten  + 30,255.0), 0.0);
                            auto __b = max(min(each_object.texture_map[texture_x][texture_y][2] * inten  + 30,255.0), 0.0);

                            if(o_k == 2 || o_k == 3)
                                __r = __g = __b = 0;

                            if(o_k == 4 || o_k == 5)
                                __r = __g = __b = 200;

//                            auto __r = real_texture_x * 255;
//                            auto __g = real_texture_x * 255;
//                            auto __b = real_texture_x * 255;

//                            auto __r = gray;
//                            auto __g = gray;
//                            auto __b = gray;

//                            auto __r = polygon_random_color;
//                            auto __g = polygon_random_color;
//                            auto __b = polygon_random_color;
//                            cout << span_left->texture_start.first << endl;
//                            if(l == (int)span_left->x_start) {
                                pixel_buffer[ei][l][0] = __r;
                                pixel_buffer[ei][l][1] = __g;
                                pixel_buffer[ei][l][2] = __b;
//                            }
//                            pixel_buffer[ei][l][0] =  each_object.texture_map[texture_x][texture_y][0];
//                            pixel_buffer[ei][l][1] =  each_object.texture_map[texture_x][texture_y][1];
//                            pixel_buffer[ei][l][2] =  each_object.texture_map[texture_x][texture_y][2];

//                            cout << each_object.texture_map[texture_x][texture_y][0] << endl;

                        }
                        z_current += z_delta_to_x;
                    }
                    // Update x and z
                    span_left->x_start += span_left->delta;
                    span_right->x_start += span_right->delta;

                    span_left->z_start += span_left->z_delta_to_y;
                    span_right->z_start += span_right->z_delta_to_y;

                    // update texture map
                    span_left->texture_start.first += span_left->texture_delta_to_y.first, span_left->texture_start.second += span_left->texture_delta_to_y.second;
                    span_right->texture_start.first += span_right->texture_delta_to_y.first, span_right->texture_start.second += span_right->texture_delta_to_y.second;
//                    cout << span_left->texture_delta_to_y.first << "  ,  " << span_left->texture_delta_to_y.second << endl;
//
//                    if(abs(span_left->texture_start.first) >= 1)
//                        span_left->texture_start.first = 0;

                }
            }

        }

    }
//    cout << "______" << endl;

}



#endif //GRAPHIC_ENGINE_H

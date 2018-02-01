//
// Created by Zheng XiangYue on 28/1/2018.
//

#include "homework1.h"
using namespace std;

/**
 * read data from .d file, only compatable for .d file
 * @param file_name
 * @return bool
 */
bool homework1::set_object_position(const char* file_name) {

    int points_number, faces_number, temp_point_index, face_points_number;
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
        return false;
    }

    // read how many points and faces are in the object
    each_line_in >> points_number >> faces_number;

    // save all points;
    for(int i = 0 ; i < points_number ; ++i) {
        point3d new_point;
        getline(data_stream, each_line);
        istringstream each_line_in(each_line);
        each_line_in >> new_point.x >> new_point.y >> new_point.z;
        object.points.push_back(new_point);
    }

    // save all faces denoted by the point index clockwise
    for(int i = 0 ; i < faces_number ; ++i) {
        vector<int> face;
        getline(data_stream, each_line);
        istringstream each_line_in(each_line);
        each_line_in >> face_points_number;
        for(int j = 0 ; j < face_points_number ; ++j) {
            each_line_in >> temp_point_index;
            face.push_back(temp_point_index-1);
        }
        object.faces.push_back(face);
    }

    return true;
}


/**
 * The file contains content like "5 5 5 0 0 0 0 1 0", meaning the camera is at (5, 5, 5), looking at (0,0,0), the up direction is vector(0,1,0)
 * @param file_name
 */
void homework1::set_camera_position(const char* file_name) {
    ifstream data_stream;
    data_stream.open(file_name);

    string each_line;

    getline(data_stream, each_line);
    istringstream each_line_in(each_line);
    each_line_in >> camera_position.x >> camera_position.y >> camera_position.z
                 >> viewing_point.x >> viewing_point.y >> viewing_point.z
                 >> camera_up_direction.x >> camera_up_direction.y >> camera_up_direction.z;
}

/**
 * for each face in the polygon, if the normal Np .* N <= 0 , denote it as a back_face
 */
void homework1::denote_back_face() {
    back_face_indexs.clear();
    for(int i = 0 ; i < object.faces.size() ; ++i) {
        if(object.normal(object.faces[i]).dot(camera_position - object.points[object.faces[i][0]]) <= 0)
            back_face_indexs.insert(i);
    }
}




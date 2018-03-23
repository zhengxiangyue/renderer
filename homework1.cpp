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
bool homework1::set_object_position(const char* file_name, const vector<int> position ) {

    int points_number, faces_number, temp_point_index, face_points_number, current_point_number = object.points.size();
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

        new_point.x += position[0];
        new_point.y += position[1];
        new_point.z += position[2];

        object.points.push_back(new_point);
        object.point_normal.push_back({0,0,0});
    }

    // save all faces denoted by the point index clockwise
    for(int i = 0 ; i < faces_number ; ++i) {
        vector<int> face;
        getline(data_stream, each_line);
        istringstream each_line_in(each_line);
        each_line_in >> face_points_number;
        for(int j = 0 ; j < face_points_number ; ++j) {
            each_line_in >> temp_point_index;
            face.push_back(temp_point_index-1+current_point_number);
        }
        object.faces.push_back(face);
        object.face_normal.push_back(object.normal(face, i));
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
        // Some the polygons are denoted in anti-clockwise order

        auto view = camera_position - object.points[object.faces[i][1]];

        double cos = object.face_normal[i].dot(view / view.mold());

        cout << "Face " << i << ",View:" << view / view.mold() << ",Cos:" << cos << endl;

        if(CLOCK_WISE_FACE && cos >= 0 || !CLOCK_WISE_FACE && cos <= 0) {
            back_face_indexs.insert(i);
        }
    }
}

void homework1::object_points_to_screen_points() {
    // Calculate N vector
    N = (viewing_point - camera_position) / (viewing_point - camera_position).mold();

    // Calculate U vector
    U = (N * camera_up_direction) / (N * camera_up_direction).mold();

    // Calculate V vector
    V =  U * N;

    // Find back face
    denote_back_face();

    // convert world coordinates into the camera coordinates, using R and T matrix
    matrix<double> R(
            {{U.x, U.y, U.z, 0},
             {V.x, V.y, V.z, 0},
             {N.x, N.y, N.z, 0},
             {0, 0, 0, 1},
            });

    matrix<double> T(
            {{1, 0, 0, -camera_position.x},
             {0, 1, 0, -camera_position.y},
             {0, 0, 1, -camera_position.z},
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


    screen_points.clear();
    // Perspective transfomation for each point
    for(auto each:object.points){
        matrix<double> each_point_matrix(each.to_matrix());
        each_point_matrix.push_back({1});
        each_point_matrix = Mpers * Mview * each_point_matrix;
        point3d transformed_point(
                each_point_matrix[0][0]/each_point_matrix[3][0],
                each_point_matrix[1][0]/each_point_matrix[3][0],
                each_point_matrix[2][0]/each_point_matrix[3][0]
        );
        screen_points.push_back(transformed_point);
    }
}



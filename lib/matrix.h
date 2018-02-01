//
// Created by Zheng XiangYue on 29/1/2018, G42416206
//

#ifndef GRAPHIC_MATRIX_H
#define GRAPHIC_MATRIX_H

#include <vector>
#include <iostream>
using namespace std;

template<class T>
class matrix {
    vector<vector<T>> values;

public:

    matrix(){}

    matrix(const vector<vector<T>> values) : values(values) {}

    /**
     * get matrix value
     * @param i
     * @return
     */
    vector<T>& operator [](int i);

    /**
     *
     * @param back_matrix
     * @return
     */
    matrix<T> dot(matrix<T> back_matrix);

    matrix<T> operator * (matrix<T> back_matrix);

    /**
     *
     * @param new_column
     */
    void push_back(vector<T> new_column);

    matrix<T> transpose();

    const int size();

    void show();
};

/**
 * I always want to give something to STREAM
 * @param co
 * @param mat
 * @return
 */
template<class T> ostream& operator <<(ostream &co, const matrix<T> &mat);

template<class T> vector<T>& matrix<T>::operator [](int i) {
//    if(i >= values.size()) return {};
    return values[i];
}

template<class T> matrix<T> matrix<T>::dot(matrix<T> back_matrix) {
    int column = values.size(), row = back_matrix[0].size(), middle = back_matrix.size();

    vector<vector<T>> res_vector(column, vector<T>(row, 0));
    matrix<T> result(res_vector);

    for(int i = 0 ; i < column ; ++i) {
        for(int j = 0 ; j < middle ; ++j) {

            if(values[i][j] == 0) continue;

            for(int k = 0 ; k < row ; ++k) {
                result[i][k] += (values[i][j] * back_matrix[j][k]);
            }

        }
    }

    return result;

}

template<class T> void matrix<T>::push_back(vector<T> new_column) {
    values.push_back(new_column);
}

template<class T> matrix<T> matrix<T>::transpose() {
    int column_size = values.size();
    if(column_size == 0) return matrix<T>();
    int row_size = values[0].size();
    vector<vector<T>> new_values(row_size, vector<T>(column_size));

    for(int i = 0 ; i < column_size ; ++i) {
        for(int j = 0 ; j < row_size ; ++j) {
            new_values[j][i] = values[i][j];
        }
    }
    matrix<T> result = matrix<T>(new_values);
    return result;
}

template<class T> const int matrix<T>::size() {
    return values.size();
}

template<class T> void matrix<T>::show() {
    for(int i = 0 ; i < values.size() ; ++i ) {
        for(int j = 0 ; j < values[0].size() ; ++j) {
            cout << values[i][j] << " ";
        }
        cout << endl;
    }
}

template<class T>
matrix<T> matrix<T>::operator*(matrix<T> back_matrix) {
    int column = values.size(), row = back_matrix[0].size(), middle = back_matrix.size();

    vector<vector<T>> res_vector(column, vector<T>(row, 0));
    matrix<T> result(res_vector);

    for(int i = 0 ; i < column ; ++i) {
        for(int j = 0 ; j < middle ; ++j) {

            if(values[i][j] == 0) continue;

            for(int k = 0 ; k < row ; ++k) {
                result[i][k] += (values[i][j] * back_matrix[j][k]);
            }

        }
    }

    return result;
}


#endif //GRAPHIC_MATRIX_H

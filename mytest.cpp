//
// Created by Zheng XiangYue on 17/4/2018.
//

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

int main() {
    ifstream imgStream("earth.bmp");

    char info[54];

    imgStream.read(info, 54);

    int width = *(int*)&info[18], height = *(int*)&info[22], row_padded = (width*3 + 3) & (~3);

    char *data = new char[row_padded];

    vector<vector<vector<uint8_t> > > image(height, vector<vector<uint8_t> >(width, vector<uint8_t>(3,0)));

    for(int i = 0 ; i < height ; ++i) {
        imgStream.read(data, row_padded);
        for(int j = 0 ; j < width * 3 ; j += 3) {
            image[i][j/3][0] = (int)data[j+2]+127;
            image[i][j/3][0] = (int)data[j+1]+127;
            image[i][j/3][0] = (int)data[j]+127;
        }
    }

    delete[] data;

    glClear(GL_COLOR_BUFFER_BIT);

    glDrawPixels(height,width, image, GL_UNSIGNED_BYTE, hk.pixel_buffer);
    glFlush();

    return 0;

}
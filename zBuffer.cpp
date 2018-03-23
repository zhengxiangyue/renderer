// Auth: zousong@cad.zju.edu.cn
// Date: 2008-12-25

// 定义是否每个面片使用随机颜色值
#define RANDOM_COLOR 1

#include <stdio.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifdef RANDOM_COLOR
#include <time.h>
#include <stdlib.h>
#endif

using namespace std;

#define MAX_WIDTH (1024*2)
#define MAX_HEIGHT (768*2)

//定义图像
#define	INIT_WIDTH 1024
#define	INIT_HEIGHT 768
GLubyte image[INIT_HEIGHT][INIT_WIDTH][3];
// 定义颜色结构体
typedef struct
{
    unsigned char r, b, g, a;
} color_t;

// 定义点结构
typedef struct
{
    double x, y, z;
} point_t;

// 定义分类边表
typedef struct _line_t
{
    int x;						// 边的上端点的x坐标
    double dx;					// 相邻两条扫描线交点的x坐标差dx(-1/k)
    int dy;						// 边跨越的扫描线数目
    int id;						// 边所属多边形的编号
    //_line_t * next;				// 下个分类边表指针, 由于本算法中分类边表存于STL Vector中, 故不需要
} line_t;

// 定义活化边表
typedef struct _active_line_t
{
    double xl;						// 左交点的x坐标
    double dxl;					// (左交点边上)两相邻扫描线交点的x坐标之差
    int dyl;					// 以和左交点所在边相交的扫描线数为初值, 以后向下每处理一条扫描线减1
    double zl;						// 左交点处多边形所在平面的深度值
    double dzx;					// 沿扫描线向右走过一个像素时, 多边形所在平面的深度增量. 多于平面方程, dzx = -a/c (c!= 0)
    double dzy;					// 沿y方向向下移过一根扫描线时, 多边形所在平面的深度增量. 对于平面方程, dzy = b/c (c!= 0)

#if 0  // 由于边对分拆在两个活化边表结构中，故右节点的元素可以省略
    //	double xr;						// 右交点的x坐标
	//	double dxr;					// (右交点边上)两相邻扫描线交点的x坐标之差
	//	int dyr;					// 以右左交点所在边相交的扫描线数为初值, 以后向下每处理一条扫描线减1
#endif
    //	int	id;						// 交点所在多边形的编号, 由于将活化边表存于各自所属的活化多边形中, 故所属ID不需要保存
    //_active_line_t * next;		// 下个活化边表指针, 由于本算法中活化边表存于STL Vector中, 故不需要
} active_line_t;

// 定义分类多边形表 ||  活化多边形表
typedef struct  _poly_t
{
    int id;						// 多边形编号
    double a,b,c,d;				// 多边形所在的平面的方程系数 ax + by + cz + d = 0
    int dy;						// 多边形跨越的扫描线数目 || 多边形跨越的剩余扫描线数目
    color_t color;			// 多边形颜色
    vector <active_line_t> ale;  // 活化边表, 放在活化多边形中, 对于分类多边形表无意义
    //_poly_t * next;				// 下个分类多边形指针 || 下个活化多边形指针, 由于本算法中多边形表存于STL Vector中, 故不需要
} poly_t;


inline bool is_zero(double f)
{
    if (abs(f) < 1e-8)
        return true;
    return false;
}

inline int double_to_int(double f)
{
    return (int)(f+0.5);
}

// 对活化多边形表排序的比较函数,
bool cmp(const active_line_t &lhs, const active_line_t &rhs)
{
    if (double_to_int(lhs.xl) < double_to_int(rhs.xl) ) {
        return true;
    } else if ( double_to_int(lhs.xl) == double_to_int(rhs.xl)  ){
        if (double_to_int(lhs.dxl) < double_to_int(rhs.dxl))
            return true;
    }
    return false;
}


// 从obj文件中读取的点
vector <point_t>	point;
// 分类多形型表
vector <poly_t>	poly[MAX_HEIGHT];
// 分类边表
vector <line_t>		line[MAX_HEIGHT];
// 活化多边形表, 内部包含各个活化多边形表中的活化边表对
vector <poly_t> ape;


// z值数组
double						z[MAX_WIDTH];
// 颜色数组
color_t					color[MAX_WIDTH];
// 处理字符串用的buffer
char						buf[1024];

int cal(const char * file_name, int max_width, int max_height, double scale)
{
    // 判断参数是否合法
    if (file_name == NULL || max_width <= 0 || max_width > MAX_WIDTH || max_height <= 0 || max_height > MAX_HEIGHT || scale < 1.0) {
        printf("cal err!\n");
        return -1;
    }

#ifdef RANDOM_COLOR
    // 随机种子
    srand(time(NULL));
#endif
    double max_xy = -1000000.0;

    // 首先建立分类的多边形表和边表
    FILE * file = fopen(file_name,"r");
    if (file == NULL) {
        printf("open %s err!\n",file_name);
        return -1;
    }
    // 读取点
    while (fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
            case 'v':
                switch(buf[1]) {
                    case '\0':  // 扫描到一个点
                        fgets(buf, sizeof(buf), file);
                        point_t pt;
                        sscanf(buf,"%lf%lf%lf", &(pt.x), &(pt.y), &(pt.z));
                        // 存储最大的坐标值
                        if (abs(pt.x) > max_xy)
                            max_xy = abs(pt.x);
                        if (abs(pt.y) > max_xy)
                            max_xy = abs(pt.y);
                        //printf("%lf %lf %lf\n",pt.x,pt.y,pt.z);
                        point.push_back(pt);
                        break;
                    default:
                        fgets(buf, sizeof(buf), file);
                        break;
                }
                break;
            default:
                fgets(buf, sizeof(buf), file); // 读取此line中剩余字符
                break;
        }
    }
    printf("read %d points.\n",point.size());
    if (point.size() <= 0) {
        fclose(file);
        return -1;
    }
    // 将扫描到的点转化为屏幕中像素坐标
    double max_scale;
    if (max_width < max_height) {
        max_scale = max_width/(2*max_xy*scale);
    } else {
        max_scale = max_height/(2*max_xy*scale);
    }
    for (vector <point_t> :: iterator it = point.begin(); it != point.end(); ++it) {
        it->x = double_to_int( it->x*max_scale + max_width / 2);
        it->y = max_height - double_to_int( it->y*max_scale + max_height / 2);
        it->z = double_to_int( it->z*max_scale);
    }

    // 读取面
    fseek(file,0,SEEK_SET);
    int poly_idx = 0;
    int line_idx = 0;
    int v,n;

    while (fscanf(file, "%s", buf) != EOF) {
        switch(buf[0]) {
            case 'f':
                v = n = 0;
                fscanf(file, "%s", buf);
                if (strstr(buf, "//")) {  // 获取到一个面
                    line_t line_1;
                    int max_idx;
                    int min_idx;
                    int idx[3];
                    int max_y = 0;
                    int min_y = max_height;

                    poly_t poly_1;
                    poly_1.id = poly_idx; // 设置多边形ID
                    // TODO get color, 设置多边形颜色
                    poly_1.color.r = 0xFF;
                    poly_1.color.g = 0;
                    poly_1.color.b = 0;
#ifdef RANDOM_COLOR
                    poly_1.color.r = rand() % 0xFF;
                    poly_1.color.g = rand() % 0xFF;
                    poly_1.color.b = rand() % 0xFF;
#endif
                    // 先读取三个点
                    sscanf(buf, "%d//%d", &v, &n);
                    idx[0] = v - 1;
                    fscanf(file, "%d//%d", &v, &n);
                    idx[1] = v - 1;
                    fscanf(file, "%d//%d", &v, &n);
                    idx[2] = v - 1;
                    // 先添加前两条边
                    for (int i = 0; i < 2; i++) {
                        // 添加一条新的边
                        memset(&line_1,0,sizeof(line_1));
                        // 设置线所属的多边形id
                        line_1.id = poly_idx;
                        // 哪个点的y值较大
                        if (point[ idx[i] ].y < point[ idx[i+1] ].y ) {
                            max_idx =idx[i+1];
                            min_idx = idx[i];
                        } else {
                            max_idx = idx[i];
                            min_idx = idx[i+1];
                        }
                        // 将线的x字段设置为y较大值的x值
                        line_1.x = point[max_idx].x;
                        if (line_1.x < 0) {
                            printf("line_1.x %lf < 0!\n",line_1.x);
                        }
                        //printf("max_idx %d, min_idx %d\n",max_idx,min_idx);
                        // 将线的dy字段设置为两个点的y差
                        line_1.dy = point[max_idx].y - point[min_idx].y;
                        // 将线的dx字段设置为两个点的x差
                        line_1.dx =  point[min_idx].x - point[max_idx].x; // TODO
                        //printf("new line: id=%d, x=%d, dx=%lf, dy=%d\n", line_1.id, line_1.x, line_1.dx,line_1.dy);
                        // 将此边添加进分类边表中
                        line[ double_to_int(point[max_idx].y) ].push_back(line_1);
                        line_idx++;
                    }


                    // 计算 ax + by + cz + d = 0
                    poly_1.a = ( point[ idx[1] ].y - point[ idx[0] ].y ) * ( point[ idx[2] ].z - point[ idx[0] ].z )
                               - ( point[ idx[1] ].z - point[ idx[0] ].z ) * ( point[ idx[2] ].y - point[ idx[0] ].y );
                    poly_1.b = ( point[ idx[1] ].z - point[ idx[0] ].z ) * ( point[ idx[2] ].x - point[ idx[0] ].x )
                               - ( point[ idx[1] ].x - point[ idx[0] ].x ) * ( point[ idx[2] ].z - point[ idx[0] ].z );
                    poly_1.c = ( point[ idx[1] ].x - point[ idx[0] ].x ) * ( point[ idx[2] ].y - point[ idx[0] ].y )
                               - ( point[ idx[1] ].y - point[ idx[0] ].y ) * ( point[ idx[2] ].x - point[ idx[0] ].x );
                    poly_1.d = - ( poly_1.a * point[ idx[0] ].x + poly_1.b * point[ idx[0] ].y + poly_1.c * point[ idx[0] ].z );

                    // 计算多边形最大和最小的y值
                    for (int i = 0; i < 3; i++) {
                        if (double_to_int(point[ idx[i] ].y) < min_y) {
                            min_y = double_to_int(point[ idx[i] ].y);
                        }
                        if (double_to_int(point[ idx[i] ].y) > max_y) {
                            max_y = double_to_int(point[ idx[i] ].y);
                        }
                    }

                    // 继续处理多于3个的点
                    while(fscanf(file, "%d//%d", &v, &n) > 0) {
                        // TODO
                        idx[1] = idx[2];
                        idx[2] = v - 1;
                        if (1) // 与上面添加边的过程一致
                        {
                            // 添加一条新的边
                            int i = 1;
                            memset(&line_1,0,sizeof(line_1));
                            line_1.id = poly_idx;
                            if (point[ idx[i] ].y < point[ idx[i+1] ].y ) {
                                max_idx =idx[i+1];
                                min_idx = idx[i];
                            } else {
                                max_idx = idx[i];
                                min_idx = idx[i+1];
                            }
                            line_1.x = point[max_idx].x;
                            if (line_1.x < 0) {
                                printf("line_1.x %lf < 0!\n",line_1.x);
                            }
                            line_1.dy = point[max_idx].y - point[min_idx].y;
                            line_1.dx =  point[min_idx].x - point[max_idx].x; // TODO
                            //printf("new line: id=%d, x=%d, dx=%lf, dy=%lf\n", line_1.id, line_1.x, line_1.dx,line_1.dy);
                            line[ double_to_int(point[max_idx].y) ].push_back(line_1);
                            line_idx++;
                        }
                        // 计算多边形最大和最小的y值
                        if (double_to_int(point[ idx[2] ].y) < min_y) {
                            min_y = double_to_int(point[ idx[2] ].y);
                        }
                        if (double_to_int(point[ idx[2] ].y) > max_y) {
                            max_y = double_to_int(point[ idx[2] ].y);
                        }
                    }
                    if (1) // 处理最后一个点与第一个点的连线, 与上面添加边的过程一致
                    {
                        // 添加一条新的边
                        idx[1] = idx[2];
                        idx[2] = idx[0];
                        int i = 1;
                        memset(&line_1,0,sizeof(line_1));
                        line_1.id = poly_idx;
                        //line_1.next = NULL;
                        if (point[ idx[i] ].y < point[ idx[i+1] ].y )
                        {
                            max_idx =idx[i+1];
                            min_idx = idx[i];
                        } else {
                            max_idx = idx[i];
                            min_idx = idx[i+1];
                        }
                        line_1.x = point[max_idx].x;
                        if (line_1.x < 0) {
                            printf("line_1.x %lf < 0!\n",line_1.x);
                        }
                        line_1.dy = point[max_idx].y - point[min_idx].y;
                        line_1.dx =  point[min_idx].x - point[max_idx].x; // TODO
                        //printf("new line: id=%d, x=%d, dx=%lf, dy=%lf\n", line_1.id, line_1.x, line_1.dx,line_1.dy);
                        line_idx++;
                        line[ double_to_int(point[max_idx].y) ].push_back(line_1);
                    }
                    //printf("%d:a=%lf, b=%lf, c=%lf, d=%lf\n",(int)max_y, poly_1.a,poly_1.b,poly_1.c,poly_1.d);
                    poly_1.dy = max_y - min_y + 1;
                    poly[max_y].push_back(poly_1);
                    poly_idx++;
                } else {
                    //printf("TODO with others!\n");
                    fgets(buf, sizeof(buf), file);
                }
                break;
            default:
                fgets(buf, sizeof(buf), file); // 读取此line中剩余字符
                break;
        }
    }

    fclose(file);

    // 至此已从obj文件中读取分类多边形与分类边表信息
    printf("read %d polys, %d lines\n",poly_idx,line_idx);
#if 1
    for (int i = 0; i < max_height; i++)
    {
        if (poly[i].size() > 0 || line[i].size() > 0)
        {
            printf("y = %4d: %d polys; %d lines.\n", i, poly[i].size(),line[i].size());
        }
    }
#endif

    // 扫描顺序从上到下: 在处理最上面一条扫描线之前, 活化的多边形表和边表是空的
    // 在处理每条扫描线时, 做如下工作:
    int y;
    for (y = max_height-1; y >= 0; y--) {
        // 把帧缓冲器的相应行设置成底色
        memset(color,0,sizeof(color));
        // 把z缓冲器的各个单元设置成最小值(表示离视点最远)
        for (int i = 0; i < max_width; i++)
        {
            z[i] = -99999;
        }
        //memset(z,-9999,sizeof(z));
        // 检查分类的多边形表, 如果有新的多边形涉及该扫描线, 则把它放入活化的多边形表中
        for (vector <poly_t>::const_iterator it = poly[y].begin(); it != poly[y].end(); ++it) {
            ape.push_back(*it);
            //printf("add a new poly, now %d\n",ape.size());
        }

        // 如果有新的多边形加入到活化多边形表中, 则把该多边形在oxy平面上的投影和扫描线相交的边加入到活化边表中
        // 如果有些边在这条扫描线处结束了, 而其所在的多边形仍在活化多边形表内,
        // 则可以从分类多边形表中找到该多边形在oxy平面上的投影于扫描线相交的新边或对边, 修改或加到活化边表中去。
        // 边对在活化边表中的次序是不重要的

        bool flag = false;
        // 对此y扫描线上的活化多边形分别处理
        for (vector <poly_t>::iterator it = ape.begin(); it != ape.end(); ) {
            if (it->dy <= 0) {		// 如果此活化多边形的dy<= 0, 则删除此活化多边形
                ape.erase(it,it+1);
                //printf("erase a poly, now %d\n",ape.size());
            } else {
                // 加入新的活化边
                for (vector <line_t>::iterator it_1 = line[y].begin(); it_1 != line[y].end(); ) {
                    if (it->id == it_1->id) { // 此边属于此活化多边形, 加入
                        active_line_t al;
                        al.xl	= it_1->x;
                        if (al.xl < 0) {
                            printf("al.xl %lf < 0!\n",al.xl);
                        }
                        if (is_zero(it_1->dy)) {
                            al.dxl	= 0;
                        } else {
                            al.dxl	= it_1->dx / it_1->dy;
                        }
                        al.dyl	= it_1->dy;
                        if (is_zero(it->c)) {
                            //printf("TODO c == 0 !\n");
                            al.zl = 0;
                            al.dzx	= 0;
                            al.dzy	= 0;
                        } else {
                            al.zl	= - ( it->a*it_1->x + it->b*y + it->d) / it->c;  // ax + by + cz + d = 0;
                            al.dzx	= - (it->a / it->c);
                            al.dzy	= it->b / it->c;
                        }
                        it->ale.push_back(al); // 将此活化边表加入此活化多边形表中
                        // 为了下个活化多边形查找分类边表时方便, 将此分类边表删除(此分类边表已经不需要了)
                        line[y].erase(it_1, it_1 + 1);
                    } else {
                        ++it_1;
                    }
                }
                // 删除已经结束的活化边
                for ( vector <active_line_t>::iterator it_1 = it->ale.begin(); it_1 != it->ale.end();  ) {
                    if (it_1->dyl <= 0) {
                        it->ale.erase(it_1,it_1+1);
                    } else {
                        ++it_1;
                    }
                }
                // 属于此活化多边形的活化边表肯定是成对的
                if (it->ale.size() %2) {
                    printf("ERR: it->ale.size() = %d!\n",it->ale.size());
                    return -1;
                }
                // 对新的活化边表排序
                sort(it->ale.begin(), it->ale.end(), cmp);
                // 对活化边表对之间区间进行着色， 每两个活化边表分别为左右两边
                for ( vector <active_line_t>::iterator left = it->ale.begin(); left != it->ale.end();  left += 2) {
                    vector <active_line_t>::iterator right = left + 1;
                    double zx =left->zl;
                    for (int x = double_to_int(left->xl); x < double_to_int(right->xl); x++) {
                        if (zx >= z[x]) {
                            z[x] = zx;
                            // TODO  设置颜色
                            color[x] = it->color;
                        }
                        zx += left->dzx;
                    }
                    // 修改活化边
                    left->dyl --;
                    right->dyl --;
                    // 边和下一条扫描线交点的x值
                    left->xl += left->dxl;
                    right->xl += right->dxl;
                    // 多边形所在的平面对应下一条扫描线在x=xl处的深度为 zl = zl + dzl * dxl + dzy
                    // 此处课件中dzl其实应为dzx?
                    left->zl += left->dzx* left->dxl +left->dzy;
                    right->zl += right->dzx* right->dxl +right->dzy;
                    flag = true;
                }
                //修改活化边表的dy值
                it->dy--;
                ++it;
            }
        }
        if (flag) {
            // 如果此扫描线的颜色值被修改过，则做处理
            for (int ii = 0; ii < max_width; ii++) {

                image[INIT_HEIGHT-y][ii][0]=color[ii].r;
                image[INIT_HEIGHT-y][ii][1]=color[ii].g;
                image[INIT_HEIGHT-y][ii][2]=color[ii].b;
            }
        }
    }

    // 释放内存
    point.clear();
    for (int i = 0; i < max_height; i++)
    {
        if (poly[i].size() > 0)
        {
            poly[i].clear();
        }
#if 0 // 前面已经在处理过程中将分类边表删除
        if (line[i].size() > 0)
		{
			printf("line %d: %d\n",i,line[i].size());
			line[i].clear();
			printf("line %d: %d\n",i,line[i].size());
		}
#endif
    }
    ape.clear();

    return 0;
}
void init(const char* file_name)
{
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    cal(file_name,INIT_WIDTH,INIT_HEIGHT,1.01);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(0, 0);
    glDrawPixels(INIT_WIDTH,INIT_HEIGHT, GL_RGB,
                 GL_UNSIGNED_BYTE, image);
    glFlush();
}
void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(INIT_WIDTH,INIT_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    const char* file_name="/Users/Cancel/Develop/c++/graphicAssignment/assets/test.obj";
    if (argc > 1)
    {
        file_name = argv[1];
    }
    init(file_name);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
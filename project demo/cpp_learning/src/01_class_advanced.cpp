/**
 * 01_class_advanced.cpp — 类的进阶用法
 *
 * 拷贝控制、static 成员、友元
 * 这些在 V4L2 采集和图像处理项目中会直接用到
 *
 * 编译：g++ 01_class_advanced.cpp -o 01_class_advanced -std=c++11 && ./01_class_advanced
 */

#include <iostream>
#include <cstring>
using namespace std;

// ============================================================
// 1. 拷贝构造函数 — 图像数据传递的核心
// ============================================================

// 项目场景：摄像头采集一帧图像，需要传递给 GUI 线程显示
// 如果不做深拷贝，两个对象共享同一块内存，一个释放后另一个就野指针了

class ImageBuffer {
private:
    unsigned char *m_data;
    int m_width;
    int m_height;

public:
    ImageBuffer(int width, int height)
        : m_width(width), m_height(height)
    {
        m_data = new unsigned char[width * height * 3];  // RGB
        cout << "[构造] 分配 " << width * height * 3 << " 字节" << endl;
    }

    // ⭐ 拷贝构造函数：深拷贝
    // 项目中：emit frameReady(image.copy()) 就是触发深拷贝
    ImageBuffer(const ImageBuffer &other)
        : m_width(other.m_width), m_height(other.m_height)
    {
        int size = m_width * m_height * 3;
        m_data = new unsigned char[size];       // 分配新内存
        memcpy(m_data, other.m_data, size);     // 拷贝数据
        cout << "[拷贝构造] 深拷贝 " << size << " 字节" << endl;
    }

    // ⭐ 赋值运算符：也需要深拷贝
    ImageBuffer& operator=(const ImageBuffer &other) {
        if (this == &other) return *this;  // 防止自赋值

        delete[] m_data;  // 释放旧内存

        m_width = other.m_width;
        m_height = other.m_height;
        int size = m_width * m_height * 3;
        m_data = new unsigned char[size];
        memcpy(m_data, other.m_data, size);
        cout << "[赋值] 深拷贝 " << size << " 字节" << endl;
        return *this;
    }

    // 析构函数
    ~ImageBuffer() {
        delete[] m_data;
        cout << "[析构] 释放内存" << endl;
    }

    // 模拟填充数据
    void fill(unsigned char value) {
        memset(m_data, value, m_width * m_height * 3);
    }

    unsigned char getPixel(int index) const { return m_data[index]; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
};


// ============================================================
// 2. static 成员 — 所有对象共享的数据
// ============================================================

// 项目场景：OneVideo::WIDTH 和 OneVideo::HEIGHT 是所有视频窗口共享的常量

class VideoWindow {
private:
    int m_id;

public:
    // ⭐ static 常量：属于类，不属于某个对象
    // 项目中：const int OneVideo::WIDTH = 480;
    static const int DEFAULT_WIDTH = 640;
    static const int DEFAULT_HEIGHT = 480;

    // ⭐ static 变量：所有对象共享，可以用来计数
    static int s_windowCount;

    VideoWindow() : m_id(++s_windowCount) {
        cout << "创建窗口 #" << m_id
             << " (总共 " << s_windowCount << " 个)" << endl;
    }

    ~VideoWindow() {
        cout << "销毁窗口 #" << m_id << endl;
        s_windowCount--;
    }

    // ⭐ static 函数：不需要对象就能调用
    static int getWindowCount() {
        return s_windowCount;
        // 注意：static 函数不能访问非 static 成员（因为没有 this 指针）
    }
};

// static 变量必须在类外定义（分配存储空间）
int VideoWindow::s_windowCount = 0;


// ============================================================
// 3. 友元 — 允许外部函数访问 private 成员
// ============================================================

// 项目场景：QImage 和 cv::Mat 之间的转换函数需要访问内部数据

class Matrix {
private:
    int m_rows, m_cols;
    int *m_data;

public:
    Matrix(int rows, int cols) : m_rows(rows), m_cols(cols) {
        m_data = new int[rows * cols]();
    }

    ~Matrix() { delete[] m_data; }

    void set(int row, int col, int value) {
        m_data[row * m_cols + col] = value;
    }

    // ⭐ 声明友元函数：允许 printMatrix 访问 private 成员
    friend void printMatrix(const Matrix &mat);
};

// 友元函数不是类的成员，但可以访问 private
void printMatrix(const Matrix &mat) {
    for (int i = 0; i < mat.m_rows; i++) {
        for (int j = 0; j < mat.m_cols; j++) {
            cout << mat.m_data[i * mat.m_cols + j] << " ";
        }
        cout << endl;
    }
}


// ============================================================
// 4. 运算符重载（简单了解）
// ============================================================

// 项目中不常自己写，但需要看懂 OpenCV 的 Mat 运算

class Vec2 {
public:
    float x, y;

    Vec2(float x = 0, float y = 0) : x(x), y(y) {}

    // 重载 + 运算符
    Vec2 operator+(const Vec2 &other) const {
        return Vec2(x + other.x, y + other.y);
    }

    // 重载 << 运算符（用于 cout 输出）
    friend ostream& operator<<(ostream &os, const Vec2 &v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
};


int main() {
    // --- 1. 拷贝控制 ---
    cout << "===== 拷贝控制 =====" << endl;
    {
        ImageBuffer img1(320, 240);
        img1.fill(128);

        ImageBuffer img2 = img1;  // 拷贝构造
        cout << "img2 pixel[0] = " << (int)img2.getPixel(0) << endl;

        ImageBuffer img3(640, 480);
        img3 = img1;  // 赋值运算符
    }

    // --- 2. static 成员 ---
    cout << "\n===== static 成员 =====" << endl;
    cout << "默认分辨率: " << VideoWindow::DEFAULT_WIDTH
         << "x" << VideoWindow::DEFAULT_HEIGHT << endl;
    cout << "当前窗口数: " << VideoWindow::getWindowCount() << endl;
    {
        VideoWindow w1, w2, w3;
        cout << "当前窗口数: " << VideoWindow::getWindowCount() << endl;
    }
    cout << "当前窗口数: " << VideoWindow::getWindowCount() << endl;

    // --- 3. 友元 ---
    cout << "\n===== 友元 =====" << endl;
    Matrix mat(2, 3);
    mat.set(0, 0, 1);
    mat.set(0, 1, 2);
    mat.set(1, 2, 5);
    printMatrix(mat);

    // --- 4. 运算符重载 ---
    cout << "\n===== 运算符重载 =====" << endl;
    Vec2 a(1.0, 2.0), b(3.0, 4.0);
    Vec2 c = a + b;
    cout << a << " + " << b << " = " << c << endl;

    return 0;
}

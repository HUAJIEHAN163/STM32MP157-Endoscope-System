/**
 * 05_template.cpp — 模板基础
 *
 * 模板让一份代码适用于多种类型
 * 你不需要自己写模板，但需要看懂：
 *   vector<int>          — int 类型的动态数组
 *   cv::Ptr<cv::CLAHE>   — OpenCV 的智能指针模板
 *   QVector<QImage>      — Qt 的图像列表
 *
 * 编译：g++ 05_template.cpp -o 05_template -std=c++11 && ./05_template
 */

#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ============================================================
// 1. 函数模板 — 一个函数处理多种类型
// ============================================================

// 没有模板：每种类型写一个函数
int maxInt(int a, int b) { return a > b ? a : b; }
double maxDouble(double a, double b) { return a > b ? a : b; }

// ⭐ 有模板：一个函数搞定所有类型
template <typename T>
T myMax(T a, T b) {
    return a > b ? a : b;
}

void demo_function_template() {
    cout << "===== 函数模板 =====" << endl;

    // 编译器自动推导类型
    cout << "max(3, 5) = " << myMax(3, 5) << endl;           // T = int
    cout << "max(3.14, 2.71) = " << myMax(3.14, 2.71) << endl;  // T = double

    // 也可以显式指定类型
    cout << "max<int>(3, 5) = " << myMax<int>(3, 5) << endl;
}


// ============================================================
// 2. 类模板 — 一个类处理多种类型
// ============================================================

// ⭐ 这就是 vector、map 等 STL 容器的原理
template <typename T>
class SimpleArray {
private:
    T *m_data;
    int m_size;

public:
    SimpleArray(int size) : m_size(size) {
        m_data = new T[size]();  // () 表示零初始化
    }

    ~SimpleArray() { delete[] m_data; }

    T& operator[](int index) { return m_data[index]; }
    const T& operator[](int index) const { return m_data[index]; }
    int size() const { return m_size; }
};

void demo_class_template() {
    cout << "\n===== 类模板 =====" << endl;

    SimpleArray<int> intArr(5);
    intArr[0] = 10;
    intArr[1] = 20;
    cout << "intArr[0] = " << intArr[0] << endl;

    SimpleArray<double> doubleArr(3);
    doubleArr[0] = 3.14;
    cout << "doubleArr[0] = " << doubleArr[0] << endl;

    SimpleArray<string> strArr(2);
    strArr[0] = "/dev/video0";
    strArr[1] = "/dev/video1";
    cout << "strArr[0] = " << strArr[0] << endl;
}


// ============================================================
// 3. 看懂项目中的模板用法
// ============================================================

void demo_read_templates() {
    cout << "\n===== 看懂模板用法 =====" << endl;

    // 你在项目中会遇到这些模板写法：

    // 1. STL 容器
    vector<int> nums = {1, 2, 3};           // int 的动态数组
    vector<string> names = {"cam0", "cam1"}; // string 的动态数组
    // vector<FrameBuffer> m_buffers;        // 自定义类型的动态数组

    // 2. 嵌套模板
    vector<vector<int>> matrix = {{1,2}, {3,4}};  // 二维数组
    cout << "matrix[1][0] = " << matrix[1][0] << endl;

    // 3. OpenCV 中的模板（看懂就行，不需要自己写）
    // cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(3.0);
    // cv::Ptr 是 OpenCV 的智能指针模板，类似 shared_ptr
    // cv::CLAHE 是模板参数（具体类型）

    // 4. Qt 中的模板
    // QVector<QImage> imageList;
    // QList<QPushButton*> buttons;
    // QMap<QString, int> settings;

    cout << "（模板主要是看懂别人的代码，不需要自己设计复杂模板）" << endl;
}


int main() {
    demo_function_template();
    demo_class_template();
    demo_read_templates();
    return 0;
}

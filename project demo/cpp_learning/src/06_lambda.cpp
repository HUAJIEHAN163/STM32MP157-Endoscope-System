/**
 * 06_lambda.cpp — Lambda 表达式
 *
 * Lambda 是 C++11 引入的匿名函数，Qt5 的信号槽连接中大量使用
 *
 * 项目中的体现：
 *   connect(camera, &V4l2Camera::errorOccurred, [this](const QString &msg) {
 *       m_lblStatus->setText("错误: " + msg);
 *   });
 *
 * 编译：g++ 06_lambda.cpp -o 06_lambda -std=c++11 && ./06_lambda
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
using namespace std;

// ============================================================
// 1. Lambda 基本语法
// Lambda = 就地定义的匿名函数，用来写一次性、简短的回调函数，代码更紧凑。
// 你可以把它理解成：不用起名、直接写在要用的地方的小函数。
// [捕获] (参数) -> 返回值 { 函数体 };
//[]        // 不捕获
//[a]       // 捕获a（值）
//[&a]      // 捕获a（引用）
//[=]       // 全部值
//[&]       // 全部引用
//[this]    // 类里用
// ① [ ] 捕获列表（最核心）
// 用来把外部变量抓进 Lambda 里面用。
// int a = 10;

// 值捕获：拷贝一份 a
// [a]() { cout << a; };

// 引用捕获：用原来的 a，可修改
// [&a]() { a = 20; };

// 捕获全部外部变量（值）
// [=]() { ... };

// 捕获全部外部变量（引用）
// [&]() { ... };
// ② ( ) 参数列表
// 和普通函数一样，写形参。
// [](int x, int y) {
//     return x + y;
// };
// ③ -> int 返回值类型
// 可写可不写，编译器能自动推导。
// // 自动推导返回 int
// [](int x) { return x * 2; };
// ④ { } 函数体
// 正常写代码就行。
// ============================================================

void demo_basics() {
    cout << "===== Lambda 基本语法 =====" << endl;

    // Lambda 语法：
    // [捕获列表](参数列表) -> 返回类型 { 函数体 }
    // [capture](params) -> return_type { body }

    // 最简单的 lambda
    auto hello = []() {
        cout << "Hello Lambda!" << endl;
    };
    hello();  // 调用

    // 带参数
    auto add = [](int a, int b) {
        return a + b;
    };
    cout << "3 + 5 = " << add(3, 5) << endl;

    // 带返回类型（通常可以省略，编译器自动推导）
    auto divide = [](double a, double b) -> double {
        return a / b;
    };
    cout << "10 / 3 = " << divide(10, 3) << endl;
}


// ============================================================
// 2. 捕获列表 — Lambda 的核心
// ============================================================

void demo_capture() {
    cout << "\n===== 捕获列表 =====" << endl;

    int x = 10;
    string name = "camera";

    // [=] 按值捕获所有外部变量（拷贝一份，lambda 内修改不影响外部）
    auto f1 = [=]() {
        cout << "按值捕获: x=" << x << " name=" << name << endl;
        // x = 20;  // 编译错误！按值捕获默认是 const
    };
    f1();

    // [&] 按引用捕获所有外部变量（lambda 内修改会影响外部）
    auto f2 = [&]() {
        x = 20;  // 修改外部的 x
        cout << "按引用捕获: x=" << x << endl;
    };
    f2();
    cout << "外部 x = " << x << endl;  // 20（被 lambda 修改了）

    // [x, &name] 混合捕获：x 按值，name 按引用
    auto f3 = [x, &name]() {
        cout << "混合捕获: x=" << x << " name=" << name << endl;
        name = "sensor";  // 可以修改 name（引用捕获）
    };
    f3();
    cout << "外部 name = " << name << endl;  // "sensor"

    // ⭐ [this] 捕获当前对象的 this 指针
    // 项目中最常用！在类的成员函数中写 lambda 时：
    // connect(btn, &QPushButton::clicked, [this]() {
    //     m_lblStatus->setText("已点击");  // 通过 this 访问成员
    // });
}


// ============================================================
// 3. Lambda 在 STL 算法中的用法
// ============================================================

void demo_stl_usage() {
    cout << "\n===== Lambda + STL =====" << endl;

    vector<int> nums = {5, 2, 8, 1, 9, 3};

    // sort 自定义排序
    sort(nums.begin(), nums.end(), [](int a, int b) {
        return a < b;  // 升序
    });
    cout << "升序: ";
    for (int n : nums) cout << n << " ";
    cout << endl;

    // find_if 查找满足条件的元素
    auto it = find_if(nums.begin(), nums.end(), [](int n) {
        return n > 5;
    });
    if (it != nums.end()) {
        cout << "第一个大于5的: " << *it << endl;
    }

    // for_each 对每个元素执行操作
    cout << "翻倍: ";
    for_each(nums.begin(), nums.end(), [](int &n) {
        n *= 2;
    });
    for (int n : nums) cout << n << " ";
    cout << endl;

    // count_if 统计满足条件的个数
    int count = count_if(nums.begin(), nums.end(), [](int n) {
        return n > 10;
    });
    cout << "大于10的个数: " << count << endl;
}


// ============================================================
// 4. 模拟 Qt 信号槽中的 Lambda 用法
// ============================================================

// 模拟 Qt 的 connect 机制
class Button {
public:
    // 存储点击时要执行的回调函数
    function<void()> onClick;

    void click() {
        cout << "[按钮被点击]" << endl;
        if (onClick) onClick();
    }
};

class Window {
private:
    string m_status;
    int m_photoCount;

public:
    Window() : m_status("就绪"), m_photoCount(0) {}

    void setup() {
        Button captureBtn;
        Button recordBtn;

        // ⭐ 用 lambda 连接按钮事件（模拟 Qt 的 connect）
        // 项目中：
        // connect(m_btnCapture, &QPushButton::clicked, [this]() {
        //     onCapturePhoto();
        // });

        captureBtn.onClick = [this]() {
            m_photoCount++;
            m_status = "拍照 #" + to_string(m_photoCount);
            cout << "状态: " << m_status << endl;
        };

        recordBtn.onClick = [this]() {
            m_status = "录像中...";
            cout << "状态: " << m_status << endl;
        };

        // 模拟用户操作
        captureBtn.click();
        captureBtn.click();
        recordBtn.click();
        captureBtn.click();
    }
};


// ============================================================
// 5. Lambda 作为回调函数
// ============================================================

// 项目场景：V4l2Camera 采集到帧后通知主窗口

using FrameCallback = function<void(int frameId, int size)>;
//这是 C++ 定义「回调函数类型别名」的现代写法！
//等价于C的typedef void (*FrameCallback)(int frameId, int size);
/*
① using = C++ 版 typedef
C：typedef 原来的类型 新名字;
C++：using 新名字 = 原来的类型;
② function<void(...)>
这是 C++ 的万能可调用对象包装器它能装：
普通函数
函数指针
带捕获的 Lambda（最重要！）
类成员函数
你可以把它理解成：
超级加强版的函数指针

装带捕获的 Lambda：
// C 函数指针 → 不行！
typedef void (*CB)(int,int);
CB cb = [x](int a,int b) { ... }; // 报错

// C++ function → 完美！
using CB = function<void(int,int)>;
CB cb = [x](int a,int b) { ... }; // 正常

③ void(int frameId, int size)
这是函数签名：
返回值：void
参数：int frameId, int size
*/
class CameraSimple {
private:
    FrameCallback m_callback;

public:
    // 注册回调
    void onFrameReady(FrameCallback callback) {
        m_callback = callback;
    }

    // 模拟采集
    void capture(int count) {
        for (int i = 0; i < count; i++) {
            int fakeSize = 640 * 480 * 2;
            if (m_callback) {
                m_callback(i, fakeSize);
            }
        }
    }
};

void demo_callback() {
    cout << "\n===== Lambda 作为回调 =====" << endl;

    CameraSimple cam;
    int totalFrames = 0;

    // 用 lambda 注册回调
    cam.onFrameReady([&totalFrames](int frameId, int size) {
        totalFrames++;
        cout << "收到帧 #" << frameId << " 大小=" << size << endl;
    });

    cam.capture(3);
    cout << "总共收到 " << totalFrames << " 帧" << endl;
}


int main() {
    demo_basics();
    demo_capture();
    demo_stl_usage();

    cout << "\n===== 模拟 Qt 信号槽 =====" << endl;
    Window win;
    win.setup();

    demo_callback();

    return 0;
}

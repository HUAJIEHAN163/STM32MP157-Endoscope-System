/**
 * 08_opencv_basics.cpp — OpenCV 基础
 *
 * 本文件可以在宿主机上编译运行（用 x86 版 OpenCV）
 * 重点：cv::Mat、颜色空间转换、基本图像处理
 *
 * 编译：g++ 08_opencv_basics.cpp -o 08_opencv $(pkg-config --cflags --libs opencv) -std=c++11 && ./08_opencv
 */

#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

// ============================================================
// 1. cv::Mat — OpenCV 的图像容器
// ============================================================

void demo_mat() {
    cout << "===== cv::Mat 基础 =====" << endl;

    // 创建图像
    // Mat(行数, 列数, 类型)
    // CV_8UC3 = 8位无符号，3通道（BGR）
    Mat img1(480, 640, CV_8UC3, Scalar(0, 0, 0));  // 640x480 黑色图像
    cout << "img1: " << img1.cols << "x" << img1.rows
         << " 通道数=" << img1.channels()
         << " 类型=" << img1.type() << endl;

    // ⭐ 常用类型
    // CV_8UC1  — 8位单通道（灰度图）
    // CV_8UC3  — 8位三通道（彩色图，BGR 顺序）
    // CV_8UC4  — 8位四通道（带透明通道）
    // CV_32FC1 — 32位浮点单通道（算法中间结果）

    Mat gray(480, 640, CV_8UC1, Scalar(128));  // 灰度图
    cout << "gray: " << gray.cols << "x" << gray.rows
         << " 通道数=" << gray.channels() << endl;

    // ⭐ Mat 的引用计数（类似 shared_ptr）
    Mat img2 = img1;  // 浅拷贝！img2 和 img1 共享数据
    img2.at<Vec3b>(0, 0) = Vec3b(255, 0, 0);  // 修改 img2
    // img1 也被修改了！因为共享同一块内存

    Mat img3 = img1.clone();  // 深拷贝，独立的数据
    img3.at<Vec3b>(0, 0) = Vec3b(0, 255, 0);  // 只修改 img3

    cout << "img1[0,0] = " << img1.at<Vec3b>(0, 0) << " (被 img2 修改了)" << endl;
    cout << "img3[0,0] = " << img3.at<Vec3b>(0, 0) << " (独立的)" << endl;

    // ⭐ 从原始数据创建 Mat（项目中最常用）
    // 摄像头采集到 YUYV 数据后：
    // unsigned char *yuyvData = ...; // 来自 V4L2
    // Mat yuyv(480, 640, CV_8UC2, yuyvData);  // 不拷贝，直接包装

    // ⭐ 常用属性
    cout << "行=" << img1.rows << " 列=" << img1.cols << endl;
    cout << "大小=" << img1.size() << endl;
    cout << "是否为空=" << img1.empty() << endl;
    cout << "总字节数=" << img1.total() * img1.elemSize() << endl;
    cout << "数据指针=" << (void*)img1.data << endl;
}


// ============================================================
// 2. 颜色空间转换
// ============================================================

void demo_color_convert() {
    cout << "\n===== 颜色空间转换 =====" << endl;

    // 创建一个彩色测试图像
    Mat bgr(100, 100, CV_8UC3, Scalar(100, 150, 200));  // BGR

    // ⭐ BGR → 灰度（最常用）
    Mat gray;
    cvtColor(bgr, gray, COLOR_BGR2GRAY);
    cout << "BGR→灰度: " << gray.channels() << " 通道" << endl;

    // ⭐ BGR → RGB（Qt 用 RGB，OpenCV 用 BGR）
    Mat rgb;
    cvtColor(bgr, rgb, COLOR_BGR2RGB);

    // ⭐ YUYV → BGR（项目中摄像头数据转换）
    // Mat yuyv(480, 640, CV_8UC2, yuyvData);
    // Mat bgr;
    // cvtColor(yuyv, bgr, COLOR_YUV2BGR_YUYV);

    // ⭐ BGR → Lab（CLAHE 增强用）
    Mat lab;
    cvtColor(bgr, lab, COLOR_BGR2Lab);
    cout << "BGR→Lab: " << lab.channels() << " 通道" << endl;

    // 颜色空间总结：
    // BGR  — OpenCV 默认格式（注意不是 RGB！）
    // RGB  — Qt 的 QImage 格式
    // GRAY — 灰度图
    // Lab  — L(亮度) a(绿红) b(蓝黄)，CLAHE 在 L 通道上做
    // YUV  — 摄像头原始格式
}


// ============================================================
// 3. 项目中用到的图像处理
// ============================================================

void demo_image_processing() {
    cout << "\n===== 图像处理 =====" << endl;

    // 创建测试图像（模拟摄像头采集的一帧）
    Mat src(480, 640, CV_8UC3);
    randu(src, Scalar(0, 0, 0), Scalar(255, 255, 255));  // 随机填充

    // ⭐ 1. CLAHE 自适应直方图均衡化（内窥镜最常用）
    {
        Mat lab, dst;
        cvtColor(src, lab, COLOR_BGR2Lab);

        vector<Mat> channels;
        split(lab, channels);  // 分离 L, a, b 三个通道

        // 只对 L 通道（亮度）做 CLAHE
        Ptr<CLAHE> clahe = createCLAHE(3.0, Size(8, 8));
        //                              ↑ clipLimit  ↑ tileSize
        clahe->apply(channels[0], channels[0]);

        merge(channels, lab);  // 合并回去
        cvtColor(lab, dst, COLOR_Lab2BGR);
        cout << "CLAHE 完成: " << dst.size() << endl;
    }

    // ⭐ 2. 高斯模糊（降噪基础）
    {
        Mat blurred;
        GaussianBlur(src, blurred, Size(5, 5), 0);
        //                         ↑ 核大小（必须是奇数）
        cout << "高斯模糊完成" << endl;
    }

    // ⭐ 3. USM 锐化
    {
        Mat blurred, sharpened;
        GaussianBlur(src, blurred, Size(0, 0), 3.0);
        addWeighted(src, 1.5, blurred, -0.5, 0, sharpened);
        //          原图 权重  模糊图  权重  偏移  输出
        cout << "USM 锐化完成" << endl;
    }

    // ⭐ 4. 双边滤波（保边降噪，内窥镜常用）
    {
        Mat denoised;
        bilateralFilter(src, denoised, 5, 50, 50);
        //                             d  sigmaColor sigmaSpace
        cout << "双边滤波完成" << endl;
    }

    // ⭐ 5. Canny 边缘检测
    {
        Mat gray, edges;
        cvtColor(src, gray, COLOR_BGR2GRAY);
        Canny(gray, edges, 50, 150);
        //                  低阈值 高阈值
        cout << "Canny 边缘检测完成: " << edges.size() << endl;
    }

    // ⭐ 6. 阈值分割
    {
        Mat gray, binary;
        cvtColor(src, gray, COLOR_BGR2GRAY);
        threshold(gray, binary, 128, 255, THRESH_BINARY);
        //                       阈值  最大值  方式
        cout << "阈值分割完成" << endl;
    }
}


// ============================================================
// 4. 图像 I/O
// ============================================================

void demo_io() {
    cout << "\n===== 图像 I/O =====" << endl;

    // 创建测试图像
    Mat img(200, 300, CV_8UC3, Scalar(50, 100, 200));

    // 画一些内容
    putText(img, "Hello OpenCV", Point(30, 100),
            FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 255, 255), 2);
    rectangle(img, Point(10, 10), Point(290, 190), Scalar(0, 255, 0), 2);

    // ⭐ 保存图像
    imwrite("/tmp/test_opencv.jpg", img);
    cout << "保存到 /tmp/test_opencv.jpg" << endl;

    // ⭐ 读取图像
    // Mat loaded = imread("/tmp/test_opencv.jpg");
    // if (loaded.empty()) { cout << "读取失败" << endl; }

    // ⭐ 项目中的用法：拍照功能
    // m_displayImage.save("./captures/photo.jpg", "JPEG", 95);  // Qt 方式
    // imwrite("./captures/photo.jpg", mat);                      // OpenCV 方式
}


// ============================================================
// 5. 视频录制（VideoWriter）
// ============================================================

void demo_video_writer() {
    cout << "\n===== VideoWriter =====" << endl;

    // ⭐ 项目中的录像功能
    // VideoWriter writer;
    // writer.open("output.avi",
    //             VideoWriter::fourcc('M','J','P','G'),  // 编码格式
    //             25,                                     // 帧率
    //             Size(640, 480));                         // 分辨率
    //
    // // 每帧写入
    // writer.write(frame);
    //
    // // 结束录制
    // writer.release();

    cout << "（VideoWriter 需要实际的视频数据，这里只展示 API）" << endl;

    // fourcc 常用编码：
    // 'M','J','P','G' — MJPEG（最兼容）
    // 'X','V','I','D' — XVID
    // 'H','2','6','4' — H.264（需要额外编解码库）
}


// ============================================================
// 6. cv::Mat 与指针操作（高级）
// ============================================================

void demo_mat_pointer() {
    cout << "\n===== Mat 指针操作 =====" << endl;

    Mat img(3, 4, CV_8UC3, Scalar(0, 0, 0));

    // ⭐ 方式一：at<> 访问（安全但慢）
    img.at<Vec3b>(0, 0) = Vec3b(255, 0, 0);    // 第0行第0列 = 蓝色
    img.at<Vec3b>(1, 2) = Vec3b(0, 255, 0);    // 第1行第2列 = 绿色

    // ⭐ 方式二：ptr<> 行指针（快，项目中常用）
    uchar *row = img.ptr<uchar>(2);  // 第2行的指针
    row[0] = 0; row[1] = 0; row[2] = 255;  // 第2行第0列 = 红色 (B=0,G=0,R=255)

    // ⭐ 方式三：data 指针（最快，C 风格）
    uchar *data = img.data;
    int step = img.step;  // 每行的字节数
    // 访问第 r 行第 c 列的第 ch 通道：
    // data[r * step + c * 3 + ch]

    // 打印
    for (int r = 0; r < img.rows; r++) {
        for (int c = 0; c < img.cols; c++) {
            Vec3b pixel = img.at<Vec3b>(r, c);
            if (pixel != Vec3b(0,0,0)) {
                cout << "  [" << r << "," << c << "] = "
                     << "B=" << (int)pixel[0]
                     << " G=" << (int)pixel[1]
                     << " R=" << (int)pixel[2] << endl;
            }
        }
    }
}


int main() {
    demo_mat();
    demo_color_convert();
    demo_image_processing();
    demo_io();
    demo_video_writer();
    demo_mat_pointer();

    cout << "\n===== 总结 =====" << endl;
    cout << "项目中 OpenCV 的使用流程：" << endl;
    cout << "  1. V4L2 采集 YUYV 数据" << endl;
    cout << "  2. cvtColor(YUYV → BGR) 或手动转 RGB" << endl;
    cout << "  3. 图像处理（CLAHE/锐化/降噪/去雾）" << endl;
    cout << "  4. cvtColor(BGR → RGB) 转给 QImage 显示" << endl;
    cout << "  5. 拍照：imwrite 保存" << endl;
    cout << "  6. 录像：VideoWriter 写入" << endl;

    return 0;
}

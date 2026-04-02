/**
 * 07_qt_basics.cpp — Qt 核心机制（伪代码讲解）
 *
 * 本文件不能直接编译运行（需要 Qt 环境），
 * 但通过注释和伪代码帮你理解 Qt 的核心概念。
 * 等你开始写项目代码时，这些概念就能直接用上。
 *
 * 重点：信号槽、QThread、QImage、事件处理
 */

#include <iostream>
using namespace std;

// ============================================================
// 1. 信号槽机制 — Qt 的核心
// ============================================================

/*
信号槽是 Qt 的对象间通信机制，替代了 C 的回调函数。

核心概念：
  - 信号（signal）：对象发出的通知，"我这里发生了某件事"
  - 槽（slot）：接收信号的函数，"收到通知后我要做什么"
  - connect：把信号和槽连接起来

类比：
  信号 = 按门铃
  槽   = 听到门铃后去开门
  connect = 把门铃和开门动作关联起来

项目中的信号槽：

  // V4l2Camera 采集到一帧 → 发出 frameReady 信号
  // MainWindow 收到信号 → 更新显示

  // 声明信号（在 V4l2Camera 类中）
  signals:
      void frameReady(const QImage &image);   // 采集到新帧
      void errorOccurred(const QString &msg);  // 出错了

  // 声明槽（在 MainWindow 类中）
  private slots:
      void onFrameReady(const QImage &image);  // 处理新帧

  // 连接（在 MainWindow 构造函数中）
  connect(m_camera, &V4l2Camera::frameReady,
          this, &MainWindow::onFrameReady);

  // 也可以用 lambda 连接（更简洁）
  connect(m_camera, &V4l2Camera::errorOccurred, [this](const QString &msg) {
      m_lblStatus->setText("错误: " + msg);
  });
*/


// ============================================================
// 2. QThread — 多线程
// ============================================================

/*
为什么需要多线程：
  摄像头采集是阻塞操作（等待帧数据），如果在 GUI 线程中做，界面会卡死。
  解决方案：采集放在独立线程，通过信号槽把帧数据传给 GUI 线程。

QThread 的用法（我们项目中的方式）：

  class V4l2Camera : public QThread {
      Q_OBJECT  // Qt 宏，启用信号槽

  signals:
      void frameReady(const QImage &image);

  protected:
      // ⭐ 覆盖 run()，这个函数在新线程中执行
      void run() override {
          while (m_running) {
              // 1. 等待并获取一帧（在采集线程中）
              grabFrame(&data, &size);

              // 2. 颜色转换
              yuyvToRgb888(data, rgbBuf, width, height);

              // 3. 创建 QImage
              QImage image(rgbBuf, width, height, QImage::Format_RGB888);

              // 4. 发信号给主线程（跨线程安全传递）
              emit frameReady(image.copy());
              //                     ↑ copy() 很重要！
              // 因为 rgbBuf 在下一帧会被覆盖，必须深拷贝
          }
      }
  };

  // 使用：
  V4l2Camera *cam = new V4l2Camera("/dev/video0", 640, 480);
  cam->start();   // 启动线程，自动调用 run()
  cam->stop();    // 设置 m_running = false
  cam->wait();    // 等待线程结束
*/


// ============================================================
// 3. QImage — 图像容器
// ============================================================

/*
QImage 是 Qt 的图像类，用于存储和操作像素数据。

常用格式：
  QImage::Format_RGB888    — 每像素 3 字节 (R,G,B)，我们主要用这个
  QImage::Format_Grayscale8 — 灰度图，每像素 1 字节
  QImage::Format_ARGB32    — 带透明通道，每像素 4 字节

创建 QImage：

  // 从原始数据创建（不拥有数据，data 释放后 image 无效）
  QImage image(data, width, height, bytesPerLine, QImage::Format_RGB888);

  // 深拷贝（拥有独立的数据副本）
  QImage safeCopy = image.copy();

  // 从文件加载
  QImage image("photo.jpg");

  // 保存到文件
  image.save("output.jpg", "JPEG", 95);  // 95 是质量

常用操作：

  image.width();           // 宽度
  image.height();          // 高度
  image.size();            // QSize(width, height)
  image.isNull();          // 是否为空
  image.scaled(800, 480);  // 缩放
  image.mirrored(true, false);  // 水平翻转

QImage 与 cv::Mat 互转（项目中的 Util 类）：

  // QImage → cv::Mat
  cv::Mat mat = Util::QImage2cvMat(image);

  // cv::Mat → QImage
  QImage image = Util::cvMat2QImage(mat);
*/


// ============================================================
// 4. 事件处理 — 覆盖虚函数
// ============================================================

/*
Qt 的事件处理通过覆盖 QWidget 的虚函数实现：

class MainWindow : public QMainWindow {
protected:
    // ⭐ 绘制事件：每次 update() 调用后触发
    void paintEvent(QPaintEvent *event) override {
        QMainWindow::paintEvent(event);  // 先调用父类
        QPainter painter(this);
        painter.drawImage(x, y, m_displayImage);  // 绘制摄像头画面
    }

    // ⭐ 键盘事件：按键时触发
    void keyPressEvent(QKeyEvent *event) override {
        switch (event->key()) {
        case Qt::Key_Escape: close(); break;       // ESC 退出
        case Qt::Key_Space: onToggleFreeze(); break; // 空格冻结
        case Qt::Key_C: onCapturePhoto(); break;    // C 拍照
        }
    }

    // ⭐ 窗口大小改变事件
    void resizeEvent(QResizeEvent *event) override {
        QMainWindow::resizeEvent(event);
        layoutChild();  // 重新布局子控件
    }
};
*/


// ============================================================
// 5. Qt 常用控件速查
// ============================================================

/*
项目中用到的控件：

  QLabel        — 显示文字/图片
                  m_lblFps = new QLabel("FPS: --");
                  m_lblFps->setText("FPS: 30.0");

  QPushButton   — 按钮
                  m_btnCapture = new QPushButton("📷 拍照");
                  connect(m_btnCapture, &QPushButton::clicked, this, &MainWindow::onCapture);

  QCheckBox     — 复选框（开关图像处理算法）
                  m_chkClahe = new QCheckBox("CLAHE增强");
                  if (m_chkClahe->isChecked()) { ... }

  QSlider       — 滑块（调节亮度/对比度）
                  m_sliderContrast = new QSlider(Qt::Horizontal);
                  m_sliderContrast->setRange(0, 255);
                  connect(m_sliderContrast, &QSlider::valueChanged, this, &MainWindow::onContrastChanged);

  QGroupBox     — 分组框
                  QGroupBox *group = new QGroupBox("图像处理");

布局管理器：

  QHBoxLayout   — 水平排列
  QVBoxLayout   — 垂直排列
  QGridLayout   — 网格排列

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(m_chkClahe);
  layout->addWidget(m_chkSharpen);
  layout->addWidget(m_chkDenoise);
  group->setLayout(layout);
*/


// ============================================================
// 6. Q_OBJECT 宏和 MOC
// ============================================================

/*
每个使用信号槽的类都必须：
1. 继承 QObject（或其子类如 QWidget、QThread）
2. 在类的开头写 Q_OBJECT 宏

  class V4l2Camera : public QThread {
      Q_OBJECT    // ⭐ 必须有这个！
  public:
      ...
  signals:
      void frameReady(const QImage &image);
  };

Q_OBJECT 宏的作用：
  - 启用信号槽机制
  - 启用 Qt 的元对象系统（运行时类型信息）
  - 编译时 MOC（Meta-Object Compiler）会处理这个宏，生成额外的 C++ 代码

如果忘记写 Q_OBJECT，connect() 会编译通过但运行时信号槽不工作。
这是 Qt 新手最常见的坑之一。
*/


int main() {
    cout << "本文件是 Qt 概念讲解，不能直接运行。" << endl;
    cout << "请阅读源码中的注释来学习 Qt 核心机制。" << endl;
    cout << endl;
    cout << "学习顺序：" << endl;
    cout << "  1. 信号槽 — 对象间通信" << endl;
    cout << "  2. QThread — 多线程采集" << endl;
    cout << "  3. QImage — 图像数据容器" << endl;
    cout << "  4. 事件处理 — paintEvent/keyPressEvent" << endl;
    cout << "  5. 常用控件 — QLabel/QPushButton/QCheckBox/QSlider" << endl;
    cout << "  6. Q_OBJECT — 信号槽的前提条件" << endl;
    return 0;
}

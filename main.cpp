
// 本程序使用 640 * 480 作为输入图像尺寸

#include <iostream>
#include <thread>
#include <opencv2/core/core.hpp>
#include <serial.h>                    // 串口模块
#include <camera/video_wrapper.h>      // 从文件读取视频的包装器
#include <camera/wrapper_head.h>
#include <energy/energy.h>             // 能量机关部分
#include <armor_finder/armor_finder.h> // 自瞄装甲板部分
#include "armor_finder/kalman.h"                     // 卡尔曼基类
#include "armor_finder/predictor_kalman.h"           // 卡尔曼滤波与预测
#include <options.h>                   // 调试指令交互
#include <additions.h>                 // 拓展模块
#include "camera/GxCamera/GxCamera.h"

#define DO_NOT_CNT_TIME                // 模块记时器(调试用)

#include <log.h>                       // 日志模块
#include<X11/Xlib.h>
#include <unistd.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include "main.h"


using namespace cv;
using namespace std;

McuData mcu_data;


uint8_t last_state = ARMOR_STATE;// 上次状态，用于初始化
WrapperHead *video = nullptr;    // 云台摄像头视频源
Serial serial;    // 串口对象

//HikCamera MVS_cap;
PredictorKalman predictor;       // 初始化卡尔曼
//cv::Mat src ;
cv::Mat ori_src;

// 自瞄主程序对象
ArmorFinder armor_finder(mcu_data.enemy_color, serial, PROJECT_DIR"/tools/para/", false);
// 能量机关主程序对象
Energy energy(serial, mcu_data.enemy_color);
// image
pthread_t thread1;
double bind_yaw = 0; //muc.curr_yaw TODO
double bind_pitch = 0;
extern double bind_yaw;
extern double bind_pitch;

int main(int argc, char *argv[]) {
    processOptions(argc, argv);             // 处理命令行参数
    thread receive(uartReceive, &serial);   // 开启串口接收线程

    // 如果不能从裁判系统读取颜色则手动设置目标颜色
    if (!recv_close) // 默认为【红色】装甲板为目标，更改目标装甲板颜色指令请查阅 options.cpp 功能列表

        mcu_data.enemy_color = ENEMY_RED;
    else
        mcu_data.enemy_color = ENEMY_BLUE;


    // 根据条件输入选择视频源 (1、相机  0、视频文件)
    int from_camera = 1 ; // 默认视频源
    if (!run_with_camera) {
//        cout << "输入 1 使用相机, 输入 0 运行视频" << endl;
//        cin >> from_camera;
        from_camera = 0;
    }


    // 打开视频源
    if (from_camera) {
        XInitThreads();
        pthread_mutex_init(&Globalmutex, NULL);
        pthread_cond_init(&GlobalCondCV, NULL);
        pthread_create(&thread1, NULL, imageUpdatingThread, NULL);
        if (src.empty()) {
            cout << "empty\n" << endl;
        }
        //cv::imshow("src",src);
    } else {
        video = new VideoWrapper(PROJECT_DIR"/videoTest/armor_red.mp4"); // 视频文件路径
        if (video->init()) {
            LOGM("video_source initialization successfully.");
        } else {
            LOGW("video_source unavailable!");
        }
    }
    cout << "\x1B[2J\x1B[H"; // 程序开始前清空终端区显示
    while (true) {

//           从相机捕获一帧图像
        if (from_camera) {
            //cout<<"Debug: "<<__FUNCTION__ <<__LINE__<<endl;
            pthread_mutex_lock(&Globalmutex);

            if (imageReadable == true) {
                src.copyTo(ori_src);
                imageReadable = false;
                //pthread_cond_wait(&GlobalCondCV, &Globalmutex);
            }
            pthread_mutex_unlock(&Globalmutex);

            //MVS_cap.ReadImg(ori_src);

            if (ori_src.empty())          // 海康相机初始化时开启线程需要一定时间,防止空图
                continue;
        } else {
           // cout<<"Debug: "<<__FUNCTION__ <<__LINE__<<endl;
            video->read(ori_src);
        }
        static float nowTime_ms = 0.0f, deltaTime_ms = 0.0f, lastTime_ms = 0.0f, sleep_time = 0.0f, vision_max_hz = 100.0f;
        nowTime_ms = cv::getTickCount() / cv::getTickFrequency() * 1000;    //ms
        deltaTime_ms = (float) (nowTime_ms - lastTime_ms);                    //ms
        lastTime_ms = nowTime_ms;
//        DebugT(30, 1, "帧率:" << std::left << setw(5) << 1000.0 / deltaTime_ms);
//        DebugT(40, 1, " 程序耗时:" << std::left << setw(11) << deltaTime_ms << "ms");
        if (deltaTime_ms < (1000.0 / vision_max_hz)) {
            sleep_time = ((1000.0 / vision_max_hz) - deltaTime_ms) * 1000.0;
//            usleep(sleep_time); // ms
        }
//        flip(ori_src, ori_src, -1); // 图像翻转（视实际相机安装情况）# TODO:记得改回来！
//        char curr_state = mcu_data.state; // # TODO:记得改回来！
        char curr_state = ARMOR_STATE;
        CNT_TIME("Total", {
            if (curr_state != ARMOR_STATE) {  // 能量机关模式
                DebugT(1, 1,"目标颜色:" << (int)mcu_data.enemy_color << " 图像源:"<< from_camera<< " 当前模式: 能量机关" ); // 右边
                //DebugT(1, 2,"云台姿态 Yaw:" << bind_yaw*180.0/M_PI << "  "<<"Pitch:"<<bind_pitch*180.0/M_PI); // 右边

                if (last_state == ARMOR_STATE) {//若上一帧不是大能量机关模式，即刚往完成切换，则需要初始化

                    destroyAllWindows();


                    if (curr_state == BIG_ENERGY_STATE) {            // 大能量机关模式
                        energy.is_small = false;
                        energy.is_big = true;
                        LOGM(STR_CTR(WORD_BLUE, "开始大能量机关模式!"));
                    } else if (curr_state == SMALL_ENERGY_STATE) {   // 小能量机关模式
                        energy.is_small = true;
                        energy.is_big = false;
                        LOGM(STR_CTR(WORD_GREEN, "开始小能量机关模式"));
                    }
                    energy.setEnergyInit();
                }
                if (!from_camera) extract(ori_src);  // 画幅 resize
                if (save_video) saveVideos(ori_src); // 保存视频
                if (show_origin) showOrigin(ori_src);// 显示原始图像
                energy.run(ori_src);
            } else {

                // 自瞄模式
                if (last_state != ARMOR_STATE) {
                    LOGM(STR_CTR(WORD_RED, "开始自瞄模式"));
                    destroyAllWindows();
                };
                CNT_TIME("something whatever", {
                        if (!from_camera) extract(ori_src);  // 画幅 resize
                        if (save_video) saveVideos(ori_src); // 保存视频
                        if (show_origin) showOrigin(ori_src);// 显示原始图像
                });
                CNT_TIME(STR_CTR(WORD_CYAN, "Armor Time"), {

                        armor_finder.run(ori_src);

                        //std::cout<< "speed time ："<<end - start<<std::endl;
                });
            }

            last_state = curr_state; // 更新上一帧状态

            cv::waitKey(1);

        });
    }
    return 0;
}


#ifndef _SERIAL_H_
#define _SERIAL_H_
#define RECEIVE_SIZE 25
#ifdef Windows

#include <Windows.h>

class Serial
{
public:
    Serial(UINT  baud = CBR_115200, char  parity = 'N', UINT  databits = 8, UINT  stopsbits = 1, DWORD dwCommEvents = EV_RXCHAR);
    ~Serial();

    bool InitPort(UINT  portNo = 1, UINT  baud = CBR_9600, char  parity = 'N', UINT  databits = 8, UINT  stopsbits = 1, DWORD dwCommEvents = EV_RXCHAR);
    UINT GetBytesInCOM() const ;
    bool WriteData(const unsigned char* pData, unsigned int length);
    bool ReadData(unsigned char* buffer, unsigned int length);
private:
    bool openPort(UINT  portNo);
    void ClosePort();
    void ErrorHandler();
private:
    HANDLE hComm;
    UINT portNo;
    UINT baud;
    char parity;
    UINT databits;
    UINT stopsbits;
    DWORD dwCommEvents;
};

#elif defined(Linux) || defined(Darwin)

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstdint>
#include <opencv2/core/hal/interface.h>

//32位共用体
typedef union {
    uchar u8_temp[4];
    float float_temp;
} FormatTrans32Struct;

//16位共用体
typedef union {
    uchar u8_temp[2];
    int16_t s16_temp;
    uint16_t u16_temp;
} FormatTrans16Struct;
// 单片机端回传数据结构体
struct McuData {
    FormatTrans32Struct curr_yaw;      // 当前云台yaw角度
    FormatTrans32Struct curr_pitch;    // 当前云台pitch角
    uint8_t state;       // 当前状态，自瞄-大符-小符
    uint8_t mark;        // speed
    uint8_t anti_top;    // 是否为反陀螺模式
    uint8_t enemy_color; // 敌方颜色
    int delta_x;         // 能量机关x轴补偿量
    int delta_y;         //

};

class Serial {
private:
    ssize_t read_message_;
    int fd;
    McuData recv_data;
    int nSpeed;
    char nEvent;
    int nBits;
    int nStop;
    unsigned char receive_buff_temp_[RECEIVE_SIZE * 2];
    unsigned char receive_buff_[RECEIVE_SIZE];

    int set_opt(int fd, int nSpeed, char nEvent, int nBits, int nStop);

public:

    Serial(int nSpeed = 115200, char nEvent = 'N', int nBits = 8, int nStop = 1);

    ~Serial();

    bool InitPort(int nSpeed = 115200, char nEvent = 'N', int nBits = 8, int nStop = 1);

//    int GetBytesInCOM() const ;
    bool WriteData(const unsigned char *pData, unsigned int length);

    bool ReadData(unsigned char *buffer, unsigned int length);

    void get_data();

};

#endif

#endif /* _SERIAL_H_ */

/*
 *
 *
 *  不同电子设备的通讯协议，用于读取和写入不同寄存器内数据
 *
*/

#ifndef DEVICE_COMMUNICATION_H
#define DEVICE_COMMUNICATION_H

#include <QObject>
#include <QTcpSocket>

typedef char               int8;
typedef unsigned char      uint8;
typedef short              int16;
typedef unsigned short     uint16;
typedef int                int32;
typedef unsigned int       uint32;
typedef long long          int64;
typedef unsigned long long uint64;


class SanLing_PLC: public QObject
{
    Q_OBJECT
public:
    /*************************   暂只能用于数据寄存器(D)的读写  ***********************/
    //
    /// \brief   生成MC协议读取二进制报文
    /// \param registerName - [in] 寄存器名字，'M'为M寄存器，'D'为数据寄存器
    /// \param startNum     - [in] 寄存器开始序列号，如100
    /// \param len          - [in] 寄存器连续个数。
    /// \return             - 生成的二进制报文
    ///
    static QByteArray MC_Bin3E_read_message(const QByteArray registerName, const uint32 startNum, const int32 len, QByteArray* commandLog);

    ///
    /// \brief   生成MC协议写入16位数据二进制报文
    /// \param registerName - [in] 寄存器名字，'M'为M寄存器，'D'为数据寄存器
    /// \param startNum     - [in] 寄存器开始序列号，如100
    /// \param len          - [in] 寄存器连续个数。
    /// \param data[]       - [in] 输入的数据，以单个'M'或'D'输入。
    /// \return             - 生成的二进制报文
    ///
    static QByteArray MC_Bin3E_write_message(const QByteArray registerName, const uint32 startNum, const int32 len, const int32 data[], QByteArray* commandLog);

    ///
    /// \brief   生成MC协议写入32位数据二进制报文
    /// \param registerName - [in] 寄存器名字，'M'为M寄存器，'D'为数据寄存器
    /// \param startNum     - [in] 寄存器开始序列号，如100
    /// \param len          - [in] 寄存器连续个数。
    /// \param data[]       - [in] 输入的数据，以双个'D'输入。
    /// \return             - 生成的二进制报文
    ///
    static QByteArray write_D_register_bit16(const uint32& startNum, const uint32& len, const int32 data[]);
    static QByteArray MC_Bin3E_write_message_bit32(const QByteArray registerName, const uint32 startNum, const int32 len, const int32 data[], QByteArray* commandLog);

    static QByteArray gen_read_8bit_datagram(const uint32 &startNum, const uint32 &len);

};


class Anchuan_PLC: public QObject
{
   Q_OBJECT
public:
    /**********************   暂只能用于数据寄存器(MW)的读写  ***********************/
    //
    /// \brief   读取二进制报文（16位） 连续读16位
    /// \param registerName - [in] 寄存器名字，暂时无效
    /// \param startNum     - [in] 寄存器开始序列号，如100
    /// \param len          - [in] 寄存器连续个数。
    /// \return             - 生成的二进制报文
    ///
    static QByteArray read_message(const QByteArray registerName, const uint32 startNum, const int32 len, QByteArray* commandLog);

    ///
    /// \brief   写入16位数据二进制报文  连续写16位
    /// \param registerName - [in] 寄存器名字，暂时无效
    /// \param startNum     - [in] 寄存器开始序列号，如100
    /// \param len          - [in] 寄存器连续个数。
    /// \param data[]       - [in] 输入的数据，MW寄存器写入。
    /// \return             - 生成的二进制报文
    ///
    static QByteArray write_message(const QByteArray registerName, const uint32 startNum, const int32 len, const int32 data[], QByteArray* commandLog);

    ///
    /// \brief   写入32位数据二进制报文
    /// \param registerName - [in] 寄存器名字，暂时无效
    /// \param startNum     - [in] 寄存器开始序列号，如100
    /// \param len          - [in] 寄存器连续个数。
    /// \param data[]       - [in] 输入的数据，ML寄存器写入。
    /// \return             - 生成的二进制报文
    ///
    static QByteArray write_message_bit32(const QByteArray registerName, const uint32 startNum, const int32 len, const int32 data[], QByteArray* commandLog);

};


class Anchuan_Robot
{
public:

    ///
    /// \brief   单个寄存器读取数据
    /// \param registerName - [in] 寄存器名字，如"B", "D", "R", 目前仅支持B、D寄存器读取
    /// \param startNum     - [in] 寄存器开始序列号，如100
    /// \param len          - [in] 寄存器连续个数。
    /// \return             - 生成的二进制报文
    ///
    static QByteArray read_single_register(const QByteArray registerName, const uint32 startNum, QByteArray* commandLog);


    ///
    /// \brief   连续多个寄存器读取数据, 目前无寄存器写入
    /// \param registerName - [in] 寄存器名字，如"B", "D", "R"
    /// \param startNum     - [in] 寄存器开始序列号，如100
    /// \param len          - [in] 寄存器连续个数。
    /// \return             - 生成的二进制报文
    ///
    static QByteArray read_multiple_register(const QByteArray registerName, const uint32 startNum, const int32 len, QByteArray* commandLog);


    ///
    /// \brief   单个寄存器写入数据
    /// \param registerName - [in] 寄存器名字，如写入"B", "D", "R"等寄存器, 目前仅支持B、D寄存器写入
    /// \param startNum     - [in] 寄存器开始序列号，如100
    /// \param len          - [in] 寄存器连续个数。
    /// \param data[]       - [in] 输入的数据，寄存器写入。
    /// \return             - 生成的二进制报文
    ///
    static QByteArray write_single_register(const QByteArray registerName, const uint32 startNum, const int32 data[], QByteArray* commandLog);


    ///
    /// \brief   连续多个寄存器写入数据
    /// \param registerName - [in] 寄存器名字，如写入"B", "D", "R"等寄存器, 目前仅支持D、R寄存器写入
    /// \param startNum     - [in] 寄存器开始序列号，如100
    /// \param len          - [in] 寄存器连续个数。
    /// \param data[]       - [in] 输入的数据，寄存器写入。
    /// \return             - 生成的二进制报文
    ///
    static QByteArray write_multiple_register(const QByteArray registerName, const uint32 startNum, const int32 len, const int32 data[], QByteArray* commandLog);


    ///
    /// \brief   解码出寄存器数据
    /// \param registerName - [in] 解码的寄存器名字，如"B", "D", "R"等, 目前仅支持B、D寄存器解码
    /// \param message      - [in] 解码的报文
    /// \param dataLength   - [in] 解码的寄存器个数，如1个寄存器则1，n个则为n
    /// \param data         - [out] 返回的数据，以32位整型返回
    /// \return             - 若 message 是正确返回的报文，则返回true
    ///
    static bool decode_message_data(const QByteArray registerName, const QByteArray &message, const int32 dataLength, int32 data[]);

};


class LightController_CCS: public QObject
{
    Q_OBJECT
public:
    // portNum - 控制器通道数，如4,8等
    LightController_CCS(uint8 portNum);
    ~LightController_CCS();


public:
    ///
    /// \brief 生成打开所有通道的命令
    /// \return - 生成的命令字符串
    ///
     QByteArray gen_open_all_chanels_command();

    ///
    /// \brief 生成关闭所有通道的命令
    /// \return - 生成的命令字符串
    ///
     QByteArray gen_close_all_chanels_command();

    ///
    /// \brief 生成设置某通道亮度的命令
    /// \param chanelIndex - [in] 控制器端口号，如0、1、2、3等
    /// \param intensity   - [in] 亮度值。0-255
    /// \return            - 生成的命令字符串
    ///
     QByteArray gen_one_chanel_light_intensity_command(const uint8 chanelIndex, const uint8 intensity);

    ///
    /// \brief 生成设置所有通道亮度的命令, 控制器型号带(A)才可以
    /// \param chanelNum - [in] 控制器端口通道数，如两通道为2，三通道为3
    /// \param intensity - [in] 亮度值。0-255
    /// \return          - 生成的命令字符串
    ///
     QByteArray gen_all_chanel_light_intensity_command(const uint8 chanelNum, const uint8 intensities[]);

public slots:
    ///
    /// \brief open_socket
    /// \param hostName
    /// \param port
    /// \return
    ///
    bool open_socket(const QString &hostName, quint16 port);
    void close_socket();

    ///
    /// \brief 打开控制器所有通道，仅ON操作，不改变原通道亮度值
    /// \param error - [out] 是否设置成功。true-打开成功，false-打开失败
    ///
    void open_all_chanels(bool *error);

    ///
    /// \brief 关闭所有通道，仅OFF操作，不改变原通道亮度值
    /// \param error - [out] 是否设置成功。true-关闭成功，false-关闭失败
    ///
    void close_all_chanels(bool *error);

    ///
    /// \brief 设置某通道亮度
    /// \param chanelIndex - [in] 控制器端口号，如0、1、2、3等
    /// \param intensity   - [in] 亮度值。0-255
    ///
    void set_one_chanel_light_intensity(const uint8 chanelIndex, const uint8 intensity);

    ///
    /// \brief 生成设置所有通道亮度的命令, 控制器型号带(A)才可以
    /// \param chanelNum   - [in] 控制器端口通道数，如3通道，4通道
    /// \param intensities - [in] 亮度值数组。0-255，跟通道数对应
    ///
    void set_all_chanel_light_intensity(const uint8 chanelNum, const uint8 intensities[]);

private:
    QTcpSocket* m_Socket = nullptr;
    uint8 m_PortNum = 4;
};


class LightController_OPT
{
public:
    ///
    /// \brief 生成打开所有通道的命令
    /// \return - 生成的命令字符串
    ///
    static QByteArray open_all_chanels();

    ///
    /// \brief 生成关闭所有通道的命令
    /// \return - 生成的命令字符串
    ///
    static QByteArray close_all_chanels();

    ///
    /// \brief 生成设置某通道亮度的命令
    /// \param chanelIndex - [in] 通道序列号，如两通道为:0,1。三通道为:0,1,2
    /// \param intensity   - [in] 亮度值。0-255
    /// \return            - 生成的命令字符串
    ///
    static QByteArray light_intensity_setting_all(const uint8 chanelIndex, const uint8 intensity);

private:
    QTcpSocket* m_Socket;

};


#endif // DEVICE_COMMUNICATION_H

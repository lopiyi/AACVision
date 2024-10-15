#include "device_communication.h"

#include <QDebug>
#include <QException>


char ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
        return ch-0x30;
    else if((ch >= 'A') && (ch <= 'F'))
        return ch-'A'+10;
    else if((ch >= 'a') && (ch <= 'f'))
        return ch-'a'+10;
    else return (-1);
}

void StringToHex(QString str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;
    for(int i=0; i<len; )
    {
        //char lstr,
        hstr=str[i].toLatin1();
        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}


QByteArray SanLing_PLC::MC_Bin3E_read_message(const QByteArray registerName, const uint32 startNum, const int32 len, QByteArray * commandLog)
{
    QByteArray temp= "";
    temp.append("5000");  //副头部
    temp.append("00");    //Q头部-网络编号
    temp.append("FF");    //Q头部-可编程控制器编号
    temp.append("FF03");  //Q头部-请求目标模块I/O编号
    temp.append("00");    //Q头部-请求目标模块站号

    temp.append("0C00");  //Q头部-请求数据长度，需根据实际填写
    temp.append("1000");  //Q头部-CPU监视定时器
    temp.append("0104");  //指令
    temp.append("0000");  //子指令

    QByteArray Hex = QByteArray::number(startNum,16).toUpper();
    while(6-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    Hex.remove(Hex.length()-2,2);
    temp.append(Hex.right(2)); //数字位16进制高低交换
    Hex.remove(Hex.length()-2,2);
    temp.append(Hex.right(2)); //数字位16进制高低交换

    if(registerName == "M"){
        temp.append("90"); //M寄存器
    }
    else if(registerName == "D"){
        temp.append("A8"); //D寄存器
    }

    Hex = QByteArray::number(len,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换

    *commandLog = temp;

    QByteArray dHex;
    int length = temp.length()/2;
    for(int i=0;i<length;i++)
    {
        dHex[i] = temp.left(2).toInt(Q_NULLPTR, 16);
        temp.remove(0,2);
    }

    return dHex;
}

QByteArray SanLing_PLC::gen_read_8bit_datagram(const uint32 &startNum, const uint32 &len)
{
    QByteArray datagram = "";
    datagram[0] = 0x50;
    datagram[1] = 0x00;  //副头部[5000]

    datagram[2] = 0x00;  //Q头部-网络编号[00]
    datagram[3] = 0xFF;  //Q头部-可编程控制器编号[FF]
    datagram[4] = 0xFF;
    datagram[5] = 0x03;  //Q头部-请求目标模块I/O编号[03FF]
    datagram[6] = 0x00;  //Q头部-请求目标模块站号[00]

    datagram[7] = 0x0C;
    datagram[8] = 0x00;  //Q头部-请求数据长度，需根据实际填写[000C]
    datagram[9]  = 0x10;
    datagram[10] = 0x00; //Q头部-CPU监视定时器[0010]
    datagram[11] = 0x01;
    datagram[12] = 0x04; //指令[0401]
    datagram[13] = 0x00;
    datagram[14] = 0x00; //子指令[0000]

    //起始软元件
    datagram[15] = 0xFF & startNum;
    datagram[16] = 0xFF & (startNum>>8);
    datagram[17] = 0xFF & (startNum>>16);

    datagram[18] = 0xA8; //D寄存器

    //软元件点数
    uint32 len_bytes = len/2 + len%2;
    datagram[19] = 0xFF & len_bytes;
    datagram[20] = 0xFF & (len_bytes>>8);

        return datagram;
}


QByteArray SanLing_PLC::MC_Bin3E_write_message(const QByteArray registerName, const uint32 startNum, const int32 len, const int32 data[], QByteArray * commandLog)
{
    QByteArray temp= "";
    temp.append("5000"); //副头部
    temp.append("00");   //Q头部-网络编号
    temp.append("FF");   //Q头部-可编程控制器编号
    temp.append("FF03"); //Q头部-请求目标模块I/O编号
    temp.append("00");   //Q头部-请求目标模块站号

    int datalen = 12+2*len;
    QByteArray Hex = QByteArray::number(datalen,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //Q头部-请求数据长度
    temp.append(Hex.left(2));  //Q头部-请求数据长度
    temp.append("1000");       //Q头部-CPU监视定时器
    temp.append("0114");       //指令
    temp.append("0000");       //子指令

    //起始寄存器
    Hex = QByteArray::number(startNum,16).toUpper();
    while(6-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    Hex.remove(Hex.length()-2,2);
    temp.append(Hex.right(2)); //数字位16进制高低交换
    Hex.remove(Hex.length()-2,2);
    temp.append(Hex.right(2)); //数字位16进制高低交换

    if(registerName == "M"){
        temp.append("90"); //M寄存器
    }
    else if(registerName == "D"){
        temp.append("A8"); //D寄存器
    }

    //数据长度
    Hex = QByteArray::number(len,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换

    //加入数据
    for(int i=0; i<len; i++)
    {
        Hex = QByteArray::number(data[i],16).toUpper();
        if(Hex.length()==8)
        {
            Hex = Hex.right(4);
        }
        while(4-Hex.length()) {
            Hex.insert(0,"0");
        }
        temp.append(Hex.right(2)); //数字位16进制高低交换
        temp.append(Hex.left(2));  //数字位16进制高低交换
    }
    //commandLog->clear();
    //commandLog->append(temp);
    *commandLog = temp;
    int length = temp.length() / 2;
    QByteArray dHex;
    for(int i=0;i<length;i++)
    {
        dHex.append(temp.left(2).toInt(Q_NULLPTR, 16));
        temp.remove(0,2);
    }

    return dHex;
}

QByteArray SanLing_PLC::write_D_register_bit16(const uint32& startNum, const uint32& len, const int32 data[])
{
    QByteArray command = "";
    command[0] = 0x50;
    command[1] = 0x00;  //副头部[5000]
    command[2] = 0x00;  //Q头部-网络编号[00]
    command[3] = 0xFF;  //Q头部-可编程控制器编号[FF]
    command[4] = 0xFF;
    command[5] = 0x03;  //Q头部-请求目标模块I/O编号[03FF]
    command[6] = 0x00;  //Q头部-请求目标模块站号[00]

    //Q头部-请求数据长度
    uint32 datalen = 12 + 2 * len;
    command[7] = 0xFF & datalen;
    command[8] = 0xFF & (datalen >> 8);

    command[9] = 0x10;
    command[10] = 0x00; //Q头部-CPU监视定时器[0010]
    command[11] = 0x01;
    command[12] = 0x14; //指令[1401]
    command[13] = 0x00;
    command[14] = 0x00; //子指令[0000]

    //起始软元件编号
    command[15] = 0xFF & startNum;
    command[16] = 0xFF & (startNum >> 8);
    command[17] = 0xFF & (startNum >> 16);

    //软元件代号名
    command[18] = 0xA8; //D寄存器

    //软元件写入点数
    command[19] = 0xFF & len;
    command[20] = 0xFF & (len >> 8);

    //写入的数据
    for (uint32 i = 0; i < len; i++)
    {
        uint32 n = 2 * i;
        int32  d = data[i];
        command[21 + n] = 0xFF & d;
        command[21 + n + 1] = 0xFF & (d >> 8);
    }

    return command;
}


QByteArray SanLing_PLC::MC_Bin3E_write_message_bit32(const QByteArray registerName, const uint32 startNum, const int32 len, const int32 data[], QByteArray * commandLog)
{
    QByteArray temp= "";
    temp.append("5000"); //副头部
    temp.append("00");   //Q头部-网络编号
    temp.append("FF");   //Q头部-可编程控制器编号
    temp.append("FF03"); //Q头部-请求目标模块I/O编号
    temp.append("00");   //Q头部-请求目标模块站号

    int datalen = 12+4*len;
    QByteArray Hex = QByteArray::number(datalen,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //Q头部-请求数据长度
    temp.append(Hex.left(2));  //Q头部-请求数据长度
    temp.append("1000");       //Q头部-CPU监视定时器
    temp.append("0114");       //指令
    temp.append("0000");       //子指令

    //起始寄存器
    Hex = QByteArray::number(startNum,16).toUpper();
    while(6-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    Hex.remove(Hex.length()-2,2);
    temp.append(Hex.right(2)); //数字位16进制高低交换
    Hex.remove(Hex.length()-2,2);
    temp.append(Hex.right(2)); //数字位16进制高低交换

    if(registerName == "M"){
        temp.append("90"); //M寄存器
    }
    else if(registerName == "D"){
        temp.append("A8"); //D寄存器
    }

    //数据长度
    Hex = QByteArray::number(len*2,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换

    //加入数据
    for(int i=0; i<len; i++)
    {
        Hex = QByteArray::number(data[i],16).toUpper();

        while(8-Hex.length()) {
            Hex.insert(0,"0");
        }
        //temp.append(Hex.mid(2,2));  //数字位字节2
        //temp.append(Hex.mid(0,2));  //数字位字节1
        //temp.append(Hex.mid(6,2));  //数字位字节4
        //temp.append(Hex.mid(4,2));  //数字位字节3
        temp.append(Hex.mid(6,2));  //数字位字节4
        temp.append(Hex.mid(4,2));  //数字位字节3
        temp.append(Hex.mid(2,2));  //数字位字节2
        temp.append(Hex.mid(0,2));  //数字位字节1

    }
    //qDebug()<<"temp"<<temp;
    *commandLog = temp;

    //* 转换成16进制
    QByteArray dHex;
    int length = temp.length()/2;
    for(int i=0;i<length;i++)
    {
        dHex[i] = temp.left(2).toInt(Q_NULLPTR, 16);
        temp.remove(0,2);
    }

    return dHex;
}


//***********************安川电机****************************//
//*********************************************************//

//读取保持寄存器的内容，相邻编号的寄存器可以连续读取(扩展)    连续读十六位
QByteArray Anchuan_PLC::read_message(const QByteArray registerName, const uint32 startNum, const int32 len, QByteArray *commandLog)
{
    QByteArray temp= "";
    //218标题
    temp.append("11");        //MEMOBUS指令
    temp.append("00");        //识别编号
    temp.append("00");        //发送目标编通道
    temp.append("00");        //发送源通道
    temp.append("0000");      //未使用
    temp.append("1600");      //报文总长度，218报头与应用数据的总数据长度(字节数)，16位读取时为1600
    temp.append("00000000");  //未使用

    //数据部分
    temp.append("0800");      //数据部分的长度，16位读取默认为0800
    temp.append("20");        //MFC，主功能码，16位地址为20H
    temp.append("09");        //SFC，子功能码，读取保持寄存器的内容(扩展)
    temp.append("00");        //CPU编号
    temp.append("00");        //未使用

    //寄存器编号
    QByteArray Hex = QByteArray::number(startNum,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    Hex.remove(Hex.length()-2,2);
    temp.append(Hex.right(2)); //数字位16进制高低交换

    //寄存器数量
    Hex = QByteArray::number(len,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换
    //*****************报文结束************************

    *commandLog = temp;

    //* 转换成16进制
    QByteArray dHex;
    int length = temp.length()/2;
    for(int i=0;i<length;i++)
    {
        dHex[i] = temp.left(2).toInt(Q_NULLPTR, 16);
        temp.remove(0,2);
    }

    return dHex;
}

//16位地址16位数据，寄存器的不连续写入     连续写16位
QByteArray Anchuan_PLC::write_message(const QByteArray registerName, const uint32 startNum, const int32 len, const int32 data[], QByteArray *commandLog)
{
    QByteArray temp= "";
    //218标题
    temp.append("11");        //MEMOBUS指令
    temp.append("00");        //识别编号
    temp.append("00");        //发送目标编通道
    temp.append("00");        //发送源通道
    temp.append("0000");      //未使用
    //报文总长度，12为218标题长度，6为数据部分固定长度，每多加一个数据，长度加4
    int all_len = 12 + 6 + len * 4;
    QByteArray Hex = QByteArray::number(all_len, 16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换

    temp.append("00000000");  //未使用

    //数据部分的长度
    int data_len = 6 + len * 4;
    Hex = QByteArray::number(data_len,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换

    //数据部分
    temp.append("20");        //MFC，主功能码，16位地址为20，32位地址为43
    temp.append("0E");        //SFC，子功能码，寄存器的不连续写入
    temp.append("01");        //CPU编号
    temp.append("00");        //未使用

    //寄存器数量
    Hex = QByteArray::number(len,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换

    //按照寄存器的数量，添加寄存器种类，寄存器数据类型，寄存器编号与数据
    for(int i=0; i<len; i++)
    {
        //寄存器编号
        Hex = QByteArray::number(startNum + i,16).toUpper();
        while(4-Hex.length()) {
            Hex.insert(0,"0");
        }
        temp.append(Hex.right(2)); //数字位16进制高低交换
        Hex.remove(Hex.length()-2,2);
        temp.append(Hex.right(2)); //数字位16进制高低交换

        //数据
        Hex = QByteArray::number(data[i],16).toUpper();
        while(4-Hex.length()) {
            Hex.insert(0,"0");
        }
        temp.append(Hex.mid(2,2));  //数字位字节2
        temp.append(Hex.mid(0,2));  //数字位字节1
    }
    //*****************报文结束************************

    //qDebug()<<"temp"<<temp;
    *commandLog = temp;

    //* 转换成16进制
    QByteArray dHex;
    int length = temp.length()/2;
    for(int i=0;i<length;i++)
    {
        dHex[i] = temp.left(2).toInt(Q_NULLPTR, 16);
        temp.remove(0,2);
    }

    return dHex;
}

//32位地址32位数据,寄存器的不连续写入
QByteArray Anchuan_PLC::write_message_bit32(const QByteArray registerName, const uint32 startNum, const int32 len, const int32 data[], QByteArray *commandLog)
{
    QByteArray temp= "";
    //218标题
    temp.append("11");        //MEMOBUS指令
    temp.append("00");        //识别编号
    temp.append("00");        //发送目标编通道
    temp.append("00");        //发送源通道
    temp.append("0000");      //未使用
    //报文总长度，12为218标题长度，8为数据部分固定长度，每多加一个数据，长度加10，32地址32位数据写入一个寄存器时为1E00
    int all_len = 12 + 8 + len * 10;
    QByteArray Hex = QByteArray::number(all_len,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换

    temp.append("00000000");  //未使用

    //数据部分的长度，32地址32位数据写入一个寄存器时为1000
    int data_len = 6 + len * 10;
    Hex = QByteArray::number(data_len,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换

    //数据部分
    temp.append("43");        //MFC，主功能码，16位地址为20，32位地址为43
    temp.append("4E");        //SFC，子功能码，寄存器的不连续写入
    temp.append("00");        //CPU编号
    temp.append("00");        //未使用

    //寄存器数量
    Hex = QByteArray::number(len,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换

    //按照寄存器的数量，添加寄存器种类，寄存器数据类型，寄存器编号与数据
    for(int i=0; i<len; i++)
    {
        temp.append("4D");         //寄存器种类
        temp.append("03");         //寄存器数据类型，16位数据为02,32位数据为03

        //寄存器编号
        Hex = QByteArray::number(startNum + 2 * i,16).toUpper();
        while(8-Hex.length()) {
            Hex.insert(0,"0");
        }
        temp.append(Hex.right(2)); //数字位16进制高低交换
        Hex.remove(Hex.length()-2,2);
        temp.append(Hex.right(2)); //数字位16进制高低交换
        Hex.remove(Hex.length()-2,2);
        temp.append(Hex.right(2)); //数字位16进制高低交换
        Hex.remove(Hex.length()-2,2);
        temp.append(Hex.right(2)); //数字位16进制高低交换

        //数据
        Hex = QByteArray::number(data[i],16).toUpper();
        while(8-Hex.length()) {
            Hex.insert(0,"0");
        }
        temp.append(Hex.mid(6,2));  //数字位字节4
        temp.append(Hex.mid(4,2));  //数字位字节3
        temp.append(Hex.mid(2,2));  //数字位字节2
        temp.append(Hex.mid(0,2));  //数字位字节1
    }
    //*****************报文结束************************

    //qDebug()<<"temp"<<temp;
    *commandLog = temp;

    //* 转换成16进制
    QByteArray dHex;
    int length = temp.length()/2;
    for(int i=0;i<length;i++)
    {
        dHex[i] = temp.left(2).toInt(Q_NULLPTR, 16);
        temp.remove(0,2);
    }

    return dHex;
}


QByteArray Anchuan_Robot::read_single_register(const QByteArray registerName, const uint32 startNum, QByteArray *commandLog)
{
    QByteArray temp = "";
    //* 1
    temp.append("594552432000000003010001000000003939393939393939");  //
    //* 1

    if(registerName == "B")
        temp.append("7a");
    else if(registerName == "D")
        temp.append("7c");
    else if(registerName == "")
        temp.append("7");
    temp.append("00");

    //* 2
    //变量起始地址，如D9，地址为9
    QByteArray Hex = QByteArray::number(startNum,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换
    //* 2

    //* 3
    temp.append("01010000");  //未使用
    //* 3
    //*****************报文结束************************
    //qDebug()<<"temp"<<temp;
    *commandLog = temp;

    //* 转换成16进制
    QByteArray dHex;
    int length = temp.length()/2;
    for(int i=0;i<length;i++)
    {
        dHex[i] = temp.left(2).toInt(Q_NULLPTR, 16);
        temp.remove(0,2);
    }

    return dHex;
}

QByteArray Anchuan_Robot::read_multiple_register(const QByteArray registerName, const uint32 startNum, const int32 len, QByteArray *commandLog)
{
    QByteArray temp = "";
    return temp;
}

QByteArray Anchuan_Robot::write_single_register(const QByteArray registerName, const uint32 startNum, const int32 data[], QByteArray *commandLog)
{
    QByteArray temp = "";
    //* 1
    temp.append("594552432000");  //
    //* 1

    if(registerName == "B")
        temp.append("01");
    else if(registerName == "D")
        temp.append("04");
    temp.append("0003010001000000003939393939393939");

    if(registerName == "B")
        temp.append("7a");
    else if(registerName == "D")
        temp.append("7c");
    else if(registerName == "")
        temp.append("7");
    temp.append("00");

    //* 2
    //变量起始地址，如D9，地址为9
    QByteArray Hex = QByteArray::number(startNum,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换
    //* 2

    //* 3
    temp.append("01020000");  //未使用
    //* 3

    //* 4
    //数据
    if(registerName == "B")
    {
        Hex = QByteArray::number(data[0],16).toUpper();
        while(2-Hex.length()) {
            Hex.insert(0,"0");
        }
        temp.append(Hex.mid(0,2));  //数字位字节1
    }
    else if(registerName == "D")
    {
        Hex = QByteArray::number(data[0],16).toUpper();
        while(8-Hex.length()) {
            Hex.insert(0,"0");
        }
        temp.append(Hex.mid(6,2));  //数字位字节4
        temp.append(Hex.mid(4,2));  //数字位字节3
        temp.append(Hex.mid(2,2));  //数字位字节2
        temp.append(Hex.mid(0,2));  //数字位字节1
    }
    //* 4
    //*****************报文结束************************
    *commandLog = temp;

    //* 转换成16进制
    QByteArray dHex;
    int length = temp.length()/2;
    for(int i=0;i<length;i++)
    {
        dHex[i] = temp.left(2).toInt(Q_NULLPTR, 16);
        temp.remove(0,2);
    }

    return dHex;
}

QByteArray Anchuan_Robot::write_multiple_register(const QByteArray registerName, const uint32 startNum, const int32 len, const int32 data[], QByteArray *commandLog)
{
    QByteArray temp = "";
    //* 1
    temp.append("59");  //
    temp.append("45");  //
    temp.append("52");  //
    temp.append("43");  //
    temp.append("20");  //未使用
    temp.append("00");  //未使用
    //* 1

    //* 2
    //报文总长度，4为"变量数量"固定长度，每多加一个数据，长度加4
    int all_len = 4 + len * 4;
    QByteArray Hex = QByteArray::number(all_len,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换
    //* 2

    //* 3
    temp.append("03010001000000003939393939393939");  //未使用
    //* 3

    if(registerName == "B")
        temp.append("7a");
    else if(registerName == "D")
        temp.append("04");
    else if(registerName == "R")
        temp.append("05");
    temp.append("03");

    //* 4
    //变量起始地址，如D9，地址为9
    Hex = QByteArray::number(startNum,16).toUpper();
    while(4-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.right(2)); //数字位16进制高低交换
    temp.append(Hex.left(2));  //数字位16进制高低交换
    //* 4

    //* 5
    temp.append("00340000");  //未使用
    //* 5

    //* 6
    //连续变量数量
    Hex = QByteArray::number(len,16).toUpper();
    while(8-Hex.length()) {
        Hex.insert(0,"0");
    }
    temp.append(Hex.mid(6,2));  //数字位字节4
    temp.append(Hex.mid(4,2));  //数字位字节3
    temp.append(Hex.mid(2,2));  //数字位字节2
    temp.append(Hex.mid(0,2));  //数字位字节1
    //* 6

    //* 7
    //连续数据转化
    for(int i=0; i<len; i++)
    {
        //数据
        Hex = QByteArray::number(data[i],16).toUpper();
        while(8-Hex.length()) {
            Hex.insert(0,"0");
        }
        temp.append(Hex.mid(6,2));  //数字位字节4
        temp.append(Hex.mid(4,2));  //数字位字节3
        temp.append(Hex.mid(2,2));  //数字位字节2
        temp.append(Hex.mid(0,2));  //数字位字节1
    }
    //* 7
    //*****************报文结束************************

    *commandLog = temp;

    //* 转换成16进制
    QByteArray dHex;
    int length = temp.length()/2;
    for(int i=0;i<length;i++)
    {
        dHex[i] = temp.left(2).toInt(Q_NULLPTR, 16);
        temp.remove(0,2);
    }

    return dHex;
}

bool Anchuan_Robot::decode_message_data(const QByteArray registerName, const QByteArray &message, const int32 dataLength, int32 data[])
{
    char D = registerName[0];

    //解码数据
    QByteArray Hex  = "";
    QByteArray temp = "";

    switch (D) {
    case 'B':
    {
        if (message.mid(12, 2) != "01") //特定字节为某个数值则是正确返回的报文
            return false;
        temp = message.right(2);
        Hex  = temp;

        break;
    }
    case 'D':
    {
        if (message.mid(12, 2) != "04") //特定字节为某个数值则是正确返回的报文
            return false;
        temp = message.right(8);
        Hex.append(temp.mid(6,2));  //数字位字节4
        Hex.append(temp.mid(4,2));  //数字位字节3
        Hex.append(temp.mid(2,2));  //数字位字节2
        Hex.append(temp.mid(0,2));  //数字位字节1

        break;
    }
    case 'R':
    {
        if (message.mid(12, 2) != "04") //特定字节为某个数值则是正确返回的报文
            return false;
        temp = message.right(8);
        Hex.append(temp.mid(6,2));  //数字位字节4
        Hex.append(temp.mid(4,2));  //数字位字节3
        Hex.append(temp.mid(2,2));  //数字位字节2
        Hex.append(temp.mid(0,2));  //数字位字节1

        break;
    }
    }
    //qDebug()<<"Hex"<<Hex;

    for(int i=0; i<dataLength; i++)
    {
        data[i] = Hex.toInt(nullptr, 16);
    }

    return true;
}


bool LightController_CCS::open_socket(const QString &hostName, quint16 port)
{
    try
    {
        close_socket();

        m_Socket = new QTcpSocket();
        m_Socket->connectToHost(hostName, port);
        return m_Socket->waitForConnected(5000);
    }
    catch(QException& e)
    {
        return false;;
    }
}

void LightController_CCS::close_socket()
{
    try
    {
        if(m_Socket != nullptr && QAbstractSocket::ConnectedState == m_Socket->state())
        {
            m_Socket->close();
            delete m_Socket;
            m_Socket = nullptr;
        }
    }
    catch(QException& e)
    {
        ;
    }
}

void LightController_CCS::open_all_chanels(bool *error)
{
    QByteArray command = gen_open_all_chanels_command();

    if(m_Socket != nullptr && QAbstractSocket::ConnectedState == m_Socket->state())
    {
        m_Socket->write(command);
        m_Socket->waitForBytesWritten(50);
        m_Socket->waitForReadyRead(1000);
        QByteArray buffer = m_Socket->readAll();
        //qDebug()<<"m_Socket:"<< buffer;
    }
}

void LightController_CCS::close_all_chanels(bool *error)
{
    QByteArray command = gen_close_all_chanels_command();

    if(m_Socket != nullptr && QAbstractSocket::ConnectedState == m_Socket->state())
    {
        m_Socket->write(command);
        m_Socket->waitForBytesWritten(50);
        m_Socket->waitForReadyRead(1000);
        QByteArray buffer = m_Socket->readAll();
        //qDebug()<<"m_Socket:"<< buffer;
    }
}

void LightController_CCS::set_one_chanel_light_intensity(const uint8 chanelIndex, const uint8 intensity)
{
    QByteArray command = gen_one_chanel_light_intensity_command(chanelIndex, intensity);

    if(m_Socket != nullptr && QAbstractSocket::ConnectedState == m_Socket->state())
    {
        m_Socket->write(command);
        m_Socket->waitForBytesWritten(50);
        m_Socket->waitForReadyRead(1000);
        QByteArray buffer = m_Socket->readAll();
        //qDebug()<<"m_Socket:"<< buffer;
    }
}

void LightController_CCS::set_all_chanel_light_intensity(const uint8 chanelNum, const uint8 intensities[])
{
    QByteArray command = gen_all_chanel_light_intensity_command(chanelNum, intensities);

    if(m_Socket != nullptr && QAbstractSocket::ConnectedState == m_Socket->state())
    {
        m_Socket->write(command);
        m_Socket->waitForBytesWritten(50);
        m_Socket->waitForReadyRead(1000);
        QByteArray buffer = m_Socket->readAll();
        //qDebug()<<"m_Socket:"<< buffer;
    }
}

LightController_CCS::LightController_CCS(uint8 portNum)
{
    m_PortNum = portNum;
}


LightController_CCS::~LightController_CCS()
{

}

QByteArray LightController_CCS::gen_open_all_chanels_command()
{
    QByteArray command;
    command = "@FFL1/1/1/1/1/1/1/1";
    int l   = 0;
    foreach (char c, command)
    {
        l += c;
    }
    command.append(QByteArray::number(l, 16).toUpper().right(2));
    command.append('\x0D');
    command.append('\x0A');

    return command;
}

QByteArray LightController_CCS::gen_close_all_chanels_command()
{
    QByteArray command;
    //command = "@FFLF/F/0/0";
    command = "@FFL0/0/0/0/0/0/0/0";
    int l   = 0;
    foreach (char c, command)
    {
        l += c;
    }
    command.append(QByteArray::number(l, 16).toUpper().right(2));
    command.append('\x0D');
    command.append('\x0A');

    return command;
}

QByteArray LightController_CCS::gen_one_chanel_light_intensity_command(const uint8 chanelIndex, const uint8 intensity)
{
    QByteArray command = "@";
    QByteArray index = QByteArray::number(chanelIndex);
    while (2 - index.length())
    {
        index.insert(0, "0");
    }
    command.append(index);
    command.append("F");

    index = QByteArray::number(intensity);
    while (3 - index.length())
    {
        index.insert(0, "0");
    }
    command.append(index);
    int l = 0;
    foreach (char c, command)
    {
        l += c;
    }
    command.append(QByteArray::number(l, 16).toUpper().right(2));
    command.append('\x0D');
    command.append('\x0A');

    return command;
}

QByteArray LightController_CCS::gen_all_chanel_light_intensity_command(const uint8 chanelNum, const uint8 intensities[])
{
//    QByteArray command = "";
//    return command;
    //测试5通道灯光
    QByteArray d = "50,100,150,200,250,60,70,80";
    QList<QByteArray> datalist = d.split(',');
    while (3 - datalist[0].length())
    {
        datalist[0].insert(0, "0");
    }
    while (3 - datalist[1].length())
    {
        datalist[1].insert(0, "0");
    }
    while (3 - datalist[2].length())
    {
        datalist[2].insert(0, "0");
    }
    while (3 - datalist[3].length())
    {
        datalist[3].insert(0, "0");
    }
    while (3 - datalist[4].length())
    {
        datalist[4].insert(0, "0");
    }
    while (3 - datalist[5].length())
    {
        datalist[5].insert(0, "0");
    }
    while (3 - datalist[6].length())
    {
        datalist[6].insert(0, "0");
    }
    while (3 - datalist[7].length())
    {
        datalist[7].insert(0, "0");
    }

    QByteArray command = "@FFF";
    command.append(datalist[0]);
    command.append("/");
    command.append(datalist[1]);
    command.append("/");
    command.append(datalist[2]);
    command.append("/");
    command.append(datalist[3]);
    command.append("/");
    command.append(datalist[4]);
    command.append("/");
    command.append(datalist[5]);
    command.append("/");
    command.append(datalist[6]);
    command.append("/");
    command.append(datalist[7]);
    int l = 0;
    foreach (char c, command)
    {
        l += c;
    }
    command.append(QByteArray::number(l, 16).toUpper().right(2));
    command.append('\x0D');
    command.append('\x0A');
    //qDebug()<<"command"<<command;
    return command;

}

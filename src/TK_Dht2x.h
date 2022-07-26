#ifndef TK_DHT2X_h
#define TK_DHT2X_h

#include "InputAdapter.h"
#include "Adafruit_I2CDevice.h"

typedef struct
{
    int temperature;
    int humidity;
}dht2x_data;

//DHT2x温湿度计
class TK_Dht2x {
public:
    TK_Dht2x(Adafruit_I2CDevice*i2c_device);
    TK_Dht2x();
    ~TK_Dht2x();
    bool init();
    bool start();       //开始测量
    bool isFinish();    //查看测量是否完成
    //bool getTempHum(float *Temp,float *Hum);
    bool dht2xRead(dht2x_data*res); //读取测量值
private:
    bool _dhtSendAc();//向dht2x发送AC命令
    uint8_t _dhtGetStatus();    //获取dht2x状态
    bool _dhtReadResult(uint8_t dat[6]);     //读取原始数据
    bool _dht2xInit();
    bool dht2xIsInit;
    bool measureIsRead;     //上次测量结果已经被读取标志
    Adafruit_I2CDevice *i2cdevice;
    bool isSelfObj;
};

#endif // TK_DHT2X_h

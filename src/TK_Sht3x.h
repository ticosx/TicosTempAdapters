#ifndef Tk_Sht3x_h
#define Tk_Sht3x_h

#include <Arduino.h>
#include "Adafruit_I2CDevice.h"

//SHT3x温湿度计
class TK_Sht3x {
public:
    TK_Sht3x();
    TK_Sht3x(TwoWire *towWire,uint8_t addr = 0x44);
    TK_Sht3x(Adafruit_I2CDevice *i2cDevice);
    ~TK_Sht3x();
    bool init();
    bool start();       //调用后需要等待20ms再去获取数据
    bool getTempHum(float *Temp,float *Hum);
protected:
    bool isSelfObj;     //是否是自己实例化的Adafruit_I2CDevice
    Adafruit_I2CDevice *i2cDev;
};

#endif  //Tk_Sht3x_h 


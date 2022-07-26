#include "TK_Sht3x.h"

TK_Sht3x::TK_Sht3x(){ 
    i2cDev = new Adafruit_I2CDevice(0x44);
    isSelfObj = true;
}

TK_Sht3x::TK_Sht3x(TwoWire *towWire,uint8_t addr){
    i2cDev = new Adafruit_I2CDevice(0x44,towWire);
    isSelfObj = true;
}

TK_Sht3x::TK_Sht3x(Adafruit_I2CDevice *i2cDevice){
    i2cDev = i2cDevice;
    isSelfObj = false;
}

TK_Sht3x::~TK_Sht3x(){
    if(isSelfObj){
        delete i2cDev;
    }
}

bool TK_Sht3x::init(){
    return  i2cDev->begin();
}

bool TK_Sht3x::start(){
    uint8_t buf[2] = {0x24,0x00};
    return  i2cDev->write(buf,2);
}

bool TK_Sht3x::getTempHum(float *Temp,float *Hum){
    uint8_t readbuffer[6];
    i2cDev->read(readbuffer,sizeof(readbuffer));

    int32_t stemp = (int32_t)(((uint32_t)readbuffer[0] << 8) | readbuffer[1]);
    stemp = ((4375 * stemp) >> 14) - 4500;
    *Temp = (float)stemp / 100.0f;

    uint32_t shum = ((uint32_t)readbuffer[3] << 8) | readbuffer[4];
    shum = (625 * shum) >> 12;
    *Hum = (float)shum / 100.0f;
    return true;
}

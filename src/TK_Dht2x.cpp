#include "TK_Dht2x.h"
#include "Log.h"

TK_Dht2x::TK_Dht2x(Adafruit_I2CDevice *i2c_device) {
    this->i2cdevice = i2c_device;
    dht2xIsInit = false;
    isSelfObj = false;
}

TK_Dht2x::TK_Dht2x() {
    //配置默认地址 0x38 -> DHT20
    //默认使用Arduino i2c wire总线
    this->i2cdevice = new Adafruit_I2CDevice(0x38);
    dht2xIsInit = false;
    isSelfObj = true;
}

TK_Dht2x::~TK_Dht2x(){
    if(isSelfObj){
        delete i2cdevice;
    }
}

bool TK_Dht2x::start(){
    return _dhtSendAc();//向AHT10发送AC命令
}

bool TK_Dht2x::isFinish(){
    if( (_dhtGetStatus()&0x80) ==0x80 ){
        return false;
    }
    return true;
}

bool TK_Dht2x::init(){
    if(!dht2xIsInit)
        dht2xIsInit = _dht2xInit();
    return dht2xIsInit;
}

bool TK_Dht2x::_dht2xInit(){
    bool ret;
    uint8_t init1_buf[] = {0xa8, 0x00, 0x00};
    uint8_t init2_buf[] = {0xbe, 0x08, 0x00};

    if( !(i2cdevice->begin()) )
        return false;
    ret = i2cdevice->write(init1_buf,3);
    //ret = i2c_master_write_to_device(I2C_MASTER_NUM, DHT20_ADDR, init1_buf, sizeof(init1_buf), I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    if(!ret) return ret;
    delay(50);

    ret = i2cdevice->write(init2_buf,3);
    //ret = i2c_master_write_to_device(I2C_MASTER_NUM, DHT20_ADDR, init2_buf, sizeof(init2_buf), I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);

    delay(20);
    return ret;
}

bool TK_Dht2x::dht2xRead(dht2x_data *res)
{
    int cnt = 0;
    uint32_t RetuData = 0;
    while(((_dhtGetStatus()&0x80)==0x80))//直到状态bit[7]为0，表示为空闲状态，若为1，表示忙状态
    {
        delay(10);
        logDebug("dht2x jam");  //dht2x发生堵塞!
        if(cnt++>=100)
        {
            break;
        }
        return false;
    }

    uint8_t dat[6];
    _dhtReadResult(dat);

    RetuData = (RetuData|dat[1])<<8;
    RetuData = (RetuData|dat[2])<<8;
    RetuData = (RetuData|dat[3]);
    RetuData = RetuData >>4;
    res->humidity = RetuData*100*10/1024/1024; //湿度

    RetuData = 0;
    RetuData = (RetuData|dat[3])<<8;
    RetuData = (RetuData|dat[4])<<8;
    RetuData = (RetuData|dat[5]);
    RetuData = RetuData&0xfffff;
    res->temperature = RetuData*200*10/1024/1024-500; //温度
    return true;
}

bool TK_Dht2x::_dhtSendAc(){
    int ret;
    uint8_t ac_cmd[] = {0xac, 0x33, 0x00};
    ret = i2cdevice->write(ac_cmd,3);
    //ret = i2c_master_write_to_device(I2C_MASTER_NUM, DHT20_ADDR, ac_cmd, sizeof(ac_cmd), I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    return ret;
}

uint8_t TK_Dht2x::_dhtGetStatus(){
    uint8_t status[1];
    i2cdevice->read(status,1);
    //int ret = i2c_master_read_from_device(I2C_MASTER_NUM, DHT20_ADDR, status, sizeof(status), I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
    return status[0];
}

bool TK_Dht2x::_dhtReadResult(uint8_t dat[6]){
    bool ret = i2cdevice->read( dat, 6);
    return ret;
}

#ifndef TEMPNTC_h
#define TEMPNTC_h

#include <Arduino.h>
#include "TemperatureAdapter.h"
//ADC头文件
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/gpio.h"

#define TEMP_TABLE_LEN   121 //温度区间总值
const uint16_t c_au16TempTable[TEMP_TABLE_LEN]=
{
    1778,1733,1689,1645,1601,1558,1515,1472,1430,1389,   /*-10° ~ -1°*/
    1348,1308,1269,1230,1192,1154,1118,1082,1047,1013,   /*0° ~ 9°*/
    980,947,916,885,855,826,798,770,744,718,             /*10° ~ 19°*/
    693,668,645,622,600,579,558,539,519,501,             /*20° ~ 29°*/
    483,466,449,433,417,403,388,374,361,348,             /*30° ~ 39°*/
    336,324,312,301,290,280,270,261,251,243,            /*40° ~ 49°*/
    234,226,218,211,203,196,190,183,177,171,             /*50° ~ 59°*/
    165,159,154,149,144,139,135,130,126,122,             /*60° ~ 69°*/
    118,114,110,107,103,100,97,94,91,88,                 /*70° ~ 79°*/
    85,82,80,77,75,73,70,68,66,64,                      /*80° ~ 89°*/
    62,60,58,57,55,53,52,50,49,47,                      /*90° ~ 99°*/
    46,45,43,42,41,40,                                 /*100° ~ 105°*/
};
/*!
 * @brief The class of ntc
 */
class TkTempNtc: public TemperatureAdapter{
    public:
        TkTempNtc(temp_info_t *info);
        /*!
        *    @brief  获取温度值
        *    @return 初始化成功则返回 温度值，单位是°C
        */
        virtual  int16_t getTempValue(void)  override;
        /*!
        *    @brief  获取NTC电压值
        *    @return 初始化成功则返回电压值，单位是mV
        */
        virtual  uint32_t getVotValue(void)  override; 
            /*!
        *    @brief  获取Adc原始值，12bit
        *    @return 初始化成功则返回原始ADC值
        */
        virtual  uint32_t getAdcValue(void) override; 
        /*!
        *    @brief  初始化设备
        *    @return 初始化成功则返回 true
        */
        virtual bool init() override ;
        /*!
        *    @brief  取消初始化设备
        *    @return 取消成功返回 true
        */
        virtual bool deinit() override;
    private:
        bool begin();
        bool end() ;
        gpio_num_t  ADC_PIN;
        adc1_channel_t ADC_CHN = ADC1_CHANNEL_3;
        uint16_t ADC_VEF = 3300;
        uint32_t tw_u32AdcNtcQueue[10];

};

#endif
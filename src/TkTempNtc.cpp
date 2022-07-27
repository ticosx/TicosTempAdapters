#include "TkTempNtc.h"
#include "Log.h"
//ADC头文件
#include "driver/adc.h"
#include "esp_adc_cal.h"
// ADC斜率曲线
static esp_adc_cal_characteristics_t  temp_adc_chars;

TkTempNtc::TkTempNtc(temp_info_t *info) : TemperatureAdapter(info) {

}

bool TkTempNtc::begin(){

    esp_err_t err = ESP_OK;
    err = adc1_config_width(ADC_WIDTH_BIT_12);// 12位分辨率
    if (err) {
        logInfo("adc1_config_width failed: %d", err);
        return false;
    }
    err = adc1_config_channel_atten(chn, ADC_ATTEN_DB_11);// 电压输入衰减
    if (err) {
        logInfo("adc1_config_channel_atten failed: %d", err);
        return false;
    }
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, vef, &temp_adc_chars);
    
    logInfo("esp_adc_cal_characterize: %d", val_type);

    return true;
}

bool TkTempNtc::end(){
    gpio_reset_pin(adcio);
    return true;
}

bool TkTempNtc::init() {

    chn = (adc1_channel_t)info->adc_chn;
    adcio = (gpio_num_t)info->adc_pin ;
    vef = info->adc_vref;
    begin();
    logInfo("TkTempNtc_init ,ADC_CHN = %d,ADC_VEF= %d\n",chn,vef);

    return true;
}
bool TkTempNtc::deinit() {
    end();
    logInfo("ntc deinit \n");
    return true;
}


/*****************************************************************************
*函数名  : getVotValue
*函数功能 :获取ADC转换电压值
*输入参数 : 无
*输出参数 : 无
*返回值   : vol_average电压值
*****************************************************************************/
uint32_t TkTempNtc::getVotValue(void)
{
    uint8_t i =0;
    uint32_t ret =0;

    uint32_t adc_max = 0;
    uint32_t adc_min = 0;
    uint32_t adc_sum = 0;
    uint32_t adc_average = 0;
    uint32_t vol_average = 0; //电压值
    //TEMP ADC CHECK
   // 
    for(i=0;i<ADC_NTC_QUEUE_SIZE;i++)//一次读取10个
    {
        tw_u32AdcNtcQueue[i]= adc1_get_raw(chn);// 采集ADC原始值
    }  

    adc_max = tw_u32AdcNtcQueue[0];
    adc_min = tw_u32AdcNtcQueue[0];

    for(i=0; i<ADC_NTC_QUEUE_SIZE; i++)
    {
        if(adc_max < tw_u32AdcNtcQueue[i])
            adc_max = tw_u32AdcNtcQueue[i];
        if(adc_min > tw_u32AdcNtcQueue[i])
            adc_min = tw_u32AdcNtcQueue[i];
        adc_sum +=tw_u32AdcNtcQueue[i];
    }
    adc_sum = adc_sum-adc_max-adc_min;

    /*平均AD值*/
    adc_sum +=(ADC_NTC_QUEUE_SIZE-2)/2;
    adc_average = adc_sum/(ADC_NTC_QUEUE_SIZE-2);
    //  Ref	= 3.3V
    vol_average =  esp_adc_cal_raw_to_voltage(adc_average, &temp_adc_chars);//根据电阻分压计算电压

    return	vol_average;
}

/*****************************************************************************
*函数名  : getAdcValue
*函数功能 :获取ADC平均值
*输入参数 : 无
*输出参数 : 
*返回值   : adc_average  ADC平均值
*****************************************************************************/
uint32_t TkTempNtc::getAdcValue(void)
{
    uint8_t i =0;
    uint32_t ret =0;

    uint32_t adc_max = 0;
    uint32_t adc_min = 0;
    uint32_t adc_sum = 0;
    uint32_t adc_average = 0;
    uint32_t vol_average = 0; //电压值
    //TEMP ADC CHECK
   // 
    for(i=0;i<ADC_NTC_QUEUE_SIZE;i++)//一次读取10个
    {
        tw_u32AdcNtcQueue[i]= adc1_get_raw(chn);// 采集ADC原始值
    }  

    adc_max = tw_u32AdcNtcQueue[0];
    adc_min = tw_u32AdcNtcQueue[0];

    for(i=0; i<ADC_NTC_QUEUE_SIZE; i++)
    {
        if(adc_max < tw_u32AdcNtcQueue[i])
            adc_max = tw_u32AdcNtcQueue[i];
        if(adc_min > tw_u32AdcNtcQueue[i])
            adc_min = tw_u32AdcNtcQueue[i];
        adc_sum +=tw_u32AdcNtcQueue[i];
    }
    adc_sum = adc_sum-adc_max-adc_min;

    /*平均AD值*/
    adc_sum +=(ADC_NTC_QUEUE_SIZE-2)/2;
    adc_average = adc_sum/(ADC_NTC_QUEUE_SIZE-2);

    return	adc_average;
}
/*****************************************************************************
*函数名  : getTempValue
*函数功能 :获取温度值
*输入参数 : 无
*输出参数 : 无
*返回值   : i16temp  温度值
*****************************************************************************/
int16_t TkTempNtc::getTempValue(void)
{
    uint32_t Vot_value = 0;
    uint16_t u16i = 0;
    int16_t i16temp = 0;

    Vot_value = getVotValue();
    for(u16i=0;u16i<TEMP_TABLE_LEN;u16i++)//查表
    {
        if(Vot_value >= c_au16TempTable[u16i])
        {
           break;
        }
    }
    i16temp = u16i - 10;//温度对应ADC电压表是从-10℃起

    return i16temp ;
}
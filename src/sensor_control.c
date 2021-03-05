/*
 * @Author: zgw
 * @email: liang.zhang@tuya.com
 * @LastEditors: zgw
 * @file name: sensor_control.c
 * @Description: 
 * @Copyright: HANGZHOU TUYA INFORMATION TECHNOLOGY CO.,LTD
 * @Company: http://www.tuya.com
 * @Date: 2021-1-5 16:21:15
 * @LastEditTime: 2021-1-5 16:21:15
 */

#include "sensor_control.h"
#include "tuya_gpio.h"
#include "soc_adc.h"
/***********************************************************
*************************types define***********************
***********************************************************/
typedef enum
{
    LOW = 0,
    HIGH,
}default_level;



DEVICE_DATA_T device_data = {0};

APP_REPORT_DATA_T app_report_data = {0};

#define MAX_DEPTH                        (40)  // unit: mm

/***********************************************************
*************************IO control device define***********
***********************************************************/
#define IIC_SDA_PORT                     (6)
#define IIC_SCL_PORT                     (7)



/***********************************************************
*************************about adc init*********************
***********************************************************/
#define TEMP_ADC_DATA_LEN           (4)

tuya_adc_dev_t tuya_adc;

/***********************************************************
*************************about iic init*********************
***********************************************************/



/***********************************************************
*************************function***************************
***********************************************************/

STATIC VOID __ctrl_gpio_init(CONST TY_GPIO_PORT_E port, CONST BOOL_T high)
{
    tuya_gpio_inout_set(port, FALSE);
    tuya_gpio_write(port, high);
}

VOID app_device_init(VOID)
{
    INT_T op_ret = 0;
    
    // adc driver init
    tuya_adc.priv.pData = Malloc(TEMP_ADC_DATA_LEN * sizeof(USHORT_T));
    memset(tuya_adc.priv.pData, 0, TEMP_ADC_DATA_LEN*sizeof(USHORT_T));
    tuya_adc.priv.data_buff_size = TEMP_ADC_DATA_LEN; //设置数据缓存个数

}



OPERATE_RET app_get_all_sensor_data(VOID)
{
    OPERATE_RET ret = 0;

    tuya_hal_adc_init(&tuya_adc);
    tuya_hal_adc_value_get(TEMP_ADC_DATA_LEN, &device_data.liquid_value);
    PR_NOTICE("liquid_value = %d",device_data.liquid_value);
    tuya_hal_adc_finalize(&tuya_adc);

    
    return ret;
}



VOID app_ctrl_handle(VOID)
{   
    PR_DEBUG("ctrl handle");

    float value = 0;
    UINT16_T depth = 0;
    value = device_data.liquid_value;

    if(value < 10) {
        app_report_data.liquid_depth = 0;
    }else if(value < 1000) {
        app_report_data.liquid_depth = 1;
    }else if(value > 3000) {
        app_report_data.liquid_depth = MAX_DEPTH;
    }else {
        app_report_data.liquid_depth = (UINT16_T)(((value - 1000.0)/2000.0)*MAX_DEPTH);
    }
    PR_NOTICE("---------------liquid_depth = %d-----------",app_report_data.liquid_depth);
    
    if(app_report_data.liquid_depth > device_data.liquid_depth_old) {
        app_report_data.liquid_state = upper_alarm;
    }else if(app_report_data.liquid_depth < device_data.liquid_depth_old) {
        app_report_data.liquid_state = lower_alarm;
    }else {
        app_report_data.liquid_state = normal;
    }
    device_data.liquid_depth_old = app_report_data.liquid_depth;
    PR_NOTICE("liquid_state = %d",app_report_data.liquid_state);
}

VOID app_ctrl_all_off(VOID)
{   
    ;
}
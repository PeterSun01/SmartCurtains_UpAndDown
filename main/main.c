#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"


#include "Wind.h"
#include "Wallkey.h"
#include "Led.h"
#include "E2prom.h"
#include "Localcalculation.h"
#include "RtcUsr.h"
#include "Fire.h"
#include "Motorctl.h"


static int8_t Motor_Height;//用于本地计算得到高角值，并控制百叶帘
static int8_t Motor_Angle;//用于本地计算得到高角值，并控制百叶帘



//static uint8_t WallKeyId[4]={0x24,0x31,0x04,0x00};  //定义KEY ID 后续在eeprom中读取，蓝牙发送
static uint8_t WallKeyId[4]={0x4b,0x8d,0x05,0x00};
static int8_t Switch=0;//定义开关左右方向

void timer_periodic_cb(void *arg);//定时器函数声明
esp_timer_handle_t timer_periodic_handle = 0;//定义重复定时器句柄


//定义一个重复运行的定时器结构体
esp_timer_create_args_t timer_periodic_arg = { .callback =
        &timer_periodic_cb, //设置回调函数
        .arg = NULL, //不携带参数
        .name = "PeriodicTimer" //定时器名字
        };


void timer_periodic_cb(void *arg) //1ms中断一次
{
    static int64_t timer_count=0;
    float Wind_speed=0;
    int year,month,day,hour,min,sec; 

    static int M1;
    static int M2;

    timer_count++;
    if(timer_count>=1000)//1s
    {
        timer_count=0;
    }
    
}

static void Wallkey_Read_Task(void* arg)
{
    while(1)
    {
        Wallkey_App(WallKeyId,Switch);
    }  
}



static void Motor_Task(void* arg)
{
    while(1)
    {
      Motor_Ctl_App(); 
    }
}

void app_main()
{
   	uint8_t data_write[100]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	uint8_t data_read[256]="0";

 
    Led_Init();
    Motor_Init();
    Wind_Init();
    Wallkey_Init();
    Led_On();
    E2prom_Init();
    Fire_Init();
    vTaskDelay(20 / portTICK_RATE_MS);



       
    /*if(Motor_SetAllDown()==MOTOROK)
    {
        printf("set down over main\n");
        Motor_Height=100;
        Motor_Angle=80;
    }*/

    //Motor_AutoCtl(0,0);
    //Motor_HandCtl_Height(ADD);

    Rtc_Set(2018,12,4,15,21,55);

    /*******************************timer 1s init**********************************************/  
    esp_err_t err = esp_timer_create(&timer_periodic_arg, &timer_periodic_handle);
    err = esp_timer_start_periodic(timer_periodic_handle, 1000);//创建定时器，单位us 定时1ms
    if(err != ESP_OK)
    {
        printf("timer periodic create err code:%d\n", err);
    }
    xTaskCreate(&Wallkey_Read_Task, "Wallkey_Read_Task", 2048, NULL, 10, NULL);


    /*ret= Motor_AutoCtl(100,0);
    printf("ret1=%d\n", ret);
    ret= Motor_AutoCtl(0,0);
    printf("ret2=%d\n", ret);
    ret= Motor_AutoCtl(0,0);
    printf("ret3=%d\n", ret);*/

}



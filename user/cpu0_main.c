#include "zf_common_headfile.h"
#pragma section all "cpu0_dsram"
// ���������#pragma section all restore���֮���ȫ�ֱ���������CPU0��RAM��


// **************************** �������� ****************************
int core0_main(void)
{
    clock_init();                   // ��ȡʱ��Ƶ��<��ر���>
    debug_init();                   // ��ʼ��Ĭ�ϵ��Դ���
    interrupt_global_disable();
    system_delay_init();

    /*************Ӳ��ʼ��**************/
    Motor_Init();
    Servo_Init();
    Encoder_Init();
    key_init(10);
    Led_Init();
    Buzz_Init();
    oled_init();
    wireless_uart_init();
    disable_Watchdog();
    /*************Ӳ����ʼ��**************/

    /*************������ʼ��**************/
    Init_usbStr();
    Pid_Init(&CurveDirectionPID);
    Pid_Init(&SpeedPID);
    FF_Init(&SpeedFF,38,0,0.2);
    PidValue();
    Kalman_Init(&CarSpeedAfterFilter);
    Kalman_Init(&GyroYAfterFilter);
    Kalman_Init(&GyroZAfterFilter);
    Menu_Init();
    RescueInit();
    /*************������ʼ��**************/

    interrupt_global_enable(0);
    pit_ms_init(CCU60_CH0, 5);
    pit_start(CCU60_CH0);
    pit_ms_init(CCU60_CH1, 5);
    pit_start(CCU60_CH1);
    adc_init (ADC0_CH0_A0, ADC_10BIT);
    cpu_wait_event_ready();         // �ȴ����к��ĳ�ʼ�����

    /*************��������**************/
    Buzz_Ctrl(BUZZ0, ON);
    oled_set_font(1);
    oled_show_string(0,3,"Sir,I'm ready");
    system_delay_ms(700);
    Buzz_Ctrl(BUZZ0, OFF);
    oled_clear();
    oled_set_font(0);
    /*************��������**************/

    cpu_wait_event_ready();         // �ȴ����к��ĳ�ʼ�����
    while (TRUE)
    {
        MainFunc();
        //CarState_Show();
    }
}

#pragma section all restore
// **************************** �������� ****************************

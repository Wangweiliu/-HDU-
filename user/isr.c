#include "isr_config.h"
#include "isr.h"

#define _e 2.71828
int temp = 0;

float f(float x) {
    float exponential_numerator = exp(-fabs(x / 16.0)) - 1.0;
    float exponential_denominator = exp(-fabs(x)) + 1.0;
    float result = (fabs((exponential_numerator / exponential_denominator)) / 2.0 + 0.4) * 118.0;

    return result;
}

// ����TCϵ��Ĭ���ǲ�֧���ж�Ƕ�׵ģ�ϣ��֧���ж�Ƕ����Ҫ���ж���ʹ�� interrupt_global_enable(0); �������ж�Ƕ��
// �򵥵�˵ʵ���Ͻ����жϺ�TCϵ�е�Ӳ���Զ������� interrupt_global_disable(); ���ܾ���Ӧ�κε��жϣ������Ҫ�����Լ��ֶ����� interrupt_global_enable(0); �������жϵ���Ӧ��

// **************************** PIT�жϺ��� ****************************
IFX_INTERRUPT(cc60_pit_ch0_isr, 0, CCU6_0_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU60_CH0);

    EncoderValue_Get();
    if(MotorFlag != 0)
    {
        Cal_RoadLength(0.005);

        switch(TrackState)
        {
            /***************************��ͳ����*****************************/
            case BaseHandle:
                flyFrame ++;
                _Speed = SpeedOfBaseHandle;
            break;

            case BaseHandle_speedup:
                flyFrame += 2;
                _Speed = SpeedOfBaseHandle_speedup;
            break;

            case BaseHandle_down:
                if (SpeedOfBaseHandle > 85)
                {
                    if (flyFrame > 10)
                    {
                        _Speed = SpeedOfBaseHandle_wan - flyFrame * SLp;
                    }
                }
                break;
            case BaseHandle_wan:
                flyFrame = 0;
                _Speed = SpeedOfBaseHandle_wan - func_abs(TrueDirError) * KLp;
                if (_Speed < Speed_min) { _Speed = Speed_min; }
                break;

            case BigRCurve:
                _Speed = SpeedOfBigRCurve;
            break;

            case RuHuan_Small:
                _Speed = SpeedOfRuHuan_Small;
                CurveDirectionPID.kp = KpofRuHuan_Small;
                CurveDirectionPID.kd = KpofRuHuan_Small;
            break;

            case HuanZhong_Small:
                _Speed = SpeedOfHuanZhong_Small;
                CurveDirectionPID.kp = KpofHuanZhong_Small;
                CurveDirectionPID.kd = KpofHuanZhong_Small;
            break;

            case ChuHuan_Small:
                _Speed = SpeedOfChuHuan_Small;
                CurveDirectionPID.kp = KpofChuHuan_Small;
                CurveDirectionPID.kd = KpofChuHuan_Small;
            break;

            case RuHuan_Big:
                _Speed = SpeedOfRuHuan_Big;
                CurveDirectionPID.kp = KpofRuHuan_Big;
                CurveDirectionPID.kd = KpofRuHuan_Big;
            break;

            case HuanZhong_Big:
                _Speed = SpeedOfHuanZhong_Big;
                CurveDirectionPID.kp = KpofHuanZhong_Big;
                CurveDirectionPID.kd = KpofHuanZhong_Big;
            break;

            case ChuHuan_Big:
                _Speed = SpeedOfChuHuan_Big;
                CurveDirectionPID.kp = KpofChuHuan_Big;
                CurveDirectionPID.kd = KpofChuHuan_Big;
            break;

            case Ramp:
                _Speed = SpeedOfRamp;
            break;

            case Ramp_fly:
                _Speed = SpeedOfRamp_fly;
            break;

            case Stop:
                _Speed = 0;
            break;

            case ZebraCrossing:
                ZebraCrossing_Process();
            break;
            /***************************��ͳ����*****************************/


            /***************************ģ�Ͳ���*****************************/
            case RescueScene:
                _Speed = SpeedOfSlowDown_Big;
            break;

            case DangerScene:
                _Speed = SpeedOfDanger;
                CurveDirectionPID.kp = KpofBlocks;
                CurveDirectionPID.kd = KdofBlocks;
                SpeedPID.kp = SpeedKpofBlocks;
                SpeedPID.ki = SpeedKiofBlocks;
            break;

            case SlowDown:
                _Speed = SpeedOfSlowDown;
            break;

            case SlowDown_big:
                _Speed = SpeedOfSlowDown_Big;
            break;
            /***************************ģ�Ͳ���*****************************/

            case -1:
                _Speed = 0;
            break;

            default:
            break;
        }

        if(!If_ReceivedInfo)//δ���ܵ���λ����Ϣ
        {
            _Speed = 0;
        }

        ElemBuzz();
        ZebraControl();

        if( TrackState != DangerScene      && TrackState != RuHuan_Small   &&
            TrackState != HuanZhong_Small  && TrackState != ChuHuan_Small  &&
            TrackState != ChuHuan_Big      && TrackState != RuHuan_Big     &&
            TrackState != HuanZhong_Big                                        )
        {
            if(IfOpenFuzzy == OPEN)
            {
                CurveDirectionPID.kp = KP_Fuzzy((float)TrueDirError,(float)(TrueDirError - LastTrueDirError));
                CurveDirectionPID.kd = Kd_Fuzzy((float)(TrueDirError - LastTrueDirError));
            }
            else
            {
                CurveDirectionPID.kp = SteerKp;
                CurveDirectionPID.kd = SteerKd;
            }
        }
        if(ZebraBeforeStopping == 0 && TrackState != DangerScene)
        {
            SpeedPID.kp = KpofCommon;
            SpeedPID.ki = KiofCommon;
            SpeedPID.kd = KdofCommon;
            Pid_update(&SpeedPID ,(float)( _Speed - CarSpeed ));
        }


        if(IfOpenFF == OPEN){ FF_Calculate(&SpeedFF,(float)_Speed );PWMOut += SpeedFF.Out; }

        if(TrackState != RescueHandle_L && TrackState != RescueHandle_R && TrackState != Ramp)
            _ServoDutyfactor=ServoMid - 2*(Pid_LocCtrl(&CurveDirectionPID, (float)TrueDirError));
        Servo_Constrain(&_ServoDutyfactor);
        Set_Angel((uint16)_ServoDutyfactor);
        PWMOut = (int)Pid_IncCtrl(&SpeedPID, (float)( _Speed - CarSpeed ));

        if(MotorPower == 1)
            Set_MoterSpeed(PWMOut);
        else
            Set_MoterSpeed(0);
    }
    else
    {
        if(TestMotor_Flag ==0 )
        {
            Set_MoterSpeed(0);
        }
        else if(TestMotor_Flag == 1)
        {
            Set_MoterSpeed(PWMOut);
        }
    }

}


IFX_INTERRUPT(cc60_pit_ch1_isr, 0, CCU6_0_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU60_CH1);

    if(rescuehandle_lrmain.RescueHandle_LR_FLAG == 1)
    {
        rescuehandle_lrmain.S_sec ++;
    }

    Init_If_ReceivedInfo ++;
    if(Init_If_ReceivedInfo > 10)
    {
        Init_If_ReceivedInfo = 0;
        If_ReceivedInfo = 0;
    }

}

IFX_INTERRUPT(cc61_pit_ch0_isr, 0, CCU6_1_CH0_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU61_CH0);




}

IFX_INTERRUPT(cc61_pit_ch1_isr, 0, CCU6_1_CH1_ISR_PRIORITY)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    pit_clear_flag(CCU61_CH1);




}
// **************************** PIT�жϺ��� ****************************


// **************************** �ⲿ�жϺ��� ****************************
IFX_INTERRUPT(exti_ch0_ch4_isr, 0, EXTI_CH0_CH4_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    if(exti_flag_get(ERU_CH0_REQ0_P15_4))           // ͨ��0�ж�
    {
        exti_flag_clear(ERU_CH0_REQ0_P15_4);

    }

    if(exti_flag_get(ERU_CH4_REQ13_P15_5))          // ͨ��4�ж�
    {
        exti_flag_clear(ERU_CH4_REQ13_P15_5);




    }
}

IFX_INTERRUPT(exti_ch1_ch5_isr, 0, EXTI_CH1_CH5_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��

    if(exti_flag_get(ERU_CH1_REQ10_P14_3))          // ͨ��1�ж�
    {
        exti_flag_clear(ERU_CH1_REQ10_P14_3);

        tof_module_exti_handler();                  // ToF ģ�� INT �����ж�

    }

    if(exti_flag_get(ERU_CH5_REQ1_P15_8))           // ͨ��5�ж�
    {
        exti_flag_clear(ERU_CH5_REQ1_P15_8);


    }
}

// ��������ͷpclk����Ĭ��ռ���� 2ͨ�������ڴ���DMA��������ﲻ�ٶ����жϺ���
// IFX_INTERRUPT(exti_ch2_ch6_isr, 0, EXTI_CH2_CH6_INT_PRIO)
// {
//  interrupt_global_enable(0);                     // �����ж�Ƕ��
//  if(exti_flag_get(ERU_CH2_REQ7_P00_4))           // ͨ��2�ж�
//  {
//      exti_flag_clear(ERU_CH2_REQ7_P00_4);
//  }
//  if(exti_flag_get(ERU_CH6_REQ9_P20_0))           // ͨ��6�ж�
//  {
//      exti_flag_clear(ERU_CH6_REQ9_P20_0);
//  }
// }

IFX_INTERRUPT(exti_ch3_ch7_isr, 0, EXTI_CH3_CH7_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    if(exti_flag_get(ERU_CH3_REQ6_P02_0))           // ͨ��3�ж�
    {
        exti_flag_clear(ERU_CH3_REQ6_P02_0);
        camera_vsync_handler();                     // ����ͷ�����ɼ�ͳһ�ص�����
    }
    if(exti_flag_get(ERU_CH7_REQ16_P15_1))          // ͨ��7�ж�
    {
        exti_flag_clear(ERU_CH7_REQ16_P15_1);




    }
}
// **************************** �ⲿ�жϺ��� ****************************


// **************************** DMA�жϺ��� ****************************
IFX_INTERRUPT(dma_ch5_isr, 0, DMA_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    camera_dma_handler();                           // ����ͷ�ɼ����ͳһ�ص�����
}
// **************************** DMA�жϺ��� ****************************


// **************************** �����жϺ��� ****************************
// ����0Ĭ����Ϊ���Դ���
IFX_INTERRUPT(uart0_tx_isr, 0, UART0_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��



}
IFX_INTERRUPT(uart0_rx_isr, 0, UART0_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��

    If_ReceivedInfo = uart_query_byte(UART_0,&DataFromEdgeBoard);
    uart_receiveVerify(DataFromEdgeBoard);
    Receive_Frame ++;
    if(Receive_Frame >= 65534)
    {
        Receive_Frame = 0;
    }
    Init_If_ReceivedInfo = 0;
//    if(!If_ReceivedInfo) {
//        If_ReceivedInfo_Frame ++;
//    }
//    else{
//        If_ReceivedInfo_Frame = 0;
//    }
//
//    if(If_ReceivedInfo_Frame >= 50) {
//        If_ReceivedInfo_Frame = 50;
//    }

#if DEBUG_UART_USE_INTERRUPT                        // ������� debug �����ж�
        debug_interrupr_handler();                  // ���� debug ���ڽ��մ����� ���ݻᱻ debug ���λ�������ȡ
#endif                                              // ����޸��� DEBUG_UART_INDEX ����δ�����Ҫ�ŵ���Ӧ�Ĵ����ж�ȥ
}


// ����1Ĭ�����ӵ�����ͷ���ô���
IFX_INTERRUPT(uart1_tx_isr, 0, UART1_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��




}
IFX_INTERRUPT(uart1_rx_isr, 0, UART1_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    camera_uart_handler();                          // ����ͷ��������ͳһ�ص�����
}

// ����2Ĭ�����ӵ�����ת����ģ��
IFX_INTERRUPT(uart2_tx_isr, 0, UART2_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��



}

IFX_INTERRUPT(uart2_rx_isr, 0, UART2_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    wireless_module_uart_handler();                 // ����ģ��ͳһ�ص�����



}
// ����3Ĭ�����ӵ�GPS��λģ��
IFX_INTERRUPT(uart3_tx_isr, 0, UART3_TX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��



}

IFX_INTERRUPT(uart3_rx_isr, 0, UART3_RX_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    gnss_uart_callback();                           // GNSS���ڻص�����



}

// ����ͨѶ�����ж�
IFX_INTERRUPT(uart0_er_isr, 0, UART0_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart0_handle);
}
IFX_INTERRUPT(uart1_er_isr, 0, UART1_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart1_handle);
}
IFX_INTERRUPT(uart2_er_isr, 0, UART2_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart2_handle);
}
IFX_INTERRUPT(uart3_er_isr, 0, UART3_ER_INT_PRIO)
{
    interrupt_global_enable(0);                     // �����ж�Ƕ��
    IfxAsclin_Asc_isrError(&uart3_handle);
}

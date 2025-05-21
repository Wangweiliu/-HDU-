#ifndef CODE_COMMUNICATION_H_
#define CODE_COMMUNICATION_H_

#include "zf_common_headfile.h"

#define USB_FRAME_HEAD              0x42          //USBͨ������֡ͷ
#define USB_FRAME_END               0x10          //USBͨ������֡β
#define USB_FRAME_LENGTH            8             //USBͨ�������ֽڳ���

#define USB_ADDR_SPEEDBACK          0x05  //�ٶ�
#define USB_ADDR_RESETBACK          0x02  //��ʼ��
#define USB_ADDR_RUNBACK            0x08  //��������������

#define USB_ADDR_RESCUEBACK         0x06  //��Ԯ��
#define USB_ADDR_Elem               0x09  //Ԫ�ر�
#define USB_ADDR_ANCHOR             0x10  //ǰհ

extern uint8 IfReceived;
typedef struct
{
    bool ReceiveStart;                            //���ݽ��տ�ʼ
    uint8 ReceiveIndex;                           //��������
    bool ReceiveFinished;                         //���ݶ��н��ղ�У�����
    uint8 ReceiveBuff[USB_FRAME_LENGTH];          //USB���ն��У���ʱ����
    uint8 ReceiveBuffFinished[USB_FRAME_LENGTH];  //USB���ն��У�У��ɹ�
}USB_STRUCT;
extern USB_STRUCT usbStr;

typedef union
{
    uint8 U8_Buff[2];
    int16 SignedShort;
}UNION_BIT16;

extern uint8 Reset_flag;
extern uint8 Rescue_stopFinish_flag;
extern uint8 Run_flag;
extern uint8 If_ReceivedInfo;
extern uint8 If_ReceivedInfo_Frame;
extern uint16 Receive_Frame;
extern uint16 Init_If_ReceivedInfo;

void uart_receiveVerify(uint8 data);
void Init_usbStr(void);
void USB_Edgeboard_Rescue(void);
void USB_Edgeboard_Reset(void);
void USB_Edgeboard_Run(void);
void USB_Edgeboard_Elem(void);
void USB_Edgeboard_Anchor(void);

#endif

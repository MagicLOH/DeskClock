#include "sys.h"

/*
****************************************************************************************************************
    
    ����ԭ�ͣ�void CM3_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
    �������ܣ������ж����ȼ� [misc.h][core_cm3.h]
    �����βΣ�IRQn_Type IRQn, uint32_t PreemptPriority ��ռ���ȼ�, uint32_t SubPriority �����ȼ�
    Ӳ�����ӣ���
    ����ֵ����
    
****************************************************************************************************************  
*/
void CM3_NVIC_SetPriority(IRQn_Type IRQn,uint32_t PreemptPriority, uint32_t SubPriority)
{
    //step 1: set priority group
    NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);//[core_cm3_h,line:1469]
    //step 2:return priority result for [NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)]
    u32 _priority = NVIC_EncodePriority (NVIC_PriorityGroup_2,PreemptPriority,SubPriority);//[core_cm3_h,line:1648]
    //step 3: set priority
    NVIC_SetPriority(IRQn,_priority);//[core_cm3_h,line:1600]
    //step 4:enable interupt
    NVIC_EnableIRQ(IRQn);//[core_cm3_h,line:1516]
}

void IoT(char *_pBuf)
{
    if (strstr(_pBuf, "BEEP_ON"))        BEEP = 1;
    else if (strstr(_pBuf, "BEEP_OFF"))  BEEP = 0;
    else if (strstr(_pBuf, "LED1_ON"))   LED1 = 0;
    else if (strstr(_pBuf, "LED1_OFF"))  LED1 = 1;
    else if (strstr(_pBuf, "LED2_ON"))   LED2 = 0;
    else if (strstr(_pBuf, "LED2_OFF"))  LED2 = 1;
    else if (strstr(_pBuf, "LED3_ON"))   LED3 = 0;
    else if (strstr(_pBuf, "LED3_OFF"))  LED3 = 1;
    else if (strstr(_pBuf, "LED4_ON"))   LED4 = 0;
    else if (strstr(_pBuf, "LED4_OFF"))  LED4 = 1;
}

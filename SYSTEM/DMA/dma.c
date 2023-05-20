#include "dma.h"

/*
****************************************************************************************************************
   
    ����ԭ��: void DMA1_Channelx_MtoP_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth)
    ��������: DMA1 ͨ����ʼ��[�洢����Ƭ������]
    �����β�: 
            _DMA1_Channelx -->DMAͨ��
            _ucWidth -->���ݿ��
    Ӳ������: ��
    �� �� ֵ: ��
    
    ͨ�����ù���
    ����������DMAͨ��x�Ĺ���(x����ͨ����)��
    1. ��DMA_CPARx�Ĵ�������������Ĵ����ĵ�ַ�������������ݴ�������ʱ�������ַ��
    �����ݴ����Դ��Ŀ�ꡣ
    2. ��DMA_CMARx�Ĵ������������ݴ洢���ĵ�ַ�������������ݴ�������ʱ���������
    �ݽ��������ַ������д�������ַ��
    3. ��DMA_CNDTRx�Ĵ���������Ҫ���������������ÿ�����ݴ���������ֵ�ݼ���
    4. ��DMA_CCRx�Ĵ�����PL[1:0]λ������ͨ�������ȼ���
    5. ��DMA_CCRx�Ĵ������������ݴ���ķ���ѭ��ģʽ������ʹ洢��������ģʽ����
    ��ʹ洢�������ݿ�ȡ�����һ������жϻ�����ɲ����жϡ�
    6. ����DMA_CCRx�Ĵ�����ENABLEλ��������ͨ����
****************************************************************************************************************  
*/
void DMA1_Channelx_MtoP_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth)
{
    /*����ʱ��ʹ��*/
    RCC->AHBENR |= 1<<0;

    /*���� DMA_Channelx ����*/
    _DMA1_Channelx->CCR = 0x00000000; //�ֶ���λȫ������
    _DMA1_Channelx->CCR |= 0<<0; //EN:ͨ������-1/�ر�-0(channel enable)
    _DMA1_Channelx->CCR |= 1<<1; //TCIE:����������ж� (Transfer complete interrupt enable)
    _DMA1_Channelx->CCR |= 1<<4; //DIR:���ݴ��䷽��[�Ӵ洢����-1/�������-0] (Data transfer direction)
    _DMA1_Channelx->CCR |= 0<<5; //ѭ��ģʽ off-0/on-1
    _DMA1_Channelx->CCR |= 0<<6; //PINC:�����ַ����ģʽ[��1/��0] (Peripheral increment mode)
    _DMA1_Channelx->CCR |= 1<<7; //MINC:�洢����ַ����ģʽ[��1/��0] (Memory increment mode)
    _DMA1_Channelx->CCR |= _ucWidth<<8; //PSIZE[8:9]���������ݿ�� (Peripheral size)
    _DMA1_Channelx->CCR |= _ucWidth<<10;//MSIZE[10:11]���洢�����ݿ�� (Memory size)
    _DMA1_Channelx->CCR |= 3<<12;//PL[12:13]��ͨ�����ȼ� (Channel priority level)
    _DMA1_Channelx->CCR |= 0<<14;//�Ǵ洢�����洢��ģʽ-0/�洢�����洢��ģʽ-1
}

/*
****************************************************************************************************************
   
    ����ԭ��: void DMA1_Channelx_PtoM_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth)
    ��������: DMA1 ͨ����ʼ��[Ƭ�����赽�洢��]
    �����β�: 
            _DMA1_Channelx -->DMAͨ��x
            _ucWidth -->���ݿ��
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
void DMA1_Channelx_PtoM_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth)
{
    /*����ʱ��ʹ��*/
    RCC->AHBENR |= 1<<0;

    /*���� DMA_Channelx ����*/
    _DMA1_Channelx->CCR = 0x00000000; //�ֶ���λȫ������
    _DMA1_Channelx->CCR |= 0<<0; //EN:ͨ������-1/�ر�-0(channel enable)
    _DMA1_Channelx->CCR |= 0<<1; //TCIE:����������ж� (Transfer complete interrupt enable)
    _DMA1_Channelx->CCR |= 0<<4; //DIR:���ݴ��䷽��[�Ӵ洢����-1/�������-0] (Data transfer direction)
    _DMA1_Channelx->CCR |= 0<<5; //ѭ��ģʽ off-0/on-1
    _DMA1_Channelx->CCR |= 0<<6; //PINC:�����ַ����ģʽ[��1/��0] (Peripheral increment mode)
    _DMA1_Channelx->CCR |= 1<<7; //MINC:�洢����ַ����ģʽ[��1/��0] (Memory increment mode)
    _DMA1_Channelx->CCR |= _ucWidth<<8; //PSIZE[8:9]���������ݿ�� (Peripheral size)
    _DMA1_Channelx->CCR |= _ucWidth<<10;//MSIZE[10:11]���洢�����ݿ�� (Memory size)
    _DMA1_Channelx->CCR |= 3<<12;//PL[12:13]��ͨ�����ȼ� (Channel priority level)
    _DMA1_Channelx->CCR |= 0<<14;//�Ǵ洢�����洢��ģʽ-0/�洢�����洢��ģʽ-1
}

/*
****************************************************************************************************************
   
    ����ԭ��: void DMA1_Channelx_MtoM_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth)
    ��������: DMA1 ͨ����ʼ��[�洢�����洢��]
    �����β�: _DMA1_Channelx -->DMAͨ��x, _ucWidth -->���ݿ��
    Ӳ������: ��
    �� �� ֵ: ��
    
****************************************************************************************************************  
*/
void DMA1_Channelx_MtoM_Init(DMA_Channel_TypeDef *_DMA1_Channelx,u8 _ucWidth)
{
    /*����ʱ��ʹ��*/
    RCC->AHBENR |= 1<<0;

    /*���� DMA_Channelx ����*/
    _DMA1_Channelx->CCR = 0x00000000; //�ֶ���λȫ������
    _DMA1_Channelx->CCR |= 0<<0; //EN:ͨ������-1/�ر�-0(channel enable)
    _DMA1_Channelx->CCR |= 0<<1; //TCIE:����������ж� (Transfer complete interrupt enable)
    _DMA1_Channelx->CCR |= 0<<4; //DIR:���ݴ��䷽��[�Ӵ洢����-1/�������-0] (Data transfer direction)
    _DMA1_Channelx->CCR |= 0<<5; //ѭ��ģʽ off-0/on-1
    _DMA1_Channelx->CCR |= 0<<6; //PINC:�����ַ����ģʽ[��1/��0] (Peripheral increment mode)
    _DMA1_Channelx->CCR |= 1<<7; //MINC:�洢����ַ����ģʽ[��1/��0] (Memory increment mode)
    _DMA1_Channelx->CCR |= _ucWidth<<8; //PSIZE[8:9]���������ݿ�� (Peripheral size)
    _DMA1_Channelx->CCR |= _ucWidth<<10;//MSIZE[10:11]���洢�����ݿ�� (Memory size)
    _DMA1_Channelx->CCR |= 3<<12;//PL[12:13]��ͨ�����ȼ� (Channel priority level)
    _DMA1_Channelx->CCR |= 1<<14;//�Ǵ洢�����洢��ģʽ-0/�洢�����洢��ģʽ-1
}

/*
****************************************************************************************************************
   
    ����ԭ��: void DMA_Channelx_Start(DMA_Channel_TypeDef *_DMA1_Channelx,u32 _ulCmar,u32 _ulCpar,u16 _usCndtr)
    ��������: ���� DMA ͨ������
    �����β�: _DMA1_Channelx ͨ��x
            _ulCmar �洢����ַ
            _ulCpar �����ַ
            _usCndtr ����������
    Ӳ������: ��
    �� �� ֵ: ��

****************************************************************************************************************  
*/
void DMAx_Channelx_Start(DMA_Channel_TypeDef *_DMAx_Channelx,u32 _ulCmar,u32 _ulCpar,u16 _usCndtr)
{
    _DMAx_Channelx->CCR &= ~(1<<0);  //Open the channel
    _DMAx_Channelx->CMAR = _ulCmar;  //�洢����ַ
    _DMAx_Channelx->CNDTR = _usCndtr;//������������(���εݼ�)
    _DMAx_Channelx->CPAR = _ulCpar; //�����ַ
    _DMAx_Channelx->CCR |= 1<<0;     //Close the channel

}

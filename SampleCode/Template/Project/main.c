/*_____ I N C L U D E S ____________________________________________________*/
#include "ML51.h"

#include	"project_config.h"



/*_____ D E C L A R A T I O N S ____________________________________________*/
volatile uint8_t u8TH0_Tmp = 0;
volatile uint8_t u8TL0_Tmp = 0;

//UART 0
//bit BIT_TMP;
//bit BIT_UART;
//bit uart0_receive_flag=0;
//unsigned char uart0_receive_data;

/*_____ D E F I N I T I O N S ______________________________________________*/
volatile uint32_t BitFlag = 0;
volatile uint32_t counter_tick = 0;
volatile uint32_t counter_tick_WKT = 0;

unsigned char DIV_125ms = 0;
unsigned char DIV_250ms = 0;
unsigned char DIV_500ms = 0;
unsigned char DIV_1000ms = 0;
/*_____ M A C R O S ________________________________________________________*/
#define SYS_CLOCK 								(24000000ul)


/*_____ F U N C T I O N S __________________________________________________*/

void tick_counter_WKT(void)
{
	counter_tick++;
}

uint32_t get_tick_WKT(void)
{
	return (counter_tick);
}

void set_tick_WKT(uint32_t t)
{
	counter_tick = t;
}


void tick_counter(void)
{
	counter_tick++;
}

uint32_t get_tick(void)
{
	return (counter_tick);
}

void set_tick(uint32_t t)
{
	counter_tick = t;
}

void compare_buffer(uint8_t *src, uint8_t *des, int nBytes)
{
    uint16_t i = 0;	
	
    for (i = 0; i < nBytes; i++)
    {
        if (src[i] != des[i])
        {
            printf("error idx : %4d : 0x%2X , 0x%2X\r\n", i , src[i],des[i]);
			set_flag(flag_error , Enable);
        }
    }

	if (!is_flag_set(flag_error))
	{
    	printf("compare_buffer finish \r\n");	
		set_flag(flag_error , Disable);
	}

}

void reset_buffer(void *dest, unsigned int val, unsigned int size)
{
    uint8_t *pu8Dest;
//    unsigned int i;
    
    pu8Dest = (uint8_t *)dest;

	#if 1
	while (size-- > 0)
		*pu8Dest++ = val;
	#else
	memset(pu8Dest, val, size * (sizeof(pu8Dest[0]) ));
	#endif
	
}

void copy_buffer(void *dest, void *src, unsigned int size)
{
    uint8_t *pu8Src, *pu8Dest;
    unsigned int i;
    
    pu8Dest = (uint8_t *)dest;
    pu8Src  = (uint8_t *)src;


	#if 0
	  while (size--)
	    *pu8Dest++ = *pu8Src++;
	#else
    for (i = 0; i < size; i++)
        pu8Dest[i] = pu8Src[i];
	#endif
}

void dump_buffer(uint8_t *pucBuff, int nBytes)
{
    uint16_t i = 0;
    
    printf("dump_buffer : %2d\r\n" , nBytes);    
    for (i = 0 ; i < nBytes ; i++)
    {
        printf("0x%2X," , pucBuff[i]);
        if ((i+1)%8 ==0)
        {
            printf("\r\n");
        }            
    }
    printf("\r\n\r\n");
}

void  dump_buffer_hex(uint8_t *pucBuff, int nBytes)
{
    int     nIdx, i;

    nIdx = 0;
    while (nBytes > 0)
    {
        printf("0x%04X  ", nIdx);
        for (i = 0; i < 16; i++)
            printf("%02X ", pucBuff[nIdx + i]);
        printf("  ");
        for (i = 0; i < 16; i++)
        {
            if ((pucBuff[nIdx + i] >= 0x20) && (pucBuff[nIdx + i] < 127))
                printf("%c", pucBuff[nIdx + i]);
            else
                printf(".");
            nBytes--;
        }
        nIdx += 16;
        printf("\n");
    }
    printf("\n");
}

void delay(uint16_t dly)
{
/*
	delay(100) : 14.84 us
	delay(200) : 29.37 us
	delay(300) : 43.97 us
	delay(400) : 58.5 us	
	delay(500) : 73.13 us	
	
	delay(1500) : 0.218 ms (218 us)
	delay(2000) : 0.291 ms (291 us)	
*/

	while( dly--);
}


void send_UARTString(uint8_t* Data)
{
	#if 1
	uint16_t i = 0;

	while (Data[i] != '\0')
	{
		#if 1
		SBUF = Data[i++];
		#else
		UART_Send_Data(UART0,Data[i++]);		
		#endif
	}

	#endif

	#if 0
	uint16_t i = 0;
	
	for(i = 0;i< (strlen(Data)) ;i++ )
	{
		UART_Send_Data(UART0,Data[i]);
	}
	#endif

	#if 0
    while(*Data)  
    {  
        UART_Send_Data(UART0, (unsigned char) *Data++);  
    } 
	#endif
}

void send_UARTASCII(uint16_t Temp)
{
    uint8_t print_buf[16];
    uint16_t i = 15, j;

    *(print_buf + i) = '\0';
    j = (uint16_t)Temp >> 31;
    if(j)
        (uint16_t) Temp = ~(uint16_t)Temp + 1;
    do
    {
        i--;
        *(print_buf + i) = '0' + (uint16_t)Temp % 10;
        (uint16_t)Temp = (uint16_t)Temp / 10;
    }
    while((uint16_t)Temp != 0);
    if(j)
    {
        i--;
        *(print_buf + i) = '-';
    }
    send_UARTString(print_buf + i);
}


void WakeUp_Timer_ISR (void)   interrupt 17     //ISR for self wake-up timer
{
	_push_(SFRS); 
	
//	P33 ^= 1;

	tick_counter_WKT();

	if ((get_tick_WKT() % DIV_125ms) == 0)
	{
		set_flag(flag_125ms,ENABLE);
	}

	if ((get_tick_WKT() % DIV_250ms) == 0)
	{
		set_flag(flag_250ms,ENABLE);
	}	

	if ((get_tick_WKT() % DIV_500ms) == 0)
	{
		set_flag(flag_500ms,ENABLE);
	}

	if ((get_tick_WKT() % DIV_1000ms) == 0)
	{
		set_flag(flag_1000ms,ENABLE);
	}
	
    clr_WKCON_WKTF;                                   //clear interrupt flag
	_pop_(SFRS);
}


/*
	when LXT : 32.768
	target timing : 
	62.5 ms
	125 ms
	250 ms
	500 ms

	psc set as 1/256

	RWK will be
	8
	16
	32
	64

	FORMULA : RWK = target timing / 1000 * (LXT / PSC)

*/
const WKT_TypeDef wkt_table[] = 
{
	{WKT_62_5_ms_16_psc		,128} ,
	{WKT_62_5_ms_64_psc		,32} ,
	{WKT_62_5_ms_256_psc	,8} ,	

	{WKT_125_ms_256_psc		,16} ,
	{WKT_250_ms_256_psc		,32} ,	
	{WKT_500_ms_256_psc		,64} ,
			
};

void WKT_Init(WKT_Timer_Index idx)
{
	unsigned char i = 0;
	unsigned int u16WKTRLData = 0;

	SFRS = 0;

	#if 1	// LXT
	MFP_P55_X32_IN;
	MFP_P54_X32_OUT;
	set_CKEN_ELXTEN;                        //step3: Enable LXT.
	while((CKSWT|CLR_BIT6)==CLR_BIT6);      //step4: check clock source status and wait for ready
	WKCON |= 0x20;
	#else	// LIRC
	WKCON &= 0xDF;
	#endif

	//WKT Pre-Scalar , and timer division
	#if 1
	switch(idx)
	{
		case WKT_62_5_ms_16_psc:
			WKCON &= 0xF8; WKCON |= 0x02;//	1/ 16

			DIV_125ms 	= 2;
			DIV_250ms 	= 4;
			DIV_500ms 	= 8;
			DIV_1000ms 	= 16;
		
			break;
		case WKT_62_5_ms_64_psc:
			WKCON &= 0xF8; WKCON |= 0x03;	//	1/ 64	

			DIV_125ms 	= 2;
			DIV_250ms 	= 4;
			DIV_500ms 	= 8;
			DIV_1000ms 	= 16;
		
			break;
		case WKT_62_5_ms_256_psc:
			WKCON &= 0xF8; WKCON |= 0x04;	//	1/ 256

			DIV_125ms 	= 2;
			DIV_250ms 	= 4;
			DIV_500ms 	= 8;
			DIV_1000ms 	= 16;
		
			break;		
	
		case WKT_125_ms_256_psc:
			WKCON &= 0xF8; WKCON |= 0x04;	//	1/ 256	
			
			DIV_125ms 	= 1;
			DIV_250ms 	= 2;
			DIV_500ms 	= 4;
			DIV_1000ms 	= 8;
		
			break;
		case WKT_250_ms_256_psc:
			WKCON &= 0xF8; WKCON |= 0x04;	//	1/ 256		

			DIV_125ms 	= 1;		// not available for 125ms , since the base timing is 250ms
			DIV_250ms 	= 1;
			DIV_500ms 	= 2;
			DIV_1000ms 	= 4;
		
			break;
		case WKT_500_ms_256_psc:
			WKCON &= 0xF8; WKCON |= 0x04;	//	1/ 256		

			DIV_125ms 	= 1;		// not available for 125ms , since the base timing is 500ms
			DIV_250ms 	= 1;		// not available for 250ms , since the base timing is 500ms
			DIV_500ms 	= 1;
			DIV_1000ms 	= 2;
		
			break;			
	}
	#else
//	WKCON &= 0xF8; WKCON |= 0x02;//	1/ 16	
	WKCON &= 0xF8; WKCON |= 0x03;	//	1/ 64
//	WKCON &= 0xF8; WKCON |= 0x04;	//	1/ 256
	#endif

	u16WKTRLData = wkt_table[idx].rwk;
	
	/*
		ML51 TRM 2.01
		6.8.2 block diagram
		RWK support 16 bit , when flash size 64K
		otherwise , support 8 bit
	*/
	#if 0
	SFRS=0;
	RWKL = 256 - u16WKTRLData;
	
	#else	// 64K flash
	SFRS=2;
	RWKH = HIBYTE ( 65536ul - u16WKTRLData);		//65535ul-(u8WKTRLData*512ul/1000ul)
	SFRS=0;
	RWKL = LOBYTE ( 65536ul - u16WKTRLData);		//65535ul-(u8WKTRLData*512ul/1000ul)
	#endif
	
    ENABLE_WKT_INTERRUPT;             // enable WKT interrupt
    set_WKCON_WKTR;                   // Wake-up timer run 

    ENABLE_GLOBAL_INTERRUPT;	
}

void GPIO_Init(void)
{
	P03_PUSHPULL_MODE;		

	P33_PUSHPULL_MODE;	
}

void Timer0_IRQHandler(void)
{
	
	tick_counter();

	if ((get_tick() % 1000) == 0)
	{
		P03 ^= 1;
	}

	if ((get_tick() % 50) == 0)
	{

	}
	P33 ^= 1;
	
}

//void Timer0_ISR(void) interrupt 1        // Vector @  0x0B
//{
//    TH0 = u8TH0_Tmp;
//    TL0 = u8TL0_Tmp;
//    clr_TCON_TF0;
//	
//	Timer0_IRQHandler();
//}

void Timer0_Init(void)
{
	uint16_t res = 0;

	ENABLE_TIMER0_MODE1;
    TIMER0_FSYS_DIV12;
	
	u8TH0_Tmp = HIBYTE(TIMER_DIV12_VALUE_1ms_FOSC_240000);
	u8TL0_Tmp = LOBYTE(TIMER_DIV12_VALUE_1ms_FOSC_240000); 

    TH0 = u8TH0_Tmp;
    TL0 = u8TL0_Tmp;

    ENABLE_TIMER0_INTERRUPT;                       //enable Timer0 interrupt
    ENABLE_GLOBAL_INTERRUPT;                       //enable interrupts
  
    set_TCON_TR0;                                  //Timer0 run
}


//void Serial_ISR (void) interrupt 4 
//{
//    _push_(SFRS);

//    if (RI)
//    {   
//      uart0_receive_flag = 1;
//      uart0_receive_data = SBUF;
//      clr_SCON_RI;                                         // Clear RI (Receive Interrupt).
//    }
//    if  (TI)
//    {
//      if(!BIT_UART)
//      {
//          TI = 0;
//      }
//    }

//    _pop_(SFRS);	
//}


void UART0_Init(void)
{
	MFP_P31_UART0_TXD;                              // UART0 TXD use P1.6
	P31_QUASI_MODE;                                  // set P1.6 as Quasi mode for UART0 trasnfer
	UART_Open(SYS_CLOCK,UART0_Timer3,115200);        // Open UART0 use timer1 as baudrate generate and baud rate = 115200
	ENABLE_UART0_PRINTF;


//	printf("UART0_Init\r\n");
}

void SYS_Init(void)
{
//	FsysSelect(FSYS_HIRC);

    ALL_GPIO_QUASI_MODE;
//    ENABLE_GLOBAL_INTERRUPT;                // global enable bit	
}

void main (void) 
{
    SYS_Init();

//    UART0_Init();
	GPIO_Init();
//	Timer0_Init();

	WKT_Init(WKT_62_5_ms_64_psc);
		
    while(1)
    {
		if (is_flag_set(flag_125ms))
		{
			set_flag(flag_125ms , DISABLE);
			P33 ^= 1;
		}

		if (is_flag_set(flag_250ms))
		{
			set_flag(flag_250ms , DISABLE);
//			P33 ^= 1;
		}
		
		if (is_flag_set(flag_500ms))
		{
			set_flag(flag_500ms , DISABLE);
//			P33 ^= 1;
		}

		if (is_flag_set(flag_1000ms))
		{
			set_flag(flag_1000ms , DISABLE);
//			P33 ^= 1;
		}		
    }
}




#include "LSTD_TYPES.h"
#include "MRCC_interface.h"
#include "MGPIO_interface.h"
#include "MNVIC_Interface.h"
#include "MUSART_Interface.h"
#include"DCMotor_interface.h"
#include"Delay_interface.h"
#include"MTIMER4_interface.h"
#include <stdio.h>
#include <string.h>

#define mobileNumber "+201015244400"

static u8 L_u8msg=0;
static u8 L_u8msg1=0;
u8 flag=0;
u8 i=1;
u8 x=0;
u8 y=0;
u8 lat[30];
u8 longt[30];
u8 payload[100];
u8 apiKey[] = "T8XWRYUH8AYN1TB2"; 
u8 dataLength;
u8 ATcommand[80];
u8 buffer[50] = {0};
u8 ATisOK = 0;

int main(void)
{
	/*************************************************************************************************************************/
	/********************************Step 1: initialize System Clock is 16MHz from HSI****************************************/
	/*************************************************************************************************************************/

	MRCC_voidInitSystemClk();

	/*************************************************************************************************************************/
	/********************************************Step 2: Enable peripherls clock**********************************************/
	/*************************************************************************************************************************/

	MRCC_voidEnablePeripheralClock(AHB1,_PERIPHERAL_EN_GPIOA ) ;
	MRCC_voidEnablePeripheralClock(AHB1,_PERIPHERAL_EN_GPIOB ) ;

	MRCC_voidEnablePeripheralClock(APB2, PERIPHERAL_EN_USART1);
	MRCC_voidEnablePeripheralClock(APB1,PERIPHERAL_EN_TIM2) ;


	/*************************************************************************************************************************/
	/******************************************** Configure UART1 ,UART6  Pins******************************************/
	/*************************************************************************************************************************/

	MGPIO_voidSetPinMode (_GPIOA_PORT, _PIN_9, _MODE_ALTF);   
	MGPIO_voidSetPinMode(_GPIOA_PORT, _PIN_10, _MODE_ALTF);   
	MGPIO_voidSetPinMode(_GPIOA_PORT, _PIN_11, _MODE_ALTF);   
	MGPIO_voidSetPinMode(_GPIOA_PORT, _PIN_12, _MODE_ALTF);  
	MGPIO_voidSetPinAltFn  (_GPIOA_PORT, _PIN_9, _ALTFN_7);    
	MGPIO_voidSetPinAltFn (_GPIOA_PORT, _PIN_10, _ALTFN_7);   
	/*************************************************************************************************************************/
		/******************************************** Configure TIMER2 PIN  Pins******************************************/
		/*************************************************************************************************************************/

			MGPIO_voidSetPinMode(_GPIOA_PORT, _PIN_5,  _MODE_ALTF) ;
			MGPIO_voidSetPinAltFn      (_GPIOA_PORT,_PIN_5,_ALTFN_1);

	/*************************************************************************************************************************/
	/********************************************Step 4: initialize Output Pins  in System ***********************************/
	/*************************************************************************************************************************/

	for(int PIN =0; PIN<4; PIN++)
	{
		MGPIO_voidSetPinMode		    	(_GPIOA_PORT, PIN, _MODE_OUTPUT) ;
		MGPIO_voidSetPinOutPutType   (_GPIOA_PORT,PIN,_OUTPUT_TYPE_PUSH_PULL);
		MGPIO_voidSetPinOutSpeed	    (_GPIOA_PORT, PIN, _OUTPUT_SPEED_LOW);
	}

	/*************************************************************************************************************************/
	/********************************************Step 5: Enable Peripherials interrupt from NVIC *****************************/
	/*************************************************************************************************************************/

	MNVIC_voidEnableInterrupt(37) ;              

	/*************************************************************************************************************************/
	/********************************************Step 6: Initialize USART1 ***************************************************/
	/*************************************************************************************************************************/
    MTIMER2_voidInit( );
    MTIMER2_void_PWM(24000);
	MSUART_voidInit();

	/*************************************************************************************************************************/
	/********************************************Step 6:  initialize system Peripherials *************************************/
	/*************************************************************************************************************************/
	MUSART_voidEnable(1) ; 
	MUSART_voidEnable(6) ; 

	 while(!ATisOK){
	   	sprintf(ATcommand,"AT\r\n");
        MUSART_voidSendString(1,(u8 *)ATcommand);
        HAL_Delay(500);
	    
        MUSART_u8ReadString(1, buffer, 500);
	    if(strstr((char *)buffer,"OK")){
		    ATisOK = 1;
	    }
	    HAL_Delay(1000);
	    memset(buffer,0,sizeof(buffer));
     }
     
     MUSART_voidSendString(1, (u8*)"AT+CMEE=1\r\n");
     HAL_Delay(500);

     MUSART_voidSendString(1, (u8*)"AT+CSQ\r\n");
     HAL_Delay(1000);

     MUSART_voidSendString(1, (u8*)"AT+CREG?\r\n");
     HAL_Delay(1000);

     MUSART_voidSendString(1, (u8*)"AT+CGATT=1\r\n");
     HAL_Delay(1000);

     sprintf(ATcommand,"AT+CSTT=\"internet.vodafone.net\",\"\",\"\"\r\n");
     MUSART_voidSendString(1,(u8 *)ATcommand);
     HAL_Delay(1000);

     // Bring up the wireless connection
     sprintf(ATcommand,"AT+CIICR\r\n");
     MUSART_voidSendString(1,(u8 *)ATcommand);
     HAL_Delay(5000); 

     // Get local IP address
     sprintf(ATcommand,"AT+CIFSR\r\n");
     MUSART_voidSendString(1,(u8 *)ATcommand);
     HAL_Delay(1000);
	
	/* Loop forever */
	while(1)
	{

		if(L_u8msg=='G')
		{
			MTIMER2_void_PWM(8000);

		}

		if(L_u8msg=='L')
		{
			H_DCMotor1_void_ClockWise();
			H_DCMotor2_void_Stop();

		}
		else if(L_u8msg=='R')
		{
			H_DCMotor2_void_ClockWise();
			H_DCMotor1_void_Stop();
		}
		else if(L_u8msg=='F')
				{
			H_DCMotor1_void_ClockWise();
			H_DCMotor2_void_ClockWise();
				}
		else if(L_u8msg=='B')
				{
			H_DCMotor1_void_untyClockWise();
			H_DCMotor2_void_untyClockWise();
				}

	    if (flag == 0) {
            
            
            sprintf(ATcommand,"AT+CMGF=1\r\n");
	 	 	MUSART_voidSendString(1,(u8 *)ATcommand);
            HAL_Delay(1000);
            
            sprintf((char*)ATcommand,"AT+CMGS=\"%s\"\r\n", mobileNumber);
            MUSART_voidSendString(1,(u8 *)ATcommand);
            HAL_Delay(500);

            sprintf((char*)ATcommand,"There is a car has accident here its location is latitude: %s longitude: %s%c", (char*)lat, (char*)longt, 0x1a);
            MUSART_voidSendString(1,(u8 *)ATcommand);
            
            MUSART_u8ReadString(1, buffer, 4000); 
            memset(buffer,0,sizeof(buffer));
	        flag=1;
	    }
	}
}

void USART1_IRQHandler(void){
	L_u8msg=MUSART_u8ReadData(1);
}

void USART6_IRQHandler(void){
	L_u8msg1=MUSART_u8ReadData(6);
}

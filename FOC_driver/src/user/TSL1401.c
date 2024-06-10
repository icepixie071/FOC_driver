/***********************************************************************************************************************
* File Name    : TSL1401.c
* Version      : 1.8.1
* Device(s)    : R5F566TEAxFM
* Description  : main file.
* Creation Date: 2022-01-04
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "machine.h"
#include "stdint.h"
#include "iodefine.h"
#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"

#include "Config_PORT.h"
#include "Config_S12AD1.h"

#include "TSL1401.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
uint32_t exposure_cnt;

uint16_t result_cap[128];
uint16_t result_3lv[128];

uint16_t cl_position;
uint16_t cl_l_end;
uint16_t cl_r_end;

/***********************************************************************************************************************
* Function Name: TSL1401_CaptureProcess
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void TSL1401_CaptureProcess(void)
{    
	exposure_cnt ++;
	
	if(exposure_cnt >= 4)									
	{	
		TSL1401_PORT_SI = 1;								
		TSL1401_PORT_CLK = 1;						
		TSL1401_PORT_SI = 0;
		
		// 1. capture routin
		for(uint8_t i = 0; i < 128; i ++)
		{
			TSL1401_PORT_CLK = 1;	
			
			R_Config_S12AD1_Start();
			while(S12AD1.ADCSR.BIT.ADST == 1U)
			
			R_Config_S12AD1_Get_ValueResult(ADCHANNEL0, &result_cap[i]);
			
			TSL1401_PORT_CLK = 0;		
		}
		
		exposure_cnt = 0;
		
		// 2. digitalize routin
		for(uint8_t j = 0; j < 128; j ++)
		{	
			if(result_cap[j] >= 2000)
			{
				result_3lv[j] = 2;
			}
			else if(result_cap[j] >= 1000)
			{
				result_3lv[j] = 1;
			}
			else
			{
				result_3lv[j] = 0;
			}
		}
		
		
		cl_l_end = 0;
		cl_r_end = 0;
		
		// 3. center line ditection routin
		for(uint8_t k = 32; k < 96; k ++)
		{	
			if(cl_l_end == 0)
			{
				if((result_3lv[k] == 2) && (result_3lv[k + 1] == 2) 
				                    && (result_3lv[k + 2] == 2))
				{
					cl_l_end = k;
				}
				else
				{
					/* Do nothing */	
				}
			}
			else if(cl_r_end == 0)
			{	
				if((result_3lv[k] <= 1) && (result_3lv[k + 1] <= 1) 
				                    && (result_3lv[k + 2] <= 1))
				{
					cl_r_end = k - 1;
				}
				else
				{
					/* Do nothing */
				}
			}		
		}
		
		cl_position = (cl_l_end + cl_r_end) >> 1;
		
		// TODO: アナログ的制御量計算方法も考える(フォトトラと同方式等)
		
    }
	else
	{
		nop();
	}

}
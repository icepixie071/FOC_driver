/***********************************************************************************************************************
* File Name    : Motor.c
* Version      : 1.8.1
* Device(s)    : R5F566TEAxFM
* Description  : 
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

#include "Config_MTU3_MTU4.h"
#include "Config_S12AD0.h"

#include "Motor.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/

uint8_t hall_signal;

uint16_t curr_debug;

/* PI controll for R motor current */
int32_t curr_data[8];
int32_t curr_sum;
int32_t curr_ave;

int8_t dat_cnt;

int32_t curr_now;
int32_t curr_err;
int32_t curr_err_intg;
int32_t curr_out;
int32_t curr_prop;
int32_t curr_intg;

int8_t curr_kp = 5;
int8_t curr_ki = 1;

/***********************************************************************************************************************
* Function Name: Mot_R_SetVPattern
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void Mot_R_SetVPattern(mot_mode_t ref_mode)
{
	hall_signal  =  R_MTR_PORT_HALL_W << 2;
	hall_signal |=  R_MTR_PORT_HALL_V << 1;
	hall_signal |=  R_MTR_PORT_HALL_U;
    
	if (ref_mode == ROT_FOR) 
   	{
		switch (hall_signal)
        {
            case MTR_PATTERN_CW_U_V:	/* U ¨ V */
			
	            User_MTU3_MTU4_SetInverterMode(INV1_U_PWM_VN_ON);
				break;

            case MTR_PATTERN_CW_U_W:	/* U ¨ W */
			
            	User_MTU3_MTU4_SetInverterMode(INV1_U_PWM_WN_ON);
            	break;

            case MTR_PATTERN_CW_V_W: 	/* V ¨ W */
			
            	User_MTU3_MTU4_SetInverterMode(INV1_V_PWM_WN_ON);
            	break;

            case MTR_PATTERN_CW_V_U: 	/* V ¨ U */
			
            	User_MTU3_MTU4_SetInverterMode(INV1_V_PWM_UN_ON);
            	break;
	
            case MTR_PATTERN_CW_W_U: 	/* W ¨ U */
			
				User_MTU3_MTU4_SetInverterMode(INV1_W_PWM_UN_ON);
            	break;
			
			case MTR_PATTERN_CW_W_V: 	/* W ¨ V */
			
				User_MTU3_MTU4_SetInverterMode(INV1_W_PWM_VN_ON);
		    	break;

            default:					/* pattern error */
			
				User_MTU3_MTU4_SetInverterMode(INV1_FREE);
            	break;
        }
    }
    else if (ref_mode == ROT_BACK)								
    {
        switch (hall_signal)
        {
			case MTR_PATTERN_CCW_W_V:	/* W ¨ V */
			
				User_MTU3_MTU4_SetInverterMode(INV1_W_PWM_VN_ON);
            	break;
				
            case MTR_PATTERN_CCW_W_U:	/* W ¨ U */
			
				User_MTU3_MTU4_SetInverterMode(INV1_W_PWM_UN_ON);
            	break;

            case MTR_PATTERN_CCW_V_U:	/* V ¨ U */
			
				User_MTU3_MTU4_SetInverterMode(INV1_V_PWM_UN_ON);
            	break;

            case MTR_PATTERN_CCW_V_W:	/* V ¨ W */
			
				User_MTU3_MTU4_SetInverterMode(INV1_V_PWM_UN_ON);
            	break;

            case MTR_PATTERN_CCW_U_W:	/* U ¨ W */
			
				User_MTU3_MTU4_SetInverterMode(INV1_U_PWM_WN_ON);
            	break;

            case MTR_PATTERN_CCW_U_V:	/* U ¨ V */
			
				User_MTU3_MTU4_SetInverterMode(INV1_U_PWM_VN_ON);
            	break;

            default:					/* pattern error */
			
				User_MTU3_MTU4_SetInverterMode(INV1_FREE);
           		break;
        }
    }
    else
    {
        /* Do Nothing */
    }
}
/***********************************************************************************************************************
* Function Name: Mot_L_SetVPattern
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void Mot_L_SetVPattern(mot_mode_t ref_mode)
{
	hall_signal  =  L_MTR_PORT_HALL_W << 2;
	hall_signal |=  L_MTR_PORT_HALL_V << 1;
	hall_signal |=  L_MTR_PORT_HALL_U;
    
	if (ref_mode == ROT_FOR) 
   	{
		switch (hall_signal)
        {
            case MTR_PATTERN_CW_U_V:	/* U ¨ V */
			
            	User_MTU6_MTU7_SetInverterMode(INV2_U_PWM_VN_ON);
            	break;

            case MTR_PATTERN_CW_U_W:	/* U ¨ W */
			
            	User_MTU6_MTU7_SetInverterMode(INV2_U_PWM_WN_ON);
            	break;

            case MTR_PATTERN_CW_V_W: 	/* V ¨ W */
			
            	User_MTU6_MTU7_SetInverterMode(INV2_V_PWM_WN_ON);
            	break;

            case MTR_PATTERN_CW_V_U: 	/* V ¨ U */
			
            	User_MTU6_MTU7_SetInverterMode(INV2_V_PWM_UN_ON);
            	break;
	
            case MTR_PATTERN_CW_W_U: 	/* W ¨ U */
			
				User_MTU6_MTU7_SetInverterMode(INV2_W_PWM_UN_ON);
            	break;
				
			case MTR_PATTERN_CW_W_V: 	/* W ¨ V */
			
				User_MTU6_MTU7_SetInverterMode(INV2_W_PWM_VN_ON);
	    		break;

            default: 					/* pattern error */
			
				User_MTU6_MTU7_SetInverterMode(INV2_FREE);
            	break;
        }
    }
    else if (ref_mode == ROT_BACK)
    {
        switch (hall_signal)
        {
            case MTR_PATTERN_CCW_W_V:	/* W ¨ V */
			
				User_MTU6_MTU7_SetInverterMode(INV2_W_PWM_VN_ON);
            	break;
				
            case MTR_PATTERN_CCW_W_U:	/* W ¨ U */
			
				User_MTU6_MTU7_SetInverterMode(INV2_W_PWM_UN_ON);
            	break;

            case MTR_PATTERN_CCW_V_U:	/* V ¨ U */
			
				User_MTU6_MTU7_SetInverterMode(INV2_V_PWM_UN_ON);
            	break;

            case MTR_PATTERN_CCW_V_W:	/* V ¨ W */
			
				User_MTU6_MTU7_SetInverterMode(INV2_V_PWM_UN_ON);
            	break;

            case MTR_PATTERN_CCW_U_W:	/* U ¨ W */
			
				User_MTU6_MTU7_SetInverterMode(INV2_U_PWM_WN_ON);
            	break;

            case MTR_PATTERN_CCW_U_V:	/* U ¨ V */
			
				User_MTU6_MTU7_SetInverterMode(INV2_U_PWM_VN_ON);
            	break;

            default:					/* pattern error */
						
				User_MTU6_MTU7_SetInverterMode(INV2_FREE);
            	break;
        }
    }
    else
    {
        /* Do Nothing */
    }
}

/***********************************************************************************************************************
* Function Name: Mot_ControllCurrent
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void Mot_R_CtrlCurrentPI(uint16_t curr_ref)
{	
	/* moving average */
	
	if (dat_cnt == 7) { dat_cnt = 0; }
	
	curr_sum -= curr_data[dat_cnt];
	curr_data[dat_cnt] = ((uint32_t)(S12AD.ADDR1) >> 4);			// 16 = 1A = 2482
	curr_sum += curr_data[dat_cnt];
	dat_cnt++;
	curr_now = curr_sum >> 3;
	
	/* PI control */
	
	curr_err =  curr_ref - curr_now;
	
	if (Utl_CheckAddOvf(curr_err_intg, curr_err))
	{
		/* over flow */
	}
	else
	{
		curr_err_intg = curr_err_intg + curr_err;
	}
		
	if (curr_err_intg > 6000) { curr_err_intg = 6000; }
	if (curr_err_intg < -6000) { curr_err_intg = -6000; }
	
	curr_prop = curr_err * curr_kp;
	curr_intg = curr_err_intg * curr_ki;
	
	if (Utl_CheckAddOvf(curr_err_intg, curr_err))
	{
		/* over flow */
	}
	else
	{
		curr_out = curr_prop + curr_intg;
	}
	
	
	if (curr_out > 6600) { curr_out = 6600; }
	if (curr_out < 0) { curr_out = 0; }
	
	User_MTU3_MTU4_SetPwmDuty((uint16_t)(curr_out));
}

/***********************************************************************************************************************
* Function Name: Mot_R_HallInterrupt
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void Mot_R_HallInterrupt(void)
{
	clrpsw_i();
    Mot_R_SetVPattern(st_mot_param_r.ref_mode);
	setpsw_i();
}

/***********************************************************************************************************************
* Function Name: Mot_L_HallInterrupt
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void Mot_L_HallInterrupt(void)
{
	clrpsw_i();
    Mot_L_SetVPattern(st_mot_param_l.ref_mode);
	setpsw_i();
}

/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name        : Config_MTU3_MTU4.h
* Component Version: 1.11.0
* Device(s)        : R5F566TEAxFM
* Description      : This file implements device driver for Config_MTU3_MTU4.
***********************************************************************************************************************/

#ifndef CFG_Config_MTU3_MTU4_H
#define CFG_Config_MTU3_MTU4_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_mtu3.h"

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define MTU3_PCLK_COUNTER_DIVISION      (1)
#define _0258_3TCNT_VALUE               (0x0258U) /* MTU3.TCNT value */
#define _0258_TDDRA_VALUE               (0x0258U) /* MTU.TDDRA value */
#define _1770_TCDRA_VALUE               (0x1770U) /* MTU.TCDRA value */
#define _19C8_3TGRB_VALUE               (0x19C8U) /* MTU3.TGRB value */
#define _19C8_4TGRA_VALUE               (0x19C8U) /* MTU4.TGRA value */
#define _19C8_4TGRB_VALUE               (0x19C8U) /* MTU4.TGRB value */
#define _19C8_SUM_VALUE                 (0x19C8U) /* Timer General Register (TGR) value */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_Config_MTU3_MTU4_Create(void);
void R_Config_MTU3_MTU4_Create_UserInit(void);
void R_Config_MTU3_MTU4_Start(void);
void R_Config_MTU3_MTU4_Stop(void);
/* Start user code for function. Do not edit comment generated here */
void User_MTU3_MTU4_SetInverterMode(inv1_mode_t);
void User_MTU3_MTU4_SetPwmDuty(uint16_t);
/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

#define 	MTU3_4_PWM_MAX				(6600)						

#define     INV1_PORT_UP				(PORT7.PODR.BIT.B1)         /* output port of PWM timer(Up) */
#define     INV1_PORT_VP				(PORT7.PODR.BIT.B2)         /* output port of PWM timer(Vp) */
#define     INV1_PORT_WP				(PORT7.PODR.BIT.B3)         /* output port of PWM timer(Wp) */
#define     INV1_PORT_UN				(PORT7.PODR.BIT.B4)         /* output port of PWM timer(Un) */
#define     INV1_PORT_VN				(PORT7.PODR.BIT.B5)         /* output port of PWM timer(Vn) */
#define     INV1_PORT_WN				(PORT7.PODR.BIT.B6)         /* output port of PWM timer(Wn) */

/***********************************************************************************************************************
Typedef definitions (user)
***********************************************************************************************************************/

typedef enum
{
	INV1_U_PWM_VN_ON,
	INV1_U_PWM_WN_ON,
	INV1_V_PWM_UN_ON,
	INV1_V_PWM_WN_ON,
	INV1_W_PWM_UN_ON,
	INV1_W_PWM_VN_ON,
	INV1_UP_ON_V_PWM,
	INV1_UP_ON_W_PWM,
	INV1_VP_ON_U_PWM,
	INV1_VP_ON_W_PWM,
	INV1_WP_ON_U_PWM,
	INV1_WP_ON_V_PWM,
	INV1_FREE,
	INV1_BRAKE
}
inv1_mode_t;

/* End user code. Do not edit comment generated here */
#endif

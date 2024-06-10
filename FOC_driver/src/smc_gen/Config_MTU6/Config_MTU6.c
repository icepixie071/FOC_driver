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
* File Name        : Config_MTU6.c
* Component Version: 1.12.0
* Device(s)        : R5F566TEAxFM
* Description      : This file implements device driver for Config_MTU6.
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "Config_MTU6.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MTU6_Create
* Description  : This function initializes the MTU6 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU6_Create(void)
{
    /* Release MTU channel 6 from stop state */
    MSTP(MTU3) = 0U;

    /* Enable read/write to MTU6 registers */
    MTU.TRWERB.BIT.RWE = 1U;

    /* Stop MTU channel 6 counter */
    MTU.TSTRB.BIT.CST6 = 0U;

    /* Set timer synchronous clear */
    MTU6.TSYCR.BYTE = _00_MTU6_CL0A_OFF | _00_MTU6_CL0B_OFF | _00_MTU6_CL0C_OFF | _00_MTU6_CL0D_OFF | 
                      _00_MTU6_CL1A_OFF | _00_MTU6_CL1B_OFF | _00_MTU6_CL2A_OFF | _00_MTU6_CL2B_OFF;

    /* Set A/D conversion signal output for ADSM0, ADSM1 pins */
    MTU.TADSTRGR0.BYTE = _00_MTU_TADSMEN_DISABLE;
    MTU.TADSTRGR1.BYTE = _00_MTU_TADSMEN_DISABLE;

    /* MTU channel 6 is used as normal mode */
    MTU.TSYRB.BIT.SYNC6 = 0U;
    MTU6.TCR.BYTE = _00_MTU_PCLK_1 | _00_MTU_CKCL_DIS;
    MTU6.TCR2.BYTE = _00_MTU_PCLK_1;
    MTU6.TIER.BYTE = _00_MTU_TGIEA_DISABLE | _00_MTU_TGIEB_DISABLE | _00_MTU_TGIEC_DISABLE | _00_MTU_TGIED_DISABLE | 
                     _00_MTU_TCIEV_DISABLE | _00_MTU_TTGE_DISABLE;
    MTU6.TIORH.BYTE = _00_MTU_IOA_DISABLE | _A0_MTU_IOB_IB;
    MTU6.TIORL.BYTE = _00_MTU_IOC_DISABLE | _00_MTU_IOD_DISABLE;
    MTU6.TMDR1.BYTE = _00_MTU_NORMAL;
    MTU6.TGRA = _2EDF_TGRA6_VALUE;
    MTU6.TGRC = _2EDF_TGRC6_VALUE;
    MTU6.TGRD = _2EDF_TGRD6_VALUE;

    /* Disable read/write to MTU6 registers */
    MTU.TRWERB.BIT.RWE = 0U;

    /* Set MTIOC6B pin */
    MPC.P95PFS.BYTE = 0x01U;
    PORT9.PMR.BYTE |= 0x20U;

    R_Config_MTU6_Create_UserInit();
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU6_Start
* Description  : This function starts the MTU6 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU6_Start(void)
{
    /* Start MTU channel 6 counter */
    MTU.TSTRB.BIT.CST6 = 1U;
}

/***********************************************************************************************************************
* Function Name: R_Config_MTU6_Stop
* Description  : This function stops the MTU6 channel counter
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU6_Stop(void)
{
    /* Stop MTU channel 6 counter */
    MTU.TSTRB.BIT.CST6 = 0U;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

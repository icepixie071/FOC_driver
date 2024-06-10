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
* File Name        : Config_MTU3_MTU4_user.c
* Component Version: 1.11.0
* Device(s)        : R5F566TEAxFM
* Description      : This file implements device driver for Config_MTU3_MTU4.
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
#include "Config_MTU3_MTU4.h"
/* Start user code for include. Do not edit comment generated here */
#include "Config_S12AD0.h"
#include "math.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
/* Start user code for global. Do not edit comment generated here */

void User_MTU3_MTU4_SetInverterMode(inv1_mode_t);
void User_MTU3_MTU4_SetPwmDuty(uint16_t);

void MotorControl(bool);
void HIP(void);

inv1_mode_t mode_debug;

#define  ONEPI		(3.1415926535)			// 1��
#define  TWOPI 		(2.0 * 3.1415926535)	// 2��
#define  Ts0		(0.00005)				// �T���v�����O���� 50[��s]
#define	 Edc0		(24.0)					// �����d��Edc
#define	 R2Edc0		(0.083333333333) 		// = 2.0 / Edc
#define  TH_ofs		(2.094395102393)		// 2�� / 3 [rad]
#define  TH_ofs2	(2.0 * 2.094395102393)	// 2 * (2�� / 3) [rad]

/* �d���E�d�������l�@*/
#define V1_LIM	12.0	// �d���w�ߍő�l[V]
#define I1_LIM	3.1		// �d���w�ߍő�l[A]
#define m1_LIM	1.27	// �ϒ����ő�l 

/* �x�N�g������p�ϐ� */
unsigned short	an000, an001, an002, an107, dumy_buf;
float			iu_ad, iv_ad, iw_ad;
float	vr1_ad;
float	vdc_ad;

float	offset_u = 0.00;		// Iu�̃I�t�Z�b�g�l
float   offset_v = 0.00;		// Iv�̃I�t�Z�b�g�l
float   offset_w = 0.00;		// Iw�̃I�t�Z�b�g�l

float   R1f = 0.307f;  			// ������RR1/��[��]
float	Ldf = 0.000000f; 		// d���C���_�N�^���X[H] ������
float	Lqf	= 0.000062f; 		// q���C���_�N�^���X[H]
float	Kef	= 0.011785f; 		// Ke���d�萔�i��Εϊ��j

float	Nrpm 	= 0;			// ��]��[r/min]
float	Nrpm_1 	= 0;			// ��]��[r/min] old
float	Nrpm_s 	= 0;			// ��]��[r/min]

float	Iac0 	= 0.0;			// a���d��
float	Ibc0 	= 0.0;			// b���d��
float	Idc0 	= 0.0;			// d���d��
float	Iqc0 	= 0.0;			// q���d��
float	Vdc0 	= 0.0;   		// d���d���w��
float	Vqc0 	= 1.0;   		// q���d���w��
float	Vdc1 	= 0.0;   		// d���d���w��
float	Vqc1 	= 0.0;   		// q���d���w��
float	Vac0 	= 0.0;   		// a���d���w��
float	Vbc0 	= 0.0;   		// b���d���w��
float	Vac0_t 	= 0.0;   		// a���d���w�߁i�v�Z�p�e���|�����j
float	Vbc0_t 	= 0.0;   		// b���d���w�߁i�v�Z�p�e���|�����j
float	Vuc0 	= 0.0;			// U���d���w��
float	Vvc0 	= 0.0;			// V���d���w��
float	Vwc0 	= 0.0;			// W���d���w��
float	Vuc1 	= 0.0;			// U���C����d���w��
float	Vvc1 	= 0.0;			// V���C����d���w��
float	Vwc1 	= 0.0;			// W���C����d���w��

/* �Z���T���X */
float	dN_LOW 		= (2000 * 0.00005) / 5.0;	// �����쓮���������C�g
float	N_LOW_MAX 	= 800.0;					// �����쓮���ő��]��
float   Nref_LOW 	= 2.0;

float	dN_HI_P 	= (8000 * 0.00005) / 2.0; 	// �Z���T���X�����������C�g
float	dN_HI_N 	= (8000 * 0.00005) / 5.0; 	// �Z���T���X�����������C�g
float	N_HI_MAX 	= 3000.0;					// �Z���T���X���ő��]��

float   Vu_ad_oft 	= 12.4; 					// Vu���o�l�I�t�Z�b�g
float	Vv_ad_oft 	= 12.4; 					// Vv���o�l�I�t�Z�b�g
float	Vw_ad_oft 	= 12.4; 					// Vw���o�l�I�t�Z�b�g

float	Emd0			= 0.0;					// ���x�N�d��Emd
float	Emq0			= 0.0;					// ���x�N�d��Emq
float	THdc_err_slv	= 0.0;					// ���덷
float	THdc_err_slv0	= 0.0;					// ���덷
float	THdc_err_max	= 1.0471;				// ���덷Max60deg 

unsigned short an003_1, an100_1, an101_1;		// �ߋ��l

/* PLL���� */
float	THdc_err 	= 0.0;		// ����dc���덷
float	kpPLL		= 200.0;	// ���Q�C�� 200[rad/s]
float	kiPLL 		= 0.4;		// �ϕ��Q�C�� 200[rad/s] (Kp/5)��c�~Ts
float	THdcPLL 	= 0.0;  	// PLL�ʑ��p��dc[rad]
float	s_kiPLL 	= 0.0; 		// �ϕ��l�̒��g
float	W1PLL		= 0.0;

unsigned short  PLL_ON = 1;		// �ʑ���PLL�I��


/* �d������ */
float	I_FB 		= 0; 		// ���o�d��
float	I_ref_d 	= 0; 		// d���d���w��
float	I_ref_q 	= 0; 		// q���d���w��
float	I_ref_ASR 	= 0.0;		// ASR�̏o��
float	I_ref_q_ini = 0.0;		// ASR�����l

float	I_ref_LOW 	= 0.0; 						// �ʒu���߁E�����쓮�p
float	I_LOW_MAX 	= 2.7;						// �ʒu���ߍő�d��
float	dI_LOW 		= (2.7 * 0.00005) / 0.5; 	// �ʒu���ߎ���
float	I_REF0 		= 0.5;

float	I_err_d 	= 0.0;		// �����d���덷
float	I_err_q 	= 0.0;		// �����d���덷

//float	kpACR 		= 0.1860;	// ���Q�C�� 800[rad/s] ( Rline=0.35ohm, Lline=0.35mH )
//float	kiACR 		= 0.00930;	// �ϕ��Q�C�� 800[rad/s] ( Rline=0.35ohm, Lline=0.35mH )
float	kpACR 		= 0.09376;	// ���Q�C�� 800[rad/s] ( Rline=0.61ohm, Lline=0.117mH )
float	kiACR 		= 0.02420;	// �ϕ��Q�C�� 800[rad/s] ( Rline=0.61ohm, Lline=0.117mH )

float	s_kiACR_d 	= 0.0; 		// �ϕ��l�̒��g
float	s_kiACR_q 	= 0.0; 		// �ϕ��l�̒��g
float	mtest 		= 0.0;		// �ϒ����w�߃e�X�g�p

float	s_LPF_i 	= 0.0;		// �d�����o�p�C�ꎟ�x��t�B���^
float	k_LPF_i 	= 0.05; 	// =Ts/Tf, Ts=0.00005,Tf=0.001
float	s_LPF_idref = 0.0;		// �ꎟ�x��t�B���^
float	k_LPF_idref = 0.005; 	// =Ts/Tf, Ts=0.00005,Tf=0.01
float	s_LPF_id0 	= 0.0;		// �ꎟ�x��t�B���^
float	k_LPF_id0 	= 0.05; 	// =Ts/Tf, Ts=0.00005,Tf=0.001
float	s_LPF_iq0 	= 0.0;		// �ꎟ�x��t�B���^
float	k_LPF_iq0 	= 0.05; 	// =Ts/Tf, Ts=0.00005,Tf=0.001
float	s_LPF_vd0 	= 0.0;		// �ꎟ�x��t�B���^
float	k_LPF_vd0 	= 0.05; 	// =Ts/Tf, Ts=0.00005,Tf=0.001
float	s_LPF_vq0 	= 0.0;		// �ꎟ�x��t�B���^
float	k_LPF_vq0 	= 0.05; 	// =Ts/Tf, Ts=0.00005,Tf=0.001


/* ���x���� */
float	Nerr = 0.0;				// ���x�덷
float	kpASR = 0.000844;		// ���Q�C�� 10[rad/s]
float	kiASR = 0.0000000844;	// �ϕ��Q�C�� 10[rad/s]

float	s_kiASR = 0.0; 			// �ϕ��풆�g
float	Nref0 = 0.0; 			// �Z���T���X�����l
float	Nrpm_ref = 0.0; 		// ���x�w��[r/min]
float	Nrpm_ref0 = 0.0; 		// ���x�w��[r/min]

float	s_LPF_N3 = 0.0;			// SLV�p���x����ꎟ�x��t�B���^
float	k_LPF_N3 = 0.00250; 	// =Ts/Tf, Ts=0.00005,Tf=0.002

float 	Vzero = 0.0;			// �둊�␳�d��

unsigned char 	DRV_sts		= 0;	//�h���C�u�X�e�[�^�X
unsigned char 	DRV1_end 	= 0; 	// DRV_sts=1�̏I���t���O
unsigned char 	DRV2_end 	= 0; 	// DRV_sts=2�̏I���t���O
unsigned short 	DDRV2_ON 	= 1;	// DRV_sts=2���I��点�Ȃ��ꍇ�́C�h0�h��

unsigned char 	dir_r		= 0;	// �w�߉�]����

float	Fe1		= 0.0;				// �d�C�p���g�� [Hz]
float	We1   	= 0.0;				// �d�C�p���g�� [rad/s]
float	THdc  	= 0.0;				// �����ʑ��p��dc [rad]
float	THdcV 	= 0.0;				// �����ʑ�dq�t�ϊ��p [rad]
float	dTHdc 	= 0.0;				// �T���v�����Ԃ̈ʑ��p ��1�ETs

uint16_t Vru = 0;
uint16_t Vrv = 0;
uint16_t Vrw = 0;

float	SIN0 = 0.0;			// sin�Ƃ̒l
float	COS0 = 0.0;			// cos�Ƃ̒l

/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: R_Config_MTU3_MTU4_Create_UserInit
* Description  : This function adds user code after initializing the MTU3 channel
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void R_Config_MTU3_MTU4_Create_UserInit(void)
{
    /* Start user code for user init. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_MTU3_MTU4_tgia3_interrupt
* Description  : This function is TGIA3 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_PERIA_INTA223
#pragma interrupt r_Config_MTU3_MTU4_tgia3_interrupt(vect=VECT(PERIA,INTA223),fint)
#else
#pragma interrupt r_Config_MTU3_MTU4_tgia3_interrupt(vect=VECT(PERIA,INTA223))
#endif
static void r_Config_MTU3_MTU4_tgia3_interrupt(void)
{
    /* Start user code for r_Config_MTU3_MTU4_tgia3_interrupt. Do not edit comment generated here */
    
	MotorControl(false);
	
	/* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_Config_MTU3_MTU4_c4_tciv4_interrupt
* Description  : This function is TCIV4 interrupt service routine
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

#if FAST_INTERRUPT_VECTOR == VECT_PERIA_INTA232
#pragma interrupt r_Config_MTU3_MTU4_c4_tciv4_interrupt(vect=VECT(PERIA,INTA232),fint)
#else
#pragma interrupt r_Config_MTU3_MTU4_c4_tciv4_interrupt(vect=VECT(PERIA,INTA232))
#endif
static void r_Config_MTU3_MTU4_c4_tciv4_interrupt(void)
{
    /* Start user code for r_Config_MTU3_MTU4_c4_tciv4_interrupt. Do not edit comment generated here */
	
	MotorControl(true);
	
	/* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */

void MotorControl(bool int_v_flg)
{
	PORT9.PODR.BIT.B0 = ~PORT9.PODR.BIT.B0;		// �f�o�b�O�p
	
	/* AD�ϊ��X�^�[�g */
	S12AD.ADCSR.BIT.ADST 	= 1U;
	S12AD1.ADCSR.BIT.ADST 	= 1U;
	
	while (S12AD.ADCSR.BIT.ADST == 1U) { nop(); }
	
	/* �쓮��ԑJ�� */
	if (DRV1_end == 0)
	{
		DRV_sts = 1;
	}
	else
	{
		if (DRV2_end == 0)
		{
			DRV_sts = 2;
		}
		else
		{
			DRV_sts = 3;
		}
	}
	
	/* �d��AD�l��̑I�� */
	if (int_v_flg)
	{
		an000 = S12AD.ADDR0;
		an001 = S12AD.ADDR1;
		an002 = S12AD.ADDR2;
	}
	else
	{
		dumy_buf = S12AD.ADDR0;
		dumy_buf = S12AD.ADDR1;
		dumy_buf = S12AD.ADDR2;
	}
	
	an107 = S12AD1.ADDR7;
	
	/* ��]�����ؑցi���d���j */
	if (dir_r == 0)  
	{
		iu_ad 	= -((float)(an000 - 2048)) / 200.0f - offset_u;
		iv_ad 	= -((float)(an001 - 2048)) / 200.0f - offset_v;
		iw_ad 	= -((float)(an002 - 2048)) / 200.0f - offset_w;
	}
	else
	{
		iv_ad 	= -((float)(an000 - 2048)) / 200.0f - offset_u;
		iu_ad 	= -((float)(an001 - 2048)) / 200.0f - offset_v;
		iw_ad 	= -((float)(an002 - 2048)) / 200.0f - offset_w;		
	}
	
	vdc_ad 	= an107 / 4096.0 * 55.0;
	
	/* Iuvw -> Iab */
	Iac0 = 0.816496580928 	* (iu_ad - 0.5 * (iv_ad + iw_ad));	// Ia = sqrt(2/3) * {Iu - (Iv + Iw) / 2}
	Ibc0 = 0.7071067811866	* (iv_ad - iw_ad);					// Ib = sqrt(1/2) * (Iv - Iw)
	
	SIN0 = sin(THdcPLL);	// ���莲�ʒu���g�p���Čv�Z
	COS0 = cos(THdcPLL);	// ���莲�ʒu���g�p���Čv�Z
	
	/* Iab -> Idq */
	Idc0 =  Iac0 * COS0 + Ibc0 * SIN0;
	Iqc0 = -Iac0 * SIN0 + Ibc0 * COS0;
	
	/* LPF(Id,Iq) */
	s_LPF_id0 += (Idc0 - s_LPF_id0) * k_LPF_id0; 
    s_LPF_iq0 += (Iqc0 - s_LPF_iq0) * k_LPF_iq0; 
	
	
/* ******************************/
/* [ x ]  �@�@���덷����		*/
/* ******************************/
/*
	Emd0 = Vdc1 - R1f * Idc0 + W1PLL * Lqf * Iqc0;	//
	Emq0 = W1PLL * Kef;								//
*/

	Emd0 = Vdc1 - R1f * Idc0 + W1PLL * Lqf * Iqc0;	//
	Emq0 = Vqc1 - R1f * Iqc0 - W1PLL * Lqf * Idc0;	//

	if (Emq0 < 0.01)	Emq0 = 0.01;
  	
//	THdc_err_slv0 = (-Emd0 / Emq0);
	THdc_err_slv0 = atan(-Emd0 / Emq0);
   
    switch (DRV_sts)
    {
		case 0:
			THdc_err_slv = 0.0;
	        break;
        
		case 1:
			THdc_err_slv = 0.0;
			break;
        
		case 2:
			THdc_err_slv = 0.0;
	        break;
        
		case 3:
			THdc_err_slv = THdc_err_slv0;
		
			if (THdc_err_max < THdc_err_slv)
			{
		  		THdc_err_slv = THdc_err_max;
			}
			else
			{
				if ((-THdc_err_max) > THdc_err_slv)	THdc_err_slv = -THdc_err_max;
			}
	        break;
        
		default:
			THdc_err_slv = 0.0;
			break;
	}
  
    s_LPF_N3 += (W1PLL * 2.38732 - s_LPF_N3 ) * k_LPF_N3;	// 2.38732 = (1/(2��)) * (2/Pole) * 60
    
    We1		= W1PLL;  		// �Z���T���X
    dTHdc 	= We1 * Ts0;  	// ��s = ��1�ETs
	
	
/* ******************************/
/* [ x ]  �@�@PLL����			*/
/* ******************************/
	
	/* PLL�w�ߒl�I�� */
    switch (DRV_sts)
    {
        case 0:
			s_kiPLL = 0.0;
			break;
        
		case 1:
			s_kiPLL = 0.0;
	        break;
        
		case 2:
			THdc_err_slv = 0.0;
			s_kiPLL = Nrpm_ref * 0.418879;	// Nrpm * (1/60) * (Pole/2) * 2 * ��
			break;
        
		case 3:
			s_kiPLL += kiPLL * (THdc_err_slv);
			break;
        
		default:
			THdc_err_slv = 0.0;
	        break;
	}
	
	W1PLL 	= s_kiPLL + kpPLL * (THdc_err_slv);	// ����ʃ�1���Z�iPI����j
    THdcPLL = THdcPLL + W1PLL * Ts0;			// ����p��dc = ��dc + (��1 * Ts) �i����d�C�p���X�V�j
    
	 /* �u0 < ��dc < 2�΁v�ɐ��� */
	while(THdcPLL > TWOPI)	THdcPLL -= TWOPI;
    while(THdcPLL < 0.0)	THdcPLL += TWOPI; 
	
	/* LPF�o�b�t�@������ */
	switch (DRV_sts)
	{
		case 0:
			s_LPF_N3 = 0.0;
			break;
		
		case 1:
			s_LPF_N3 = 0.0;
			break;
        	
		case 2:
	        break;
        
		case 3:
	        break;
        
		default: 
			break;
	}
	

/* ******************************/
/* [ x ]  	���x���� (ASR)		*/
/* ******************************/

	Vdc1 = Vdc0; // �O��l�ۑ�
	Vqc1 = Vqc0; // �O��l�ۑ�
	I_FB = Iqc0; // ���o�d�� = q���d��
	
	/* �����쓮���̑��x�w�� */
	if (DRV_sts == 2)
	{
		Nref_LOW += dN_LOW;
	
		if (Nref_LOW > N_LOW_MAX)
		{
			Nref_LOW = N_LOW_MAX;
			DRV2_end = 1 * DDRV2_ON;
		}
    }  

    /* ��]���w�߁C�덷�M���̏��� */
	switch (DRV_sts)
	{
		case 0: 
			Nrpm_ref = 0.0;
			Nerr = 0.0;
	    	break;
	
		case 1: 
			Nrpm_ref = 0.0;
	        Nerr = 0.0;
			break;    
	
		case 2: 
			Nrpm_ref = Nref_LOW;
	        Nerr = 0.0;
			break;
			
		case 3: 
//			Nrpm_ref0 = N_HI_MAX * vr1_ad;
			Nrpm_ref0 = N_HI_MAX;
			
			if (Nref0 > Nrpm_ref0)
			{
				Nref0 -= dN_HI_N;
	    	
				if (Nref0 < Nrpm_ref0)	Nref0 = Nrpm_ref0;
			}
		
			if (Nref0 < Nrpm_ref0)
			{
				Nref0 += dN_HI_P;
			
				if (Nref0 > Nrpm_ref0)	Nref0 = Nrpm_ref0;
			}
			
			if (Nref0 < N_LOW_MAX)	Nref0 = N_LOW_MAX;
			
			Nrpm_ref = Nref0;
			Nerr = Nrpm_ref - s_LPF_N3;	// �t�B���^��   

			break;
	
			default: 
				break;
    }
	
    /* ASR��PI���� */
    s_kiASR += kiASR * Nerr;
	
    if (s_kiASR > I1_LIM)
	{
		s_kiASR = I1_LIM;
	}
    else
    {
		if (s_kiASR < (-I1_LIM))	s_kiASR = -I1_LIM; 
    }

	I_ref_ASR = s_kiASR + kpASR * Nerr;
    
	if (I_ref_ASR > I1_LIM)
	{
		I_ref_ASR = I1_LIM;
	}
	else
	{
		if (I_ref_ASR < (-I1_LIM))	I_ref_ASR = -I1_LIM; 
    }
    
    /* ASR�̏����l */
    if (DRV_sts != 3)
    {
		s_kiASR 	= I_ref_q_ini;
		I_ref_ASR 	= I_ref_q_ini;
    } 
	

/* ******************************/
/* [ x ]  Idq �d������ (ACR)	*/
/* ******************************/

	/* �ʒu���߁E�����쓮���̓d���w�� */
	if ((DRV_sts == 1)||(DRV_sts == 2))	I_ref_LOW += dI_LOW;
	
	if (I_ref_LOW > I_LOW_MAX)
	{
	   I_ref_LOW	= I_LOW_MAX;
	   DRV1_end 	= 1;
	}

	/* �d���w�ߒl�I�� */
    switch (DRV_sts)
	{
		case 0: // �ҋ@���
			I_ref_d 	= 0.0;			// d���d���w�� ������
			I_err_d 	= 0.0;			// d���d���΍� ������
			I_ref_q 	= 0.0;			// q���d���w�� ������
   			I_err_q 	= 0.0;			// q���d���΍� ������
			s_kiACR_q 	= 0.0;			// q��ACR�̐ϕ���N���A
			s_kiASR 	= 0.0;			// ASR�̐ϕ���N���A 
			break;
			
		case 1: // ���ʒu�Œ�
			I_ref_d 	= I_ref_LOW;		// d���d���w��
			I_err_d 	= I_ref_d - Idc0;	// d���d���΍�
			I_ref_q 	= 0.0;				// q���d���w��
			I_err_q 	= I_ref_q - Iqc0;	// q���d���΍�
			break;
			
		case 2: 
			I_ref_d 	= I_ref_LOW;		// d���d���w��
			s_LPF_idref = I_ref_d;			//
			I_err_d 	= I_ref_d - Idc0;	//
			I_ref_q 	= 0.0;				//
			I_err_q 	= I_ref_q - Iqc0;	//
			break;
		
		case 3: 
			s_LPF_idref += (- s_LPF_idref) * k_LPF_idref;	//
			I_ref_d 	= s_LPF_idref;						//
			I_err_d 	= I_ref_d - Idc0;					//
			I_ref_q 	= I_ref_ASR;						//
			I_err_q 	= I_ref_q - Iqc0;					//
			break;
			
		default: 
			I_ref_d		= 0.0;				//
			I_err_d 	= 0.0;				//
			I_ref_q 	= 0.0;				//
			I_err_q 	= 0.0;				//
			s_kiACR_q 	= 0.0;				//
			s_kiASR 	= 0.0;				//
			break;
	}
	
	/* Id ACR */
	s_kiACR_d += kiACR * I_err_d; 						// d���ϕ�����

	if (s_kiACR_d > V1_LIM) 	s_kiACR_d = V1_LIM;		// �ϕ����~�b�^(+)
	if (s_kiACR_d < (-V1_LIM))	s_kiACR_d = -V1_LIM;	// �ϕ����~�b�^(-)
	
	Vdc0 = s_kiACR_d + kpACR * I_err_d; 				// d���d���w��Vd
	
	if (Vdc0 > V1_LIM)		Vdc0 = V1_LIM;				// �ϕ����~�b�^(+)
	if (Vdc0 < (-V1_LIM))	Vdc0 = -V1_LIM;				// �ϕ����~�b�^(-)
	
	/* Iq ACR */
	s_kiACR_q += kiACR * I_err_q; 						// q���ϕ�����

	if (s_kiACR_q > V1_LIM) 	s_kiACR_q = V1_LIM;		// �ϕ����~�b�^(+)
	if (s_kiACR_q < (-V1_LIM))	s_kiACR_q = -V1_LIM;	// �ϕ����~�b�^(-)
	
	Vqc0 = s_kiACR_q + kpACR * I_err_q; 				// q���d���w��Vq
	
	if (Vqc0 > V1_LIM)		Vqc0 = V1_LIM;				// �ϕ����~�b�^(+)
	if (Vqc0 < (-V1_LIM))	Vqc0 = -V1_LIM;				// �ϕ����~�b�^(-)
	
	/* LPF(�d�����o) */
    s_LPF_i += (Iqc0 - s_LPF_i) * k_LPF_i;
	
	
/* ******************************/
/* [ x ]  	PWM�̐ݒ�			*/
/* ******************************/
	
	THdcV = THdcPLL + dTHdc * 0.5;	// ����ݒ肷��dq�t�ϊ��ʑ�
	
	/* �u0 < ��dc < 2�΁v�ɐ��� */
	while(THdcV > TWOPI)	THdcV -= TWOPI;
    while(THdcV < 0.0)		THdcV += TWOPI; 
	
	s_LPF_vd0 += (Vdc0 - s_LPF_vd0) * k_LPF_vd0; 
	s_LPF_vq0 += (Vqc0 - s_LPF_vq0) * k_LPF_vq0; 
    
	SIN0 = sin(THdcV);
	COS0 = cos(THdcV);
	
    /* Vdq -> Vab */
    Vac0 = Vdc0 * COS0 - Vqc0 * SIN0;
    Vbc0 = Vdc0 * SIN0 + Vqc0 * COS0;

    /* Vab -> Vuvw */
    Vac0_t = 0.40824892046 * Vac0;
    Vbc0_t = 0.70710678118 * Vbc0;
	
    Vuc0 = 2.0 * Vac0_t;
    Vvc0 = -Vac0_t + Vbc0_t;
    Vwc0 = -Vac0_t - Vbc0_t;

	/* 3�����g���Z */
	HIP();
	
	/* PWM�g�`�ϊ� */
	Vuc1 += 1.0;
	Vvc1 += 1.0;
	Vwc1 += 1.0;
	
	/* ��]�����ؑցi���d���j */
	if (dir_r == 0)  
	{
		Vru = Vuc1 * 3300;
		Vrv = Vvc1 * 3300;
		Vrw = Vwc1 * 3300;
	}
	else
	{
		Vrv = Vuc1 * 3300;
		Vru = Vvc1 * 3300;
		Vrw = Vwc1 * 3300;
	}
	
	MTU3.TGRB = 6600 - Vru;
    MTU3.TGRD = 6600 - Vru;
	
    MTU4.TGRA = 6600 - Vrv;
    MTU4.TGRC = 6600 - Vrv;
	
    MTU4.TGRB = 6600 - Vrw;
    MTU4.TGRD = 6600 - Vrw;
	
	PORT9.PODR.BIT.B0 = ~PORT9.PODR.BIT.B0;		// �f�o�b�O�p
}

void HIP(void)
{
	float Vmax;	// �ő�d���i��I�ԁj
	float Vmin;	// �ŏ��d���i��I�ԁj
 
	if ( Vuc0 > Vvc0 )
	{
		if (Vuc0 > Vwc0 )	// Max = Vu
		{
			Vmax = Vuc0;
	
			if ( Vvc0 > Vwc0 )
			{
				Vmin = Vwc0;	// Min = Vw
			}
			else
			{
				Vmin = Vvc0;	// Min = Vv
			}
		}
		else
		{
			Vmax = Vwc0;	// Max = Vw
			Vmin = Vvc0;	// Min = Vv
		}	
	}
	else
	{
		if (Vvc0 > Vwc0 )	// Max = Vv
		{
			Vmax = Vvc0;
	
			if ( Vuc0 > Vwc0 )
			{
				Vmin = Vwc0;	// Min = Vw
			}
			else
			{
				Vmin = Vuc0;	// Min = Vu
			}
		}
		else
		{
			Vmax = Vwc0;	// Max = Vw
			Vmin = Vuc0;	// Min = Vu
		}
	}
  
	Vzero = (Vmax + Vmin) * 0.5;	// Max��Min�̕��ϒl
 	Vuc1 = Vuc0 - Vzero;
	Vvc1 = Vvc0 - Vzero;
	Vwc1 = Vwc0 - Vzero;
}

/***********************************************************************************************************************
* Function Name: User_MTU3_MTU4_SetInverterMode
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void User_MTU3_MTU4_SetInverterMode(inv1_mode_t mode)
{
	switch (mode)
	{
		case INV1_W_PWM_VN_ON:
		
			PORT7.PMR.BIT.B1 = 0;						/* Up = GPIO  */
			PORT7.PMR.BIT.B2 = 0;						/* Vp = GPIO  */
			PORT7.PMR.BIT.B3 = 1;						/* Wp = TIMER */
			PORT7.PMR.BIT.B4 = 0;						/* Un = GPIO  */
			PORT7.PMR.BIT.B5 = 0; 						/* Vn = GPIO  */
			PORT7.PMR.BIT.B6 = 1;                       /* Wn = TIMER */
		
			INV1_PORT_UN = 1;                            /* Un = "L" */
			INV1_PORT_VN = 0;                            /* Vn = "H" */
			INV1_PORT_WN = 1;                            /* Wn = "L" */

			break;
		
		case INV1_U_PWM_VN_ON:
		
			PORT7.PMR.BIT.B1 = 1;						/* Up = TIMER */
			PORT7.PMR.BIT.B2 = 0;						/* Vp = GPIO  */
			PORT7.PMR.BIT.B3 = 0;						/* Wp = GPIO  */
			PORT7.PMR.BIT.B4 = 1;						/* Un = TIMER */
			PORT7.PMR.BIT.B5 = 0; 						/* Vn = GPIO  */
			PORT7.PMR.BIT.B6 = 0;                       /* Wn = GPIO  */
			
			INV1_PORT_UN = 1;                            /* Un = "L" */
			INV1_PORT_VN = 0;                            /* Vn = "H" */
			INV1_PORT_WN = 1;                            /* Wn = "L" */
		
			break;

		case INV1_U_PWM_WN_ON:
		
			PORT7.PMR.BIT.B1 = 1;						/* Up = TIMER */
			PORT7.PMR.BIT.B2 = 0;						/* Vp = GPIO  */
			PORT7.PMR.BIT.B3 = 0;						/* Wp = GPIO  */
			PORT7.PMR.BIT.B4 = 1;						/* Un = TIMER */
			PORT7.PMR.BIT.B5 = 0; 						/* Vn = GPIO  */
			PORT7.PMR.BIT.B6 = 0;                       /* Wn = GPIO  */
			
			INV1_PORT_UN = 1;                            /* Un = "L" */
			INV1_PORT_VN = 1;                            /* Vn = "L" */
			INV1_PORT_WN = 0;                            /* Wn = "H" */
		
			break;

		case INV1_V_PWM_WN_ON:
		
			PORT7.PMR.BIT.B1 = 0;						/* Up = GPIO  */
			PORT7.PMR.BIT.B2 = 1;						/* Vp = TIMER */
			PORT7.PMR.BIT.B3 = 0;						/* Wp = GPIO  */
			PORT7.PMR.BIT.B4 = 0;						/* Un = GPIO  */
			PORT7.PMR.BIT.B5 = 1; 						/* Vn = TIMER */
			PORT7.PMR.BIT.B6 = 0;                       /* Wn = GPIO  */
			
			INV1_PORT_UN = 1;                            /* Un = "L" */
			INV1_PORT_VN = 1;                            /* Vn = "L" */
			INV1_PORT_WN = 0;                            /* Wn = "H" */
		
			break;

		case INV1_V_PWM_UN_ON:
		
			PORT7.PMR.BIT.B1 = 0;						/* Up = GPIO  */
			PORT7.PMR.BIT.B2 = 1;						/* Vp = TIMER */
			PORT7.PMR.BIT.B3 = 0;						/* Wp = GPIO  */
			PORT7.PMR.BIT.B4 = 0;						/* Un = GPIO  */
			PORT7.PMR.BIT.B5 = 1; 						/* Vn = TIMER */
			PORT7.PMR.BIT.B6 = 0;                       /* Wn = GPIO  */
			
			INV1_PORT_UN = 0;                            /* Un = "H" */
			INV1_PORT_VN = 1;                            /* Vn = "L" */
			INV1_PORT_WN = 1;                            /* Wn = "L" */
		
			break;
	
		case INV1_W_PWM_UN_ON:
		
			PORT7.PMR.BIT.B1 = 0;						/* Up = GPIO  */
			PORT7.PMR.BIT.B2 = 0;						/* Vp = GPIO  */
			PORT7.PMR.BIT.B3 = 1;						/* Wp = TIMER */
			PORT7.PMR.BIT.B4 = 0;						/* Un = GPIO  */
			PORT7.PMR.BIT.B5 = 0; 						/* Vn = GPIO  */
			PORT7.PMR.BIT.B6 = 1;                       /* Wn = TIMER */
			
			INV1_PORT_UN = 0;                            /* Un = "H" */
			INV1_PORT_VN = 1;                            /* Vn = "L" */
			INV1_PORT_WN = 1;                            /* Wn = "L" */
		
			break;
		
		case INV1_FREE:
		
			PORT7.PMR.BIT.B1 = 0;						/* Up = GPIO  */
			PORT7.PMR.BIT.B2 = 0;						/* Vp = GPIO  */
			PORT7.PMR.BIT.B3 = 0;						/* Wp = GPIO  */
			PORT7.PMR.BIT.B4 = 0;						/* Un = GPIO  */
			PORT7.PMR.BIT.B5 = 0; 						/* Vn = GPIO  */
			PORT7.PMR.BIT.B6 = 0;                       /* Wn = GPIO  */
			
			INV1_PORT_UN = 1;                            /* Un = "L" */
			INV1_PORT_VN = 1;                            /* Vn = "L" */
			INV1_PORT_WN = 1;                            /* Wn = "L" */
		
			break;
		
		case INV1_BRAKE:
		
			PORT7.PMR.BIT.B1 = 0;						/* Up = GPIO  */
			PORT7.PMR.BIT.B2 = 0;						/* Vp = GPIO  */
			PORT7.PMR.BIT.B3 = 0;						/* Wp = GPIO  */
			PORT7.PMR.BIT.B4 = 0;						/* Un = GPIO  */
			PORT7.PMR.BIT.B5 = 0; 						/* Vn = GPIO  */
			PORT7.PMR.BIT.B6 = 0;                       /* Wn = GPIO  */
			
			INV1_PORT_UN = 0;                            /* Un = "H" */
			INV1_PORT_VN = 0;                            /* Vn = "H" */
			INV1_PORT_WN = 0;                            /* Wn = "H" */
		
			break;

		default: 
		
			PORT7.PMR.BIT.B1 = 0;						/* Up = GPIO  */
			PORT7.PMR.BIT.B2 = 0;						/* Vp = GPIO  */
			PORT7.PMR.BIT.B3 = 0;						/* Wp = GPIO  */
			PORT7.PMR.BIT.B4 = 0;						/* Un = GPIO  */
			PORT7.PMR.BIT.B5 = 0; 						/* Vn = GPIO  */
			PORT7.PMR.BIT.B6 = 0;                       /* Wn = GPIO  */
		
			INV1_PORT_UP = 0;                            /* Up = "L" */
			INV1_PORT_VP = 0;                            /* Vp = "L" */
			INV1_PORT_WP = 0;                            /* Wp = "L" */		
			INV1_PORT_UN = 1;                            /* Un = "L" */
			INV1_PORT_VN = 1;                            /* Vn = "L" */
			INV1_PORT_WN = 1;                            /* Wn = "L" */
		
			break;
	}
	
	mode_debug = mode;
}

/***********************************************************************************************************************
* Function Name: User_MTU3_MTU4_SetPwmDuty
* Description  : 
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/

void User_MTU3_MTU4_SetPwmDuty(uint16_t pwm_duty_ref)
{
	if (pwm_duty_ref > MTU3_4_PWM_MAX) { pwm_duty_ref = MTU3_4_PWM_MAX; }
/*	
	MTU3.TGRD = pwm_duty_ref;
	MTU4.TGRC = pwm_duty_ref;
    MTU4.TGRD = pwm_duty_ref;
*/
    MTU3.TGRB = MTU3_4_PWM_MAX - pwm_duty_ref;
    MTU3.TGRD = MTU3_4_PWM_MAX - pwm_duty_ref;
    MTU4.TGRA = MTU3_4_PWM_MAX - pwm_duty_ref;
    MTU4.TGRC = MTU3_4_PWM_MAX - pwm_duty_ref;
    MTU4.TGRB = MTU3_4_PWM_MAX - pwm_duty_ref;
    MTU4.TGRD = MTU3_4_PWM_MAX - pwm_duty_ref;
}

/* End user code. Do not edit comment generated here */

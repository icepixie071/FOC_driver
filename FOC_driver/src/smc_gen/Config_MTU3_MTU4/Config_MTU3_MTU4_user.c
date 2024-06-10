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

#define  ONEPI		(3.1415926535)			// 1π
#define  TWOPI 		(2.0 * 3.1415926535)	// 2π
#define  Ts0		(0.00005)				// サンプリング周期 50[μs]
#define	 Edc0		(24.0)					// 直流電圧Edc
#define	 R2Edc0		(0.083333333333) 		// = 2.0 / Edc
#define  TH_ofs		(2.094395102393)		// 2π / 3 [rad]
#define  TH_ofs2	(2.0 * 2.094395102393)	// 2 * (2π / 3) [rad]

/* 電流・電圧制限値　*/
#define V1_LIM	12.0	// 電圧指令最大値[V]
#define I1_LIM	3.1		// 電流指令最大値[A]
#define m1_LIM	1.27	// 変調律最大値 

/* ベクトル制御用変数 */
unsigned short	an000, an001, an002, an107, dumy_buf;
float			iu_ad, iv_ad, iw_ad;
float	vr1_ad;
float	vdc_ad;

float	offset_u = 0.00;		// Iuのオフセット値
float   offset_v = 0.00;		// Ivのオフセット値
float   offset_w = 0.00;		// Iwのオフセット値

float   R1f = 0.307f;  			// 巻線抵抗R1/相[Ω]
float	Ldf = 0.000000f; 		// d軸インダクタンス[H] 未測定
float	Lqf	= 0.000062f; 		// q軸インダクタンス[H]
float	Kef	= 0.011785f; 		// Ke発電定数（絶対変換）

float	Nrpm 	= 0;			// 回転数[r/min]
float	Nrpm_1 	= 0;			// 回転数[r/min] old
float	Nrpm_s 	= 0;			// 回転数[r/min]

float	Iac0 	= 0.0;			// a軸電流
float	Ibc0 	= 0.0;			// b軸電流
float	Idc0 	= 0.0;			// d軸電流
float	Iqc0 	= 0.0;			// q軸電流
float	Vdc0 	= 0.0;   		// d軸電圧指令
float	Vqc0 	= 1.0;   		// q軸電圧指令
float	Vdc1 	= 0.0;   		// d軸電圧指令
float	Vqc1 	= 0.0;   		// q軸電圧指令
float	Vac0 	= 0.0;   		// a軸電圧指令
float	Vbc0 	= 0.0;   		// b軸電圧指令
float	Vac0_t 	= 0.0;   		// a軸電圧指令（計算用テンポラリ）
float	Vbc0_t 	= 0.0;   		// b軸電圧指令（計算用テンポラリ）
float	Vuc0 	= 0.0;			// U相電圧指令
float	Vvc0 	= 0.0;			// V相電圧指令
float	Vwc0 	= 0.0;			// W相電圧指令
float	Vuc1 	= 0.0;			// U相修正後電圧指令
float	Vvc1 	= 0.0;			// V相修正後電圧指令
float	Vwc1 	= 0.0;			// W相修正後電圧指令

/* センサレス */
float	dN_LOW 		= (2000 * 0.00005) / 5.0;	// 同期駆動時加速レイト
float	N_LOW_MAX 	= 800.0;					// 同期駆動時最大回転数
float   Nref_LOW 	= 2.0;

float	dN_HI_P 	= (8000 * 0.00005) / 2.0; 	// センサレス時動加速レイト
float	dN_HI_N 	= (8000 * 0.00005) / 5.0; 	// センサレス時動加速レイト
float	N_HI_MAX 	= 3000.0;					// センサレス時最大回転数

float   Vu_ad_oft 	= 12.4; 					// Vu検出値オフセット
float	Vv_ad_oft 	= 12.4; 					// Vv検出値オフセット
float	Vw_ad_oft 	= 12.4; 					// Vw検出値オフセット

float	Emd0			= 0.0;					// 速度起電圧Emd
float	Emq0			= 0.0;					// 速度起電圧Emq
float	THdc_err_slv	= 0.0;					// 軸誤差
float	THdc_err_slv0	= 0.0;					// 軸誤差
float	THdc_err_max	= 1.0471;				// 軸誤差Max60deg 

unsigned short an003_1, an100_1, an101_1;		// 過去値

/* PLL制御 */
float	THdc_err 	= 0.0;		// Δθdc軸誤差
float	kpPLL		= 200.0;	// 比例ゲイン 200[rad/s]
float	kiPLL 		= 0.4;		// 積分ゲイン 200[rad/s] (Kp/5)ωc×Ts
float	THdcPLL 	= 0.0;  	// PLL位相角θdc[rad]
float	s_kiPLL 	= 0.0; 		// 積分値の中身
float	W1PLL		= 0.0;

unsigned short  PLL_ON = 1;		// 位相のPLLオン


/* 電流制御 */
float	I_FB 		= 0; 		// 検出電流
float	I_ref_d 	= 0; 		// d軸電流指令
float	I_ref_q 	= 0; 		// q軸電流指令
float	I_ref_ASR 	= 0.0;		// ASRの出力
float	I_ref_q_ini = 0.0;		// ASR初期値

float	I_ref_LOW 	= 0.0; 						// 位置決め・同期駆動用
float	I_LOW_MAX 	= 2.7;						// 位置決め最大電流
float	dI_LOW 		= (2.7 * 0.00005) / 0.5; 	// 位置決め時間
float	I_REF0 		= 0.5;

float	I_err_d 	= 0.0;		// ｑ軸電流誤差
float	I_err_q 	= 0.0;		// ｑ軸電流誤差

//float	kpACR 		= 0.1860;	// 比例ゲイン 800[rad/s] ( Rline=0.35ohm, Lline=0.35mH )
//float	kiACR 		= 0.00930;	// 積分ゲイン 800[rad/s] ( Rline=0.35ohm, Lline=0.35mH )
float	kpACR 		= 0.09376;	// 比例ゲイン 800[rad/s] ( Rline=0.61ohm, Lline=0.117mH )
float	kiACR 		= 0.02420;	// 積分ゲイン 800[rad/s] ( Rline=0.61ohm, Lline=0.117mH )

float	s_kiACR_d 	= 0.0; 		// 積分値の中身
float	s_kiACR_q 	= 0.0; 		// 積分値の中身
float	mtest 		= 0.0;		// 変調率指令テスト用

float	s_LPF_i 	= 0.0;		// 電流検出用，一次遅れフィルタ
float	k_LPF_i 	= 0.05; 	// =Ts/Tf, Ts=0.00005,Tf=0.001
float	s_LPF_idref = 0.0;		// 一次遅れフィルタ
float	k_LPF_idref = 0.005; 	// =Ts/Tf, Ts=0.00005,Tf=0.01
float	s_LPF_id0 	= 0.0;		// 一次遅れフィルタ
float	k_LPF_id0 	= 0.05; 	// =Ts/Tf, Ts=0.00005,Tf=0.001
float	s_LPF_iq0 	= 0.0;		// 一次遅れフィルタ
float	k_LPF_iq0 	= 0.05; 	// =Ts/Tf, Ts=0.00005,Tf=0.001
float	s_LPF_vd0 	= 0.0;		// 一次遅れフィルタ
float	k_LPF_vd0 	= 0.05; 	// =Ts/Tf, Ts=0.00005,Tf=0.001
float	s_LPF_vq0 	= 0.0;		// 一次遅れフィルタ
float	k_LPF_vq0 	= 0.05; 	// =Ts/Tf, Ts=0.00005,Tf=0.001


/* 速度制御 */
float	Nerr = 0.0;				// 速度誤差
float	kpASR = 0.000844;		// 比例ゲイン 10[rad/s]
float	kiASR = 0.0000000844;	// 積分ゲイン 10[rad/s]

float	s_kiASR = 0.0; 			// 積分器中身
float	Nref0 = 0.0; 			// センサレス初期値
float	Nrpm_ref = 0.0; 		// 速度指令[r/min]
float	Nrpm_ref0 = 0.0; 		// 速度指令[r/min]

float	s_LPF_N3 = 0.0;			// SLV用速度推定一次遅れフィルタ
float	k_LPF_N3 = 0.00250; 	// =Ts/Tf, Ts=0.00005,Tf=0.002

float 	Vzero = 0.0;			// 零相補正電圧

unsigned char 	DRV_sts		= 0;	//ドライブステータス
unsigned char 	DRV1_end 	= 0; 	// DRV_sts=1の終わりフラグ
unsigned char 	DRV2_end 	= 0; 	// DRV_sts=2の終わりフラグ
unsigned short 	DDRV2_ON 	= 1;	// DRV_sts=2を終わらせない場合は，”0”に

unsigned char 	dir_r		= 0;	// 指令回転方向

float	Fe1		= 0.0;				// 電気角周波数 [Hz]
float	We1   	= 0.0;				// 電気角周波数 [rad/s]
float	THdc  	= 0.0;				// ｄ軸位相角θdc [rad]
float	THdcV 	= 0.0;				// ｄ軸位相dq逆変換用 [rad]
float	dTHdc 	= 0.0;				// サンプル期間の位相角 ω1・Ts

uint16_t Vru = 0;
uint16_t Vrv = 0;
uint16_t Vrw = 0;

float	SIN0 = 0.0;			// sinθの値
float	COS0 = 0.0;			// cosθの値

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
	PORT9.PODR.BIT.B0 = ~PORT9.PODR.BIT.B0;		// デバッグ用
	
	/* AD変換スタート */
	S12AD.ADCSR.BIT.ADST 	= 1U;
	S12AD1.ADCSR.BIT.ADST 	= 1U;
	
	while (S12AD.ADCSR.BIT.ADST == 1U) { nop(); }
	
	/* 駆動状態遷移 */
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
	
	/* 電流AD値取捨選択 */
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
	
	/* 回転方向切替（相電流） */
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
	
	SIN0 = sin(THdcPLL);	// 推定軸位置を使用して計算
	COS0 = cos(THdcPLL);	// 推定軸位置を使用して計算
	
	/* Iab -> Idq */
	Idc0 =  Iac0 * COS0 + Ibc0 * SIN0;
	Iqc0 = -Iac0 * SIN0 + Ibc0 * COS0;
	
	/* LPF(Id,Iq) */
	s_LPF_id0 += (Idc0 - s_LPF_id0) * k_LPF_id0; 
    s_LPF_iq0 += (Iqc0 - s_LPF_iq0) * k_LPF_iq0; 
	
	
/* ******************************/
/* [ x ]  　　軸誤差推定		*/
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
  
    s_LPF_N3 += (W1PLL * 2.38732 - s_LPF_N3 ) * k_LPF_N3;	// 2.38732 = (1/(2π)) * (2/Pole) * 60
    
    We1		= W1PLL;  		// センサレス
    dTHdc 	= We1 * Ts0;  	// θs = ω1・Ts
	
	
/* ******************************/
/* [ x ]  　　PLL制御			*/
/* ******************************/
	
	/* PLL指令値選択 */
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
			s_kiPLL = Nrpm_ref * 0.418879;	// Nrpm * (1/60) * (Pole/2) * 2 * π
			break;
        
		case 3:
			s_kiPLL += kiPLL * (THdc_err_slv);
			break;
        
		default:
			THdc_err_slv = 0.0;
	        break;
	}
	
	W1PLL 	= s_kiPLL + kpPLL * (THdc_err_slv);	// 操作量ω1演算（PI制御）
    THdcPLL = THdcPLL + W1PLL * Ts0;			// 推定角θdc = θdc + (ω1 * Ts) （推定電気角を更新）
    
	 /* 「0 < θdc < 2π」に制限 */
	while(THdcPLL > TWOPI)	THdcPLL -= TWOPI;
    while(THdcPLL < 0.0)	THdcPLL += TWOPI; 
	
	/* LPFバッファ初期化 */
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
/* [ x ]  	速度制御 (ASR)		*/
/* ******************************/

	Vdc1 = Vdc0; // 前回値保存
	Vqc1 = Vqc0; // 前回値保存
	I_FB = Iqc0; // 検出電流 = q軸電流
	
	/* 同期駆動中の速度指令 */
	if (DRV_sts == 2)
	{
		Nref_LOW += dN_LOW;
	
		if (Nref_LOW > N_LOW_MAX)
		{
			Nref_LOW = N_LOW_MAX;
			DRV2_end = 1 * DDRV2_ON;
		}
    }  

    /* 回転数指令，誤差信号の処理 */
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
			Nerr = Nrpm_ref - s_LPF_N3;	// フィルタ後   

			break;
	
			default: 
				break;
    }
	
    /* ASRのPI制御 */
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
    
    /* ASRの初期値 */
    if (DRV_sts != 3)
    {
		s_kiASR 	= I_ref_q_ini;
		I_ref_ASR 	= I_ref_q_ini;
    } 
	

/* ******************************/
/* [ x ]  Idq 電流制御 (ACR)	*/
/* ******************************/

	/* 位置決め・同期駆動中の電流指令 */
	if ((DRV_sts == 1)||(DRV_sts == 2))	I_ref_LOW += dI_LOW;
	
	if (I_ref_LOW > I_LOW_MAX)
	{
	   I_ref_LOW	= I_LOW_MAX;
	   DRV1_end 	= 1;
	}

	/* 電流指令値選択 */
    switch (DRV_sts)
	{
		case 0: // 待機状態
			I_ref_d 	= 0.0;			// d軸電流指令 初期化
			I_err_d 	= 0.0;			// d軸電流偏差 初期化
			I_ref_q 	= 0.0;			// q軸電流指令 初期化
   			I_err_q 	= 0.0;			// q軸電流偏差 初期化
			s_kiACR_q 	= 0.0;			// q軸ACRの積分器クリア
			s_kiASR 	= 0.0;			// ASRの積分器クリア 
			break;
			
		case 1: // 軸位置固定
			I_ref_d 	= I_ref_LOW;		// d軸電流指令
			I_err_d 	= I_ref_d - Idc0;	// d軸電流偏差
			I_ref_q 	= 0.0;				// q軸電流指令
			I_err_q 	= I_ref_q - Iqc0;	// q軸電流偏差
			break;
			
		case 2: 
			I_ref_d 	= I_ref_LOW;		// d軸電流指令
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
	s_kiACR_d += kiACR * I_err_d; 						// d軸積分処理

	if (s_kiACR_d > V1_LIM) 	s_kiACR_d = V1_LIM;		// 積分リミッタ(+)
	if (s_kiACR_d < (-V1_LIM))	s_kiACR_d = -V1_LIM;	// 積分リミッタ(-)
	
	Vdc0 = s_kiACR_d + kpACR * I_err_d; 				// d軸電圧指令Vd
	
	if (Vdc0 > V1_LIM)		Vdc0 = V1_LIM;				// 積分リミッタ(+)
	if (Vdc0 < (-V1_LIM))	Vdc0 = -V1_LIM;				// 積分リミッタ(-)
	
	/* Iq ACR */
	s_kiACR_q += kiACR * I_err_q; 						// q軸積分処理

	if (s_kiACR_q > V1_LIM) 	s_kiACR_q = V1_LIM;		// 積分リミッタ(+)
	if (s_kiACR_q < (-V1_LIM))	s_kiACR_q = -V1_LIM;	// 積分リミッタ(-)
	
	Vqc0 = s_kiACR_q + kpACR * I_err_q; 				// q軸電圧指令Vq
	
	if (Vqc0 > V1_LIM)		Vqc0 = V1_LIM;				// 積分リミッタ(+)
	if (Vqc0 < (-V1_LIM))	Vqc0 = -V1_LIM;				// 積分リミッタ(-)
	
	/* LPF(電流検出) */
    s_LPF_i += (Iqc0 - s_LPF_i) * k_LPF_i;
	
	
/* ******************************/
/* [ x ]  	PWMの設定			*/
/* ******************************/
	
	THdcV = THdcPLL + dTHdc * 0.5;	// 今回設定するdq逆変換位相
	
	/* 「0 < θdc < 2π」に制限 */
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

	/* 3次調波加算 */
	HIP();
	
	/* PWM波形変換 */
	Vuc1 += 1.0;
	Vvc1 += 1.0;
	Vwc1 += 1.0;
	
	/* 回転方向切替（相電圧） */
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
	
	PORT9.PODR.BIT.B0 = ~PORT9.PODR.BIT.B0;		// デバッグ用
}

void HIP(void)
{
	float Vmax;	// 最大電圧（を選ぶ）
	float Vmin;	// 最小電圧（を選ぶ）
 
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
  
	Vzero = (Vmax + Vmin) * 0.5;	// MaxとMinの平均値
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

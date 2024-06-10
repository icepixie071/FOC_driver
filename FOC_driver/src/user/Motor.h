/***********************************************************************************************************************
* File Name    : Motor.h
* Version      : 1.8.1
* Device(s)    : R5F566TEAxFM
* Description  : 
* Creation Date: 2022-01-08
***********************************************************************************************************************/

#ifndef MOTOR_H
#define MOTOR_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/

#define     R_MTR_PORT_HALL_U			(PORTB.PIDR.BIT.B3)         /* input port of sensor U */
#define     R_MTR_PORT_HALL_V			(PORTB.PIDR.BIT.B2)         /* input port of sensor V */
#define     R_MTR_PORT_HALL_W			(PORTB.PIDR.BIT.B1)         /* input port of sensor W */

#define     L_MTR_PORT_HALL_U			(PORT0.PIDR.BIT.B0)         /* input port of sensor U */
#define     L_MTR_PORT_HALL_V			(PORT2.PIDR.BIT.B2)         /* input port of sensor V */
#define     L_MTR_PORT_HALL_W			(PORT0.PIDR.BIT.B1)         /* input port of sensor W */

#if(1)		/* for 1722RB-06A motor */

#define     MTR_PATTERN_CW_W_V          (1)                 		/* W to V phase */
#define     MTR_PATTERN_CW_U_V          (3)                 		/* U to V phase */
#define     MTR_PATTERN_CW_U_W          (2)                 		/* U to W phase */
#define     MTR_PATTERN_CW_V_W          (6)                 		/* V to W phase */
#define     MTR_PATTERN_CW_V_U          (4)                 		/* V to U phase */
#define     MTR_PATTERN_CW_W_U          (5)                 		/* W to U phase */

#define     MTR_PATTERN_CCW_V_U         (3)                 		/* V to U phase */
#define     MTR_PATTERN_CCW_V_W         (2)                 		/* V to W phase */
#define     MTR_PATTERN_CCW_U_W         (6)                		 	/* U to W phase */
#define     MTR_PATTERN_CCW_U_V         (4)                 		/* U to V phase */
#define     MTR_PATTERN_CCW_W_V         (5)                 		/* W to V phase */
#define     MTR_PATTERN_CCW_W_U         (1)                 		/* W to U phase */

#endif

#if(0)		/* for HPBL18-18R3 motor */

#define     MTR_PATTERN_CW_W_V          (2)                 		/* W to V phase */
#define     MTR_PATTERN_CW_U_V          (6)                 		/* U to V phase */
#define     MTR_PATTERN_CW_U_W          (4)                 		/* U to W phase */
#define     MTR_PATTERN_CW_V_W          (5)                 		/* V to W phase */
#define     MTR_PATTERN_CW_V_U          (1)                 		/* V to U phase */
#define     MTR_PATTERN_CW_W_U          (3)                 		/* W to U phase */

#define     MTR_PATTERN_CCW_V_U         (3)                 		/* V to U phase */
#define     MTR_PATTERN_CCW_V_W         (2)                 		/* V to W phase */
#define     MTR_PATTERN_CCW_U_W         (6)                		 	/* U to W phase */
#define     MTR_PATTERN_CCW_U_V         (4)                 		/* U to V phase */
#define     MTR_PATTERN_CCW_W_V         (5)                 		/* W to V phase */
#define     MTR_PATTERN_CCW_W_U         (1)                 		/* W to U phase */

#endif

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

typedef enum
{
	ROT_FOR, 
	ROT_BACK, 
	FREE,
	BRAKE
}
mot_mode_t;

typedef struct
{
	mot_mode_t ref_mode;
	
	uint16_t ref_curr;
}
st_mot_param_t;
st_mot_param_t st_mot_param_r;
st_mot_param_t st_mot_param_l;

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/

void Mot_R_HallInterrupt(void);
void Mot_L_HallInterrupt(void);
void Mot_CtrlCurrentPI(uint16_t);

#endif


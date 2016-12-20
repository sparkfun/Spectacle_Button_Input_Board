/*******************************************************************************
* File Name: B_2.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_B_2_ALIASES_H) /* Pins B_2_ALIASES_H */
#define CY_PINS_B_2_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define B_2_0			(B_2__0__PC)
#define B_2_0_PS		(B_2__0__PS)
#define B_2_0_PC		(B_2__0__PC)
#define B_2_0_DR		(B_2__0__DR)
#define B_2_0_SHIFT	(B_2__0__SHIFT)
#define B_2_0_INTR	((uint16)((uint16)0x0003u << (B_2__0__SHIFT*2u)))

#define B_2_INTR_ALL	 ((uint16)(B_2_0_INTR))
#define B_2_int1			(B_2__int1__PC)
#define B_2_int1_PS		(B_2__int1__PS)
#define B_2_int1_PC		(B_2__int1__PC)
#define B_2_int1_DR		(B_2__int1__DR)
#define B_2_int1_SHIFT	(B_2__int1__SHIFT)
#define B_2_int1_INTR	((uint16)((uint16)0x0003u << (B_2__0__SHIFT*2u)))


#endif /* End Pins B_2_ALIASES_H */


/* [] END OF FILE */

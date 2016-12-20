/*******************************************************************************
* File Name: B_8.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "B_8.h"

static B_8_BACKUP_STRUCT  B_8_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: B_8_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function must be called for SIO and USBIO
*  pins. It is not essential if using GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet B_8_SUT.c usage_B_8_Sleep_Wakeup
*******************************************************************************/
void B_8_Sleep(void)
{
    #if defined(B_8__PC)
        B_8_backup.pcState = B_8_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            B_8_backup.usbState = B_8_CR1_REG;
            B_8_USB_POWER_REG |= B_8_USBIO_ENTER_SLEEP;
            B_8_CR1_REG &= B_8_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(B_8__SIO)
        B_8_backup.sioState = B_8_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        B_8_SIO_REG &= (uint32)(~B_8_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: B_8_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep().
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to B_8_Sleep() for an example usage.
*******************************************************************************/
void B_8_Wakeup(void)
{
    #if defined(B_8__PC)
        B_8_PC = B_8_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            B_8_USB_POWER_REG &= B_8_USBIO_EXIT_SLEEP_PH1;
            B_8_CR1_REG = B_8_backup.usbState;
            B_8_USB_POWER_REG &= B_8_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(B_8__SIO)
        B_8_SIO_REG = B_8_backup.sioState;
    #endif
}


/* [] END OF FILE */
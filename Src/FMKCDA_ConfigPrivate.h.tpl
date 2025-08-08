/*********************************************************************
 * @file        FMKCDA_ConfigPrivate.h
 * @brief       Template_BriefDescription.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */
  
#ifndef FMKCDA_CONFIGPRIVATE_H_INCLUDED
#define FMKCDA_CONFIGPRIVATE_H_INCLUDED





    // ********************************************************************
    // *                      Includes
    // ********************************************************************
    
    #include "stm32g4xx_hal.h"
    #include "./FMKCDA_ConfigPublic.h"
    // ********************************************************************
    // *                      Defines
    // ********************************************************************
    /* CAUTION : Automatic generated code section for ADC channels number: Start */
    #define FMKCDA_ADC_INTERN_VREFINT_CAL_1_ADDRESS ((volatile t_uint16 *)0x1FFF75AA)
    #define FMKCDA_ADC_INTERN_VREFINT_CAL_2_ADDRESS ((volatile t_uint16 *)0x1FFF75AA)
    #define FMKCDA_ADC_INTERN_VREFINT_CAL_3_ADDRESS ((volatile t_uint16 *)0x1FFF75AA)
    #define FMKCDA_ADC_INTERN_VREFINT_CAL_4_ADDRESS ((volatile t_uint16 *)0x1FFF75AA)
    #define FMKCDA_ADC_INTERN_VREFINT_CAL_5_ADDRESS ((volatile t_uint16 *)0x1FFF75AA)
    #define FMKCDA_ADC_INTERN_VBAT_ADDRESS ((volatile t_uint16*)0x00000000)
    #define FMKCDA_ADC_INTERN_TS_CAL1_ADDRESS ((volatile t_uint16*)0x1FFF75A8)
    #define FMKCDA_ADC_INTERN_TS_CAL2_ADDRESS ((volatile t_uint16*)0x1FFF75CA)
    #define FMKCDA_ADC_1_MAX_CHANNELS ((t_uint8)19)
    #define FMKCDA_ADC_2_MAX_CHANNELS ((t_uint8)19)
    #define FMKCDA_ADC_3_MAX_CHANNELS ((t_uint8)19)
    #define FMKCDA_ADC_4_MAX_CHANNELS ((t_uint8)19)
    #define FMKCDA_ADC_5_MAX_CHANNELS ((t_uint8)19)
    /* CAUTION : Automatic generated code section for ADC channels number: End */

    #define FMKCDA_TIME_BTWN_DIAG_MS ((t_uint16)100)   /**< Time between diagnostic for adc & dac channel in cyclic ope mode*/
    #define FMKCDA_OVR_CONVERSION_MS ((t_uint32)500)    /**< Delay after considering there is no conversion in circular mode  */
    #define FMKCDA_CYCLIC_CALIB      ((t_uint16)2000)   /**< Time between we get the Vref for calibration */

    ///@brief max number of conversion in a cyclic adc conversion
    #define FMKCDA_ADC_MAX_CONVERSION ((t_uint8)16)

    // ********************************************************************
    // *                      Types
    // ********************************************************************
    //-----------------------------ENUM TYPES-----------------------------//
    
    /* CAUTION : Automatic generated code section for Enum: Start */

    /* CAUTION : Automatic generated code section for Enum: End */

    //-----------------------------TYPEDEF TYPES---------------------------//
    
    //-----------------------------STRUCT TYPES---------------------------//
    /* CAUTION : Automatic generated code section for Structure: Start */

    /* CAUTION : Automatic generated code section for Structure: End */
    typedef struct 
    {
        t_eFMKCDA_Adc adc_e;
        t_eFMKCDA_AdcChannel chnl_e;
    } t_sFMKCDA_HwAdcCfg;
    
    ///@brief Adc Configuration Structure
    typedef struct 
    {
        ADC_TypeDef * adcTypedef_ps;
        t_eFMKCPU_ClockPort c_clock_e;
        t_eFMKCPU_IRQNType c_IRQNType_e;
        t_eFMKCPU_DmaRqst c_DmaAdc_e;
    } t_sFMKCDA_AdcCfg;

    ///@brief configuration for internal sensors 
    typedef struct 
    {
        t_sFMKCDA_HwAdcCfg adcCfg_s;
        t_bool isEnable_b;
    } t_sFMKCDA_ADcInternalSnsCfg;
    // ********************************************************************
    // *                      Prototypes
    // ********************************************************************
        
    // ********************************************************************
    // *                      Variables
    // ********************************************************************
    const t_uint32 c_FmkCda_AdcRankTable_ua32[FMKCDA_ADC_MAX_CONVERSION] = {
        ADC_REGULAR_RANK_1,  ADC_REGULAR_RANK_2,  ADC_REGULAR_RANK_3,  ADC_REGULAR_RANK_4,
        ADC_REGULAR_RANK_5,  ADC_REGULAR_RANK_6,  ADC_REGULAR_RANK_7,  ADC_REGULAR_RANK_8,
        ADC_REGULAR_RANK_9,  ADC_REGULAR_RANK_10, ADC_REGULAR_RANK_11, ADC_REGULAR_RANK_12,
        ADC_REGULAR_RANK_13, ADC_REGULAR_RANK_14, ADC_REGULAR_RANK_15, ADC_REGULAR_RANK_16
    };
    /* CAUTION : Automatic generated code section for Variable: Start */
    /**< Variable for Adc Config */
    const t_sFMKCDA_AdcCfg c_FmkCda_AdcCfg_as[FMKCDA_ADC_NB] = {
        [FMKCDA_ADC_1] = {
            .adcTypedef_ps = ADC1,
            .c_clock_e = FMKCPU_RCC_CLK_ADC12,
            .c_IRQNType_e = FMKCPU_NVIC_ADC1_2_IRQN,
            .c_DmaAdc_e = FMKCPU_DMA_RQSTYPE_ADC1,
        },
        [FMKCDA_ADC_2] = {
            .adcTypedef_ps = ADC2,
            .c_clock_e = FMKCPU_RCC_CLK_ADC12,
            .c_IRQNType_e = FMKCPU_NVIC_ADC1_2_IRQN,
            .c_DmaAdc_e = FMKCPU_DMA_RQSTYPE_ADC2,
        },
        [FMKCDA_ADC_3] = {
            .adcTypedef_ps = ADC3,
            .c_clock_e = FMKCPU_RCC_CLK_ADC345,
            .c_IRQNType_e = FMKCPU_NVIC_ADC3_IRQN,
            .c_DmaAdc_e = FMKCPU_DMA_RQSTYPE_ADC3,
        },
        [FMKCDA_ADC_4] = {
            .adcTypedef_ps = ADC4,
            .c_clock_e = FMKCPU_RCC_CLK_ADC345,
            .c_IRQNType_e = FMKCPU_NVIC_ADC4_IRQN,
            .c_DmaAdc_e = FMKCPU_DMA_RQSTYPE_ADC4,
        },
        [FMKCDA_ADC_5] = {
            .adcTypedef_ps = ADC5,
            .c_clock_e = FMKCPU_RCC_CLK_ADC345,
            .c_IRQNType_e = FMKCPU_NVIC_ADC5_IRQN,
            .c_DmaAdc_e = FMKCPU_DMA_RQSTYPE_ADC5,
        },
    };

    /**< Variable for Adc Max channel*/
    const t_uint8 c_FmkCda_AdcMaxChnl_ua8[FMKCDA_ADC_NB] = {
        (t_uint8)FMKCDA_ADC_1_MAX_CHANNELS,
        (t_uint8)FMKCDA_ADC_2_MAX_CHANNELS,
        (t_uint8)FMKCDA_ADC_3_MAX_CHANNELS,
        (t_uint8)FMKCDA_ADC_4_MAX_CHANNELS,
        (t_uint8)FMKCDA_ADC_5_MAX_CHANNELS,
    };

    /**<     Variable for voltage ref calibration value */
    const volatile t_uint16* c_FmkCda_VrefCalibAddress_pas16[FMKCDA_ADC_NB] = {
        (volatile t_uint16 *)FMKCDA_ADC_INTERN_VREFINT_CAL_1_ADDRESS,                     // FMKCDA_ADC_1
        (volatile t_uint16 *)FMKCDA_ADC_INTERN_VREFINT_CAL_2_ADDRESS,                     // FMKCDA_ADC_2
        (volatile t_uint16 *)FMKCDA_ADC_INTERN_VREFINT_CAL_3_ADDRESS,                     // FMKCDA_ADC_3
        (volatile t_uint16 *)FMKCDA_ADC_INTERN_VREFINT_CAL_4_ADDRESS,                     // FMKCDA_ADC_4
        (volatile t_uint16 *)FMKCDA_ADC_INTERN_VREFINT_CAL_5_ADDRESS,                     // FMKCDA_ADC_5
    };

    /**< Variable for Hardware configuration adc and channel for Voltage Reference for each adc */
    const t_sFMKCDA_HwAdcCfg c_FmkCda_HwVrefCfg[FMKCDA_ADC_NB] = {
        {FMKCDA_ADC_1,                         FMKCDA_ADC_CHANNEL_18},                // for FMKCDA_ADC_1
        {FMKCDA_ADC_1,                         FMKCDA_ADC_CHANNEL_18},                // for FMKCDA_ADC_2
        {FMKCDA_ADC_3,                         FMKCDA_ADC_CHANNEL_18},                // for FMKCDA_ADC_3
        {FMKCDA_ADC_4,                         FMKCDA_ADC_CHANNEL_18},                // for FMKCDA_ADC_4
        {FMKCDA_ADC_5,                         FMKCDA_ADC_CHANNEL_18},                // for FMKCDA_ADC_5
    };

    /**< Variable for Interna Sensors configuration*/
    const t_sFMKCDA_ADcInternalSnsCfg c_FmkCda_HwInternalSnsCfg_as[FMKCDA_ADC_INTERN_NB] ={
        {{FMKCDA_ADC_1,                         FMKCDA_ADC_CHANNEL_17},                TRUE                          },// for FMKCDA_ADC_INTERN_VBAT
        {{FMKCDA_ADC_1,                         FMKCDA_ADC_CHANNEL_16},                TRUE                          },// for FMKCDA_ADC_INTERN_TS_CAL1
        {{FMKCDA_ADC_5,                         FMKCDA_ADC_CHANNEL_4},                 FALSE                         },// for FMKCDA_ADC_INTERN_TS_CAL2
    };

    /**< Variable for Internal Sensors Calibration address */
    const volatile t_uint16* c_FmkCda_HwInternalSnsAddress_pau16[FMKCDA_ADC_INTERN_NB] = {
        (volatile t_uint16 *)FMKCDA_ADC_INTERN_VBAT_ADDRESS,                              // FMKCDA_ADC_INTERN_VBAT
        (volatile t_uint16 *)FMKCDA_ADC_INTERN_TS_CAL1_ADDRESS,                           // FMKCDA_ADC_INTERN_TS_CAL1
        (volatile t_uint16 *)FMKCDA_ADC_INTERN_TS_CAL2_ADDRESS,                           // FMKCDA_ADC_INTERN_TS_CAL2
    };

    /* CAUTION : Automatic generated code section for Variable: End */
    //********************************************************************************
    //                      Public functions - Prototyupes
    //********************************************************************************
    
    /* CAUTION : Automatic generated code section for ADCx IRQN_Handler: Start */
    /*********************************
     * ADC1_2_IRQHandler
    *********************************/
    void ADC1_2_IRQHandler(void)
    {
        HAL_ADC_IRQHandler(FMKCDA_PRIVATE_GetHandleTypeDef(FMKCDA_ADC_1));
        HAL_ADC_IRQHandler(FMKCDA_PRIVATE_GetHandleTypeDef(FMKCDA_ADC_2));
        return;
    }
    /*********************************
     * ADC3_IRQHandler
    *********************************/
    void ADC3_IRQHandler(void)
    {
        HAL_ADC_IRQHandler(FMKCDA_PRIVATE_GetHandleTypeDef(FMKCDA_ADC_3));
        return;
    }
    /*********************************
     * ADC4_IRQHandler
    *********************************/
    void ADC4_IRQHandler(void)
    {
        HAL_ADC_IRQHandler(FMKCDA_PRIVATE_GetHandleTypeDef(FMKCDA_ADC_4));
        return;
    }
    /*********************************
     * ADC5_IRQHandler
    *********************************/
    void ADC5_IRQHandler(void)
    {
        HAL_ADC_IRQHandler(FMKCDA_PRIVATE_GetHandleTypeDef(FMKCDA_ADC_5));
        return;
    }
    /* CAUTION : Automatic generated code section for ADCx IRQN_Handler: End */

#endif // FMKCDA_CONFIGPRIVATE_H_INCLUDED           
//************************************************************************************
// End of File
//************************************************************************************

/**
 *
 *	@brief
 *	@note   
 *
 *
 *	@params[in] 
 *	@params[out]
 *	 
 *
 *
 */

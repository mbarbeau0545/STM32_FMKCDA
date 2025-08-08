/*********************************************************************
 * @file        FMKCDA_ConfigPrivate.h
 * @brief       Template_BriefDescription.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */
  
#ifndef FMKCDA_CONFIGPUBLIC_H_INCLUDED
#define FMKCDA_CONFIGPUBLIC_H_INCLUDED





    // ********************************************************************
    // *                      Includes
    // ********************************************************************
    
    // ********************************************************************
    // *                      Defines
    // ********************************************************************
    ///@brief Voltage multiplier to get the actual voltage since it's devided 
    #define FMKCDA_ADC_VBAT_MULTPIPLIER         ((t_uint8)3)

    ///@brief calibration constant 
    #define FMKCDA_ADC_CALIB_VREF           ((t_float32)3.0f)
    #define FMKCDA_ADC_RESOLUTION           ((t_uint16)4095) // since we use 12 bits adc
    #define FMKCDA_ADC_VOLT_PROMILLE        ((t_uint16)1000)
    // ********************************************************************
    // *                      Types
    // ********************************************************************
    //-----------------------------ENUM TYPES-----------------------------//
    /* CAUTION : Automatic generated code section for Enum: Start */
    /**
    * @brief Internal Sensors manage by the cpu.
    */
    typedef enum
    {
        FMKCDA_ADC_INTERN_VBAT = 0,               /**< Refernce to VBAT */
        FMKCDA_ADC_INTERN_TS_CAL1,                  /**< Refernce to TS_CAL1 */
        FMKCDA_ADC_INTERN_TS_CAL2,                  /**< Refernce to TS_CAL2 */
    
        FMKCDA_ADC_INTERN_NB,
    } t_eFMKCDA_AdcInternSns;

    /**
    * @brief Nmber of ADC in this harware.
    */
    typedef enum
    {
        FMKCDA_ADC_1 = 0,                  /**< Reference to HAL ADC1 */
        FMKCDA_ADC_2,                        /**< Reference to HAL ADC2 */
        FMKCDA_ADC_3,                        /**< Reference to HAL ADC3 */
        FMKCDA_ADC_4,                        /**< Reference to HAL ADC4 */
        FMKCDA_ADC_5,                        /**< Reference to HAL ADC5 */
    
        FMKCDA_ADC_NB,
    } t_eFMKCDA_Adc;

    /**
    * @brief  Number of channel in ADC Instances.
    */
    typedef enum
    {
        FMKCDA_ADC_CHANNEL_0 = 0,                  /**< Reference to HAL adc channel 0 */
        FMKCDA_ADC_CHANNEL_1,                        /**< Reference to HAL adc channel 1 */
        FMKCDA_ADC_CHANNEL_2,                        /**< Reference to HAL adc channel 2 */
        FMKCDA_ADC_CHANNEL_3,                        /**< Reference to HAL adc channel 3 */
        FMKCDA_ADC_CHANNEL_4,                        /**< Reference to HAL adc channel 4 */
        FMKCDA_ADC_CHANNEL_5,                        /**< Reference to HAL adc channel 5 */
        FMKCDA_ADC_CHANNEL_6,                        /**< Reference to HAL adc channel 6 */
        FMKCDA_ADC_CHANNEL_7,                        /**< Reference to HAL adc channel 7 */
        FMKCDA_ADC_CHANNEL_8,                        /**< Reference to HAL adc channel 8 */
        FMKCDA_ADC_CHANNEL_9,                        /**< Reference to HAL adc channel 9 */
        FMKCDA_ADC_CHANNEL_10,                       /**< Reference to HAL adc channel 10 */
        FMKCDA_ADC_CHANNEL_11,                       /**< Reference to HAL adc channel 11 */
        FMKCDA_ADC_CHANNEL_12,                       /**< Reference to HAL adc channel 12 */
        FMKCDA_ADC_CHANNEL_13,                       /**< Reference to HAL adc channel 13 */
        FMKCDA_ADC_CHANNEL_14,                       /**< Reference to HAL adc channel 14 */
        FMKCDA_ADC_CHANNEL_15,                       /**< Reference to HAL adc channel 15 */
        FMKCDA_ADC_CHANNEL_16,                       /**< Reference to HAL adc channel 16 */
        FMKCDA_ADC_CHANNEL_17,                       /**< Reference to HAL adc channel 17 */
        FMKCDA_ADC_CHANNEL_18,                       /**< Reference to HAL adc channel 18 */
    
        FMKCDA_ADC_CHANNEL_NB,
    } t_eFMKCDA_AdcChannel;

    /* CAUTION : Automatic generated code section for Enum: End */

    //-----------------------------TYPEDEF TYPES---------------------------//
    //-----------------------------STRUCT TYPES---------------------------//
    /* CAUTION : Automatic generated code section for Structure: Start */

    /* CAUTION : Automatic generated code section for Structure: End */
    // ********************************************************************
    // *                      Prototypes
    // ********************************************************************
        
    // ********************************************************************
    // *                      Variables
    // ********************************************************************
    


    //********************************************************************************
    //                      Public functions - Prototyupes
    //********************************************************************************

#endif // FMKCDA_CONFIGPUBLIC_H_INCLUDED           
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

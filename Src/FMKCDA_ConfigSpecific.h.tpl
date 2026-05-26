/*********************************************************************
 * @file        FMKCDA_ConfigSpecific.h
 * @brief       Template_BriefDescription.
 * @details     TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */
  
#ifndef FMKCDA_CONFIGSPECIFIC_H_INCLUDED
#define FMKCDA_CONFIGSPECIFIC_H_INCLUDED





    // ********************************************************************
    // *                      Includes
    // ********************************************************************
    #include "FMK_HAL/FMK_CDA/Src/FMK_CDA.h"
    #include "../FMKCFG_ConfigFiles/FMKIO_ConfigPublic.h"
    #include "../FMKCFG_ConfigFiles/FMKTIM_ConfigPublic.h"
    // ********************************************************************
    // *                      Defines
    // ********************************************************************
    #if defined(FMKCPU_STM32_ECU_FAMILY_G4)
        #define FMKCDA_ADC_VBAT_MULTPIPLIER ((t_uint8)3)
        #define FMKCDA_ADC_CALIB_VREF       ((t_float32)3.0f)
    #elif defined(FMKCPU_STM32_ECU_FAMILY_H7)
        #define FMKCDA_ADC_VBAT_MULTPIPLIER ((t_uint8)4)
        #define FMKCDA_ADC_CALIB_VREF       ((t_float32)3.3f)
    #else
        #error "FMKCDA ADC calibration configuration is not available for this ECU family"
    #endif

    // ********************************************************************
    // *                      Types
    // ********************************************************************

	//-----------------------------ENUM TYPES-----------------------------//

	//-----------------------------STRUCT TYPES---------------------------//

	//-----------------------------TYPEDEF TYPES---------------------------//
    // ********************************************************************
    // *                      Prototypes
    // ********************************************************************
        
    // ********************************************************************
    // *                      Variables
    // ********************************************************************

    //********************************************************************************
    //                      Public functions - Prototyupes
    //********************************************************************************
    /**
     *
     *	@brief      Fill the ADC init structure with ECU-family specific HAL fields.\n
     *
     *	@param[in]  f_bspAdcInit_ps      : HAL ADC init structure to fill
     *	@param[in]  f_HwAdcCfg_e         : requested ADC hardware mode
     *	@param[in]  f_nbChnlToCfg_u8     : number of regular ADC channels to convert
     *
     *  @retval RC_OK                             @ref RC_OK
     *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
     *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
     *  @retval RC_WARNING_NO_OPERATION           @ref RC_WARNING_NO_OPERATION
     *
    */
    t_eReturnCode FMKCDA_Set_BspAdcInitCfg(ADC_InitTypeDef *f_bspAdcInit_ps,
                                           t_eFMKCDA_HwAdcCfg f_HwAdcCfg_e,
                                           t_uint8 f_nbChnlToCfg_u8);
    /**
     *
     *	@brief      Fill the ADC channel config structure with ECU-family specific HAL fields.\n
     *
     *	@param[in]  f_bspChannelCfg_ps   : HAL ADC channel structure to fill
     *
     *  @retval RC_OK                             @ref RC_OK
     *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
     *
    */
    t_eReturnCode FMKCDA_Set_BspChannelCfg(ADC_ChannelConfTypeDef *f_bspChannelCfg_ps);
    /**
     *
     *	@brief      Convert a raw VREF calibration value to the ADC resolution used by FMKCDA.\n
     *
     *	@param[in]  f_rawCalibValue_u16  : raw calibration value read from system memory
     *
     *  @retval adjusted calibration value
     *
    */
    t_uint16 FMKCDA_Get_VrefCalibRawValue(t_uint16 f_rawCalibValue_u16);
    /**
     *
     *	@brief      Function to get the bsp channel based on the value of f_channel_e.\n
    *
    *	@param[in]  f_channel_e           : enum adc channel, value from @ref t_eFMKCDA_AdcChannel
    *	@param[in]  f_bspChannel_32       : bsp adc channel uint32
    *
    *  @retval RC_OK                             @ref RC_OK
    *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
    *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
    *  @retval RC_ERROR_PARAM_NOT_SUPPORTED      @ref RC_ERROR_PARAM_NOT_SUPPORTED
    *
    */
    t_eReturnCode FMKCDA_Get_BspChannel(t_eFMKCDA_Adc f_Adc_e,
                                        t_eFMKCDA_AdcChannel f_channel_e, 
                                        t_uint32 *f_bspChannel_32);
    /**
     *
     *	@brief      Function to get the bsp channel based on the value of f_channel_e.\n
    *
    *	@param[in]  f_channel_e           : enum adc channel, value from @ref t_eFMKCDA_AdcChannel
    *	@param[in]  f_bspChannel_32       : bsp adc channel uint32
    *
    *  @retval RC_OK                             @ref RC_OK
    *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
    *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
    *  @retval RC_ERROR_PARAM_NOT_SUPPORTED      @ref RC_ERROR_PARAM_NOT_SUPPORTED
    *
    */
    t_eReturnCode FMKCDA_ConvertRawInterSnsValue(t_eFMKCDA_AdcInternSns f_AdcInternSns_e,
                                            t_float32 f_rawAnaMeasure_f32,
                                            t_float32 f_calibValue_f32,
                                            t_float32 *f_snsAnaMeasure_pf32,
                                            const volatile t_uint16 * f_AdcInterSnsCalibStatic_pua16[]);
                                        
#endif // FMKCDA_CONFIGSPECIFIC_H_INCLUDED           
//************************************************************************************
// End of File
//************************************************************************************

/**
 *
 *	@brief
 *	@note
 *
 *
 *	@param[in] 
 *	@param[out]
 *	 
 *
 *
 */

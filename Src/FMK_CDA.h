/**
 * @file        FMK_CDA.h
 * @brief       Template_BriefDescription.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */
  
#ifndef FMKCDA_H_INCLUDED
#define FMKCDA_H_INCLUDED





    // ********************************************************************
    // *                      Includes
    // ********************************************************************
    #include "FMK_CFG/FMKCFG_ConfigFiles/FMKCDA_ConfigPublic.h"
    #include "FMK_CFG/FMKCFG_ConfigFiles/FMKCPU_ConfigPublic.h"
    #include "TypeCommon.h"
    // ********************************************************************
    // *                      Defines
    // ********************************************************************

    // ********************************************************************
    // *                      Types
    // ********************************************************************
    //-----------------------------ENUM TYPES-----------------------------//
    /**< Enum dor configuration of an ADC and her channels */
    typedef enum 
    {
        FMKCDA_ADC_CFG_PERIODIC_DMA,            /**< ADC conversion in periodic dma : multiple channels -> 
                                                    Configure a periodic conversion that will be launch periodically and used dedicate function to know
                                                    the value of the pin*/
        FMKCDA_ADC_CFG_SCAN_DMA,                /**< ADC conversion in scan interrupt : multiple channels ->  DEFAULT ONE
                                                    Every module cyclic, conversion of all channel used is made and user called dediacate function
                                                    to know the value of a channel*/
        FMKCDA_ADC_CFG_TRIGGERED_DMA,            /**< ADC conversion is triggered DMA -> only one channel ->
                                                    The conversion is made on a event, call dedicate function to know the analog value */

        FMKCDA_ADC_CFG_NB,                      /**< ADC conversion available */
    } t_eFMKCDA_HwAdcCfg;

    /**< enum for Channel error status*/
    typedef enum
    {
        FMKCDA_ERRSTATE_OK               = 0x00U,           /**< No error detected on channel */
        FMKCDA_ERRSTATE_ERR_INTERNAL,                       /**< Internal/ bsp error detected on channel */
        FMKCDA_ERRSTATE_ERR_OVR,                            /**< overrun error detetcted on channel */
        FMKCDA_ERRSTATE_ERR_DMA,                            /**< Dma error detetected on channel */
        FMKCDA_ERRSTATE_CB,                                 /**< callback error detetected on channel */
        FMKCDA_ERRSTATE_JQOVF,                              /**< to do */
        FMKCDA_ERRSTATE_PRESENTS,                           /**< Conversio no longer operate when dma mode circular is choosen */
        FMKCDA_ERRSTATE_UNKNOWN,                            /**< Unknown error detected */

    } t_eFMKCDA_ChnlErrState;
    /* CAUTION : Automatic generated code section for Enum: Start */

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
    /**
    *
    *	@brief      Perform all init action to this module
    *
    *
    * @retval RC_OK                             @ref RC_OK
    *
    */
    t_eReturnCode FMKCDA_Init(void);
    /**
    *
    *	@brief      Perform all cyclic action to this module   
    *   @note       Every Cycle in OPE_MODE, this function start Adc conversion 
    *               if the adc is config as so (Interruption or Dma). If a previous conversion 
    *               is finished, this function store the value in the right channel and update 
    *               flag in consequence.\n This function also perform cyclic diagnostic on channel
    *               every x seconds, parameter reference in configPrivate.\n
    * 
    * 
    * @retval RC_OK                               @ref RC_OK
    * @retval RC_WARNING_WRONG_STATE              @ref RC_ERROR_WARNING_STATE
    * @retval RC_WARNING_BUSY                     @ref RC_WARNING_BUSY
    */
    t_eReturnCode FMKCDA_Cyclic(void);
    /**
    *
    *	@brief Function to know the module state 
    *	@param[in]  f_State_pe : store the value, value from @ref t_eCyclicModState
    *
    *   @retval RC_OK                             @ref RC_OK
    *   @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NUL
    */
    t_eReturnCode FMKCDA_GetState(t_eCyclicModState *f_State_pe);
    /**
    *
    *	@brief Function to update the module state 
    *
    *	@param[in]  f_State_e : the new value, value from @ref t_eCyclicModState
    *
    *   @retval RC_OK                             @ref RC_OK
    *   @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NUL
    */
    t_eReturnCode FMKCDA_SetState(t_eCyclicModState f_State_e);
    /**
    *
    *	@brief      Function to add a adc channel configuration to an adc instance
    *   @note       If the Adc config is not congigured yet it will be with f_adcCfg_e.\n
    *               Keep in mind that once the ADC is configured with a config every channel 
    *               of the ADC has the same configuration.\n That means every channel from an ADC 
    *               should have the same hardware config.\n If not, an error is reach.\n
    * 
    *	@param[in]  f_Adc_e               : enum adc, value from @ref t_eFMKCDA_Adc
    *	@param[in]  f_channel_e           : enum adc channel, value from @ref t_eFMKCDA_AdcChannel
    *	@param[in]  f_adcCfg_e            : enum adc config, value from  @ref t_eFMKCDA_HwAdcCfg
    *
    *   @retval RC_OK                               @ref RC_OK
    *   @retval RC_WARNING_WRONG_STATE              @ref RC_ERROR_WARNING_STATE
    *   @retval RC_ERROR_ALREADY_CONFIGURED             @ref RC_ERROR_ALREADY_CONFIGURED
    */
    t_eReturnCode FMKCDA_Set_AdcChannelCfg( t_eFMKCDA_Adc f_Adc_e, 
                                            t_eFMKCDA_AdcChannel f_channel_e,
                                            t_eFMKCDA_HwAdcCfg   f_adcCfg_e);
    /**
    *
    *	@brief      Function to get the analog value from a adc channel
    *   @note       This function has to be used in every adc configuration 
    *               except the ones involving interrupt.\n
    *               if the conversion of adc is done and the value is updated,
    *               this function store the rawAnalog value in f_AnaMeasure_u16.\n
    *               else return 0 and retcode NO_OPERATION
    * 
    *	@param[in]  f_Adc_e               : enum adc, value from @ref t_eFMKCDA_Adc
    *	@param[in]  f_channel_e           : enum adc channel, value from @ref t_eFMKCDA_AdcChannel
    *	@param[in]  f_AnaMeasure_u16            : enum adc config, value from  @ref t_eFMKCDA_HwAdcCfg
    *
    *   @retval RC_OK                               @ref RC_OK
    *   @retval RC_ERROR_PTR_NULL                   @ref RC_ERROR_PTR_NUL
    *   @retval RC_ERROR_WRONG_STATE              @ref RC_ERROR_WRONG_STATE
    *   @retval RC_WARNING_NO_OPERATION             @ref RC_WARNING_NO_OPERATION
    *   @retval RC_ERROR_MODULE_NOT_INITIALIZED     @ref RC_ERROR_MODULE_NOT_INITIALIZED
    */
    t_eReturnCode FMKCDA_Get_AnaChannelMeasure( t_eFMKCDA_Adc f_Adc_e, 
                                                t_eFMKCDA_AdcChannel f_channel_e, 
                                                t_uint16 *f_AnaMeasure_u16);
    /**
    *
    *	@brief      Function to get the error code for a adc_channel
    *
    *	@param[in]  f_adc_e                : enum value for the adc, value from @ref f_adc_e
    *	@param[in]  f_chnlErrInfo_pu16         : storage for channel error as a mask.\n
    *
    *  @retval RC_OK                             @ref RC_OK
    *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
    *  @retval RC_ERROR_PTR_NULL                 @ref RC_ERROR_PTR_NULL
    *
    */
    t_eReturnCode FMKCDA_Get_AdcError(t_eFMKCDA_Adc f_adc_e, t_uint16 *f_chnlErrInfo_pu16);
    /**
    *
    *	@brief      DO NOT USE, Function to get the Handle Type def for IRQN_Handler Function
    *
    *	@param[in]  f_timer_e                : enum value for the timer, value from @ref t_eFMKTIM_Timer
    *
    *
    */
    ADC_HandleTypeDef * FMKCDA_PRIVATE_GetHandleTypeDef(t_eFMKCDA_Adc f_adc_e);
    
#endif // FMKCPU_H_INCLUDED           
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

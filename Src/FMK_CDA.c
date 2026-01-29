/**
 * @file        FMK_CDA.c
 * @brief       Template_BriefDescription.
 * @note        TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */

// ********************************************************************
// *                      Includes
// ********************************************************************
#include "./FMK_CDA.h"
#include "FMK_HAL/FMK_CPU/Src/FMK_CPU.h"
#include "FMK_CFG/FMKCFG_ConfigFiles/FMKCDA_ConfigPrivate.h"
#include "FMK_CFG/FMKCFG_ConfigSpecific/FMKCDA_ConfigSpecific.h"
#include "APP_CTRL/APP_SYS/Src/APP_SYS.h"
#include "3_APP/APP_CTRL/APP_SDM/Src/APP_SDM.h"


// ********************************************************************
// *                      Defines
// ********************************************************************

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

/**< Structure for store and manage analog value in Scan_Dma mode */
typedef struct
{
    t_uint32 rawValue_au32[FMKCDA_ADC_CHANNEL_NB];                   /**< Array for analog value for all channel */
    t_uint16 savedVal_ua16[FMKCDA_ADC_CHANNEL_NB];
    t_eFMKCDA_AdcChannel BspChnlmapp_ae[FMKCDA_ADC_CHANNEL_NB];      /**< Mapping with raywvalue array and channelINfo structure abalog value */    
    t_bool flagOpeRW_b;                                             /**< Flag reading/ writing buffer  */
    t_uint32 lastUpate_u32;                                          /**< time between the last update */
} t_sFMKCDA_AdcBuffer;

/**< Structure for adc channel information*/
typedef struct
{
    t_uint16 rawValue_u16;              /**< the analog value for this channel */
    t_float32 adcValVolt_f32;
    t_bool isConfigured_b;          /**< Flag to know if the channel if configured well */
} t_sFMKCDA_ChnlInfo;

/**< Structure for adc information*/
typedef struct
{
    ADC_HandleTypeDef           bspIsct_s;                              /**< Store the bsp information needed */
    t_eFMKCDA_HwAdcCfg          HwCfg_e;                                /**< Store in which mode the ADC is currently set */
    t_sFMKCDA_ChnlInfo          Channel_as[FMKCDA_ADC_CHANNEL_NB];      /**< Structure channel information for each channel */
    t_eFMKCPU_ClockPort         c_clock_e;                              /**< constant to store the clock for each ADC */
    t_eFMKCPU_IRQNType          c_IRQNType_e;                           /**< constant to store the IRQN for each ADC */
    t_eFMKCPU_DmaRqst           c_DmaAdc_e;
    t_bool                      IsConfigured_b;                         /**< Flag to know if the ADC is configured */
    t_bool                      IsAdcRunning_b;                         /**< Flag to know if the Adc is running a conversion */
    t_bool                      isConversionDone_b;                     /**< flag to know if at least one conversion has been done successfully */
    t_bool                      flagErrDetected_b;                      /**< Flag in DMA/Interrupt mode Error Callback has been call */                 
    t_eFMKCDA_AdcErrState       adcError_e;                            /**< Store the adc error status */
    t_uint32                    lastCbError_u32;                        /**< To know when the last error has been submitted */    
    t_uint32                    mskChnlToCfg_u32;                       /**< mask with all channel to configure */
    t_uint8                     nbChnlToCfg_u8;                         /**< Number of channel to configure */
} t_sFMKCDA_AdcInfo;

typedef struct
{
    t_float32 cabliValue_f32;                               /**< Store the calibration tension for an adc */
    t_bool isValueSet_b;                                    /**< Store wether or not hte calibration has been set */
    t_uint32 lastCalib_u32;                                 /**< Last time the calibration has been done  */
} t_sFMKCDA_AdcCalibInfo;
// ********************************************************************
// *                      Prototypes
// ********************************************************************

// ********************************************************************
// *                      Variables
// ********************************************************************
/* CAUTION : Automatic generated code section for Variable: Start */

/* CAUTION : Automatic generated code section for Variable: End */
/**< Adc Information variable */
static t_sFMKCDA_AdcInfo g_AdcInfo_as[FMKCDA_ADC_NB];
/**< Store calibration information for each adc */
static t_sFMKCDA_AdcCalibInfo g_adcCalibInfo_as[FMKCDA_ADC_NB];

/**< store the raw value for each channel of each adc converter*/
static t_sFMKCDA_AdcBuffer g_AdcBuffer_as[FMKCDA_ADC_NB];

static t_eCyclicModState g_FmkCda_ModState_e = STATE_CYCLIC_CFG;

///@brief counter rank for adc init 
static t_uint8 g_counterRank_au8[FMKCDA_ADC_NB];
//********************************************************************************
//                      Local functions - Prototypes
//********************************************************************************
/**
 *
 *	@brief      Function to set the bsp adc Init.\n
 *  @note       Depending on f_HwAdcCfg_e this function set the bsp Init with the right 
 *              element and call hal_adc_init and set the rcc clock enable.\n 
 *              
 *
 *	@param[in]  f_Adc_e               : enum adc, value from @ref t_eFMKCDA_Adc
 *	@param[in]  f_HwAdcCfg_e          : enum for adc configuration, value from @ref t_eFMKCDA_HwAdcCfg
 *
 * @retval RC_OK                             @ref RC_OK
 * @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 * @retval RC_ERROR_WRONG_STATE              @ref RC_ERROR_WRONG_STATE
 *
 */
static t_eReturnCode s_FMKCDA_Set_BspAdcCfg(t_eFMKCDA_Adc f_Adc_e,
                                             t_eFMKCDA_HwAdcCfg f_HwAdcCfg_e);
/**
 *
 *	@brief      Function to set the bsp channel Init.\n
 *  @note       The ADC config must be set before calling this function.n
 *              This function allow user to configure a adc_channel -> f_channel_e from 
 *              an ADC -> f_Adc_e.\n This function call hal_set_adc_channel.\n
 *              If hardware failed, this function return retcode Wrong_State
 *              
 *
 *	@param[in]  f_Adc_e               : enum adc, value from @ref t_eFMKCDA_Adc
 *	@param[in]  f_channel_e           : enum adc channel, value from @ref t_eFMKCDA_AdcChannel
 *
 * @retval RC_OK                             @ref RC_OK
 * @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 * @retval RC_ERROR_ALREADY_CONFIGURED           @ref RC_ERROR_ALREADY_CONFIGURED
 * @retval RC_ERROR_WRONG_STATE              @ref RC_ERROR_WRONG_STATE
 *
 */
static t_eReturnCode s_FMKCDA_Set_BspChannelCfg(t_eFMKCDA_Adc f_Adc_e, t_eFMKCDA_AdcChannel f_channel_e);
/**
 *
 *	@brief      Perform cyclic operation for this module.\n
 *  @note       Every Cycle in OPE_MODE, this function start Adc conversion 
 *              if the adc is config as so (Interruption or Dma). If a previous conversion 
 *              is finished, this function store the value in the right channel and update 
 *              flag in consequence.\n This function also perform cyclic diagnostic on channel
 *              every x seconds, parameter reference in configPrivate.\n
 *              
 * @retval RC_OK                               @ref RC_OK
 * @retval RC_WARNING_WRONG_STATE              @ref RC_ERROR_WARNING_STATE
 * @retval RC_WARNING_BUSY                     @ref RC_WARNING_BUSY
 *
 */
static t_eReturnCode s_FMKCDA_Operational(void);
/**
 *
 *	@brief      Perform cyclic pre-operation for this module.\n
 *  @note       Make configuration for vref and vtemperature adc channel\n
 *              
 * @retval RC_OK                               @ref RC_OK
 * @retval RC_WARNING_WRONG_STATE              @ref RC_ERROR_WARNING_STATE
 * @retval RC_WARNING_BUSY                     @ref RC_WARNING_BUSY
 *
 */
static t_eReturnCode s_FMKCDA_PreOPerational(void);
/**
 *
 *	@brief      Perform Diagnostic on adc & dac
 *  @note       In basic mode we call HAL_Function to know 
 *              Diag already handle in harware lawyer and HAL_ADC_ErrorCallback implementation 
 *              Manage error.\n
 * 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_WRONG_STATE              @ref RC_ERROR_WRONG_STATE

 */
static t_eReturnCode s_FMKCDA_PerformDiagnostic(t_eFMKCDA_Adc f_adc_e);
/**
 *
 *	@brief      Start adc ocnversion
 *  @note       Depending on hardware configuration, start on adc conversion
 *              either on interrupt either on dma 
 * 
 *  @retval RC_OK                             @ref RC_OK
 *  @retval RC_ERROR_PARAM_INVALID            @ref RC_ERROR_PARAM_INVALID
 *  @retval RC_ERROR_WRONG_STATE              @ref RC_ERROR_WRONG_STATE

 */
static t_eReturnCode s_FMKCDA_StartAdcConversion(t_eFMKCDA_Adc f_Adc_e, t_eFMKCDA_HwAdcCfg f_hwAdc_e);
/**
 *
 *	@brief      Perform cyclic pre-operation for this module.\n
 *  @note       Make configuration for vref and vtemperature adc channel\n
 *              
 * @retval RC_OK                               @ref RC_OK
 * @retval RC_WARNING_WRONG_STATE              @ref RC_ERROR_WARNING_STATE
 * @retval RC_WARNING_BUSY                     @ref RC_WARNING_BUSY
 *
 */
static t_eReturnCode s_FMKCDA_UpdateChannelValue(t_eFMKCDA_Adc f_Adc_e);
/**
 *
 *	@brief      Perform cyclic pre-operation for this module.\n
 *  @note       Make configuration for vref and vtemperature adc channel\n
 *              
 * @retval RC_OK                               @ref RC_OK
 * @retval RC_WARNING_WRONG_STATE              @ref RC_ERROR_WARNING_STATE
 * @retval RC_WARNING_BUSY                     @ref RC_WARNING_BUSY
 *
 */
static t_eReturnCode s_FMKCDA_SetAdcCalibration(t_eFMKCDA_Adc f_Adc_e, t_float32 * f_calibValue_pf32);
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
static t_eReturnCode s_FMKCDA_GetBspAdcChannel( t_eFMKCDA_Adc f_Adc_e,
                                                t_eFMKCDA_AdcChannel f_channel_e, 
                                                t_uint32 *f_bspChannel_32);
//****************************************************************************
//                      Public functions - Implementation
//********************************************************************************
/*********************************
 * FMKCDA_Init
 *********************************/
t_eReturnCode FMKCDA_Init(void)
{
    t_uint8 idxAdc_u8 = 0;
    t_uint8 chnlIndex_u8 = 0;
    t_sFMKCDA_AdcInfo * adcInfo_ps;

    // initiate to default value variable structure
    for (idxAdc_u8 = (t_uint8)0; idxAdc_u8 < (t_uint8)FMKCDA_ADC_NB; idxAdc_u8++)
    { // all adc
        adcInfo_ps = (t_sFMKCDA_AdcInfo *)(&g_AdcInfo_as[idxAdc_u8]);

        adcInfo_ps->IsConfigured_b       = (t_bool)False;
        adcInfo_ps->IsAdcRunning_b       = (t_bool)False;
        adcInfo_ps->isConversionDone_b   = (t_bool)False;
        adcInfo_ps->flagErrDetected_b    = (t_bool)False;
        adcInfo_ps->mskChnlToCfg_u32     = (t_uint32)0;
        adcInfo_ps->nbChnlToCfg_u8       = (t_uint8)0; // 
        adcInfo_ps->c_clock_e = c_FmkCda_AdcCfg_as[idxAdc_u8].c_clock_e;
        adcInfo_ps->c_IRQNType_e = c_FmkCda_AdcCfg_as[idxAdc_u8].c_IRQNType_e;
        adcInfo_ps->c_DmaAdc_e = c_FmkCda_AdcCfg_as[idxAdc_u8].c_DmaAdc_e;
        adcInfo_ps->bspIsct_s.Instance = c_FmkCda_AdcCfg_as[idxAdc_u8].adcTypedef_ps;
        adcInfo_ps->adcError_e = FMKCDA_ERRSTATE_OK;
        
        g_adcCalibInfo_as[idxAdc_u8].cabliValue_f32 = (t_float32)0.0;
        g_adcCalibInfo_as[idxAdc_u8].isValueSet_b = (t_bool)False;
        g_adcCalibInfo_as[idxAdc_u8].lastCalib_u32 = (t_uint32)0;
        
        g_AdcBuffer_as[idxAdc_u8].lastUpate_u32 = (t_uint32)0;
        g_AdcBuffer_as[idxAdc_u8].flagOpeRW_b = (t_bool)False;

        g_counterRank_au8[idxAdc_u8] = (t_uint8)0;

        for (chnlIndex_u8 = (t_uint8)0; chnlIndex_u8 < (t_uint8)FMKCDA_ADC_CHANNEL_NB; chnlIndex_u8++)
        { // all channel for a adc
            adcInfo_ps->Channel_as[chnlIndex_u8].isConfigured_b = (t_bool)False;
            adcInfo_ps->Channel_as[chnlIndex_u8].rawValue_u16 = (t_uint16)0;
            adcInfo_ps->Channel_as[chnlIndex_u8].adcValVolt_f32 = (t_float32)0;

            g_AdcBuffer_as[idxAdc_u8].BspChnlmapp_ae[chnlIndex_u8] = FMKCDA_ADC_CHANNEL_NB;
            g_AdcBuffer_as[idxAdc_u8].rawValue_au32[chnlIndex_u8] = (t_uint32)0;
            
        }
    }

    return RC_OK;
}

/*********************************
 * FMKCDA_Init
 *********************************/
t_eReturnCode FMKCDA_Cyclic(void)
{
    t_eReturnCode Ret_e = RC_OK;

    switch (g_FmkCda_ModState_e)
    {
        case STATE_CYCLIC_CFG:
        {
            g_FmkCda_ModState_e = STATE_CYCLIC_WAITING;
            break;
        }
        case STATE_CYCLIC_WAITING:
        {
            break;
        }
        case STATE_CYCLIC_PREOPE:
        {
            Ret_e = s_FMKCDA_PreOPerational();
            if(Ret_e == RC_OK)
            {
                g_FmkCda_ModState_e = STATE_CYCLIC_OPE;
            }
            else if(Ret_e < RC_OK)
            {
                g_FmkCda_ModState_e = STATE_CYCLIC_ERROR;
            }
            break;
        }
        case STATE_CYCLIC_OPE:
        {
            Ret_e = s_FMKCDA_Operational();
            if(Ret_e < RC_OK)
            {
                g_FmkCda_ModState_e = STATE_CYCLIC_ERROR;
            }
            break;
        }
        
        case STATE_CYCLIC_ERROR:
        {
            break;
        }
        case STATE_CYCLIC_BUSY:
        default:
            Ret_e = RC_OK;
            break;
    }
    return Ret_e;
}

/*********************************
 * FMKCDA_GetState
 *********************************/
t_eReturnCode FMKCDA_GetState(t_eCyclicModState *f_State_pe)
{
    t_eReturnCode Ret_e = RC_OK;
    
    if(f_State_pe == (t_eCyclicModState *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if(Ret_e == RC_OK)
    {
        *f_State_pe = g_FmkCda_ModState_e;
    }

    return Ret_e;
}

/*********************************
 * FMKCDA_SetState
 *********************************/
t_eReturnCode FMKCDA_SetState(t_eCyclicModState f_State_e)
{

    g_FmkCda_ModState_e = f_State_e;
    return RC_OK;
}

/*********************************
 * FMKCDA_Set_AdcChannelCfg
 *********************************/
t_eReturnCode FMKCDA_Set_AdcChannelCfg( t_eFMKCDA_Adc f_Adc_e,
                                        t_eFMKCDA_AdcChannel f_channel_e)
{
    t_eReturnCode Ret_e = RC_OK;

    if (f_Adc_e >= FMKCDA_ADC_NB 
    || f_channel_e >= c_FmkCda_AdcMaxChnl_ua8[f_Adc_e])
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)f_Adc_e);
    }
    else
    {
        //----- depending on hardware configuration make some configuration -----//
        #warning "Only Scan/DMA mode is treated for AdcChannel Configuration"
        //----- Configure Channel -----//
        g_AdcInfo_as[f_Adc_e].nbChnlToCfg_u8++;
        SETBIT_32B(g_AdcInfo_as[f_Adc_e].mskChnlToCfg_u32, (t_uint32)f_channel_e);
    }
    
    return Ret_e;
}

/*********************************
 * FMKCDA_Get_AnaChannelMeasure
 *********************************/
t_eReturnCode FMKCDA_Get_AnaChannelMeasure(t_eFMKCDA_Adc f_Adc_e, t_eFMKCDA_AdcChannel f_channel_e, t_float32 *f_AnaMeasure_pf32)
{
    t_eReturnCode Ret_e = RC_OK;
    t_sFMKCDA_ChnlInfo * chnlInfo_ps;
    t_sFMKCDA_AdcInfo * adcInfo_ps;

    if (f_Adc_e >= FMKCDA_ADC_NB 
    || f_channel_e >= c_FmkCda_AdcMaxChnl_ua8[f_Adc_e])
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)Ret_e);
    }
    if (f_AnaMeasure_pf32 == (t_float32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
        ASSERT((t_uint16)Ret_e);
    }
    if(g_FmkCda_ModState_e != STATE_CYCLIC_OPE)
    {
        Ret_e = RC_WARNING_BUSY;
    }
    if(Ret_e == RC_OK)
    {
        adcInfo_ps = (t_sFMKCDA_AdcInfo *)(&g_AdcInfo_as[f_Adc_e]);
        chnlInfo_ps = (t_sFMKCDA_ChnlInfo *)(&g_AdcInfo_as[f_Adc_e].Channel_as[f_channel_e]);

        if(adcInfo_ps->IsConfigured_b == (t_bool)False
        || chnlInfo_ps->isConfigured_b == (t_bool)False)
        {
            Ret_e = RC_ERROR_MISSING_CONFIG;
            ASSERT((t_uint16)Ret_e);
        }
        if((adcInfo_ps->adcError_e != FMKCDA_ERRSTATE_OK)
        || (adcInfo_ps->isConversionDone_b == (t_bool)FALSE))
        {
            Ret_e = RC_WARNING_BUSY;
        }
        if (Ret_e == RC_OK)
        {            
            *f_AnaMeasure_pf32 = chnlInfo_ps->adcValVolt_f32;
            *f_AnaMeasure_pf32 *= FMKCDA_ADC_VOLT_PROMILLE;
        }
        else 
        {
            *f_AnaMeasure_pf32 = (t_float32)0.0f;
        }
    }
    return Ret_e;
}

/*********************************
 * FMKCDA_Get_AnaInternSnsMeasure
 *********************************/
t_eReturnCode FMKCDA_Get_AnaInternSnsMeasure(   t_eFMKCDA_AdcInternSns f_AdcInternSns_e, 
                                                t_float32 *f_AnaMeasure_pf32)
{
    t_eReturnCode Ret_e;
    t_eFMKCDA_Adc adcInterSn_e;
    t_eFMKCDA_AdcChannel ChnlSnsIntern_e;
    t_sFMKCDA_AdcInfo * adcInfo_ps;
    t_sFMKCDA_ChnlInfo * chnlInfo_ps;
    t_float32 snsAnaMeasure_f32 = 0.0f;

    if(f_AdcInternSns_e >= FMKCDA_ADC_INTERN_NB)
    {
        ASSERT((t_uint16)0);
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    else if (f_AnaMeasure_pf32 == (t_float32 * )NULL)
    {
        ASSERT((t_uint16)0);
        Ret_e = RC_ERROR_PTR_NULL;

    }
    else if(g_FmkCda_ModState_e != STATE_CYCLIC_OPE)
    {
        Ret_e = RC_WARNING_BUSY;
    }
    else 
    {
        Ret_e = RC_OK;
        adcInterSn_e = c_FmkCda_HwInternalSnsCfg_as[f_AdcInternSns_e].adcCfg_s.adc_e;
        ChnlSnsIntern_e = c_FmkCda_HwInternalSnsCfg_as[f_AdcInternSns_e].adcCfg_s.chnl_e;
        adcInfo_ps = (t_sFMKCDA_AdcInfo *)(&g_AdcInfo_as[adcInterSn_e]);
        chnlInfo_ps = (t_sFMKCDA_ChnlInfo *)(&g_AdcInfo_as[adcInterSn_e].Channel_as[ChnlSnsIntern_e]);
        
        if(adcInfo_ps->IsConfigured_b == (t_bool)False
        || chnlInfo_ps->isConfigured_b == (t_bool)False)
        {
            Ret_e = RC_ERROR_MISSING_CONFIG;
            ASSERT((t_uint16)Ret_e);
        }
        if((adcInfo_ps->adcError_e != FMKCDA_ERRSTATE_OK)
        || (adcInfo_ps->isConversionDone_b == (t_bool)FALSE))
        {
            Ret_e = RC_WARNING_BUSY;
        }
        if(Ret_e == RC_OK)
        {
            //--- see if operation has to be made ans raw signal value ----//
            Ret_e = FMKCDA_ConvertRawInterSnsValue( f_AdcInternSns_e,
                                                    chnlInfo_ps->rawValue_u16,
                                                    g_adcCalibInfo_as[adcInterSn_e].cabliValue_f32,
                                                    &snsAnaMeasure_f32,
                                                    c_FmkCda_HwInternalSnsAddress_pau16);
            if(Ret_e == RC_OK)
            {
                *f_AnaMeasure_pf32 = snsAnaMeasure_f32;
            }
            else 
            {
                *f_AnaMeasure_pf32 = 0.0f;
            }
        }
        else 
        {
            *f_AnaMeasure_pf32 = 0.0f;
        }
    }

    return Ret_e;
}
/*********************************
 * FMKCDA_Get_AnaChannelMeasure
 *********************************/
t_eReturnCode FMKCDA_Get_AdcError(t_eFMKCDA_Adc f_adc_e, t_uint16 * f_chnlErrInfo_pu16)
{
    t_eReturnCode Ret_e = RC_OK;

    if(f_adc_e >= FMKCDA_ADC_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)f_adc_e);
    }
    if(f_chnlErrInfo_pu16 == (t_uint16 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
        ASSERT((t_uint16)(*f_chnlErrInfo_pu16));
    }
    if(Ret_e == RC_OK)
    {
        *f_chnlErrInfo_pu16 = g_AdcInfo_as[f_adc_e].adcError_e;
    }
    
    return Ret_e;
}

/*********************************
 * FMKCDA_PRIVATE_GetHandleTypeDef
 *********************************/
ADC_HandleTypeDef * FMKCDA_PRIVATE_GetHandleTypeDef(t_eFMKCDA_Adc f_adc_e)
{
    if(g_AdcInfo_as[f_adc_e].IsConfigured_b == (t_bool)False)
    {
        ASSERT((t_uint16)0);
    }
    return (ADC_HandleTypeDef *)(&g_AdcInfo_as[f_adc_e].bspIsct_s);
}
//********************************************************************************
//                      Local functions - Implementation
//********************************************************************************
/*********************************
 * s_FMKCDA_Operational
 *********************************/
static t_eReturnCode s_FMKCDA_PreOPerational(void)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint8 idxChannel_u8;
    t_uint8 idxAdc_u8 = 0;
    t_sFMKCDA_AdcInfo * adcInfo_ps;
    t_uint8 idxInternSns_u8;

    //----- set configuration channel for Adc Internal Signal 
    //      It appears that sometimes, different ADC are connected 
    //      To the same ADC-CHANNEL for Vref Voltage, In consequence 
    //      When we loop on each Adc to Configure the Channel Link to the Vref 
    //      The function FMKCDA_Set_AdcChannelCfg will return RC_ERROR_ALREADY_CONFIGURED
    //      We consider this state OK -----//
    for(idxAdc_u8 = (t_uint8)0 ; 
           (idxAdc_u8 < FMKCDA_ADC_NB) 
        && (Ret_e == RC_OK) ; 
        idxAdc_u8++)
    {
        adcInfo_ps =  (t_sFMKCDA_AdcInfo *)(&g_AdcInfo_as[idxAdc_u8]);

        //---- check if there is internal sns to add for this adc ----//
        for(idxInternSns_u8 = (t_uint8)0; idxInternSns_u8 < FMKCDA_ADC_INTERN_NB ; idxInternSns_u8++)
        {
            if((c_FmkCda_HwInternalSnsCfg_as[idxInternSns_u8].adcCfg_s.adc_e == (t_eFMKCDA_Adc)idxAdc_u8)
            &&  c_FmkCda_HwInternalSnsCfg_as[idxInternSns_u8].isEnable_b == (t_bool)TRUE)
            {
                idxChannel_u8 = c_FmkCda_HwInternalSnsCfg_as[idxInternSns_u8].adcCfg_s.chnl_e;
                if(GETBIT(adcInfo_ps->mskChnlToCfg_u32, idxChannel_u8) == (BIT_IS_SET_32B))
                {
                    ASSERT((t_uint16)idxChannel_u8);
                    Ret_e = RC_ERROR_WRONG_CONFIG;
                    break;
                }
                else 
                {
                    SETBIT_32B(adcInfo_ps->mskChnlToCfg_u32, (t_uint32)idxChannel_u8);
                    adcInfo_ps->nbChnlToCfg_u8++;
                }

            }
        }
        //---- if there is user channel or Internal Channel configure adc and channel ----//
        if(adcInfo_ps->nbChnlToCfg_u8 > (t_uint32)0)
        {
            //---- check if there is vref channel to configure ----//
            if(c_FmkCda_HwVrefCfg[idxAdc_u8].adc_e == (t_eFMKCDA_Adc)idxAdc_u8)
            {
                idxChannel_u8 = c_FmkCda_HwVrefCfg[idxAdc_u8].chnl_e;

                if(GETBIT(adcInfo_ps->mskChnlToCfg_u32, (t_uint32)idxChannel_u8) == BIT_IS_SET_32B)
                {
                    ASSERT((t_uint16)idxChannel_u8);
                    Ret_e = RC_ERROR_WRONG_CONFIG;
                    break;
                }
                else 
                {
                    SETBIT_32B(adcInfo_ps->mskChnlToCfg_u32, (t_uint32)idxChannel_u8);
                    adcInfo_ps->nbChnlToCfg_u8++;
                }
            }

            //---- first configure the adc instance ----//
            Ret_e = s_FMKCDA_Set_BspAdcCfg((t_eFMKCDA_Adc)idxAdc_u8, FMKCDA_ADC_CFG_SCAN_DMA);

            //---- Configure wanted by user channel ----// 
            if(Ret_e == RC_OK)
            {   
                for(idxChannel_u8 = (t_uint8)0 ; idxChannel_u8 < FMKCDA_ADC_CHANNEL_NB ; idxChannel_u8++)
                {
                    if(GETBIT(adcInfo_ps->mskChnlToCfg_u32, idxChannel_u8) == BIT_IS_SET_32B)
                    {
                        Ret_e = s_FMKCDA_Set_BspChannelCfg((t_eFMKCDA_Adc)idxAdc_u8, (t_eFMKCDA_AdcChannel)idxChannel_u8);
                    }
                }
            }
        }
    }
    
    return Ret_e;
}
/*********************************
 * s_FMKCDA_Operational
 *********************************/
static t_eReturnCode s_FMKCDA_Operational(void)
{
    static t_uint32 s_SavedTime_u32 = 0;

    t_eReturnCode Ret_e = RC_OK;
    t_uint32 currentTime_u32 = 0;
    t_uint8 idxAdc_u8 = 0;
    t_sFMKCDA_AdcInfo * adcInfo_ps;

   FMKCPU_GetTick(&currentTime_u32);

    //------ For every adc in stm32 ------//
    for(idxAdc_u8 = (t_uint8)0 ; idxAdc_u8 < (t_uint8)FMKCDA_ADC_NB ; idxAdc_u8++)
    {
        adcInfo_ps = (t_sFMKCDA_AdcInfo *)(&g_AdcInfo_as[idxAdc_u8]);

        if(adcInfo_ps->IsConfigured_b == (t_bool)true)
        {
            //------ If an Adc Error has been raised, deal with it ------//
            if((adcInfo_ps->flagErrDetected_b == True)
            ||((currentTime_u32 - s_SavedTime_u32) > (t_uint32)FMKCDA_TIME_BTWN_DIAG_MS))
            {
                s_SavedTime_u32 = currentTime_u32;
                Ret_e = s_FMKCDA_PerformDiagnostic((t_eFMKCDA_Adc)idxAdc_u8);
            }

            //------ if the adc is not running and the adc is configured,
            //       launch a conversion only if error_state = NO_ERROR or PRESENTS ------//
            if((adcInfo_ps->IsAdcRunning_b == (t_bool)False)
            && ((adcInfo_ps->adcError_e == FMKCDA_ERRSTATE_OK)
            ||  (adcInfo_ps->adcError_e == FMKCDA_ERRSTATE_PRESENTS)))
            {
                Ret_e = s_FMKCDA_StartAdcConversion((t_eFMKCDA_Adc)idxAdc_u8, g_AdcInfo_as[idxAdc_u8].HwCfg_e);
                //---- busy means the adc is already running, so there is a problem with Dma callback ----//
                if((Ret_e == RC_OK) || (Ret_e == RC_WARNING_BUSY)) 
                {
                    g_AdcInfo_as[idxAdc_u8].IsAdcRunning_b = True;
                    adcInfo_ps->adcError_e = FMKCDA_ERRSTATE_OK;
                }
            }
            else
            {   
                //------ Update Current Time ------//
                FMKCPU_GetTick(&currentTime_u32);
                //------ check last time update to make actions if there is no update from adc
                // also add 5ms in case interrutpion occured during getting the Tick ------//
                if((t_uint32)(currentTime_u32 - g_AdcBuffer_as[idxAdc_u8].lastUpate_u32) > (t_uint32)FMKCDA_OVR_CONVERSION_MS)
                {
                    // update information 
                    adcInfo_ps->IsAdcRunning_b = False;
                    ASSERT((t_uint16)adcInfo_ps->adcError_e);
                    adcInfo_ps->adcError_e =  FMKCDA_ERRSTATE_PRESENTS;
                }
                else 
                {// put the buffer into adc channel block
                    Ret_e = s_FMKCDA_UpdateChannelValue((t_eFMKCDA_Adc)idxAdc_u8);
                }
            
                
            }
        }
    }

    return Ret_e;
}

/*********************************
 * s_FMKCDA_PerformDiagnostic
 *********************************/
static t_eReturnCode s_FMKCDA_StartAdcConversion(t_eFMKCDA_Adc f_Adc_e, t_eFMKCDA_HwAdcCfg f_hwAdc_e)
{
    t_eReturnCode Ret_e = RC_OK;
    HAL_StatusTypeDef bspRet_e = HAL_OK;


    if(f_Adc_e >= FMKCDA_ADC_NB
    || f_hwAdc_e >= FMKCDA_ADC_CFG_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if(Ret_e == RC_OK)
    {
        switch (f_hwAdc_e)
        {
            case FMKCDA_ADC_CFG_SCAN_DMA:
            {
                bspRet_e = HAL_ADC_Start_DMA(&g_AdcInfo_as[f_Adc_e].bspIsct_s,
                                            (t_uint32 *)g_AdcBuffer_as[f_Adc_e].rawValue_au32,
                                            (t_uint32)(g_counterRank_au8[f_Adc_e])); // corresponing to the number of channel 
                                                                        //configured for this adc
                break;                                                        
            }
            case FMKCDA_ADC_CFG_PERIODIC_DMA:
            case FMKCDA_ADC_CFG_TRIGGERED_DMA:
            default:
            {
                Ret_e = RC_WARNING_NO_OPERATION;
            }
                break;
        }
        // sometimes adc is doing something else just wait a sec
        if(bspRet_e == HAL_BUSY)
        {
            Ret_e = RC_WARNING_BUSY;
        }
        else if(bspRet_e != HAL_OK)
        {
            ASSERT((t_uint16)bspRet_e);
            Ret_e = RC_ERROR_WRONG_STATE;
        }
    }

    return Ret_e;
}
/*********************************
 * s_FMKCDA_PerformDiagnostic
 *********************************/
static t_eReturnCode s_FMKCDA_PerformDiagnostic(t_eFMKCDA_Adc f_adc_e)
{
    t_eReturnCode Ret_e = RC_OK;
    t_uint32 adcErr_u32 = HAL_ADC_ERROR_NONE;
    t_sFMKCDA_AdcInfo * adcInfo_ps;
    t_uint32 currentTime_u32;

    adcInfo_ps = (t_sFMKCDA_AdcInfo *)&g_AdcInfo_as[f_adc_e];
    adcErr_u32 = HAL_ADC_GetError(&adcInfo_ps->bspIsct_s);
    FMKCPU_GetTick(&currentTime_u32);
    
    //----- mng mapping error with enum -----//
    if((adcErr_u32 & HAL_ADC_ERROR_NONE) == HAL_ADC_ERROR_NONE)
    {
        adcInfo_ps->adcError_e = FMKCDA_ERRSTATE_OK;
    }
    else if((adcErr_u32 & HAL_ADC_ERROR_OVR) == HAL_ADC_ERROR_OVR)
    {
        adcInfo_ps->adcError_e = FMKCDA_ERRSTATE_OVR;
    }
    else if((adcErr_u32 & HAL_ADC_ERROR_DMA) == HAL_ADC_ERROR_DMA)
    {
        adcInfo_ps->adcError_e = FMKCDA_ERRSTATE_DMA;
    }
    else if((adcErr_u32 & HAL_ADC_ERROR_INTERNAL) == HAL_ADC_ERROR_INTERNAL)
    {
        adcInfo_ps->adcError_e = FMKCDA_ERRSTATE_INTERNAL;
    }
    else if((adcErr_u32 & HAL_ADC_ERROR_JQOVF) == HAL_ADC_ERROR_JQOVF)
    {
        adcInfo_ps->adcError_e = FMKCDA_ERRSTATE_JQOVF;
    }
    else 
    {
        ASSERT((t_uint16)adcErr_u32);
    }
    
    //---- see if errros is still active ----//
    if(adcInfo_ps->adcError_e != FMKCDA_ERRSTATE_OK)
    {
        APPSDM_ReportDiagEvnt(  APPSDM_DIAG_ITEM_FMK_CDA_OPE_ERROR,
                                APPSDM_DIAG_ITEM_REPORT_FAIL,
                                (t_uint16)f_adc_e,
                                (t_uint16)adcInfo_ps->adcError_e);
        //---- reset the serial line state ans see if callback still call us with errors ----//
        if((currentTime_u32 - adcInfo_ps->lastCbError_u32) > 100)
        {
            adcInfo_ps->adcError_e = FMKCDA_ERRSTATE_OK;
        }

    }
    else 
    {
        adcInfo_ps->flagErrDetected_b = (t_bool)FALSE;
        APPSDM_ReportDiagEvnt(  APPSDM_DIAG_ITEM_FMK_CDA_OPE_ERROR,
                                APPSDM_DIAG_ITEM_REPORT_PASS,
                                (t_uint16)f_adc_e,
                                (t_uint16)0);
    }   

    return Ret_e;
}

/*********************************
 * s_FMKCDA_Set_BspAdcCfg
 *********************************/
static t_eReturnCode s_FMKCDA_Set_BspAdcCfg(t_eFMKCDA_Adc f_Adc_e,
                                             t_eFMKCDA_HwAdcCfg f_HwAdcCfg_e)
{
    t_eReturnCode Ret_e = RC_OK;
    HAL_StatusTypeDef BspRet_e = HAL_OK;
    ADC_InitTypeDef * bspAdcInit_s;
    t_sFMKCDA_AdcInfo * adcInfo_ps;

    if (f_Adc_e >= FMKCDA_ADC_NB || f_HwAdcCfg_e >= FMKCDA_ADC_CFG_NB)
    {
        ASSERT((t_uint16)f_HwAdcCfg_e);
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if (Ret_e == RC_OK)
    {
        bspAdcInit_s = (ADC_InitTypeDef *)(&g_AdcInfo_as[f_Adc_e].bspIsct_s.Init);
        adcInfo_ps =  (t_sFMKCDA_AdcInfo *)(&g_AdcInfo_as[f_Adc_e]);

        //----- Generic Configuration -----//
        bspAdcInit_s->ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
        bspAdcInit_s->Overrun = ADC_OVR_DATA_OVERWRITTEN;
        bspAdcInit_s->Resolution = ADC_RESOLUTION_12B;
        bspAdcInit_s->DataAlign = ADC_DATAALIGN_RIGHT;
        bspAdcInit_s->EOCSelection = ADC_EOC_SEQ_CONV;

        //----- Specific Configuration -----//
#ifdef FMKCPU_STM32_ECU_FAMILY_F
        bspAdcInit_s->ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
        bspAdcInit_s->SamplingTimeCommon = ADC_SAMPLETIME_55CYCLES_5; // Valeur par défaut
#elif defined FMKCPU_STM32_ECU_FAMILY_G4
        bspAdcInit_s->ScanConvMode = ADC_SCAN_ENABLE;
        bspAdcInit_s->LowPowerAutoWait = DISABLE; // Désactiver l'attente automatique par défaut
        bspAdcInit_s->SamplingMode = ADC_SAMPLING_MODE_NORMAL; // Mode d'échantillonnage normal
        bspAdcInit_s->GainCompensation = 0; // Pas de compensation de gain par défaut

        //----- Over samppling parameter -----//
        bspAdcInit_s->OversamplingMode = ENABLE;
        bspAdcInit_s->Oversampling.Ratio = ADC_OVERSAMPLING_RATIO_128; // Exemple : suréchantillonnage x16
        bspAdcInit_s->Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_7;
        bspAdcInit_s->Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
        bspAdcInit_s->Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
        bspAdcInit_s->NbrOfConversion = (t_uint32)adcInfo_ps->nbChnlToCfg_u8; 
#else
            #error("Famille STM32 non supportée. Vérifiez la configuration.")
#endif

        // Gestion du mode DMA
        if (FMKCPU_ADC_DMA_MODE == DMA_CIRCULAR) 
        {
            bspAdcInit_s->DMAContinuousRequests = ENABLE;
        } 
        else 
        {
            bspAdcInit_s->DMAContinuousRequests = DISABLE;
        }

        // Gestion des modes ADC
        switch (f_HwAdcCfg_e) 
        {
            case FMKCDA_ADC_CFG_PERIODIC_DMA:
                bspAdcInit_s->ContinuousConvMode = ENABLE;
                bspAdcInit_s->ExternalTrigConv = ADC_SOFTWARE_START;
                bspAdcInit_s->ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
                break;

            case FMKCDA_ADC_CFG_SCAN_DMA:
                bspAdcInit_s->ContinuousConvMode = ENABLE;
                bspAdcInit_s->ExternalTrigConv = ADC_SOFTWARE_START;
                bspAdcInit_s->ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
                break;

            case FMKCDA_ADC_CFG_TRIGGERED_DMA:
                bspAdcInit_s->DiscontinuousConvMode = DISABLE;

#ifdef FMKCPU_STM32_ECU_FAMILY_F
                    bspAdcInit_s->ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC4; // Exemple de déclencheur
#elif defined FMKCPU_STM32_ECU_FAMILY_G4
                    //bspAdcInit_s->ExternalTrigConv = ADC_EXTERNALTRIG1_T21_CC2; // Exemple de déclencheur
#else
                    #error("Famille STM32 non supportée. Vérifiez la configuration.")
#endif
                
                bspAdcInit_s->ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
                break;

            default:
                Ret_e = RC_WARNING_NO_OPERATION;
                break;
        }

        //----- Set hardware clock register to enable -----//
        Ret_e = FMKCPU_Set_HwClock(adcInfo_ps->c_clock_e, FMKCPU_CLOCKPORT_OPE_ENABLE);

        //----- Set NVIC State -----//
        if(Ret_e == RC_OK)
        {
            Ret_e = FMKCPU_Set_NVICState(adcInfo_ps->c_IRQNType_e, FMKCPU_NVIC_OPE_ENABLE);
        }

        //----- Rqst Dma Init -----//
        if(Ret_e == RC_OK)
        {// set NVIC state and Dma Request if DMA is in hardware config
            Ret_e = FMKCPU_RqstDmaInit( adcInfo_ps->c_DmaAdc_e,
                                        FMKCPU_DMA_TYPE_ADC,
                                        (void *)(&adcInfo_ps->bspIsct_s));
        }
        //----- Init hardware ADC -----//
        if (Ret_e == RC_OK)
        {
            BspRet_e = HAL_ADC_Init(&adcInfo_ps->bspIsct_s);

            if (BspRet_e == HAL_OK)
            {
                adcInfo_ps->HwCfg_e = f_HwAdcCfg_e;
                adcInfo_ps->IsConfigured_b = (t_bool)True;
            }
            else
            {
                Ret_e = RC_ERROR_WRONG_STATE;
                ASSERT((t_uint16)BspRet_e);
            }
        }
        else
        {
            ASSERT((t_uint16)Ret_e);
            Ret_e = RC_ERROR_WRONG_STATE;
        }
    }
    return Ret_e;
}

/*********************************
 * s_FMKCDA_Set_BspChannelCfg
 *********************************/
static t_eReturnCode s_FMKCDA_Set_BspChannelCfg(t_eFMKCDA_Adc f_Adc_e, t_eFMKCDA_AdcChannel f_channel_e)
{
    t_eReturnCode Ret_e = RC_OK;
    HAL_StatusTypeDef BspRet_e = HAL_OK;
    t_uint32 bspChannel_u32 = 0;
    ADC_ChannelConfTypeDef BspChannelInit_s;
    
    if(f_channel_e >= FMKCDA_ADC_CHANNEL_NB)
    {
        ASSERT((t_uint16)f_channel_e);
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    else if(g_AdcInfo_as[f_Adc_e].IsConfigured_b == (t_bool)False)
    {
        ASSERT((t_uint16)g_AdcInfo_as[f_Adc_e].IsConfigured_b);
        Ret_e = RC_ERROR_MISSING_CONFIG;
    }
    else if(g_AdcInfo_as[f_Adc_e].Channel_as[f_channel_e].isConfigured_b == (t_bool)True)
    {
        ASSERT((t_uint16)0);
        Ret_e = RC_ERROR_ALREADY_CONFIGURED;
    }
    else if(g_counterRank_au8[f_Adc_e] > (t_uint8)FMKCDA_ADC_MAX_CONVERSION)
    {
        ASSERT((t_uint16)0);
        Ret_e = RC_ERROR_LIMIT_REACHED;
    }
    else
    {
#ifdef FMKCPU_STM32_ECU_FAMILY_F
        BspChannelInit_s.SamplingTime = ADC_SAMPLETIME_13CYCLES_5; // Configuration spécifique à la famille F
        BspChannelInit_s.SingleDiff = ADC_SINGLE_ENDE;           // Single-ended par défaut
        BspChannelInit_s.OffsetNumber = ADC_OFFSET_NONE;         // Pas d'offset initial
        BspChannelInit_s.Offset = 0;                             // Offset à 0
        BspChannelInit_s.OffsetSign = ADC_OFFSET_SIGN_POSITIVE;  // Offset positif par défaut
        BspChannelInit_s.OffsetSaturation = DISABLE;              // Saturation désactivée
#elif defined FMKCPU_STM32_ECU_FAMILY_G4
        BspChannelInit_s.SamplingTime = ADC_SAMPLETIME_247CYCLES_5; // Configuration spécifique à la famille G
        BspChannelInit_s.SingleDiff = ADC_SINGLE_ENDED;           // Single-ended par défaut
        BspChannelInit_s.OffsetNumber = ADC_OFFSET_NONE;        // Pas d'offset initial
        BspChannelInit_s.Offset = 0;                            // Offset à 0
        BspChannelInit_s.OffsetSign = ADC_OFFSET_SIGN_POSITIVE;  // Offset positif par défaut
        BspChannelInit_s.OffsetSaturation = DISABLE;           // Saturation désactivée
#else
        #error("Famille STM32 non supportée. Vérifiez la configuration.")
#endif
        //----- configure channel -----//
        Ret_e = s_FMKCDA_GetBspAdcChannel(f_Adc_e ,f_channel_e, &bspChannel_u32);

        if (Ret_e == RC_OK)
        {
            BspChannelInit_s.Rank = c_FmkCda_AdcRankTable_ua32[g_counterRank_au8[f_Adc_e]];
            BspChannelInit_s.Channel = bspChannel_u32;
            //----- configure adc channel -----//
            BspRet_e = HAL_ADC_ConfigChannel(&g_AdcInfo_as[f_Adc_e].bspIsct_s,
                                            &BspChannelInit_s);

            if (BspRet_e == HAL_OK)
            {
                //----- update mapping for dma -----//
                g_AdcBuffer_as[f_Adc_e].BspChnlmapp_ae[g_counterRank_au8[f_Adc_e]] = f_channel_e;
                g_counterRank_au8[f_Adc_e] += (t_uint8)1;

                //----- update info -----//
                g_AdcInfo_as[f_Adc_e].Channel_as[f_channel_e].isConfigured_b = (t_bool)True;
            }
            else
            {
                Ret_e = RC_ERROR_WRONG_STATE;
                ASSERT((t_uint16)BspRet_e);
            }
        }
    }
    return Ret_e;
}

/******************************************
 * s_FMKCDA_UpdateChannelValue
 *****************************************/
static t_eReturnCode s_FMKCDA_UpdateChannelValue(t_eFMKCDA_Adc f_Adc_e)
{
    /*value (en tension)= rawVal×( VREFINTcalibre / VREFINTmesure) */
    t_eReturnCode Ret_e = RC_OK;
    t_eFMKCDA_AdcChannel chnl_e = FMKCDA_ADC_CHANNEL_NB;
    t_sFMKCDA_AdcInfo * adcInfo_ps = (t_sFMKCDA_AdcInfo *)(&g_AdcInfo_as[f_Adc_e]);
    t_uint8 AdcCtrRank_u8 = (t_uint8)g_counterRank_au8[f_Adc_e];
    t_sFMKCDA_AdcCalibInfo * adcCalib_ps = (t_sFMKCDA_AdcCalibInfo *)(&g_adcCalibInfo_as[f_Adc_e]);
    t_sFMKCDA_AdcBuffer * adcBuffer_ps = (t_sFMKCDA_AdcBuffer *)(&g_AdcBuffer_as[f_Adc_e]);
    t_uint8 LLI_u8;
    t_uint32 currentTime_u32= 0;
    t_float32 calibValue_f32 = (t_float32)0.0f;
    
    FMKCPU_GetTick(&currentTime_u32);

    //---- first copy into saved buffer the raw value give by the dma ----//
    //---- to do so we momently disable ISR ----//
    (void)FMKCPU_Set_NVICState(adcInfo_ps->c_IRQNType_e, FMKCPU_NVIC_OPE_DISABLE);
    for(LLI_u8 = (t_uint8)0 ; LLI_u8 < (t_uint8)(AdcCtrRank_u8) ; LLI_u8++)
    {
        adcBuffer_ps->savedVal_ua16[LLI_u8] = (t_uint16)adcBuffer_ps->rawValue_au32[LLI_u8];
    }
    (void)FMKCPU_Set_NVICState(adcInfo_ps->c_IRQNType_e, FMKCPU_NVIC_OPE_ENABLE);

    //------ update calibration point for this adc if needed ------//
    if((currentTime_u32 - adcCalib_ps->lastCalib_u32) > (t_uint32)FMKCDA_CYCLIC_CALIB
    || adcCalib_ps->isValueSet_b == (t_bool)False)
    {
        adcCalib_ps->lastCalib_u32 = currentTime_u32;
        Ret_e = s_FMKCDA_SetAdcCalibration(f_Adc_e, &calibValue_f32);

        if(Ret_e == RC_OK)
        {
            adcCalib_ps->cabliValue_f32 = calibValue_f32;
        }
        else 
        {
            adcCalib_ps->cabliValue_f32 = 1.0f;
        }
        //------ Update Flag Value Set ------//
        adcCalib_ps->isValueSet_b = (t_bool)True;
    }

    for (LLI_u8 = (t_uint8)0 ; LLI_u8 < AdcCtrRank_u8 ; LLI_u8++)
    {
        chnl_e = adcBuffer_ps->BspChnlmapp_ae[LLI_u8];
        
        //---- raw value update ----//
        adcInfo_ps->Channel_as[chnl_e].rawValue_u16 = 
            (t_uint16)(adcBuffer_ps->savedVal_ua16[LLI_u8]);
        //---- millivolt value update ----//
        adcInfo_ps->Channel_as[chnl_e].adcValVolt_f32 = (t_float32)(
                                                                (t_float32)(adcInfo_ps->Channel_as[chnl_e].rawValue_u16 
                                                                * adcCalib_ps->cabliValue_f32 
                                                                / (t_float32)FMKCDA_ADC_RESOLUTION));

    }
    adcInfo_ps->isConversionDone_b = (t_bool)TRUE;

    return Ret_e;
}

/******************************************
 * s_FMKCDA_SetAdcCalibration
 *****************************************/
static t_eReturnCode s_FMKCDA_SetAdcCalibration(t_eFMKCDA_Adc f_Adc_e, t_float32 * f_calibValue_pf32)
{
    t_eReturnCode Ret_e;
        //---- here we set the pointor not neccessarly to the adc buffer
    //      but the buffer that coutains the calibration value
    //      cause for example adc does not have vref value, he took the one from Adc1 ----//
    t_eFMKCDA_Adc vrefAdc_e; 
    t_uint8 vrefAdcCtrRank_u8;
    t_eFMKCDA_AdcChannel vrefChannel_e;
    t_sFMKCDA_AdcBuffer * vrefAdcBuffer_ps;
    t_uint8 LLI_u8;
    t_uint8 idxBuffChnl_u8;
    t_uint16 staticCalibValue_u16;

    if(f_Adc_e >= FMKCDA_ADC_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)0);
    }
    else if(f_calibValue_pf32 == (t_float32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
        ASSERT((t_uint16)0);
    }
    else 
    {
        Ret_e = RC_OK;
        vrefAdc_e = c_FmkCda_HwVrefCfg[f_Adc_e].adc_e;
        vrefChannel_e = c_FmkCda_HwVrefCfg[f_Adc_e].chnl_e;
        vrefAdcCtrRank_u8= (t_uint8)g_counterRank_au8[vrefAdc_e];
        vrefAdcBuffer_ps = (t_sFMKCDA_AdcBuffer * )(&g_AdcBuffer_as[vrefAdc_e]);

        //------ Retrieve the Bsp Channel associated ------//
        for(LLI_u8 = (t_uint8)0 ; LLI_u8 < vrefAdcCtrRank_u8 ; LLI_u8++)
        {
            if(vrefChannel_e == vrefAdcBuffer_ps->BspChnlmapp_ae[LLI_u8])
            {
                break;
            }
        }
        //---- means we found the channel ----//
        if(LLI_u8 != vrefAdcCtrRank_u8)
        {//                         max rank in buffer, cause it's in reverse
            idxBuffChnl_u8 = (t_uint8)(LLI_u8);
            staticCalibValue_u16 = (t_uint16)*c_FmkCda_VrefCalibAddress_pas16[f_Adc_e];

            if((staticCalibValue_u16 > (t_uint16)0)
            && (vrefAdcBuffer_ps->savedVal_ua16[idxBuffChnl_u8] > (t_uint16)0))
            {
                //                                  the adc verefint value calculate by the adc 
                *f_calibValue_pf32 = (t_float32)(((t_float32)staticCalibValue_u16 
                                                / (t_float32)(vrefAdcBuffer_ps->savedVal_ua16[idxBuffChnl_u8]))
                                                * FMKCDA_ADC_CALIB_VREF);

            }
            else 
            {
                *f_calibValue_pf32 = (t_float32)1.0f;
                //ASSERT((t_uint16)0);
            }
        }
    }

    return Ret_e;
}

/******************************************
 * s_FMKCDA_GetBspAdcChannel
 *****************************************/
static t_eReturnCode s_FMKCDA_GetBspAdcChannel(t_eFMKCDA_Adc f_Adc_e,
                                            t_eFMKCDA_AdcChannel f_channel_e, 
                                            t_uint32 *f_bspChannel_32)
{
    t_eReturnCode Ret_e;
    t_uint8 idxInternSns_u8;
    t_uint32 bspChnlTmp_u32 = (t_uint32)0;

    if (f_channel_e >= FMKCDA_ADC_CHANNEL_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)0);
    }
    if (f_bspChannel_32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
        ASSERT((t_uint16)0);
    }
    else 
    {
        Ret_e = FMKCDA_Get_BspChannel(  f_Adc_e,
                                        f_channel_e,
                                        &bspChnlTmp_u32);
        //---- specific channel managment ----//
        if(Ret_e == RC_OK)
        {
            *f_bspChannel_32 = bspChnlTmp_u32; 
            //---- check internal sensors adc channel ----//
            for(idxInternSns_u8 = (t_uint8)0 ; idxInternSns_u8 < FMKCDA_ADC_INTERN_NB ; idxInternSns_u8++)
            {
                if( (c_FmkCda_HwInternalSnsCfg_as[idxInternSns_u8].isEnable_b == (t_bool)TRUE)
                &&  (c_FmkCda_HwInternalSnsCfg_as[idxInternSns_u8].adcCfg_s.adc_e == f_Adc_e)
                &&  (c_FmkCda_HwInternalSnsCfg_as[idxInternSns_u8].adcCfg_s.chnl_e == f_channel_e))
                {
                    *f_bspChannel_32 |= ADC_CHANNEL_ID_INTERNAL_CH;
                }
            }

            //---- check vref adc channel ----//
            if((c_FmkCda_HwVrefCfg[f_Adc_e].adc_e == f_Adc_e)
            && (c_FmkCda_HwVrefCfg[f_Adc_e].chnl_e == f_channel_e))
            {
                *f_bspChannel_32 |= ADC_CHANNEL_ID_INTERNAL_CH;
            }
        }
        else 
        {
            *f_bspChannel_32 = (t_uint32)0;
        }
    }

    return Ret_e;
}
//********************************************************************************
//                      HAL_Callback Implementation
//********************************************************************************
/******************************************
 * BSP CALLBACK IMPLEMENTATION
 *****************************************/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    t_uint8 idxAdc_u8 = 0;
    t_eFMKCDA_Adc IT_Adc_e = FMKCDA_ADC_NB;

    //------ Find the Adc ------//
    for (idxAdc_u8 = (t_uint8)0; idxAdc_u8 < (t_uint8)FMKCDA_ADC_NB; idxAdc_u8++)
    {
        if (&g_AdcInfo_as[idxAdc_u8].bspIsct_s == (ADC_HandleTypeDef *)hadc)
        {
            IT_Adc_e = (t_eFMKCDA_Adc)idxAdc_u8;
            break;
        }
    }
    
    if (IT_Adc_e < FMKCDA_ADC_NB)
    {
        //------ update last time the value has been changed and reset bit present error ------//
        FMKCPU_GetTick(&g_AdcBuffer_as[IT_Adc_e].lastUpate_u32);
        //------ reset present bit ------//
        if(g_AdcInfo_as[IT_Adc_e].adcError_e == FMKCDA_ERRSTATE_PRESENTS)
        {
            g_AdcInfo_as[IT_Adc_e].adcError_e = FMKCDA_ERRSTATE_OK;
        }
        
    }
    return;
}

/**
 *
 *	@brief      CallBack function called when adc in DMA or Interrupt in HalfDma.
 *  @note       Update flag last update.\n
 *             
 */
/*********************************
 * HAL_ADC_ConvHalfCpltCallback
 *********************************/
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
    return;
}
/**
 *
 *	@brief      CallBack function called when adc in DMa or Interrupt mdode
 *  @note       Update flag error detected.\n
 *             
 */
/*********************************
 * HAL_ADC_ErrorCallback
 *********************************/
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc)
{
    t_uint8 LLI_u8;

    // find enum adc corresponding
    for(LLI_u8 = (t_uint8)0 ; LLI_u8 < FMKCDA_ADC_NB ; LLI_u8++)
    {
        if(&g_AdcInfo_as[LLI_u8].bspIsct_s == hadc)
        {
            break;
        }
    }
    if(LLI_u8 < FMKCDA_ADC_NB)
    {
        FMKCPU_GetTick(&g_AdcInfo_as[LLI_u8].lastCbError_u32);
        if(g_AdcInfo_as[LLI_u8].flagErrDetected_b == (t_bool)FALSE)
        {
            g_AdcInfo_as[LLI_u8].flagErrDetected_b = (t_bool)TRUE;
        }
    }
    return;
}

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

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
    t_bool FlagValueUpdated_b;          /**< Flag to know when the rawvalue is available */
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
    t_bool                      IsConfigured_b;                      /**< Flag to know if the ADC is configured */
    t_bool                      IsAdcRunning_b;                         /**< Flag to know if the Adc is running a conversion */
    t_bool                      flagErrDetected_b;                      /**< Flag in DMA/Interrupt mode Error Callback has been call */                 
    t_uint16                    Error_u16;                                /**< Store the adc error status */
} t_sFMKCDA_AdcInfo;

typedef struct
{
    t_float32 cabliValue_f32;                               /**< Store the calibration tension for an adc */
    t_bool isValueSet_b;                                    /**< Store wether or not hte calibration has been set */
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
//****************************************************************************
//                      Public functions - Implementation
//********************************************************************************
/*********************************
 * FMKCDA_Init
 *********************************/
t_eReturnCode FMKCDA_Init(void)
{
    t_uint8 adcIndex_u8 = 0;
    t_uint8 chnlIndex_u8 = 0;
    t_sFMKCDA_AdcInfo * adcInfo_ps;

    // initiate to default value variable structure
    for (adcIndex_u8 = (t_uint8)0; adcIndex_u8 < (t_uint8)FMKCDA_ADC_NB; adcIndex_u8++)
    { // all adc
        adcInfo_ps = (t_sFMKCDA_AdcInfo *)(&g_AdcInfo_as[adcIndex_u8]);

        adcInfo_ps->IsConfigured_b       = (t_bool)False;
        adcInfo_ps->IsAdcRunning_b       = (t_bool)False;
        adcInfo_ps->flagErrDetected_b    = (t_bool)False;
        adcInfo_ps->c_clock_e = c_FmkCda_AdcCfg_as[adcIndex_u8].c_clock_e;
        adcInfo_ps->c_IRQNType_e = c_FmkCda_AdcCfg_as[adcIndex_u8].c_IRQNType_e;
        adcInfo_ps->c_DmaAdc_e = c_FmkCda_AdcCfg_as[adcIndex_u8].c_DmaAdc_e;
        adcInfo_ps->bspIsct_s.Instance = c_FmkCda_AdcCfg_as[adcIndex_u8].adcTypedef_ps;
        
        SETBIT_16B(adcInfo_ps->Error_u16, FMKCDA_ERRSTATE_OK);
        
        g_adcCalibInfo_as[adcIndex_u8].cabliValue_f32 = (t_float32)0.0;
        g_adcCalibInfo_as[adcIndex_u8].isValueSet_b = (t_bool)False;
        
        g_AdcBuffer_as[adcIndex_u8].lastUpate_u32 = (t_uint32)0;
        g_AdcBuffer_as[adcIndex_u8].flagOpeRW_b = (t_bool)False;

        g_counterRank_au8[adcIndex_u8] = (t_uint8)0;

        for (chnlIndex_u8 = (t_uint8)0; chnlIndex_u8 < (t_uint8)FMKCDA_ADC_CHANNEL_NB; chnlIndex_u8++)
        { // all channel for a adc
            adcInfo_ps->Channel_as[chnlIndex_u8].isConfigured_b = (t_bool)False;
            adcInfo_ps->Channel_as[chnlIndex_u8].rawValue_u16 = (t_uint16)0;

            g_AdcBuffer_as[adcIndex_u8].BspChnlmapp_ae[chnlIndex_u8] = FMKCDA_ADC_CHANNEL_NB;
            g_AdcBuffer_as[adcIndex_u8].rawValue_au32[chnlIndex_u8] = (t_uint32)0;
            
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
                                        t_eFMKCDA_AdcChannel f_channel_e,
                                        t_eFMKCDA_HwAdcCfg f_hwAdcCfg_e)
{
    t_eReturnCode Ret_e = RC_OK;

    if (f_Adc_e >= FMKCDA_ADC_NB 
    || f_channel_e >= c_FmkCda_AdcMaxChnl_ua8[f_Adc_e])
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)f_Adc_e);
    }
    if(g_AdcInfo_as[f_Adc_e].IsConfigured_b == (t_bool)False)
    {
        Ret_e = s_FMKCDA_Set_BspAdcCfg((t_eFMKCDA_Adc)f_Adc_e,
                                           f_hwAdcCfg_e);
    }
    if(g_AdcInfo_as[f_Adc_e].IsConfigured_b == (t_bool)True
    && f_hwAdcCfg_e != g_AdcInfo_as[f_Adc_e].HwCfg_e)
    {
        Ret_e = RC_ERROR_WRONG_CONFIG;
        ASSERT((t_uint16)g_AdcInfo_as[f_Adc_e].IsConfigured_b);
    }
    if (Ret_e == RC_OK)
    {
        //----- depending on hardware configuration make some configuration -----//
        #warning "Only Scan/DMA mode is treated for AdcChannel Configuration"
        //----- Configure Channel -----//
        Ret_e = s_FMKCDA_Set_BspChannelCfg(f_Adc_e, f_channel_e);
    }
    
    return Ret_e;
}

/*********************************
 * FMKCDA_Get_AnaChannelMeasure
 *********************************/
t_eReturnCode FMKCDA_Get_AnaChannelMeasure(t_eFMKCDA_Adc f_Adc_e, t_eFMKCDA_AdcChannel f_channel_e, t_uint16 *f_AnaMeasure_u16)
{
    t_eReturnCode Ret_e = RC_OK;
    t_sFMKCDA_ChnlInfo * cnlInfo_ps;
    t_sFMKCDA_AdcInfo * adcInfo_ps;

    if (f_Adc_e >= FMKCDA_ADC_NB 
    || f_channel_e >= c_FmkCda_AdcMaxChnl_ua8[f_Adc_e])
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
        ASSERT((t_uint16)Ret_e);
    }
    if (f_AnaMeasure_u16 == (t_uint16 *)NULL)
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
        cnlInfo_ps = (t_sFMKCDA_ChnlInfo *)(&g_AdcInfo_as[f_Adc_e].Channel_as[f_channel_e]);

        if(adcInfo_ps->IsConfigured_b == (t_bool)False
        || cnlInfo_ps->isConfigured_b == (t_bool)False)
        {
            Ret_e = RC_ERROR_MISSING_CONFIG;
            ASSERT((t_uint16)Ret_e);
        }
        if(GETBIT(adcInfo_ps->Error_u16, FMKCDA_ERRSTATE_OK) == BIT_IS_RESET_16B)
        {
            Ret_e = RC_WARNING_BUSY;
        }
        if (Ret_e == RC_OK)
        {
            
            //----- give the last raw analog value if value is updated -----//
            if (cnlInfo_ps->FlagValueUpdated_b == (t_bool)True)
            {
                *f_AnaMeasure_u16 = cnlInfo_ps->rawValue_u16;

                //----- update flag for this channel -----//
                cnlInfo_ps->FlagValueUpdated_b = (t_bool)False;
            }
            else
            {
                *f_AnaMeasure_u16 = (t_uint16)0;
                Ret_e = RC_WARNING_NO_OPERATION;
            }
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
        *f_chnlErrInfo_pu16 = g_AdcInfo_as[f_adc_e].Error_u16;
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
    t_uint8 AdcIndex_u8 = 0;

    //----- set configuration channel for Adc Internal Signal 
    //      It appears that sometimes, different ADC are connected 
    //      To the same ADC-CHANNEL for Vref Voltage, In consequence 
    //      When we loop on each Adc to Configure the Channel Link to the Vref 
    //      The function FMKCDA_Set_AdcChannelCfg will return RC_ERROR_ALREADY_CONFIGURED
    //      We consider this state OK -----//

    for(AdcIndex_u8 = (t_uint8)0 ; 
           (AdcIndex_u8 < FMKCDA_ADC_NB) 
        && (Ret_e == RC_OK) ; 
        AdcIndex_u8++)
    {
        if(g_AdcInfo_as[AdcIndex_u8].IsConfigured_b == (t_bool)True)
        {
            Ret_e = s_FMKCDA_Set_BspChannelCfg(c_FmkCda_HwVrefCfg[AdcIndex_u8].adc_e,
                                            c_FmkCda_HwVrefCfg[AdcIndex_u8].chnl_e);
            
            //----- Different ADC has the same link for Vref -> it's OK ----//
            if(Ret_e == RC_ERROR_ALREADY_CONFIGURED)
            {
                Ret_e = RC_OK;
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
    t_uint8 adcIndex_u8 = 0;
    t_sFMKCDA_AdcInfo * adcInfo_ps;

   FMKCPU_GetTick(&currentTime_u32);

    //------ For every adc in stm32 ------//
    for(adcIndex_u8 = (t_uint8)0 ; adcIndex_u8 < (t_uint8)FMKCDA_ADC_NB ; adcIndex_u8++)
    {
        adcInfo_ps = (t_sFMKCDA_AdcInfo *)(&g_AdcInfo_as[adcIndex_u8]);

        if(adcInfo_ps->IsConfigured_b == (t_bool)true)
        {
            //------ If an Adc Error has been raised, deal with it ------//
            if((adcInfo_ps->flagErrDetected_b == True)
            ||((currentTime_u32 - s_SavedTime_u32) > (t_uint32)FMKCDA_TIME_BTWN_DIAG_MS))
            {
                s_SavedTime_u32 = currentTime_u32;
                Ret_e = s_FMKCDA_PerformDiagnostic((t_eFMKCDA_Adc)adcIndex_u8);
            }

            //------ if the adc is not running and the adc is configured,
            //       launch a conversion only if error_state = NO_ERROR or PRESENTS ------//
            if((adcInfo_ps->IsAdcRunning_b == (t_bool)False)
            && ((GETBIT(adcInfo_ps->Error_u16,FMKCDA_ERRSTATE_OK) == BIT_IS_SET_16B)
            ||  (GETBIT(adcInfo_ps->Error_u16, FMKCDA_ERRSTATE_PRESENTS) == BIT_IS_SET_16B)))
            {
                Ret_e = s_FMKCDA_StartAdcConversion((t_eFMKCDA_Adc)adcIndex_u8, g_AdcInfo_as[adcIndex_u8].HwCfg_e);
                if(Ret_e == RC_OK) 
                {
                    g_AdcInfo_as[adcIndex_u8].IsAdcRunning_b = True;
                    RESETBIT_16B(adcInfo_ps->Error_u16, FMKCDA_ERRSTATE_PRESENTS);
                    SETBIT_16B(adcInfo_ps->Error_u16, FMKCDA_ERRSTATE_OK);
                }
            }
            else
            {   
                //------ Update Current Time ------//
                FMKCPU_GetTick(&currentTime_u32);
                //------ check last time update to make actions if there is no update from adc
                // also add 5ms in case interrutpion occured during getting the Tick ------//
                if(g_AdcBuffer_as[adcIndex_u8].flagOpeRW_b == (t_bool)false)
                {
                    if((t_uint32)(currentTime_u32 - g_AdcBuffer_as[adcIndex_u8].lastUpate_u32) > (t_uint32)FMKCDA_OVR_CONVERSION_MS)
                    {
                        // update information 
                        adcInfo_ps->IsAdcRunning_b = False;
                        SETBIT_16B(adcInfo_ps->Error_u16, FMKCDA_ERRSTATE_PRESENTS);
                        ASSERT((t_uint16)adcInfo_ps->Error_u16);
                    }
                    else 
                    {// put the buffer into adc channel block
                        Ret_e = s_FMKCDA_UpdateChannelValue((t_eFMKCDA_Adc)adcIndex_u8);
                    }
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

    adcInfo_ps = (t_sFMKCDA_AdcInfo *)&g_AdcInfo_as[f_adc_e];
    adcErr_u32 = HAL_ADC_GetError(&adcInfo_ps->bspIsct_s);
    
    if(adcErr_u32 != HAL_ADC_ERROR_NONE)
    {
        //----- mng mapping error with enum -----//
        if((adcErr_u32 & HAL_ADC_ERROR_OVR) == HAL_ADC_ERROR_OVR)
        {
            SETBIT_16B(adcInfo_ps->Error_u16, FMKCDA_ERRSTATE_ERR_OVR);
        }
        if((adcErr_u32 & HAL_ADC_ERROR_DMA) == HAL_ADC_ERROR_DMA)
        {
            SETBIT_16B(adcInfo_ps->Error_u16, FMKCDA_ERRSTATE_ERR_DMA);
        }
        if((adcErr_u32 & HAL_ADC_ERROR_INTERNAL) == HAL_ADC_ERROR_INTERNAL)
        {
            SETBIT_16B(adcInfo_ps->Error_u16, FMKCDA_ERRSTATE_ERR_INTERNAL);
        }
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
#elif defined FMKCPU_STM32_ECU_FAMILY_G
        bspAdcInit_s->ScanConvMode = ADC_SCAN_ENABLE;
        bspAdcInit_s->LowPowerAutoWait = DISABLE; // Désactiver l'attente automatique par défaut
        bspAdcInit_s->SamplingMode = ADC_SAMPLING_MODE_NORMAL; // Mode d'échantillonnage normal
        bspAdcInit_s->GainCompensation = 0; // Pas de compensation de gain par défaut

        //----- Over samppling parameter -----//
        bspAdcInit_s->OversamplingMode = ENABLE;
        bspAdcInit_s->Oversampling.Ratio = ADC_OVERSAMPLING_RATIO_128; // Exemple : suréchantillonnage x16
        bspAdcInit_s->Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_7;
        bspAdcInit_s->Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
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
#elif defined FMKCPU_STM32_ECU_FAMILY_G
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
    
    if((f_Adc_e >= FMKCDA_ADC_NB)
    || (f_channel_e >= FMKCDA_ADC_CHANNEL_NB))
    {
        ASSERT((t_uint16)f_channel_e);
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if( (Ret_e == RC_OK)
    &&  (g_AdcInfo_as[f_Adc_e].IsConfigured_b == (t_bool)False))
    {
        ASSERT((t_uint16)g_AdcInfo_as[f_Adc_e].IsConfigured_b);
        Ret_e = RC_ERROR_MISSING_CONFIG;
    }
    if((Ret_e == RC_OK)
    && (g_AdcInfo_as[f_Adc_e].Channel_as[f_channel_e].isConfigured_b == (t_bool)True))
    {
        ASSERT((t_uint16)0);
        Ret_e = RC_ERROR_ALREADY_CONFIGURED;
    }
    if(Ret_e == RC_OK)
    {
#ifdef FMKCPU_STM32_ECU_FAMILY_F
        BspChannelInit_s.SamplingTime = ADC_SAMPLETIME_13CYCLES_5; // Configuration spécifique à la famille F
        BspChannelInit_s.SingleDiff = ADC_SINGLE_ENDE;           // Single-ended par défaut
        BspChannelInit_s.OffsetNumber = ADC_OFFSET_NONE;         // Pas d'offset initial
        BspChannelInit_s.Offset = 0;                             // Offset à 0
        BspChannelInit_s.OffsetSign = ADC_OFFSET_SIGN_POSITIVE;  // Offset positif par défaut
        BspChannelInit_s.OffsetSaturation = DISABLE;              // Saturation désactivée
#elif defined FMKCPU_STM32_ECU_FAMILY_G
        BspChannelInit_s.SamplingTime = ADC_SAMPLETIME_640CYCLES_5; // Configuration spécifique à la famille G
        BspChannelInit_s.SingleDiff = ADC_SINGLE_ENDED;           // Single-ended par défaut
        BspChannelInit_s.OffsetNumber = ADC_OFFSET_NONE;        // Pas d'offset initial
        BspChannelInit_s.Offset = 0;                            // Offset à 0
        BspChannelInit_s.OffsetSign = ADC_OFFSET_SIGN_POSITIVE;  // Offset positif par défaut
        BspChannelInit_s.OffsetSaturation = DISABLE;           // Saturation désactivée
#else
        #error("Famille STM32 non supportée. Vérifiez la configuration.")
#endif
        //----- configure channel -----//
        Ret_e = FMKCDA_Get_BspChannel(f_channel_e, &bspChannel_u32);

        if (Ret_e == RC_OK)
        {
            //----- For mapping purpose -----// 
            g_counterRank_au8[f_Adc_e] += (t_uint8)1;
            BspChannelInit_s.Channel = bspChannel_u32;
            BspChannelInit_s.Rank = ADC_REGULAR_RANK_1;


            //----- configure adc channel -----//
            BspRet_e = HAL_ADC_ConfigChannel(&g_AdcInfo_as[f_Adc_e].bspIsct_s,
                                            &BspChannelInit_s);

            if (BspRet_e == HAL_OK)
            {
                //----- update mapping for dma -----//
                g_AdcBuffer_as[f_Adc_e].BspChnlmapp_ae[(g_counterRank_au8[f_Adc_e] - 1)] = f_channel_e;

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
    t_sFMKCDA_AdcBuffer * adcBuffer_ps = (t_sFMKCDA_AdcBuffer *)(&g_AdcBuffer_as[f_Adc_e]);
    t_sFMKCDA_AdcCalibInfo * adcCalib_ps = (t_sFMKCDA_AdcCalibInfo *)(&g_adcCalibInfo_as[f_Adc_e]);
    t_uint8 adcCtrRank_u8 = (t_uint8)g_counterRank_au8[f_Adc_e];
    t_uint8 LLI_u8 = 0;
    t_uint8 reverseLLI_u8 = 0;
    t_uint8 idxChnl_u8 = 0; 
    static t_uint32 lastTime_u32 = 0;
    t_uint32 currentTime_u32= 0;
    
    FMKCPU_GetTick(&currentTime_u32);

    //------ update calibration point for this adc if needed ------//
    if((currentTime_u32 - lastTime_u32) > (t_uint32)FMKCDA_CYCLIC_CALIB
    || adcCalib_ps->isValueSet_b == (t_bool)False)
    {
        lastTime_u32 = currentTime_u32;
        chnl_e = c_FmkCda_HwVrefCfg[f_Adc_e].chnl_e;

        //------ Retrieve the Bsp Channel associated ------//
        for(LLI_u8 = (t_uint8)0 ; LLI_u8 < adcCtrRank_u8 ; LLI_u8++)
        {
            if(chnl_e == adcBuffer_ps->BspChnlmapp_ae[LLI_u8])
            {
                break;
            }
        }
        
        if(chnl_e != FMKCDA_ADC_CHANNEL_NB)
        {//                         max rank in buffer, cause it's in reverse
            idxChnl_u8 = (t_uint8)((adcCtrRank_u8 - (t_uint8)1) - (t_uint8)LLI_u8);

            adcCalib_ps->cabliValue_f32 = (t_float32)(adcBuffer_ps->savedVal_ua16[idxChnl_u8] 
                                                            / (t_float32)(*c_FmkCda_VrefCalibAddress_pas16[f_Adc_e]));

            //------ Update Flag Value Set ------//
            adcCalib_ps->isValueSet_b = (t_bool)True;

        }
    }

    //------ update flag reading ------//
    adcBuffer_ps->flagOpeRW_b = (t_bool)True;

    //------  here the dma load the buffer with FILO method, first in last out ------//
    reverseLLI_u8 = (t_uint8)(adcCtrRank_u8 - 1);

    for (LLI_u8 = (t_uint8)0 ; LLI_u8 < adcCtrRank_u8 ; LLI_u8++)
    {
        chnl_e = adcBuffer_ps->BspChnlmapp_ae[reverseLLI_u8];

        adcInfo_ps->Channel_as[chnl_e].rawValue_u16 = 
            (t_uint16)((t_float32)adcBuffer_ps->savedVal_ua16[LLI_u8] * 
                            adcCalib_ps->cabliValue_f32);

        //------ Update flag ------ //
        adcInfo_ps->Channel_as[chnl_e].FlagValueUpdated_b = (t_bool)True;
        reverseLLI_u8 -= (t_uint8)1;
    }  
    //------ update flag reading ------//
    adcBuffer_ps->flagOpeRW_b = (t_bool)False; 

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
    t_uint8 adcIndex_u8 = 0;
    t_uint8 LLI_u8 = 0;
    t_eFMKCDA_Adc IT_Adc_e = FMKCDA_ADC_NB;

    //------ Find the Adc ------//
    for (adcIndex_u8 = (t_uint8)0; adcIndex_u8 < (t_uint8)FMKCDA_ADC_NB; adcIndex_u8++)
    {
        if (&g_AdcInfo_as[adcIndex_u8].bspIsct_s == (ADC_HandleTypeDef *)hadc)
        {
            IT_Adc_e = (t_eFMKCDA_Adc)adcIndex_u8;
            break;
        }
    }
    
    if (IT_Adc_e < FMKCDA_ADC_NB)
    {
        //------ update saved value only if cyclic is not reading it ------//
        if(g_AdcBuffer_as[IT_Adc_e].flagOpeRW_b == (t_bool)False)
        {
            g_AdcBuffer_as[IT_Adc_e].flagOpeRW_b = True;
            //                                      number of channel configured
            for(LLI_u8 = (t_uint8)0 ; LLI_u8 < (t_uint8)(g_counterRank_au8[IT_Adc_e]) ; LLI_u8++)
            {
                g_AdcBuffer_as[IT_Adc_e].savedVal_ua16[LLI_u8] = (t_uint16)g_AdcBuffer_as[IT_Adc_e].rawValue_au32[LLI_u8];
            }
            FMKCPU_GetTick(&g_AdcBuffer_as[IT_Adc_e].lastUpate_u32);
            g_AdcBuffer_as[IT_Adc_e].flagOpeRW_b = False;
        }
        //------ update last time the value has been changed and reset bit present error ------//
        //------ reset present bit ------//
        RESETBIT_16B(g_AdcInfo_as[IT_Adc_e].Error_u16, FMKCDA_ERRSTATE_PRESENTS);
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
    t_uint8 adcIndex_u8 = 0;
    t_eFMKCDA_Adc IT_Adc_e = FMKCDA_ADC_NB;

    //------ Find Adc ------//
    for (adcIndex_u8 = (t_uint8)0; adcIndex_u8 < (t_uint8)FMKCDA_ADC_NB; adcIndex_u8++)
    {
        if (&g_AdcInfo_as[adcIndex_u8].bspIsct_s == (ADC_HandleTypeDef *)hadc)
        {
            IT_Adc_e = (t_eFMKCDA_Adc)adcIndex_u8;
            break;
        }
    }
    if (IT_Adc_e < FMKCDA_ADC_NB)
    {
        //------ update last time the value has been changed ------// 
        FMKCPU_GetTick(&g_AdcBuffer_as[IT_Adc_e].lastUpate_u32);
    }
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
        g_AdcInfo_as[LLI_u8].flagErrDetected_b = (t_bool)True;
        RESETBIT_16B(g_AdcInfo_as[LLI_u8].Error_u16, FMKCDA_ERRSTATE_OK);
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

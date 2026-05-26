/*********************************************************************
 * @file        FMKCDA_ConfigSpecific.h
 * @brief       Template_BriefDescription.
 * @details     TemplateDetailsDescription.\n
 *
 * @author      xxxxxx
 * @date        jj/mm/yyyy
 * @version     1.0
 */






// ********************************************************************
// *                      Includes
// ********************************************************************
#include "./FMKCDA_ConfigSpecific.h"
// ********************************************************************
// *                      Defines
// ********************************************************************

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
//                      Local functions - Prototypes
//********************************************************************************

//****************************************************************************
//                      Public functions - Implementation
//********************************************************************************
/*********************************
 * FMKCDA_Set_BspAdcInitCfg
 *********************************/
t_eReturnCode FMKCDA_Set_BspAdcInitCfg(ADC_InitTypeDef *f_bspAdcInit_ps,
                                       t_eFMKCDA_HwAdcCfg f_HwAdcCfg_e,
                                       t_uint8 f_nbChnlToCfg_u8)
{
    t_eReturnCode Ret_e;

    if(f_bspAdcInit_ps == (ADC_InitTypeDef *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    else if(f_HwAdcCfg_e >= FMKCDA_ADC_CFG_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    else
    {
        Ret_e = RC_OK;

        //----- Generic Configuration -----//
        f_bspAdcInit_ps->ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
        f_bspAdcInit_ps->Overrun = ADC_OVR_DATA_OVERWRITTEN;
        f_bspAdcInit_ps->Resolution = ADC_RESOLUTION_12B;
        f_bspAdcInit_ps->EOCSelection = ADC_EOC_SEQ_CONV;

        //----- Specific Configuration -----//
#ifdef FMKCPU_STM32_ECU_FAMILY_F
        f_bspAdcInit_ps->ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
        f_bspAdcInit_ps->SamplingTimeCommon = ADC_SAMPLETIME_55CYCLES_5;
#elif defined FMKCPU_STM32_ECU_FAMILY_G4
        f_bspAdcInit_ps->DataAlign = ADC_DATAALIGN_RIGHT;
        f_bspAdcInit_ps->ScanConvMode = ADC_SCAN_ENABLE;
        f_bspAdcInit_ps->LowPowerAutoWait = DISABLE;
        f_bspAdcInit_ps->SamplingMode = ADC_SAMPLING_MODE_NORMAL;
        f_bspAdcInit_ps->GainCompensation = 0;
        f_bspAdcInit_ps->OversamplingMode = ENABLE;
        f_bspAdcInit_ps->Oversampling.Ratio = ADC_OVERSAMPLING_RATIO_128;
        f_bspAdcInit_ps->Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_7;
        f_bspAdcInit_ps->Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
        f_bspAdcInit_ps->Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
        f_bspAdcInit_ps->NbrOfConversion = (t_uint32)f_nbChnlToCfg_u8;
#elif defined FMKCPU_STM32_ECU_FAMILY_H7
        f_bspAdcInit_ps->ScanConvMode = ADC_SCAN_ENABLE;
        f_bspAdcInit_ps->LowPowerAutoWait = DISABLE;
        f_bspAdcInit_ps->ContinuousConvMode = ENABLE;
        f_bspAdcInit_ps->NbrOfConversion = (t_uint32)f_nbChnlToCfg_u8;
        f_bspAdcInit_ps->DiscontinuousConvMode = DISABLE;
        f_bspAdcInit_ps->NbrOfDiscConversion = 0U;
        f_bspAdcInit_ps->ExternalTrigConv = ADC_SOFTWARE_START;
        f_bspAdcInit_ps->ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        f_bspAdcInit_ps->ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
        f_bspAdcInit_ps->LeftBitShift = ADC_LEFTBITSHIFT_NONE;
        f_bspAdcInit_ps->OversamplingMode = ENABLE;
        f_bspAdcInit_ps->Oversampling.Ratio = 16UL;
        f_bspAdcInit_ps->Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_4;
        f_bspAdcInit_ps->Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
        f_bspAdcInit_ps->Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
#else
        #error("Famille STM32 non supportee. Verifiez la configuration.")
#endif

        if(FMKCPU_ADC_DMA_MODE == DMA_CIRCULAR)
        {
#if defined(FMKCPU_STM32_ECU_FAMILY_H7)
            f_bspAdcInit_ps->ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
#else
            f_bspAdcInit_ps->DMAContinuousRequests = ENABLE;
#endif
        }
        else
        {
#if defined(FMKCPU_STM32_ECU_FAMILY_H7)
            f_bspAdcInit_ps->ConversionDataManagement = ADC_CONVERSIONDATA_DMA_ONESHOT;
#else
            f_bspAdcInit_ps->DMAContinuousRequests = DISABLE;
#endif
        }

        switch(f_HwAdcCfg_e)
        {
            case FMKCDA_ADC_CFG_PERIODIC_DMA:
            case FMKCDA_ADC_CFG_SCAN_DMA:
                f_bspAdcInit_ps->ContinuousConvMode = ENABLE;
                f_bspAdcInit_ps->ExternalTrigConv = ADC_SOFTWARE_START;
                f_bspAdcInit_ps->ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
                break;

            case FMKCDA_ADC_CFG_TRIGGERED_DMA:
                f_bspAdcInit_ps->DiscontinuousConvMode = DISABLE;
#ifdef FMKCPU_STM32_ECU_FAMILY_F
                f_bspAdcInit_ps->ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC4;
#elif defined FMKCPU_STM32_ECU_FAMILY_G4
                /* Trigger source to be selected by the G4 ECU configuration. */
#elif defined FMKCPU_STM32_ECU_FAMILY_H7
                f_bspAdcInit_ps->ExternalTrigConv = ADC_SOFTWARE_START;
#else
                #error("Famille STM32 non supportee. Verifiez la configuration.")
#endif
                f_bspAdcInit_ps->ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
                break;

            case FMKCDA_ADC_CFG_NB:
            default:
                Ret_e = RC_WARNING_NO_OPERATION;
                break;
        }
    }

    return Ret_e;
}

/*********************************
 * FMKCDA_Set_BspChannelCfg
 *********************************/
t_eReturnCode FMKCDA_Set_BspChannelCfg(ADC_ChannelConfTypeDef *f_bspChannelCfg_ps)
{
    t_eReturnCode Ret_e;

    if(f_bspChannelCfg_ps == (ADC_ChannelConfTypeDef *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    else
    {
        Ret_e = RC_OK;

#ifdef FMKCPU_STM32_ECU_FAMILY_F
        f_bspChannelCfg_ps->SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
        f_bspChannelCfg_ps->SingleDiff = ADC_SINGLE_ENDE;
        f_bspChannelCfg_ps->OffsetNumber = ADC_OFFSET_NONE;
        f_bspChannelCfg_ps->Offset = 0;
        f_bspChannelCfg_ps->OffsetSign = ADC_OFFSET_SIGN_POSITIVE;
        f_bspChannelCfg_ps->OffsetSaturation = DISABLE;
#elif defined FMKCPU_STM32_ECU_FAMILY_G4
        f_bspChannelCfg_ps->SamplingTime = ADC_SAMPLETIME_247CYCLES_5;
        f_bspChannelCfg_ps->SingleDiff = ADC_SINGLE_ENDED;
        f_bspChannelCfg_ps->OffsetNumber = ADC_OFFSET_NONE;
        f_bspChannelCfg_ps->Offset = 0;
        f_bspChannelCfg_ps->OffsetSign = ADC_OFFSET_SIGN_POSITIVE;
        f_bspChannelCfg_ps->OffsetSaturation = DISABLE;
#elif defined FMKCPU_STM32_ECU_FAMILY_H7
        f_bspChannelCfg_ps->SamplingTime = ADC_SAMPLETIME_387CYCLES_5;
        f_bspChannelCfg_ps->SingleDiff = ADC_SINGLE_ENDED;
        f_bspChannelCfg_ps->OffsetNumber = ADC_OFFSET_NONE;
        f_bspChannelCfg_ps->Offset = 0;
        f_bspChannelCfg_ps->OffsetSignedSaturation = DISABLE;
#else
        #error("Famille STM32 non supportee. Verifiez la configuration.")
#endif
    }

    return Ret_e;
}

/*********************************
 * FMKCDA_Get_VrefCalibRawValue
 *********************************/
t_uint16 FMKCDA_Get_VrefCalibRawValue(t_uint16 f_rawCalibValue_u16)
{
    t_uint16 calibValue_u16 = f_rawCalibValue_u16;

#ifdef FMKCPU_STM32_ECU_FAMILY_H7
    calibValue_u16 = (t_uint16)(calibValue_u16 >> 4U);
#endif

    return calibValue_u16;
}

/*********************************
 * FMKCDA_Get_BspChannel
 *********************************/
t_eReturnCode FMKCDA_Get_BspChannel(t_eFMKCDA_Adc f_Adc_e,
                                    t_eFMKCDA_AdcChannel f_channel_e, 
                                    t_uint32 *f_bspChannel_32)
{
    t_eReturnCode Ret_e = RC_OK;

    if (f_channel_e >= FMKCDA_ADC_CHANNEL_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    if (f_bspChannel_32 == (t_uint32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    if (Ret_e == RC_OK)
    {
        switch (f_channel_e)
        {
            /* CAUTION : Automatic generated code section for switch_case ADC channel: Start */
            case FMKCDA_ADC_CHANNEL_0:
                *f_bspChannel_32 = ADC_CHANNEL_0;
                break;
            case FMKCDA_ADC_CHANNEL_1:
                *f_bspChannel_32 = ADC_CHANNEL_1;
                break;
            case FMKCDA_ADC_CHANNEL_2:
                *f_bspChannel_32 = ADC_CHANNEL_2;
                break;
            case FMKCDA_ADC_CHANNEL_3:
                *f_bspChannel_32 = ADC_CHANNEL_3;
                break;
            case FMKCDA_ADC_CHANNEL_4:
                *f_bspChannel_32 = ADC_CHANNEL_4;
                break;
            case FMKCDA_ADC_CHANNEL_5:
                *f_bspChannel_32 = ADC_CHANNEL_5;
                break;
            case FMKCDA_ADC_CHANNEL_6:
                *f_bspChannel_32 = ADC_CHANNEL_6;
                break;
            case FMKCDA_ADC_CHANNEL_7:
                *f_bspChannel_32 = ADC_CHANNEL_7;
                break;
            case FMKCDA_ADC_CHANNEL_8:
                *f_bspChannel_32 = ADC_CHANNEL_8;
                break;
            case FMKCDA_ADC_CHANNEL_9:
                *f_bspChannel_32 = ADC_CHANNEL_9;
                break;
            case FMKCDA_ADC_CHANNEL_10:
                *f_bspChannel_32 = ADC_CHANNEL_10;
                break;
            case FMKCDA_ADC_CHANNEL_11:
                *f_bspChannel_32 = ADC_CHANNEL_11;
                break;
            case FMKCDA_ADC_CHANNEL_12:
                *f_bspChannel_32 = ADC_CHANNEL_12;
                break;
            case FMKCDA_ADC_CHANNEL_13:
                *f_bspChannel_32 = ADC_CHANNEL_13;
                break;
            case FMKCDA_ADC_CHANNEL_14:
                *f_bspChannel_32 = ADC_CHANNEL_14;
                break;
            case FMKCDA_ADC_CHANNEL_15:
                *f_bspChannel_32 = ADC_CHANNEL_15;
                break;
            case FMKCDA_ADC_CHANNEL_16:
                *f_bspChannel_32 = ADC_CHANNEL_16;
                break;
            case FMKCDA_ADC_CHANNEL_17:
                *f_bspChannel_32 = ADC_CHANNEL_17;
                break;
            case FMKCDA_ADC_CHANNEL_18:
                *f_bspChannel_32 = ADC_CHANNEL_18;
                break;
            case FMKCDA_ADC_CHANNEL_19:
                *f_bspChannel_32 = ADC_CHANNEL_19;
                break;
            /* CAUTION : Automatic generated code section for switch_case ADC channel: End */
            case FMKCDA_ADC_CHANNEL_NB:
            default:
                Ret_e = RC_ERROR_PARAM_NOT_SUPPORTED;
        }
    }
    return Ret_e;
}

/*********************************
 * FMKCDA_ConvertRawInterSnsValue
 *********************************/
t_eReturnCode FMKCDA_ConvertRawInterSnsValue(t_eFMKCDA_AdcInternSns f_AdcInternSns_e,
                                            t_float32 f_rawAnaMeasure_f32,
                                            t_float32 f_calibValue_f32,
                                            t_float32 *f_snsAnaMeasure_pf32,
                                            const volatile t_uint16 * f_AdcInterSnsCalibStatic_pua16[])
{
    t_eReturnCode Ret_e;
    t_uint16 tsCalLow_u16;
    t_uint16 tsCalHigh_u16;
    t_float32 coeffDir_f32;
    t_float32 measCalibrate_f32;

    if(f_AdcInternSns_e >= FMKCDA_ADC_INTERN_NB)
    {
        Ret_e = RC_ERROR_PARAM_INVALID;
    }
    else if(f_snsAnaMeasure_pf32 == (t_float32 *)NULL)
    {
        Ret_e = RC_ERROR_PTR_NULL;
    }
    else 
    {
        Ret_e = RC_OK;

        switch(f_AdcInternSns_e)
        {
            case FMKCDA_ADC_INTERN_VBAT:
                //--- calib value is pretty simple here ----//
                measCalibrate_f32 = f_rawAnaMeasure_f32 * f_calibValue_f32;
                //---- the signal is measuring from adc divided by 3 (Datasheet)
                //          mulutiply it also by 3 ----//
                *f_snsAnaMeasure_pf32 = (t_float32)((measCalibrate_f32 * (t_float32)FMKCDA_ADC_VBAT_MULTPIPLIER) 
                                                        / (t_float32)FMKCDA_ADC_RESOLUTION);
                *f_snsAnaMeasure_pf32 *= FMKCDA_ADC_VOLT_PROMILLE;
            break;
            case FMKCDA_ADC_INTERN_TS_CAL1:
#ifdef FMKCPU_STM32_ECU_FAMILY_G4
            case FMKCDA_ADC_INTERN_TS_CAL2:
#endif
            {
                //---- calibration value has to be without dimension ----//
                measCalibrate_f32 = f_rawAnaMeasure_f32 * (f_calibValue_f32 / FMKCDA_ADC_CALIB_VREF);
                //---- get the refereence static value for linear equation ----//
                tsCalLow_u16 = (t_uint16)(*f_AdcInterSnsCalibStatic_pua16[FMKCDA_ADC_INTERN_TS_CAL1]);
                tsCalHigh_u16 = (t_uint16)(*f_AdcInterSnsCalibStatic_pua16[FMKCDA_ADC_INTERN_TS_CAL2]);
#ifdef FMKCPU_STM32_ECU_FAMILY_H7
                tsCalLow_u16 = (t_uint16)(tsCalLow_u16 >> 4U);
                tsCalHigh_u16 = (t_uint16)(tsCalHigh_u16 >> 4U);

                //---- put all in voltage ----//
                //---- calibration are made at low -> 30°C and low-> 110°C ----//
                //---- use aT + b = adcMeasure where b = 30, a = coeff dir
                coeffDir_f32 = ((t_float32)(tsCalHigh_u16 - tsCalLow_u16)) / 80.0f;
                *f_snsAnaMeasure_pf32 = 30.0f + 
                    ((measCalibrate_f32 - (t_float32)tsCalLow_u16) / coeffDir_f32);
#elif defined FMKCPU_STM32_ECU_FAMILY_G4
                //---- put all in voltage ----//
                //---- calibration are made at low -> 30°C and low-> 130°C ----//
                //---- use aT + b = adcMeasure where b = 30, a = coeff dir
                coeffDir_f32 = ((t_float32)(tsCalHigh_u16 - tsCalLow_u16)) / 100.0f;
                *f_snsAnaMeasure_pf32 = 30.0f + 
                ((measCalibrate_f32 - (t_float32)tsCalLow_u16) / coeffDir_f32);
#endif
            break;
            case FMKCDA_ADC_INTERN_NB:
            default:
                Ret_e = RC_ERROR_PARAM_NOT_SUPPORTED;
            break;
                
            }

        }
    }
    return Ret_e;
}
//********************************************************************************
//                      Local functions - Implementation
//********************************************************************************
           
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

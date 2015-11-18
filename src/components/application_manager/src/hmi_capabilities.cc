/**
 * Copyright (c) 2013, Ford Motor Company
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of the Ford Motor Company nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef MODIFY_FUNCTION_SIGN
#include <global_first.h>
#endif
#include <map>
#include "json/json.h"
#include "utils/file_system.h"
#include "interfaces/HMI_API.h"
#include "config_profile/profile.h"
#include "smart_objects/smart_object.h"
#include "application_manager/smart_object_keys.h"
#include "application_manager/message_helper.h"
#include "application_manager/hmi_capabilities.h"
#include "application_manager/smart_object_keys.h"
#include "application_manager/application_manager_impl.h"
#include "application_manager/message_helper.h"
#include "formatters/CFormatterJsonBase.hpp"

namespace application_manager {
namespace Formatters = NsSmartDeviceLink::NsJSONHandler::Formatters;

CREATE_LOGGERPTR_GLOBAL(logger_, "HMICapabilities")

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_Language::eType> languages_enum_values;
#else
std::map<std::string, hmi_apis::Common_Language::eType> languages_enum_values =
{
    {"EN_US", hmi_apis::Common_Language::EN_US},
    {"ES_MX", hmi_apis::Common_Language::ES_MX},
    {"FR_CA", hmi_apis::Common_Language::FR_CA},
    {"DE_DE", hmi_apis::Common_Language::DE_DE},
    {"ES_ES", hmi_apis::Common_Language::ES_ES},
    {"EN_GB", hmi_apis::Common_Language::EN_GB},
    {"RU_RU", hmi_apis::Common_Language::RU_RU},
    {"TR_TR", hmi_apis::Common_Language::TR_TR},
    {"PL_PL", hmi_apis::Common_Language::PL_PL},
    {"FR_FR", hmi_apis::Common_Language::FR_FR},
    {"IT_IT", hmi_apis::Common_Language::IT_IT},
    {"SV_SE", hmi_apis::Common_Language::SV_SE},
    {"PT_PT", hmi_apis::Common_Language::PT_PT},
    {"NL_NL", hmi_apis::Common_Language::NL_NL},
    {"EN_AU", hmi_apis::Common_Language::EN_AU},
    {"ZH_CN", hmi_apis::Common_Language::ZH_CN},
    {"ZH_TW", hmi_apis::Common_Language::ZH_TW},
    {"JA_JP", hmi_apis::Common_Language::JA_JP},
    {"AR_SA", hmi_apis::Common_Language::AR_SA},
    {"KO_KR", hmi_apis::Common_Language::KO_KR},
    {"PT_BR", hmi_apis::Common_Language::PT_BR},
    {"CS_CZ", hmi_apis::Common_Language::CS_CZ},
    {"DA_DK", hmi_apis::Common_Language::DA_DK},
    {"NO_NO", hmi_apis::Common_Language::NO_NO}
};
#endif
#endif
    
#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_VrCapabilities::eType> vr_enum_capabilities;
#else
std::map<std::string, hmi_apis::Common_VrCapabilities::eType> vr_enum_capabilities =
{
    {"TEXT", hmi_apis::Common_VrCapabilities::VR_TEXT}
};
#endif
#endif

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_ButtonName::eType> button_enum_name;
#else
std::map<std::string, hmi_apis::Common_ButtonName::eType> button_enum_name =
{
    {"OK"             , hmi_apis::Common_ButtonName::OK},
    {"SEEKLEFT"       , hmi_apis::Common_ButtonName::SEEKLEFT},
    {"SEEKRIGHT"      , hmi_apis::Common_ButtonName::SEEKRIGHT},
    {"TUNEUP"         , hmi_apis::Common_ButtonName::TUNEUP},
    {"TUNEDOWN"       , hmi_apis::Common_ButtonName::TUNEDOWN},
    {"PRESET_0"       , hmi_apis::Common_ButtonName::PRESET_0},
    {"PRESET_1"       , hmi_apis::Common_ButtonName::PRESET_1},
    {"PRESET_2"       , hmi_apis::Common_ButtonName::PRESET_2},
    {"PRESET_3"       , hmi_apis::Common_ButtonName::PRESET_3},
    {"PRESET_4"       , hmi_apis::Common_ButtonName::PRESET_4},
    {"PRESET_5"       , hmi_apis::Common_ButtonName::PRESET_5},
    {"PRESET_6"       , hmi_apis::Common_ButtonName::PRESET_6},
    {"PRESET_7"       , hmi_apis::Common_ButtonName::PRESET_7},
    {"PRESET_8"       , hmi_apis::Common_ButtonName::PRESET_8},
    {"PRESET_9"       , hmi_apis::Common_ButtonName::PRESET_9},
    {"CUSTOM_BUTTON"  , hmi_apis::Common_ButtonName::CUSTOM_BUTTON},
    {"SEARCH"         , hmi_apis::Common_ButtonName::SEARCH},

};
#endif
#endif

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_TextFieldName::eType> text_fields_enum_name;
#else
std::map<std::string, hmi_apis::Common_TextFieldName::eType> text_fields_enum_name =
{
    {"mainField1", hmi_apis::Common_TextFieldName::mainField1},
    {"mainField2", hmi_apis::Common_TextFieldName::mainField2},
    {"mainField3", hmi_apis::Common_TextFieldName::mainField3},
    {"mainField4", hmi_apis::Common_TextFieldName::mainField4},
    {"statusBar" , hmi_apis::Common_TextFieldName::statusBar},
    {"mediaClock", hmi_apis::Common_TextFieldName::mediaClock},
    {"mediaTrack", hmi_apis::Common_TextFieldName::mediaTrack},
    {"alertText1", hmi_apis::Common_TextFieldName::alertText1},
    {"alertText2", hmi_apis::Common_TextFieldName::alertText2},
    {"alertText3", hmi_apis::Common_TextFieldName::alertText3},
    {"scrollableMessageBody" , hmi_apis::Common_TextFieldName::scrollableMessageBody},
    {"initialInteractionText", hmi_apis::Common_TextFieldName::initialInteractionText},
    {"navigationText1"       , hmi_apis::Common_TextFieldName::navigationText1},
    {"navigationText2"       , hmi_apis::Common_TextFieldName::navigationText2},
    {"ETA"                   , hmi_apis::Common_TextFieldName::ETA},
    {"totalDistance"         , hmi_apis::Common_TextFieldName::totalDistance},
    {"audioPassThruDisplayText1", hmi_apis::Common_TextFieldName::audioPassThruDisplayText1},
    {"audioPassThruDisplayText2", hmi_apis::Common_TextFieldName::audioPassThruDisplayText2},
    {"sliderHeader"     , hmi_apis::Common_TextFieldName::sliderHeader},
    {"sliderFooter"     , hmi_apis::Common_TextFieldName::sliderFooter},
    {"notificationText" , hmi_apis::Common_TextFieldName::notificationText},
    {"menuName"         , hmi_apis::Common_TextFieldName::menuName},
    {"secondaryText"    , hmi_apis::Common_TextFieldName::secondaryText},
    {"tertiaryText"     , hmi_apis::Common_TextFieldName::tertiaryText},
    {"timeToDestination", hmi_apis::Common_TextFieldName::timeToDestination},
    {"turnText"         , hmi_apis::Common_TextFieldName::turnText},
    {"menuTitle"        , hmi_apis::Common_TextFieldName::menuTitle},
};
#endif
#endif

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_MediaClockFormat::eType> media_clock_enum_name;
#else
std::map<std::string, hmi_apis::Common_MediaClockFormat::eType> media_clock_enum_name =
{
    {"CLOCK1"    , hmi_apis::Common_MediaClockFormat::CLOCK1},
    {"CLOCK2"    , hmi_apis::Common_MediaClockFormat::CLOCK2},
    {"CLOCK3"    , hmi_apis::Common_MediaClockFormat::CLOCK3},
    {"CLOCKTEXT1", hmi_apis::Common_MediaClockFormat::CLOCKTEXT1},
    {"CLOCKTEXT2", hmi_apis::Common_MediaClockFormat::CLOCKTEXT2},
    {"CLOCKTEXT3", hmi_apis::Common_MediaClockFormat::CLOCKTEXT3},
    {"CLOCKTEXT4", hmi_apis::Common_MediaClockFormat::CLOCKTEXT4},
};
#endif
#endif

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_ImageType::eType> image_type_enum;
#else
std::map<std::string, hmi_apis::Common_ImageType::eType> image_type_enum =
{
    {"STATIC" , hmi_apis::Common_ImageType::STATIC},
    {"DYNAMIC", hmi_apis::Common_ImageType::DYNAMIC}
};
#endif
#endif

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_SamplingRate::eType> sampling_rate_enum;
#else
std::map<std::string, hmi_apis::Common_SamplingRate::eType> sampling_rate_enum =
{
    {"8KHZ" , hmi_apis::Common_SamplingRate::RATE_8KHZ},
    {"16KHZ", hmi_apis::Common_SamplingRate::RATE_16KHZ},
    {"22KHZ", hmi_apis::Common_SamplingRate::RATE_22KHZ},
    {"44KHZ", hmi_apis::Common_SamplingRate::RATE_44KHZ}
};
#endif
#endif

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_BitsPerSample::eType> bit_per_sample_enum;
#else
std::map<std::string, hmi_apis::Common_BitsPerSample::eType> bit_per_sample_enum =
{
    {"RATE_8_BIT", hmi_apis::Common_BitsPerSample::RATE_8_BIT},
    {"RATE_16_BIT", hmi_apis::Common_BitsPerSample::RATE_16_BIT}
};
#endif
#endif

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_AudioType::eType> audio_type_enum;
#else
std::map<std::string, hmi_apis::Common_AudioType::eType> audio_type_enum =
{
    {"PCM", hmi_apis::Common_AudioType::PCM}
};
#endif
#endif

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_HmiZoneCapabilities::eType> hmi_zone_enum;
#else
std::map<std::string, hmi_apis::Common_HmiZoneCapabilities::eType> hmi_zone_enum =
{
    {"FRONT", hmi_apis::Common_HmiZoneCapabilities::FRONT},
    {"BACK", hmi_apis::Common_HmiZoneCapabilities::BACK},
};
#endif
#endif

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_ImageFieldName::eType>
image_field_name_enum;
#else
const std::map<std::string, hmi_apis::Common_ImageFieldName::eType>
image_field_name_enum =
{
    {"softButtonImage", hmi_apis::Common_ImageFieldName::softButtonImage},
    {"choiceImage", hmi_apis::Common_ImageFieldName::choiceImage},
    {"choiceSecondaryImage", hmi_apis::Common_ImageFieldName::choiceSecondaryImage},
    {"vrHelpItem", hmi_apis::Common_ImageFieldName::vrHelpItem},
    {"turnIcon", hmi_apis::Common_ImageFieldName::turnIcon},
    {"menuIcon", hmi_apis::Common_ImageFieldName::menuIcon},
    {"cmdIcon", hmi_apis::Common_ImageFieldName::cmdIcon},
    {"appIcon", hmi_apis::Common_ImageFieldName::appIcon},
    {"graphic", hmi_apis::Common_ImageFieldName::graphic},
    {"showConstantTBTIcon", hmi_apis::Common_ImageFieldName::showConstantTBTIcon},
    {"showConstantTBTNextTurnIcon",
        hmi_apis::Common_ImageFieldName::showConstantTBTNextTurnIcon}
};
#endif
#endif

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_FileType::eType> file_type_enum;
#else
const std::map<std::string, hmi_apis::Common_FileType::eType> file_type_enum =
{
    {"GRAPHIC_BMP", hmi_apis::Common_FileType::GRAPHIC_BMP},
    {"GRAPHIC_JPEG", hmi_apis::Common_FileType::GRAPHIC_JPEG},
    {"GRAPHIC_PNG", hmi_apis::Common_FileType::GRAPHIC_PNG},
    {"AUDIO_WAVE", hmi_apis::Common_FileType::AUDIO_WAVE},
    {"AUDIO_MP3", hmi_apis::Common_FileType::AUDIO_MP3},
    {"AUDIO_AAC", hmi_apis::Common_FileType::AUDIO_AAC},
    {"BINARY", hmi_apis::Common_FileType::BINARY},
    {"JSON", hmi_apis::Common_FileType::JSON}
};
#endif
#endif

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_DisplayType::eType> display_type_enum;
#else
const std::map<std::string, hmi_apis::Common_DisplayType::eType> display_type_enum =
{
    {"CID", hmi_apis::Common_DisplayType::CID},
    {"TYPE2", hmi_apis::Common_DisplayType::TYPE2},
    {"TYPE5", hmi_apis::Common_DisplayType::TYPE5},
    {"NGN", hmi_apis::Common_DisplayType::NGN},
    {"GEN2_8_DMA", hmi_apis::Common_DisplayType::GEN2_8_DMA},
    {"GEN2_6_DMA", hmi_apis::Common_DisplayType::GEN2_6_DMA},
    {"MFD3", hmi_apis::Common_DisplayType::MFD3},
    {"MFD4", hmi_apis::Common_DisplayType::MFD4},
    {"MFD5", hmi_apis::Common_DisplayType::MFD5},
    {"GEN3_8_INCH", hmi_apis::Common_DisplayType::GEN3_8_INCH}
};
#endif
#endif

#ifndef BUILD_TARGET_LIB
#if defined(OS_MAC) || defined(OS_WINCE)
std::map<std::string, hmi_apis::Common_CharacterSet::eType> character_set_enum;
#else
const std::map<std::string, hmi_apis::Common_CharacterSet::eType> character_set_enum =
{
    {"TYPE2SET" , hmi_apis::Common_CharacterSet::TYPE2SET},
    {"TYPE5SET" , hmi_apis::Common_CharacterSet::TYPE5SET},
    {"CID1SET" ,  hmi_apis::Common_CharacterSet::CID1SET},
    {"CID2SET" ,  hmi_apis::Common_CharacterSet::CID2SET}
};
#endif
#endif

HMICapabilities::HMICapabilities(ApplicationManagerImpl* const app_mngr)
  : is_vr_cooperating_(false),
    is_tts_cooperating_(false),
    is_ui_cooperating_(false),
    is_navi_cooperating_(false),
    is_ivi_cooperating_(false),
    is_vr_ready_response_recieved_(false),
    is_tts_ready_response_recieved_(false),
    is_ui_ready_response_recieved_(false),
    is_navi_ready_response_recieved_(false),
    is_ivi_ready_response_recieved_(false),
    ui_language_(hmi_apis::Common_Language::INVALID_ENUM),
    vr_language_(hmi_apis::Common_Language::INVALID_ENUM),
    tts_language_(hmi_apis::Common_Language::INVALID_ENUM),
    vehicle_type_(NULL),
    attenuated_supported_(false),
    ui_supported_languages_(NULL),
    tts_supported_languages_(NULL),
    vr_supported_languages_(NULL),
    display_capabilities_(NULL),
    hmi_zone_capabilities_(NULL),
    soft_buttons_capabilities_(NULL),
    button_capabilities_(NULL),
    preset_bank_capabilities_(NULL),
    vr_capabilities_(NULL),
    speech_capabilities_(NULL),
    audio_pass_thru_capabilities_(NULL),
    app_mngr_(app_mngr),
    prerecorded_speech_(NULL) {
#if defined(OS_MAC) || defined(OS_WINCE)
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("EN_US", hmi_apis::Common_Language::EN_US));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("ES_MX", hmi_apis::Common_Language::ES_MX));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("FR_CA", hmi_apis::Common_Language::FR_CA));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("DE_DE", hmi_apis::Common_Language::DE_DE));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("ES_ES", hmi_apis::Common_Language::ES_ES));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("EN_GB", hmi_apis::Common_Language::EN_GB));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("RU_RU", hmi_apis::Common_Language::RU_RU));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("TR_TR", hmi_apis::Common_Language::TR_TR));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("PL_PL", hmi_apis::Common_Language::PL_PL));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("FR_FR", hmi_apis::Common_Language::FR_FR));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("IT_IT", hmi_apis::Common_Language::IT_IT));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("SV_SE", hmi_apis::Common_Language::SV_SE));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("PT_PT", hmi_apis::Common_Language::PT_PT));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("NL_NL", hmi_apis::Common_Language::NL_NL));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("EN_AU", hmi_apis::Common_Language::EN_AU));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("ZH_CN", hmi_apis::Common_Language::ZH_CN));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("ZH_TW", hmi_apis::Common_Language::ZH_TW));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("JA_JP", hmi_apis::Common_Language::JA_JP));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("AR_SA", hmi_apis::Common_Language::AR_SA));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("KO_KR", hmi_apis::Common_Language::KO_KR));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("PT_BR", hmi_apis::Common_Language::PT_BR));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("CS_CZ", hmi_apis::Common_Language::CS_CZ));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("DA_DK", hmi_apis::Common_Language::DA_DK));
    languages_enum_values.insert(std::map<std::string, hmi_apis::Common_Language::eType>::value_type("NO_NO", hmi_apis::Common_Language::NO_NO));
    
    vr_enum_capabilities.insert(std::map<std::string, hmi_apis::Common_VrCapabilities::eType>::value_type("TEXT", hmi_apis::Common_VrCapabilities::VR_TEXT));
    
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("OK"             , hmi_apis::Common_ButtonName::OK));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("SEEKLEFT"       , hmi_apis::Common_ButtonName::SEEKLEFT));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("SEEKRIGHT"      , hmi_apis::Common_ButtonName::SEEKRIGHT));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("TUNEUP"         , hmi_apis::Common_ButtonName::TUNEUP));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("TUNEDOWN"       , hmi_apis::Common_ButtonName::TUNEDOWN));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("PRESET_0"       , hmi_apis::Common_ButtonName::PRESET_0));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("PRESET_1"       , hmi_apis::Common_ButtonName::PRESET_1));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("PRESET_2"       , hmi_apis::Common_ButtonName::PRESET_2));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("PRESET_3"       , hmi_apis::Common_ButtonName::PRESET_3));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("PRESET_4"       , hmi_apis::Common_ButtonName::PRESET_4));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("PRESET_5"       , hmi_apis::Common_ButtonName::PRESET_5));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("PRESET_6"       , hmi_apis::Common_ButtonName::PRESET_6));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("PRESET_7"       , hmi_apis::Common_ButtonName::PRESET_7));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("PRESET_8"       , hmi_apis::Common_ButtonName::PRESET_8));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("PRESET_9"       , hmi_apis::Common_ButtonName::PRESET_9));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("CUSTOM_BUTTON"  , hmi_apis::Common_ButtonName::CUSTOM_BUTTON));
    button_enum_name.insert(std::map<std::string, hmi_apis::Common_ButtonName::eType>::value_type("SEARCH"         , hmi_apis::Common_ButtonName::SEARCH));

    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("mainField1", hmi_apis::Common_TextFieldName::mainField1));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("mainField2", hmi_apis::Common_TextFieldName::mainField2));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("mainField3", hmi_apis::Common_TextFieldName::mainField3));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("mainField4", hmi_apis::Common_TextFieldName::mainField4));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("statusBar" , hmi_apis::Common_TextFieldName::statusBar));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("mediaClock", hmi_apis::Common_TextFieldName::mediaClock));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("mediaTrack", hmi_apis::Common_TextFieldName::mediaTrack));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("alertText1", hmi_apis::Common_TextFieldName::alertText1));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("alertText2", hmi_apis::Common_TextFieldName::alertText2));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("alertText3", hmi_apis::Common_TextFieldName::alertText3));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("scrollableMessageBody" , hmi_apis::Common_TextFieldName::scrollableMessageBody));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("initialInteractionText", hmi_apis::Common_TextFieldName::initialInteractionText));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("navigationText1"       , hmi_apis::Common_TextFieldName::navigationText1));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("navigationText2"       , hmi_apis::Common_TextFieldName::navigationText2));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("ETA"                   , hmi_apis::Common_TextFieldName::ETA));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("totalDistance"         , hmi_apis::Common_TextFieldName::totalDistance));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("audioPassThruDisplayText1", hmi_apis::Common_TextFieldName::audioPassThruDisplayText1));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("audioPassThruDisplayText2", hmi_apis::Common_TextFieldName::audioPassThruDisplayText2));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("sliderHeader"     , hmi_apis::Common_TextFieldName::sliderHeader));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("sliderFooter"     , hmi_apis::Common_TextFieldName::sliderFooter));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("notificationText" , hmi_apis::Common_TextFieldName::notificationText));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("menuName"         , hmi_apis::Common_TextFieldName::menuName));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("secondaryText"    , hmi_apis::Common_TextFieldName::secondaryText));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("tertiaryText"     , hmi_apis::Common_TextFieldName::tertiaryText));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("timeToDestination", hmi_apis::Common_TextFieldName::timeToDestination));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("turnText"         , hmi_apis::Common_TextFieldName::turnText));
    text_fields_enum_name.insert(std::map<std::string, hmi_apis::Common_TextFieldName::eType>::value_type("menuTitle"         , hmi_apis::Common_TextFieldName::menuTitle));
	
    media_clock_enum_name.insert(std::map<std::string, hmi_apis::Common_MediaClockFormat::eType>::value_type("CLOCK1"    , hmi_apis::Common_MediaClockFormat::CLOCK1));
    media_clock_enum_name.insert(std::map<std::string, hmi_apis::Common_MediaClockFormat::eType>::value_type("CLOCK2"    , hmi_apis::Common_MediaClockFormat::CLOCK2));
    media_clock_enum_name.insert(std::map<std::string, hmi_apis::Common_MediaClockFormat::eType>::value_type("CLOCK3"    , hmi_apis::Common_MediaClockFormat::CLOCK3));
    media_clock_enum_name.insert(std::map<std::string, hmi_apis::Common_MediaClockFormat::eType>::value_type("CLOCKTEXT1", hmi_apis::Common_MediaClockFormat::CLOCKTEXT1));
    media_clock_enum_name.insert(std::map<std::string, hmi_apis::Common_MediaClockFormat::eType>::value_type("CLOCKTEXT2", hmi_apis::Common_MediaClockFormat::CLOCKTEXT2));
    media_clock_enum_name.insert(std::map<std::string, hmi_apis::Common_MediaClockFormat::eType>::value_type("CLOCKTEXT3", hmi_apis::Common_MediaClockFormat::CLOCKTEXT3));
    media_clock_enum_name.insert(std::map<std::string, hmi_apis::Common_MediaClockFormat::eType>::value_type("CLOCKTEXT4", hmi_apis::Common_MediaClockFormat::CLOCKTEXT4));

    image_type_enum.insert(std::map<std::string, hmi_apis::Common_ImageType::eType>::value_type("STATIC" , hmi_apis::Common_ImageType::STATIC));
    image_type_enum.insert(std::map<std::string, hmi_apis::Common_ImageType::eType>::value_type("DYNAMIC", hmi_apis::Common_ImageType::DYNAMIC));

    sampling_rate_enum.insert(std::map<std::string, hmi_apis::Common_SamplingRate::eType>::value_type("8KHZ" , hmi_apis::Common_SamplingRate::RATE_8KHZ));
    sampling_rate_enum.insert(std::map<std::string, hmi_apis::Common_SamplingRate::eType>::value_type("16KHZ", hmi_apis::Common_SamplingRate::RATE_16KHZ));
    sampling_rate_enum.insert(std::map<std::string, hmi_apis::Common_SamplingRate::eType>::value_type("22KHZ", hmi_apis::Common_SamplingRate::RATE_22KHZ));
    sampling_rate_enum.insert(std::map<std::string, hmi_apis::Common_SamplingRate::eType>::value_type("44KHZ", hmi_apis::Common_SamplingRate::RATE_44KHZ));

    bit_per_sample_enum.insert(std::map<std::string, hmi_apis::Common_BitsPerSample::eType>::value_type("RATE_8_BIT", hmi_apis::Common_BitsPerSample::RATE_8_BIT));
    bit_per_sample_enum.insert(std::map<std::string, hmi_apis::Common_BitsPerSample::eType>::value_type("RATE_16_BIT", hmi_apis::Common_BitsPerSample::RATE_16_BIT));

    audio_type_enum.insert(std::map<std::string, hmi_apis::Common_AudioType::eType>::value_type("PCM", hmi_apis::Common_AudioType::PCM));

    hmi_zone_enum.insert(std::map<std::string, hmi_apis::Common_HmiZoneCapabilities::eType>::value_type("FRONT", hmi_apis::Common_HmiZoneCapabilities::FRONT));
    hmi_zone_enum.insert(std::map<std::string, hmi_apis::Common_HmiZoneCapabilities::eType>::value_type("BACK", hmi_apis::Common_HmiZoneCapabilities::BACK));

	image_field_name_enum.insert(std::map<std::string, hmi_apis::Common_ImageFieldName::eType>::value_type("softButtonImage", hmi_apis::Common_ImageFieldName::softButtonImage));
	image_field_name_enum.insert(std::map<std::string, hmi_apis::Common_ImageFieldName::eType>::value_type("choiceImage", hmi_apis::Common_ImageFieldName::choiceImage));
	image_field_name_enum.insert(std::map<std::string, hmi_apis::Common_ImageFieldName::eType>::value_type("choiceSecondaryImage", hmi_apis::Common_ImageFieldName::choiceSecondaryImage));
	image_field_name_enum.insert(std::map<std::string, hmi_apis::Common_ImageFieldName::eType>::value_type("vrHelpItem", hmi_apis::Common_ImageFieldName::vrHelpItem));
	image_field_name_enum.insert(std::map<std::string, hmi_apis::Common_ImageFieldName::eType>::value_type("turnIcon", hmi_apis::Common_ImageFieldName::turnIcon));
	image_field_name_enum.insert(std::map<std::string, hmi_apis::Common_ImageFieldName::eType>::value_type("menuIcon", hmi_apis::Common_ImageFieldName::menuIcon));
	image_field_name_enum.insert(std::map<std::string, hmi_apis::Common_ImageFieldName::eType>::value_type("cmdIcon", hmi_apis::Common_ImageFieldName::cmdIcon));
	image_field_name_enum.insert(std::map<std::string, hmi_apis::Common_ImageFieldName::eType>::value_type("appIcon", hmi_apis::Common_ImageFieldName::appIcon));
	image_field_name_enum.insert(std::map<std::string, hmi_apis::Common_ImageFieldName::eType>::value_type("graphic", hmi_apis::Common_ImageFieldName::graphic));
	image_field_name_enum.insert(std::map<std::string, hmi_apis::Common_ImageFieldName::eType>::value_type("showConstantTBTIcon", hmi_apis::Common_ImageFieldName::showConstantTBTIcon));
	image_field_name_enum.insert(std::map<std::string, hmi_apis::Common_ImageFieldName::eType>::value_type("showConstantTBTNextTurnIcon",
        hmi_apis::Common_ImageFieldName::showConstantTBTNextTurnIcon));

	file_type_enum.insert(std::map<std::string, hmi_apis::Common_FileType::eType>::value_type("GRAPHIC_BMP", hmi_apis::Common_FileType::GRAPHIC_BMP));
	file_type_enum.insert(std::map<std::string, hmi_apis::Common_FileType::eType>::value_type("GRAPHIC_JPEG", hmi_apis::Common_FileType::GRAPHIC_JPEG));
	file_type_enum.insert(std::map<std::string, hmi_apis::Common_FileType::eType>::value_type("GRAPHIC_PNG", hmi_apis::Common_FileType::GRAPHIC_PNG));
	file_type_enum.insert(std::map<std::string, hmi_apis::Common_FileType::eType>::value_type("AUDIO_WAVE", hmi_apis::Common_FileType::AUDIO_WAVE));
	file_type_enum.insert(std::map<std::string, hmi_apis::Common_FileType::eType>::value_type("AUDIO_MP3", hmi_apis::Common_FileType::AUDIO_MP3));
	file_type_enum.insert(std::map<std::string, hmi_apis::Common_FileType::eType>::value_type("AUDIO_AAC", hmi_apis::Common_FileType::AUDIO_AAC));
	file_type_enum.insert(std::map<std::string, hmi_apis::Common_FileType::eType>::value_type("BINARY", hmi_apis::Common_FileType::BINARY));
	file_type_enum.insert(std::map<std::string, hmi_apis::Common_FileType::eType>::value_type("JSON", hmi_apis::Common_FileType::JSON));

	display_type_enum.insert(std::map<std::string, hmi_apis::Common_DisplayType::eType>::value_type("CID", hmi_apis::Common_DisplayType::CID));
	display_type_enum.insert(std::map<std::string, hmi_apis::Common_DisplayType::eType>::value_type("TYPE2", hmi_apis::Common_DisplayType::TYPE2));
	display_type_enum.insert(std::map<std::string, hmi_apis::Common_DisplayType::eType>::value_type("TYPE5", hmi_apis::Common_DisplayType::TYPE5));
	display_type_enum.insert(std::map<std::string, hmi_apis::Common_DisplayType::eType>::value_type("NGN", hmi_apis::Common_DisplayType::NGN));
	display_type_enum.insert(std::map<std::string, hmi_apis::Common_DisplayType::eType>::value_type("GEN2_8_DMA", hmi_apis::Common_DisplayType::GEN2_8_DMA));
	display_type_enum.insert(std::map<std::string, hmi_apis::Common_DisplayType::eType>::value_type("GEN2_6_DMA", hmi_apis::Common_DisplayType::GEN2_6_DMA));
	display_type_enum.insert(std::map<std::string, hmi_apis::Common_DisplayType::eType>::value_type("MFD3", hmi_apis::Common_DisplayType::MFD3));
	display_type_enum.insert(std::map<std::string, hmi_apis::Common_DisplayType::eType>::value_type("MFD4", hmi_apis::Common_DisplayType::MFD4));
	display_type_enum.insert(std::map<std::string, hmi_apis::Common_DisplayType::eType>::value_type("MFD5", hmi_apis::Common_DisplayType::MFD5));
	display_type_enum.insert(std::map<std::string, hmi_apis::Common_DisplayType::eType>::value_type("GEN3_8_INCH", hmi_apis::Common_DisplayType::GEN3_8_INCH));

	character_set_enum.insert(std::map<std::string, hmi_apis::Common_CharacterSet::eType>::value_type("TYPE2SET" , hmi_apis::Common_CharacterSet::TYPE2SET));
	character_set_enum.insert(std::map<std::string, hmi_apis::Common_CharacterSet::eType>::value_type("TYPE5SET" , hmi_apis::Common_CharacterSet::TYPE5SET));
	character_set_enum.insert(std::map<std::string, hmi_apis::Common_CharacterSet::eType>::value_type("CID1SET" ,  hmi_apis::Common_CharacterSet::CID1SET));
	character_set_enum.insert(std::map<std::string, hmi_apis::Common_CharacterSet::eType>::value_type("CID2SET" ,  hmi_apis::Common_CharacterSet::CID2SET));
#endif

  if (false == load_capabilities_from_file()) {
    LOG4CXX_ERROR(logger_, "file hmi_capabilities.json was not loaded");
  } else {
    LOG4CXX_INFO(logger_, "file hmi_capabilities.json was loaded");
  }
  if (false == profile::Profile::instance()->launch_hmi()) {
    is_vr_ready_response_recieved_ = true;
    is_tts_ready_response_recieved_ = true;
    is_ui_ready_response_recieved_ = true;
    is_navi_ready_response_recieved_ = true;
    is_ivi_ready_response_recieved_ = true;

    is_vr_cooperating_ = true;
    is_tts_cooperating_ = true;
    is_ui_cooperating_ = true;
    is_navi_cooperating_ = true;
    is_ivi_cooperating_ = true;
  }
}

HMICapabilities::~HMICapabilities() {
  delete vehicle_type_;
  delete ui_supported_languages_;
  delete tts_supported_languages_;
  delete vr_supported_languages_;
  delete display_capabilities_;
  delete hmi_zone_capabilities_;
  delete soft_buttons_capabilities_;
  delete button_capabilities_;
  delete preset_bank_capabilities_;
  delete vr_capabilities_;
  delete speech_capabilities_;
  delete audio_pass_thru_capabilities_;
  delete prerecorded_speech_;
  app_mngr_ = NULL;
}

bool HMICapabilities::is_hmi_capabilities_initialized() const {
  bool result = true;

  if (is_vr_ready_response_recieved_ && is_tts_ready_response_recieved_
      && is_ui_ready_response_recieved_ && is_navi_ready_response_recieved_
      && is_ivi_ready_response_recieved_) {
    if (is_vr_cooperating_) {
      if ((!vr_supported_languages_) ||
          (hmi_apis::Common_Language::INVALID_ENUM == vr_language_)) {
        result = false;
      }
    }

    if (is_tts_cooperating_) {
      if ((!tts_supported_languages_) ||
          (hmi_apis::Common_Language::INVALID_ENUM == tts_language_)) {
        result = false;
      }
    }

    if (is_ui_cooperating_) {
      if ((!ui_supported_languages_)  ||
          (hmi_apis::Common_Language::INVALID_ENUM == ui_language_)) {
         result = false;
      }
    }

    if (is_ivi_cooperating_) {
      if (!vehicle_type_) {
        result = false;
      }
    }
  } else {
    result = false;
  }

  return result;
}

bool HMICapabilities::VerifyImageType(int32_t image_type) const {
  if (!display_capabilities_) {
    return false;
  }

  if (display_capabilities_->keyExists(hmi_response::image_capabilities)) {
    const smart_objects::SmartObject& image_caps = display_capabilities_
        ->getElement(hmi_response::image_capabilities);
    for (int32_t i = 0; i < image_caps.length(); ++i) {
      if (image_caps.getElement(i).asInt() == image_type) {
        return true;
      }
    }
  }

  return false;
}

void HMICapabilities::set_is_vr_cooperating(bool value) {
  is_vr_ready_response_recieved_ = true;
  is_vr_cooperating_ = value;
  if (is_vr_cooperating_) {
    utils::SharedPtr<smart_objects::SmartObject> get_language(
      MessageHelper::CreateModuleInfoSO(hmi_apis::FunctionID::VR_GetLanguage));
    app_mngr_->ManageHMICommand(get_language);
    utils::SharedPtr<smart_objects::SmartObject> get_all_languages(
      MessageHelper::CreateModuleInfoSO(hmi_apis::FunctionID::VR_GetSupportedLanguages));
    app_mngr_->ManageHMICommand(get_all_languages);
    utils::SharedPtr<smart_objects::SmartObject> get_capabilities(
      MessageHelper::CreateModuleInfoSO(hmi_apis::FunctionID::VR_GetCapabilities));
    app_mngr_->ManageHMICommand(get_capabilities);
  }
}

void HMICapabilities::set_is_tts_cooperating(bool value) {
  is_tts_ready_response_recieved_ = true;
  is_tts_cooperating_ = value;
  if (is_tts_cooperating_) {
    utils::SharedPtr<smart_objects::SmartObject> get_language(
      MessageHelper::CreateModuleInfoSO(
        hmi_apis::FunctionID::TTS_GetLanguage));
    app_mngr_->ManageHMICommand(get_language);
    utils::SharedPtr<smart_objects::SmartObject> get_all_languages(
      MessageHelper::CreateModuleInfoSO(
        hmi_apis::FunctionID::TTS_GetSupportedLanguages));
    app_mngr_->ManageHMICommand(get_all_languages);
    utils::SharedPtr<smart_objects::SmartObject> get_capabilities(
      MessageHelper::CreateModuleInfoSO(
        hmi_apis::FunctionID::TTS_GetCapabilities));
    app_mngr_->ManageHMICommand(get_capabilities);
  }
}

void HMICapabilities::set_is_ui_cooperating(bool value) {
  is_ui_ready_response_recieved_ = true;
  is_ui_cooperating_ = value;
  if (is_ui_cooperating_) {
    utils::SharedPtr<smart_objects::SmartObject> get_language(
      MessageHelper::CreateModuleInfoSO(
        hmi_apis::FunctionID::UI_GetLanguage));
    app_mngr_->ManageHMICommand(get_language);
    utils::SharedPtr<smart_objects::SmartObject> get_all_languages(
      MessageHelper::CreateModuleInfoSO(
        hmi_apis::FunctionID::UI_GetSupportedLanguages));
    app_mngr_->ManageHMICommand(get_all_languages);
    utils::SharedPtr<smart_objects::SmartObject> get_capabilities(
      MessageHelper::CreateModuleInfoSO(
        hmi_apis::FunctionID::UI_GetCapabilities));
    app_mngr_->ManageHMICommand(get_capabilities);
  }
}

void HMICapabilities::set_is_navi_cooperating(bool value) {
  is_navi_ready_response_recieved_ = true;
  is_navi_cooperating_ = value;
}

void HMICapabilities::set_is_ivi_cooperating(bool value) {
  is_ivi_ready_response_recieved_ = true;
  is_ivi_cooperating_ = value;
  if (is_ivi_cooperating_) {
    utils::SharedPtr<smart_objects::SmartObject> get_type(
      MessageHelper::CreateModuleInfoSO(
        hmi_apis::FunctionID::VehicleInfo_GetVehicleType));
    app_mngr_->ManageHMICommand(get_type);
  }
}

void HMICapabilities::set_attenuated_supported(bool state) {
  attenuated_supported_ = state;
}

void HMICapabilities::set_active_ui_language(
  const hmi_apis::Common_Language::eType& language) {
  ui_language_ = language;
}

void HMICapabilities::set_active_vr_language(
  const hmi_apis::Common_Language::eType& language) {
  vr_language_ = language;
}

void HMICapabilities::set_active_tts_language(
  const hmi_apis::Common_Language::eType& language) {
  tts_language_ = language;
}

void HMICapabilities::set_ui_supported_languages(
    const smart_objects::SmartObject& supported_languages) {
  if (ui_supported_languages_) {
    delete ui_supported_languages_;
  }
  ui_supported_languages_ = new smart_objects::SmartObject(supported_languages);
}

void HMICapabilities::set_tts_supported_languages(
    const smart_objects::SmartObject& supported_languages) {
  if (tts_supported_languages_) {
    delete tts_supported_languages_;
  }
  tts_supported_languages_ = new smart_objects::SmartObject(
      supported_languages);
}

void HMICapabilities::set_vr_supported_languages(
    const smart_objects::SmartObject& supported_languages) {
  if (vr_supported_languages_) {
    delete vr_supported_languages_;
  }
  vr_supported_languages_ = new smart_objects::SmartObject(supported_languages);
}

void HMICapabilities::set_display_capabilities(
    const smart_objects::SmartObject& display_capabilities) {
  if (display_capabilities_) {
    delete display_capabilities_;
  }
  display_capabilities_ = new smart_objects::SmartObject(display_capabilities);
}

void HMICapabilities::set_hmi_zone_capabilities(
    const smart_objects::SmartObject& hmi_zone_capabilities) {
  if (hmi_zone_capabilities_) {
    delete hmi_zone_capabilities_;
  }
  hmi_zone_capabilities_ = new smart_objects::SmartObject(
      hmi_zone_capabilities);
}

void HMICapabilities::set_soft_button_capabilities(
    const smart_objects::SmartObject& soft_button_capabilities) {
  if (soft_buttons_capabilities_) {
    delete soft_buttons_capabilities_;
  }
  soft_buttons_capabilities_ = new smart_objects::SmartObject(
      soft_button_capabilities);
}

void HMICapabilities::set_button_capabilities(
    const smart_objects::SmartObject& button_capabilities) {
  if (button_capabilities_) {
    delete button_capabilities_;
  }
  button_capabilities_ = new smart_objects::SmartObject(button_capabilities);
}

void HMICapabilities::set_vr_capabilities(
    const smart_objects::SmartObject& vr_capabilities) {
  if (vr_capabilities_) {
    delete vr_capabilities_;
  }
  vr_capabilities_ = new smart_objects::SmartObject(vr_capabilities);
}

void HMICapabilities::set_speech_capabilities(
    const smart_objects::SmartObject& speech_capabilities) {
  if (speech_capabilities_) {
    delete speech_capabilities_;
  }
  speech_capabilities_ = new smart_objects::SmartObject(speech_capabilities);
}

void HMICapabilities::set_audio_pass_thru_capabilities(
    const smart_objects::SmartObject& audio_pass_thru_capabilities) {
  if (audio_pass_thru_capabilities_) {
    delete audio_pass_thru_capabilities_;
  }
  audio_pass_thru_capabilities_ = new smart_objects::SmartObject(
      audio_pass_thru_capabilities);
}

void HMICapabilities::set_preset_bank_capabilities(
    const smart_objects::SmartObject& preset_bank_capabilities) {
  if (preset_bank_capabilities_) {
    delete preset_bank_capabilities_;
  }
  preset_bank_capabilities_ = new smart_objects::SmartObject(
      preset_bank_capabilities);
}

void HMICapabilities::set_vehicle_type(
  const smart_objects::SmartObject& vehicle_type) {
  if (vehicle_type_) {
    delete vehicle_type_;
  }
  vehicle_type_ = new smart_objects::SmartObject(vehicle_type);
}

void HMICapabilities::set_prerecorded_speech(
       const smart_objects::SmartObject& prerecorded_speech) {
  if (prerecorded_speech_) {
    delete prerecorded_speech_;
    prerecorded_speech_ = NULL;
  }
  prerecorded_speech_ = new smart_objects::SmartObject(prerecorded_speech);
}

bool HMICapabilities::load_capabilities_from_file() {
  std::string json_string;
  std::string file_name =
      profile::Profile::instance()->hmi_capabilities_file_name();

  if (!file_system::FileExists(file_name)) {
    return false;
  }

  if (!file_system::ReadFile(file_name, json_string)) {
    return false;
  }

  try {

    Json::Reader reader_;
    Json::Value  root_json;

    bool result = reader_.parse(json_string, root_json, false);
    if (!result) {
      return false;
    }
    // UI
    if (check_existing_json_member(root_json, "UI")) {
      Json::Value ui = root_json.get("UI", "");

      if (check_existing_json_member(ui, "language")) {
#ifdef MODIFY_FUNCTION_SIGN
		  std::map<std::string, hmi_apis::Common_Language::eType>::iterator find_iter = 
			languages_enum_values.find(ui.get("language", "").asString());
		  if(find_iter != languages_enum_values.end()){
			  set_active_ui_language(find_iter->second);
		  }else{
			  set_active_ui_language(hmi_apis::Common_Language::EN_US);
		  }
#else
        set_active_ui_language(
            languages_enum_values.find(ui.get("language", "").asString())->second);
#endif
      }

      if (check_existing_json_member(ui, "languages")) {
        smart_objects::SmartObject ui_languages_so(
            smart_objects::SmartType_Array);
        Json::Value languages_ui = ui.get("languages", "");
        convert_json_languages_to_obj(languages_ui, ui_languages_so);
        set_ui_supported_languages(ui_languages_so);
      }

      if (check_existing_json_member(ui, "displayCapabilities")) {
        smart_objects::SmartObject display_capabilities_so;
        Json::Value display_capabilities = ui.get("displayCapabilities", "");
        Formatters::CFormatterJsonBase::jsonValueToObj(
            display_capabilities, display_capabilities_so);

        if (display_capabilities_so.keyExists(hmi_response::display_type)) {
          std::map<std::string, hmi_apis::Common_DisplayType::eType>
          ::const_iterator it = display_type_enum.find(
              (display_capabilities_so[hmi_response::display_type]).asString());
          display_capabilities_so.erase(hmi_response::display_type);
          if (display_type_enum.end() != it) {
            display_capabilities_so[hmi_response::display_type] = it->second;
          }
        }

        if (display_capabilities_so.keyExists(hmi_response::text_fields)) {
          int32_t len =
              display_capabilities_so[hmi_response::text_fields].length();

          for (int32_t i = 0; i < len; ++i) {

            if ((display_capabilities_so
                [hmi_response::text_fields][i]).keyExists(strings::name)) {
              std::map<std::string, hmi_apis::Common_TextFieldName::eType>
              ::const_iterator it_text_field_name = text_fields_enum_name.find(
                  display_capabilities_so[hmi_response::text_fields]
                                          [i][strings::name].asString());
              display_capabilities_so[hmi_response::text_fields][i].erase(strings::name);
              if (text_fields_enum_name.end() != it_text_field_name) {
                display_capabilities_so[hmi_response::text_fields]
                        [i][strings::name] =  it_text_field_name->second;
              }
            }
            if ((display_capabilities_so
                [hmi_response::text_fields][i]).keyExists(strings::character_set)) {
              std::map<std::string, hmi_apis::Common_CharacterSet::eType>
              ::const_iterator it_characte_set = character_set_enum.find(
                  display_capabilities_so[hmi_response::text_fields]
                                          [i][strings::character_set].asString());
              display_capabilities_so
              [hmi_response::text_fields][i].erase(strings::character_set);
              if (character_set_enum.end() != it_characte_set) {
                display_capabilities_so
                [hmi_response::text_fields][i][strings::character_set] =
                    it_characte_set->second;
              }
            }
          }
        }

        if (display_capabilities_so.keyExists(hmi_response::image_fields)) {
          smart_objects::SmartObject& array_image_fields =
              display_capabilities_so[hmi_response::image_fields];
          for (int32_t i = 0; i < array_image_fields.length(); ++i) {
            if (array_image_fields[i].keyExists(strings::name)) {
              std::map<std::string, hmi_apis::Common_ImageFieldName::eType>
              ::const_iterator it = image_field_name_enum.find(
                  (array_image_fields[i][strings::name]).asString());
              array_image_fields[i].erase(strings::name);
              if (image_field_name_enum.end() != it) {
                array_image_fields[i][strings::name] = it->second;
              }
            }
            if (array_image_fields[i].keyExists(strings::image_type_supported)) {
              smart_objects::SmartObject& image_type_supported_array =
                  array_image_fields[i][strings::image_type_supported];
              smart_objects::SmartObject image_type_supported_enum(
                  smart_objects::SmartType_Array);
              for (int32_t k = 0, j = 0; k < image_type_supported_array.length(); ++k) {
                std::map<std::string, hmi_apis::Common_FileType::eType>
                ::const_iterator it = file_type_enum.find(
                    (image_type_supported_array[k]).asString());
                if (file_type_enum.end() != it) {
                  image_type_supported_enum[j++] = it->second;
                }
              }
              array_image_fields[i].erase(strings::image_type_supported);
              array_image_fields[i][strings::image_type_supported] =
                  image_type_supported_enum;
            }
          }
        }
        if (display_capabilities_so.keyExists(hmi_response::media_clock_formats)) {
          smart_objects::SmartObject& media_clock_formats_array =
              display_capabilities_so[hmi_response::media_clock_formats];
          smart_objects::SmartObject media_clock_formats_enum(
              smart_objects::SmartType_Array);
          for (int32_t i = 0, j = 0; i < media_clock_formats_array.length(); ++i) {
            std::map<std::string, hmi_apis::Common_MediaClockFormat::eType>
            ::const_iterator it = media_clock_enum_name.find(
                (media_clock_formats_array[i]).asString());
            if (media_clock_enum_name.end() != it) {
              media_clock_formats_enum[j++] = it->second;
            }
          }
          display_capabilities_so.erase(hmi_response::media_clock_formats);
          display_capabilities_so[hmi_response::media_clock_formats] =
              media_clock_formats_enum;
        }

        if (display_capabilities_so.keyExists(hmi_response::image_capabilities)) {
          smart_objects::SmartObject& image_capabilities_array =
              display_capabilities_so[hmi_response::image_capabilities];
          smart_objects::SmartObject image_capabilities_enum(
              smart_objects::SmartType_Array);
          for (int32_t i = 0, j = 0; i < image_capabilities_array.length(); ++i) {
            std::map<std::string, hmi_apis::Common_ImageType::eType>
            ::const_iterator it = image_type_enum.find(
                (image_capabilities_array[i]).asString());
            if (image_type_enum.end() != it) {
              image_capabilities_enum[j++] = it->second;
            }
          }
          display_capabilities_so.erase(hmi_response::image_capabilities);
          display_capabilities_so[hmi_response::image_capabilities] =
              image_capabilities_enum;
        }
        set_display_capabilities(display_capabilities_so);
      }

      if (check_existing_json_member(ui, "audioPassThruCapabilities")) {
        Json::Value audio_capabilities = ui.get("audioPassThruCapabilities", "");
        smart_objects::SmartObject audio_capabilities_so =
            smart_objects::SmartObject(smart_objects::SmartType_Array);
        int32_t i = 0;
        audio_capabilities_so[i] =
            smart_objects::SmartObject(smart_objects::SmartType_Map);
        if (check_existing_json_member(audio_capabilities, "samplingRate")) {
          audio_capabilities_so[i]["samplingRate"] =
              sampling_rate_enum.find(
                  audio_capabilities.get("samplingRate", "").asString())->second;
        }
        if (check_existing_json_member(audio_capabilities, "bitsPerSample")) {
          audio_capabilities_so[i]["bitsPerSample"] =
              bit_per_sample_enum.find(
                  audio_capabilities.get("bitsPerSample", "").asString())->second;
        }
        if (check_existing_json_member(audio_capabilities, "audioType")) {
          audio_capabilities_so[i]["audioType"] =
              audio_type_enum.find(
                  audio_capabilities.get("audioType", "").asString())->second;
        }
        set_audio_pass_thru_capabilities(audio_capabilities_so);
      }

      if (check_existing_json_member(ui, "hmiZoneCapabilities")) {
        smart_objects::SmartObject hmi_zone_capabilities_so =
            smart_objects::SmartObject(smart_objects::SmartType_Array);
        int32_t index = 0;
        hmi_zone_capabilities_so[index] =
            hmi_zone_enum.find(ui.get("hmiZoneCapabilities", "").asString())->second;
        set_hmi_zone_capabilities(hmi_zone_capabilities_so);
      }

      if (check_existing_json_member(ui, "softButtonCapabilities")) {
        Json::Value soft_button_capabilities = ui.get(
            "softButtonCapabilities", "");
        smart_objects::SmartObject soft_button_capabilities_so;
        Formatters::CFormatterJsonBase::jsonValueToObj(
            soft_button_capabilities, soft_button_capabilities_so);
        set_soft_button_capabilities(soft_button_capabilities_so);
      }
    } //UI end

    // VR
    if (check_existing_json_member(root_json, "VR")) {
      Json::Value vr = root_json.get("VR", "");
      if (check_existing_json_member(vr, "language")) {
#ifdef MODIFY_FUNCTION_SIGN
		  std::map<std::string, hmi_apis::Common_Language::eType>::iterator find_iter = 
			languages_enum_values.find(vr.get("language", "").asString());
		  if(find_iter != languages_enum_values.end()){
			  set_active_vr_language(find_iter->second);
		  }else{
			  set_active_vr_language(hmi_apis::Common_Language::EN_US);
		  }
#else
        set_active_vr_language(
            languages_enum_values.find(vr.get("language", "").asString())->second);
#endif
      }

      if (check_existing_json_member(vr, "languages")) {
        Json::Value languages_vr = vr.get("languages", "");
        smart_objects::SmartObject vr_languages_so =
            smart_objects::SmartObject(smart_objects::SmartType_Array);
        convert_json_languages_to_obj(languages_vr, vr_languages_so);
        set_vr_supported_languages(vr_languages_so);
      }

      if (check_existing_json_member(vr, "capabilities")) {
        Json::Value capabilities = vr.get("capabilities", "");
        smart_objects::SmartObject vr_capabilities_so =
            smart_objects::SmartObject(smart_objects::SmartType_Array);
        for (int32_t i = 0; i < capabilities.size(); ++i) {
          vr_capabilities_so[i] =
              vr_enum_capabilities.find(capabilities[i].asString())->second;
        }
        set_vr_capabilities(vr_capabilities_so);
      }
    }//VR end

    //TTS
    if (check_existing_json_member(root_json, "TTS")) {
      Json::Value tts = root_json.get("TTS", "");

      if (check_existing_json_member(tts, "language")) {
#ifdef MODIFY_FUNCTION_SIGN
		  std::map<std::string, hmi_apis::Common_Language::eType>::iterator find_iter = 
			languages_enum_values.find(tts.get("language", "").asString());
		  if(find_iter != languages_enum_values.end()){
			  set_active_tts_language(find_iter->second);
		  }else{
			  set_active_tts_language(hmi_apis::Common_Language::EN_US);
		  }
#else
        set_active_tts_language(
            languages_enum_values.find(tts.get("language", "").asString())->second);
#endif
      }

      if (check_existing_json_member(tts, "languages")) {
        Json::Value languages_tts = tts.get("languages", "");
        smart_objects::SmartObject tts_languages_so =
            smart_objects::SmartObject(smart_objects::SmartType_Array);
        convert_json_languages_to_obj(languages_tts, tts_languages_so);
        set_tts_supported_languages(tts_languages_so);
      }

      if (check_existing_json_member(tts, "capabilities")) {
        set_speech_capabilities(
            smart_objects::SmartObject(tts.get("capabilities", "").asString()));
      }
    } //TTS end

    // Buttons
    if (check_existing_json_member(root_json, "Buttons")) {
      Json::Value buttons = root_json.get("Buttons", "");
      if (check_existing_json_member(buttons, "capabilities")) {
        Json::Value bt_capabilities = buttons.get("capabilities", "");
        smart_objects::SmartObject buttons_capabilities_so;
        Formatters::CFormatterJsonBase::jsonValueToObj(
            bt_capabilities, buttons_capabilities_so);

        for (int32_t i = 0; i < buttons_capabilities_so.length(); ++i) {
          if ((buttons_capabilities_so[i]).keyExists(strings::name)) {
            std::map<std::string, hmi_apis::Common_ButtonName::eType>
            ::const_iterator it = button_enum_name.find(
                (buttons_capabilities_so[i][strings::name]).asString());
            buttons_capabilities_so[i].erase(strings::name);
            if (button_enum_name.end() != it) {
              buttons_capabilities_so[i][strings::name] = it->second;
            }
          }
        }
        set_button_capabilities(buttons_capabilities_so);
      }
      if (check_existing_json_member(buttons, "presetBankCapabilities")) {
        Json::Value presetBank = buttons.get("presetBankCapabilities", "");
        smart_objects::SmartObject preset_bank_so;
        Formatters::CFormatterJsonBase::jsonValueToObj(
            presetBank, preset_bank_so);
        set_preset_bank_capabilities(preset_bank_so);
      }
    } //Buttons end

    //VehicleType
    if (check_existing_json_member(root_json, "VehicleInfo")) {
      Json::Value vehicle_info = root_json.get("VehicleInfo", "");
      smart_objects::SmartObject vehicle_type_so;
      Formatters::CFormatterJsonBase::jsonValueToObj(
          vehicle_info, vehicle_type_so);
      set_vehicle_type(vehicle_type_so);
    }// VehicleType end


  } catch (...) {
    return false;
  }
  return true;
}

bool HMICapabilities::check_existing_json_member(
    const Json::Value& json_member, const char* name_of_member) {
  return json_member.isMember(name_of_member);
}

void HMICapabilities::convert_json_languages_to_obj(Json::Value& json_languages,
                                     smart_objects::SmartObject& languages) {
  for (int32_t i = 0, j = 0; i < json_languages.size(); ++i) {
    std::map<std::string, hmi_apis::Common_Language::eType>::const_iterator it =
        languages_enum_values.find(json_languages[i].asString());
    if (languages_enum_values.end() != it) {
      languages[j++] =  it->second;
    }
  }
}

}  //  namespace application_manager

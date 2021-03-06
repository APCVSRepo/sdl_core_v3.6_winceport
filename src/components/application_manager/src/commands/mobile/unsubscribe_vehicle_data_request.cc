/*

 Copyright (c) 2013, Ford Motor Company
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following
 disclaimer in the documentation and/or other materials provided with the
 distribution.

 Neither the name of the Ford Motor Company nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef MODIFY_FUNCTION_SIGN
#include <global_first.h>
#endif
#include "application_manager/commands/mobile/unsubscribe_vehicle_data_request.h"
#include "application_manager/commands/command_impl.h"
#include "application_manager/application_manager_impl.h"
#include "application_manager/application_impl.h"
#include "application_manager/message_helper.h"
#include "interfaces/MOBILE_API.h"
#include "interfaces/HMI_API.h"
#include "application_manager/smart_object_keys.h"

namespace application_manager {
namespace commands {

UnsubscribeVehicleDataRequest::UnsubscribeVehicleDataRequest(
    const MessageSharedPtr& message)
    : CommandRequestImpl(message) {
}

UnsubscribeVehicleDataRequest::~UnsubscribeVehicleDataRequest() {
}

#ifdef HMI_DBUS_API
namespace {
  struct Subrequest {
    hmi_apis::FunctionID::eType func_id;
    const char* str;
  };
  Subrequest subrequests[] = {
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeGps, strings::gps},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeSpeed, strings::speed},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeRpm, strings::rpm},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeFuelLevel, strings::fuel_level},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeFuelLevel_State, strings::fuel_level_state},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeInstantFuelConsumption, strings::instant_fuel_consumption},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeExternalTemperature, strings::external_temp},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeVin, strings::vin},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribePrndl, strings::prndl},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeTirePressure, strings::tire_pressure},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeOdometer, strings::odometer},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeBeltStatus, strings::belt_status},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeBodyInformation, strings::body_information},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeDeviceStatus, strings::device_status},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeDriverBraking, strings::driver_braking},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeWiperStatus, strings::wiper_status},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeHeadLampStatus, strings::head_lamp_status},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeEngineTorque, strings::engine_torque},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeAccPedalPosition, strings::acc_pedal_pos},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeSteeringWheelAngle, strings::steering_wheel_angle},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeECallInfo, strings::e_call_info},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeAirbagStatus, strings::airbag_status},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeEmergencyEvent, strings::emergency_event},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeClusterModeStatus, strings::cluster_mode_status},
    { hmi_apis::FunctionID::VehicleInfo_UnsubscribeMyKey, strings::my_key},
  };
}
#endif // #ifdef HMI_DBUS_API

void UnsubscribeVehicleDataRequest::Run() {
  LOG4CXX_INFO(logger_, "UnsubscribeVehicleDataRequest::Run");

  ApplicationSharedPtr app = ApplicationManagerImpl::instance()->application(
      CommandRequestImpl::connection_key());

  if (!app) {
    LOG4CXX_ERROR(logger_, "NULL pointer");
    SendResponse(false, mobile_apis::Result::APPLICATION_NOT_REGISTERED);
    return;
  }

  // counter for items to subscribe
  int32_t items_to_unsubscribe = 0;
  // counter for subscribed items by application
  int32_t unsubscribed_items = 0;

#ifdef MODIFY_FUNCTION_SIGN
  // response params
  namespace NsSmart = NsSmartDeviceLink::NsSmartObjects;
  NsSmart::SmartObject response_params;
#endif
  const VehicleData& vehicle_data = MessageHelper::vehicle_data();
  VehicleData::const_iterator it = vehicle_data.begin();

  smart_objects::SmartObject msg_params = smart_objects::SmartObject(
      smart_objects::SmartType_Map);

  msg_params[strings::app_id] = app->app_id();

  for (; vehicle_data.end() != it; ++it) {
    std::string key_name = it->first;
    if ((*message_)[strings::msg_params].keyExists(key_name)) {
      bool is_key_enabled = (*message_)[strings::msg_params][key_name].asBool();
      if (is_key_enabled) {
        ++items_to_unsubscribe;
        msg_params[key_name] = is_key_enabled;

        VehicleDataType key_type = it->second;
        if (app->UnsubscribeFromIVI(static_cast<uint32_t>(key_type))) {
          ++unsubscribed_items;
#ifdef MODIFY_FUNCTION_SIGN
          response_params[it->first][strings::data_type] = it->second;
          response_params[it->first][strings::result_code] =
              mobile_apis::VehicleDataResultCode::VDRC_SUCCESS;
#endif
        }
      }
    }
  }

  if (0 == items_to_unsubscribe) {
    SendResponse(false, mobile_apis::Result::INVALID_DATA,
                 "No data in the request", &msg_params);
    return;
  } else if (0 == unsubscribed_items) {
    SendResponse(false, mobile_apis::Result::IGNORED,
                 "Was not subscribed on any VehicleData", &msg_params);
    return;
  }
#ifdef MODIFY_FUNCTION_SIGN
  SendResponse(true, mobile_apis::Result::SUCCESS,
                "Unsubscribed on provided VehicleData", &response_params);
#else // #ifdef MODIFY_FUNCTION_SIGN

#ifdef HMI_JSON_API
  SendHMIRequest(hmi_apis::FunctionID::VehicleInfo_UnsubscribeVehicleData,
      &msg_params, true);
#endif // #ifdef HMI_JSON_API

#ifdef HMI_DBUS_API
  //Generate list of subrequests
  for (int i = 0; i < sizeof(subrequests) / sizeof(subrequests[0]); ++i) {
    const Subrequest& sr = subrequests[i];
    if (true == (*message_)[strings::msg_params].keyExists(sr.str)
        && true == (*message_)[strings::msg_params][sr.str].asBool()) {
      HmiRequest hmi_request;
      hmi_request.str = sr.str;
      hmi_request.func_id = sr.func_id;
      hmi_request.complete = false;
      hmi_requests_.push_back(hmi_request);
    }
  }
  LOG4CXX_INFO(logger_,
               hmi_requests_.size() << " requests are going to be sent to HMI");

  //Send subrequests
  for (HmiRequests::const_iterator it = hmi_requests_.begin();
      it != hmi_requests_.end(); ++it)
    SendHMIRequest(it->func_id, &msg_params, true);
#endif // #ifdef HMI_DBUS_API
#endif // #ifdef MODIFY_FUNCTION_SIGN
}

void UnsubscribeVehicleDataRequest::on_event(const event_engine::Event& event) {
  LOG4CXX_INFO(logger_, "UnsubscribeVehicleDataRequest::on_event");

  const smart_objects::SmartObject& message = event.smart_object();

#ifdef HMI_JSON_API
  hmi_apis::Common_Result::eType hmi_result =
      static_cast<hmi_apis::Common_Result::eType>(
          message[strings::params][hmi_response::code].asInt());

  bool result =
      hmi_result == hmi_apis::Common_Result::SUCCESS;

  mobile_apis::Result::eType result_code =
      hmi_result == hmi_apis::Common_Result::SUCCESS
      ? mobile_apis::Result::SUCCESS
      : static_cast<mobile_apis::Result::eType>(
          message[strings::params][hmi_response::code].asInt());

  const char* return_info = NULL;

  if (result) {
    if (IsAnythingAlreadyUnsubscribed()) {
      result_code = mobile_apis::Result::WARNINGS;
      return_info =
          std::string("Unsupported phoneme type sent in a prompt").c_str();
    }
  }

 SendResponse(result, result_code, return_info,
              &(message[strings::msg_params]));
#endif // #ifdef HMI_JSON_API
#ifdef HMI_DBUS_API
  for (HmiRequests::iterator it = hmi_requests_.begin();
      it != hmi_requests_.end(); ++it) {
    HmiRequest & hmi_request = *it;
    if (hmi_request.func_id == event.id()) {
      hmi_request.status =
          static_cast<hmi_apis::Common_Result::eType>(message[strings::params][hmi_response::code]
              .asInt());
      if (hmi_apis::Common_Result::SUCCESS == hmi_request.status)
        hmi_request.value = message[strings::msg_params][hmi_request.str];
      hmi_request.complete = true;
      break;
    }
  }
 bool all_complete = true;
  bool any_arg_success = false;
  mobile_api::Result::eType status = mobile_api::Result::eType::SUCCESS;
  for (HmiRequests::const_iterator it = hmi_requests_.begin();
      it != hmi_requests_.end(); ++it) {
    if (!it->complete) {
      all_complete = false;
      break;
    }
    if (hmi_apis::Common_Result::SUCCESS != it->status) {
      if (mobile_api::Result::SUCCESS == status) {
        status = static_cast<mobile_apis::Result::eType>(it->status);
      } else if (status
          != static_cast<mobile_apis::Result::eType>(it->status)) {
        status = mobile_api::Result::eType::GENERIC_ERROR;
      } LOG4CXX_TRACE(logger_, "Status from HMI: " << it->status <<
          ", so response status become " << status);
    } else {
      any_arg_success = true;
    }
  }
 if (all_complete) {
    smart_objects::SmartObject response_params(smart_objects::SmartType_Map);
    if (any_arg_success) {
      for (HmiRequests::const_iterator it = hmi_requests_.begin();
          it != hmi_requests_.end(); ++it) {
        response_params[it->str] = it->value;
      }
    }
    LOG4CXX_INFO(logger_, "All HMI requests are complete");
    SendResponse(any_arg_success, status, NULL, &response_params);
  }
#endif // #ifdef HMI_DBUS_API
}

bool UnsubscribeVehicleDataRequest::IsAnythingAlreadyUnsubscribed() {
  LOG4CXX_INFO(logger_, "UnsubscribeVehicleDataRequest::Run");

  const VehicleData& vehicle_data = MessageHelper::vehicle_data();
  VehicleData::const_iterator it = vehicle_data.begin();

  for (; vehicle_data.end() != it; ++it) {
    if (true == (*message_)[strings::msg_params].keyExists(it->first)) {

      if ((*message_)[strings::msg_params][it->first]
          [strings::result_code].asInt() ==
          hmi_apis::Common_VehicleDataResultCode::VDRC_DATA_NOT_SUBSCRIBED) {
        return true;
      }
    }
  }

  return false;
}


}  // namespace commands
}  // namespace application_manager

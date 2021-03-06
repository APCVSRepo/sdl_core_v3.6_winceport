/**
 * \file connection_handlerImpl.hpp
 * \brief Connection handler class.
 * Observes TransportManager and ProtocolHandler, stores information regarding connections
 * and sessions and provides it to AppManager.
 *
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

#ifndef SRC_COMPONENTS_CONNECTION_HANDLER_INCLUDE_CONNECTION_HANDLER_CONNECTION_HANDLER_IMPL_H_
#define SRC_COMPONENTS_CONNECTION_HANDLER_INCLUDE_CONNECTION_HANDLER_CONNECTION_HANDLER_IMPL_H_

#include <map>
#include <list>
#include <string>

#ifdef OS_WIN32
#include <stdint.h>
#endif

#include "transport_manager/transport_manager_listener_empty.h"
#include "protocol_handler/session_observer.h"
#include "transport_manager/transport_manager_listener_empty.h"
#include "connection_handler/connection_handler_observer.h"
#include "connection_handler/device.h"
#include "connection_handler/connection.h"
#include "connection_handler/devices_discovery_starter.h"
#include "connection_handler/connection_handler.h"
#include "utils/logger.h"
#include "utils/macro.h"
#include "utils/lock.h"
#include "utils/stl_utils.h"
#include "utils/singleton.h"

/**
 * \namespace connection_handler
 * \brief SmartDeviceLink connection_handler namespace.
 */
namespace connection_handler {
/**
 * \class ConnectionHandlerImpl
 * \brief SmartDeviceLink connection_handler main class
 */
class ConnectionHandlerImpl : public ConnectionHandler,
  public transport_manager::TransportManagerListenerEmpty,
  public protocol_handler::SessionObserver,
  public DevicesDiscoveryStarter,
  public utils::Singleton<ConnectionHandlerImpl> {
  public:
    /**
     * \brief Destructor
     */
    virtual ~ConnectionHandlerImpl();

    /**
     * \brief Sets observer pointer for connection_handler.
     * \param observer Pointer to observer object.
     **/
    virtual void set_connection_handler_observer(
      ConnectionHandlerObserver* observer);

    /**
     * \brief Sets pointer to TransportManager.
     * \param transportManager Pointer to TransportManager object.
     **/
    virtual void set_transport_manager(
      transport_manager::TransportManager* transport_mngr);

    /**
     * \brief Connects to all services of device
     * \param deviceHandle Handle of device to connect to
     */
    virtual void ConnectToDevice(connection_handler::DeviceHandle device_handle);

    virtual void StartTransportManager();

    virtual void OnDeviceListUpdated(
      const std::vector<transport_manager::DeviceInfo>&);

    /**
    * @brief Reaction on event, when new applications are started on device
    * and SDL found this application
    *
    * @param device_handle Unique ID of device with new application list
    */
    virtual void OnApplicationListUpdated(DeviceHandle device_handle);

    /**
     * \brief Available devices list updated.
     *
     * Called when device scanning initiated with scanForNewDevices
     * is completed or devices connected via background procedures.
     *
     * \param DeviceList New list of available devices.
     **/
    virtual void OnDeviceFound(const transport_manager::DeviceInfo& device_info);
    virtual void OnDeviceAdded(const transport_manager::DeviceInfo& device_info);
    virtual void OnDeviceRemoved(const transport_manager::DeviceInfo& device_info);

    virtual void OnScanDevicesFinished();
    virtual void OnScanDevicesFailed(
      const transport_manager::SearchDeviceError& error);

    /**
     * \brief Notifies about established connection.
     *
     * \param connection_id ID of new connection.
     **/
    virtual void OnConnectionEstablished(
      const transport_manager::DeviceInfo& device_info,
      const transport_manager::ConnectionUID& connection_id);
    virtual void OnConnectionFailed(
      const transport_manager::DeviceInfo& device_info,
      const transport_manager::ConnectError& error);
    virtual void OnConnectionClosed(
      transport_manager::ConnectionUID connection_id);
    virtual void OnConnectionClosedFailure(
      transport_manager::ConnectionUID connection_id,
      const transport_manager::DisconnectError& error);
    virtual void OnUnexpectedDisconnect(
        transport_manager::ConnectionUID connection_id,
        const transport_manager::CommunicationError& error);
    virtual void OnDeviceConnectionLost(
      const connection_handler::DeviceHandle& device,
      const transport_manager::DisconnectDeviceError& error);
    /**
     * \brief Informs about failure during DisconnectDevice procedure of TM
     * \param device Information about disconnected device
     * \param error Information about possible reason of loosing connection
     */
    virtual void OnDisconnectFailed(
      const connection_handler::DeviceHandle& device,
      const transport_manager::DisconnectDeviceError& error);

    /**
     * \brief Callback function used by ProtocolHandler
     * when Mobile Application initiates start of new session.
     * \param connection_handle Connection identifier within which session has to be started.
     * \param sessionId Identifier of the session to be started
     * \return int32_t Id (number) of new session if successful otherwise -1.
     */
    virtual int32_t OnSessionStartedCallback(
      const transport_manager::ConnectionUID& connection_handle,
      const uint8_t session_id,
      const protocol_handler::ServiceType& service_type);

    /**
     * \brief Callback function used by ProtocolHandler
     * when Mobile Application initiates session ending.
     * \param connection_handle Connection identifier within which session exists
     * \param sessionId Identifier of the session to be ended
     * \param hashCode Hash used only in second version of SmartDeviceLink protocol.
     * If not equal to hash assigned to session on start then operation fails.
     * \return int32_t -1 if operation fails session key otherwise
     */
    virtual uint32_t OnSessionEndedCallback(
      const transport_manager::ConnectionUID& connection_handle,
      const uint8_t session_id, const uint32_t& hashCode,
      const protocol_handler::ServiceType& service_type);

    /**
     * \brief Creates unique identifier of session (can be used as hash)
     * from given connection identifier
     * whithin which session exists and session number.
     * \param  connection_handle Connection identifier within which session exists
     * \param sessionId Identifier of the session
     * \return int32_t Unique key for session
     */
    virtual uint32_t KeyFromPair(
      transport_manager::ConnectionUID connection_handle,
      uint8_t session_id);

    /**
     * \brief Returns connection identifier and session number from given session key
     * \param key Unique key used by other components as session identifier
     * \param connection_handle Returned: Connection identifier whithin which session exists
     * \param sessionId Returned: Number of session
     */
    virtual void PairFromKey(uint32_t key,
                             transport_manager::ConnectionUID* connection_handle,
                             uint8_t* session_id);

    /**
     * \brief information about given Connection Key.
     * \param key Unique key used by other components as session identifier
     * \param app_id Returned: ApplicationID
     * \param sessions_list Returned: List of session keys
     * \param device_id Returned: DeviceID
     * \return int32_t -1 in case of error or 0 in case of success
     */
    virtual int32_t GetDataOnSessionKey(uint32_t key, uint32_t* app_id = 0,
                                    std::list<int32_t>* sessions_list = NULL,
                                    uint32_t* device_id = 0);

    /**
     * \brief information about given Connection Key.
     * \param key Unique key used by other components as session identifier
     * \param app_id Returned: ApplicationID
     * \param sessions_list Returned: List of session keys
     * \param device_id Returned: DeviceID
     * \return int32_t -1 in case of error or 0 in case of success
     */
    virtual int32_t GetDataOnDeviceID(connection_handler::DeviceHandle device_handle,
                                  std::string* device_name = NULL,
                                  std::list<uint32_t>* applications_list = NULL,
                                  std::string* mac_address = NULL);


    /**
     * \brief Method which should start devices discovering
     */
    virtual void StartDevicesDiscovery();

    /*
     * Close all associated sessions and close the connection associated with the key
     */
    virtual void CloseConnection(uint32_t key);

    /*
     * Close all associated sessions and close the connection pointed by handle
     */
    virtual void CloseConnection(ConnectionHandle connection_handle) OVERRIDE;

    /*
     * Close session associated with the key
     */
    virtual void CloseSession(uint32_t key);

    /*
     * Function used by HearbeatMonitior to close session on HB timeout
     * \param connection_handle Connection handler within which session exists
     * \param session_id Identifier of the session to be ended
     */
    virtual void CloseSession(ConnectionHandle connection_handle,
                              uint8_t session_id);

    void SetProtocolHandler(protocol_handler::ProtocolHandler* handler);


    /*
     * \brief Return count of session for specified connection
     * \param connection_key pair of connection handle and session id
     */
    virtual uint32_t GetConnectionSessionsCount(uint32_t connection_key);

    /*
     * Send heartbeat message to mobile app
     */
    virtual void SendHeartBeat(ConnectionHandle connection_handle,
                               uint8_t session_id);

    /*
     * \brief Start heartbeat for specified session
     *
     * \param connection_key pair of connection and session id
     */
    virtual void StartSessionHeartBeat(uint32_t connection_key);

    /*
     * Keep connection associated with the key from being closed by heartbeat monitor
     */
    void KeepConnectionAlive(uint32_t connection_key, uint8_t session_id);
  private:
    /**
     * \brief Default class constructor
     */
    ConnectionHandlerImpl();

    /**
     * \brief Disconnect application.
     *
     * \param device_handle DeviceHandle of disconnected device.
     * \param connection_handle Connection handle.
     **/
    void RemoveConnection(const ConnectionHandle connection_handle);

    void OnConnectionEnded(
        const transport_manager::ConnectionUID& connection_id);

    /**
     * \brief Pointer to observer
     */
    ConnectionHandlerObserver* connection_handler_observer_;

    /**
     * \brief Pointer to TransportManager
     */
    transport_manager::TransportManager* transport_manager_;

    /**
     * \brief List of devices
     */
    DeviceList device_list_;

    /**
     * \brief List of connections
     */
    ConnectionList connection_list_;

    /**
     *  \brief Lock for applications list
     */
    sync_primitives::Lock connection_list_lock_;

    /*
     * \brief Cleans connection list on destruction
     */
    utils::StlMapDeleter<ConnectionList> connection_list_deleter_;

    protocol_handler::ProtocolHandler* protocol_handler_;

    DISALLOW_COPY_AND_ASSIGN(ConnectionHandlerImpl);
    FRIEND_BASE_SINGLETON_CLASS(ConnectionHandlerImpl);
};
}/* namespace connection_handler */

#endif  // SRC_COMPONENTS_CONNECTION_HANDLER_INCLUDE_CONNECTION_HANDLER_CONNECTION_HANDLER_IMPL_H_

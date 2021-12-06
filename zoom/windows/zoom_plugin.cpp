#include "include/zoom/zoom_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include <flutter/event_stream_handler.h>
#include <flutter/event_channel.h>

#include "zoom_sdk/h/zoom_sdk.h"
#include "zoom_sdk/h/zoom_sdk_ext.h"
#include "zoom_sdk/h/meeting_service_interface.h"
#include "zoom_sdk/h/meeting_service_components/meeting_ui_ctrl_interface.h"
#include "zoom_sdk/h/meeting_service_components/meeting_configuration_interface.h"
#include "zoom_sdk/h/meeting_service_components/meeting_sharing_interface.h"
#include "zoom_sdk/h/auth_service_interface.h"

#include <map>
#include <memory>
#include <sstream>
#include <iostream>
#include <string>

namespace
{

  std::string mapStatusToName(ZOOM_SDK_NAMESPACE::MeetingStatus status);

  template <typename T = flutter::EncodableValue>
  class ZoomPluginStreamHandler : public flutter::StreamHandler<T>, public ZOOM_SDK_NAMESPACE::IMeetingServiceEvent
  {
  public:
    std::unique_ptr<flutter::EventSink<flutter::EncodableValue>> eventSink;
    ZOOM_SDK_NAMESPACE::IMeetingService *meetingService;

    ZoomPluginStreamHandler() = default;
    virtual ~ZoomPluginStreamHandler() = default;

    // included from IMeetingServiceEvent:
    /// \brief Meeting status changed callback.
    /// \param status The value of meeting. For more details, see \link MeetingStatus \endlink.
    /// \param iResult Detailed reasons for special meeting status.
    ///If the status is MEETING_STATUS_FAILED, the value of iResult is one of those listed in MeetingFailCode enum.
    ///If the status is MEETING_STATUS_ENDED, the value of iResult is one of those listed in MeetingEndReason.
    void onMeetingStatusChanged(ZOOM_SDK_NAMESPACE::MeetingStatus status, int iResult = 0) override;

    /// \brief Meeting statistics warning notification callback.
    /// \param type The warning type of the meeting statistics. For more details, see \link StatisticsWarningType \endlink.
    void onMeetingStatisticsWarningNotification(ZOOM_SDK_NAMESPACE::StatisticsWarningType type) override;

    /// \brief Meeting parameter notification callback.
    /// \param meeting_param Meeting parameter. For more details, see \link MeetingParameter \endlink.
    /// \remarks The callback will be triggered right before the meeting starts. The meeting_param will be destroyed once the function calls end.
    void onMeetingParameterNotification(const ZOOM_SDK_NAMESPACE::MeetingParameter *meeting_param) override;

  protected:
    // ~ZoomPluginStreamHandler();

    // ZoomPluginStreamHandler(flutter::EventChannel<T> *channel,
    //                         ZOOM_SDK_NAMESPACE::IMeetingService *meetingService);

    // from flutter StreamHandler
    // Implementation of the public interface, to be provided by subclasses.
    std::unique_ptr<flutter::StreamHandlerError<T>> OnListenInternal(
        const T *arguments,
        std::unique_ptr<flutter::EventSink<T>> &&events);

    // Implementation of the public interface, to be provided by subclasses.
    std::unique_ptr<flutter::StreamHandlerError<T>> OnCancelInternal(
        const T *arguments) override;
  };

  // {
  // }
  // ZoomPluginStreamHandler::ZoomPluginStreamHandler(flutter::EventChannel<flutter::EncodableValue> *channel,
  //                                                  ZOOM_SDK_NAMESPACE::IMeetingService *meetingService)
  // {
  //     this->channel = channel;
  //     this->meetingService = meetingService;
  // }

  std::unique_ptr<flutter::StreamHandlerError<flutter::EncodableValue>> ZoomPluginStreamHandler<flutter::EncodableValue>::OnListenInternal(
      const flutter::EncodableValue *arguments,
      std::unique_ptr<flutter::EventSink<flutter::EncodableValue>> &&events)
  {
    this->eventSink = std::move(events);
    // TODO:
    return nullptr;
  };

  // Implementation of the public interface, to be provided by subclasses.
  std::unique_ptr<flutter::StreamHandlerError<flutter::EncodableValue>> ZoomPluginStreamHandler<flutter::EncodableValue>::OnCancelInternal(
      const flutter::EncodableValue *arguments)
  {
    this->eventSink = NULL;
    // TODO:
    return nullptr;
  };

  void ZoomPluginStreamHandler<flutter::EncodableValue>::onMeetingStatusChanged(ZOOM_SDK_NAMESPACE::MeetingStatus status, int iResult)
  {
    auto statusName = mapStatusToName(status);
    if (this->eventSink)
    {
      auto out = flutter::EncodableList();
      out.push_back(flutter::EncodableValue(statusName.c_str()));
      out.push_back(flutter::EncodableValue(""));
      this->eventSink->Success(out);
    }
    // TODO
  }

  void ZoomPluginStreamHandler<flutter::EncodableValue>::onMeetingStatisticsWarningNotification(ZOOM_SDK_NAMESPACE::StatisticsWarningType type)
  {
    // TODO
  }
  void ZoomPluginStreamHandler<flutter::EncodableValue>::onMeetingParameterNotification(const ZOOM_SDK_NAMESPACE::MeetingParameter *meeting_param)
  {
    // TODO
  }

  std::string mapStatusToName(ZOOM_SDK_NAMESPACE::MeetingStatus status)
  {
    switch (status)
    {
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_IDLE:
      return "MEETING_STATUS_IDLE";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_CONNECTING:
      return "MEETING_STATUS_CONNECTING";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_WAITINGFORHOST:
      return "MEETING_STATUS_WAITINGFORHOST";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_INMEETING:
      return "MEETING_STATUS_INMEETING";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_DISCONNECTING:
      return "MEETING_STATUS_DISCONNECTING";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_RECONNECTING:
      return "MEETING_STATUS_RECONNECTING";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_FAILED:
      return "MEETING_STATUS_FAILED";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_ENDED:
      return "MEETING_STATUS_ENDED";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_UNKNOW:
      return "MEETING_STATUS_UNKNOW";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_LOCKED:
      return "MEETING_STATUS_LOCKED";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_UNLOCKED:
      return "MEETING_STATUS_UNLOCKED";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_IN_WAITING_ROOM:
      return "MEETING_STATUS_IN_WAITING_ROOM";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_WEBINAR_PROMOTE:
      return "MEETING_STATUS_WEBINAR_PROMOTE";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_WEBINAR_DEPROMOTE:
      return "MEETING_STATUS_WEBINAR_DEPROMOTE";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_JOIN_BREAKOUT_ROOM:
      return "MEETING_STATUS_JOIN_BREAKOUT_ROOM";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_LEAVE_BREAKOUT_ROOM:
      return "MEETING_STATUS_LEAVE_BREAKOUT_ROOM";
    case ZOOM_SDK_NAMESPACE::MEETING_STATUS_WAITING_EXTERNAL_SESSION_KEY:
      return "MEETING_STATUS_WAITING_EXTERNAL_SESSION_KEY";
    default:
      return "UNKNOWN";
    }
  }

  // main plugin for flutter
  class ZoomPlugin : public flutter::Plugin
  {
  public:
    // static ZOOM_SDK_NAMESPACE::IMeetingService *meetingService;
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

    static ZOOM_SDK_NAMESPACE::IMeetingService *_meetingService;
    static ZOOM_SDK_NAMESPACE::IMeetingService *getMeetingService();

    ZoomPlugin();

    virtual ~ZoomPlugin();
    void ZoomPlugin::joinMeeting(const flutter::MethodCall<flutter::EncodableValue> &method_call,
                                 std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
    void ZoomPlugin::getMeetingStatus(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

  private:
    static ZoomPluginStreamHandler<flutter::EncodableValue> *streamHandler;
    // Called when a method is called on this plugin's channel from Dart.
    void HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue> &method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

    void AuthorizeSDK(const std::wstring domain, const std::wstring jwt_token, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

    void init(const flutter::MethodCall<flutter::EncodableValue> &method_call,
              std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

    /// listen to zoom sdk messages when initializing the plugin
    class ZoomPluginAuthListener : public ZOOM_SDK_NAMESPACE::IAuthServiceEvent
    {
    public:
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> init_result;
      ZoomPlugin *plugin;
      ZoomPluginAuthListener(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> res, ZoomPlugin *plugin)
      {
        this->init_result = std::move(res);
        this->plugin = plugin;
      }
      void onAuthenticationReturn(ZOOM_SDK_NAMESPACE::AuthResult result)
      {
        // flutter::EncodableValue res = flutter::EncodableValue(result); dart expects List<dynamic>
        auto out = flutter::EncodableList();
        out.push_back(flutter::EncodableValue(result));
        this->init_result->Success(out);
        auto service = getMeetingService();
        service->SetEvent(streamHandler); // TODO: pass reference to stream handler
      }

      void onLoginReturnWithReason(ZOOM_SDK_NAMESPACE::LOGINSTATUS ret, ZOOM_SDK_NAMESPACE::IAccountInfo *pAccountInfo, ZOOM_SDK_NAMESPACE::LoginFailReason reason)
      {
        // onAuthenticationReturn was already called
      }

      void onLogout()
      {
        //
      }

      void onZoomAuthIdentityExpired()
      {
        //
      }
      void onZoomIdentityExpired()
      {
        //
      }
    };

  private:
  };

  ZoomPluginStreamHandler<flutter::EncodableValue> *ZoomPlugin::streamHandler = NULL;
  ZOOM_SDK_NAMESPACE::IMeetingService *ZoomPlugin::_meetingService = NULL;

  // static
  void ZoomPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "plugins.webcare/zoom_channel",
            &flutter::StandardMethodCodec::GetInstance());

    auto plugin = std::make_unique<ZoomPlugin>();

    channel->SetMethodCallHandler(
        [plugin_pointer = plugin.get()](const auto &call, auto result)
        {
          plugin_pointer->HandleMethodCall(call, std::move(result));
        });

    auto eventChannel =
        std::make_unique<flutter::EventChannel<flutter::EncodableValue>>(
            registrar->messenger(), "plugins.webcare/zoom_event_stream",
            &flutter::StandardMethodCodec::GetInstance());

    // TODO: how do I pass both the meeting service and event channel to the stream handler
    // idea: only call set stream handler once zoom is ready
    // however, how can I convert this pointer to make it work?!
    // adapted from https://gitea.com/fawdlstty/test_camera/src/branch/master/windows/test_camera_plugin.cpp
    streamHandler = new ZoomPluginStreamHandler<flutter::EncodableValue>();
    flutter::StreamHandler<flutter::EncodableValue> *handler = static_cast<flutter::StreamHandler<flutter::EncodableValue> *>(streamHandler);
    //std::unique_ptr<flutter::StreamHandler<flutter::EncodableValue>> handler = std::make_unique<flutter::StreamHandler<flutter::EncodableValue>>(plugin->eventChannel);
    std::unique_ptr<flutter::StreamHandler<flutter::EncodableValue>> _ptr{handler};
    eventChannel->SetStreamHandler(std::move(_ptr));

    registrar->AddPlugin(std::move(plugin));
  }

  ZoomPlugin::ZoomPlugin() {}

  ZoomPlugin::~ZoomPlugin() {}

  ZOOM_SDK_NAMESPACE::IMeetingService *ZoomPlugin::getMeetingService()
  {
    if (!_meetingService)
    {

      ZOOM_SDK_NAMESPACE::IMeetingService *service;
      ZOOM_SDK_NAMESPACE::SDKError meetingServiceInitReturnVal = ZOOM_SDK_NAMESPACE::CreateMeetingService(&service);
      if (meetingServiceInitReturnVal == ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS)
      {
        // MeetingService was created successfully, it is recommended to store this value for use across the application
        _meetingService = service;
        ZoomPlugin::_meetingService = service;
      }
    }
    return _meetingService;
  }

  std::wstring toString(std::string str)
  {
    return std::wstring(str.begin(), str.end());
  }

  // https://marketplace.zoom.us/docs/sdk/native-sdks/windows/mastering-sdk/windows-sdk-functions
  // void yourAuthServiceEventListener::onAuthenticationReturn(ZOOM_SDK_NAMESPACE::AuthResult ret) {
  //   if (ret == ZOOM_SDK_NAMESPACE::SDKError::AUTHRET_JWTTOKENWRONG)
  //   {
  //     // SDK Auth call failed because the JWT token is invalid.
  //   } else if (ret == ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS)
  //   {
  //     // SDK Authenticated successfully
  //   }
  // }

  void ZoomPlugin::AuthorizeSDK(std::wstring domain, std::wstring jwt_token, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    std::cout << "authorizing sdk " << domain.c_str() << " " << jwt_token.c_str() << "\n";
    // Initialize SDK with InitParam object
    ZOOM_SDK_NAMESPACE::InitParam initParam;
    ZOOM_SDK_NAMESPACE::SDKError initReturnVal(ZOOM_SDK_NAMESPACE::SDKERR_UNINITIALIZE);

    // Set web domain to zoom.us
    initParam.strWebDomain = domain.c_str();
    initReturnVal = ZOOM_SDK_NAMESPACE::InitSDK(initParam);

    // Check if InitSDK call succeeded
    if (initReturnVal == ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS)
    {
      // Create IAuthService object to perform Auth actions
      ZOOM_SDK_NAMESPACE::IAuthService *authService;
      ZOOM_SDK_NAMESPACE::SDKError authServiceInitReturnVal = ZOOM_SDK_NAMESPACE::CreateAuthService(&authService);

      if (authServiceInitReturnVal == ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS)
      {
        // Create IAuthServiceEvent object to listen for Auth events from SDK
        ZOOM_SDK_NAMESPACE::IAuthServiceEvent *authListener;
        // Auth SDK with AuthContext object
        ZOOM_SDK_NAMESPACE::AuthContext authContext;
        ZOOM_SDK_NAMESPACE::SDKError authCallReturnValue(ZOOM_SDK_NAMESPACE::SDKERR_UNAUTHENTICATION);

        // Call SetEvent to assign your IAuthServiceEvent listener
        // TODO: handle errors
        ZoomPluginAuthListener *yourAuthServiceEventListener = new ZoomPluginAuthListener(std::move(result), this);

        authListener = yourAuthServiceEventListener;
        authService->SetEvent(authListener);

        // Provide your JWT to the AuthContext object
        authContext.jwt_token = jwt_token.c_str();

        authCallReturnValue = authService->SDKAuth(authContext);
        if (authCallReturnValue != ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS)
        {
          yourAuthServiceEventListener->init_result->Error("Init auth failed", "Failed to authorize Zoom SDK. Go error code " + authCallReturnValue);
        }
      }
      else
      {
        result->Error("Init failed", "Failed to initialize Zoom SDK. Go error code " + authServiceInitReturnVal);
      }
    }
  }

  std::wstring getString(const flutter::MethodCall<flutter::EncodableValue> &method_call, std::string arg)
  {
    const auto *arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    if (arguments)
    {
      std::wstring result = L"";
      auto result_it = arguments->find(flutter::EncodableValue(arg));
      if (result_it != arguments->end())
      {
        return toString(std::get<std::string>(result_it->second));
      }
    }
    return NULL;
  }
  void ZoomPlugin::joinMeeting(const flutter::MethodCall<flutter::EncodableValue> &method_call,
                               std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    const auto *arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    if (arguments)
    {
      auto userId = getString(method_call, "userId");
      auto meetingId = getString(method_call, "meetingId");
      auto meetingPassword = getString(method_call, "meetingPassword");
      auto disableDialIn = getString(method_call, "disableDialIn");
      auto disableDrive = getString(method_call, "disableDrive");
      // TODO: not yet supported
      auto disableInvite = getString(method_call, "disableInvite");
      auto disableShare = getString(method_call, "disableShare");
      auto noDisconnectAudio = getString(method_call, "noDisconnectAudio");
      auto noAudio = getString(method_call, "noAudio");

      // Join meeting for end user with JoinParam object
      ZOOM_SDK_NAMESPACE::JoinParam joinMeetingParam;
      // Provide meeting credentials for end user using JoinParam4NormalUser
      ZOOM_SDK_NAMESPACE::JoinParam4NormalUser joinParams;

      // TODO: the sdk says invalid param
      joinParams.meetingNumber = static_cast<UINT64>(std::stoull(meetingId));
      joinParams.psw = meetingPassword.c_str();
      joinParams.userName = userId.c_str();

      // joinParams.hDirectShareAppWnd = GetActiveWindow();
      joinParams.isAudioOff = noAudio == L"true";
      joinParams.isDirectShareDesktop = false; // == "true";

      joinParams.vanityID = NULL;
      joinParams.hDirectShareAppWnd = NULL;
      joinParams.customer_key = NULL;
      joinParams.webinarToken = NULL;
      joinParams.isVideoOff = false;
      joinParams.isAudioOff = noAudio == L"true";
      joinParams.isDirectShareDesktop = false;

      joinMeetingParam.userType = ZOOM_SDK_NAMESPACE::SDK_UT_NORMALUSER;
      joinMeetingParam.param.normaluserJoin = joinParams;

      ZOOM_SDK_NAMESPACE::SDKError joinMeetingCallReturnValue(ZOOM_SDK_NAMESPACE::SDKERR_UNKNOWN);
      auto meetingService = this->getMeetingService();

      // TODO: support more sharing options
      auto uiController = meetingService->GetUIController();
      if (uiController)
      {
        uiController->ShowSharingToolbar(disableShare == L"true");
      }

      joinMeetingCallReturnValue = meetingService->Join(joinMeetingParam);
      if (joinMeetingCallReturnValue == ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS)
      {
        // Join meeting call succeeded, listen for join meeting result using the onMeetingStatusChanged callback
        flutter::EncodableValue res = flutter::EncodableValue(joinMeetingCallReturnValue);
        result->Success(true);
      }
      else
      {
        std::string error = "Zoom join failed with error code " + (std::to_string(joinMeetingCallReturnValue));
        result->Error("Join failed", error);
      }
    }
    else
    {
      result->Error("Bad arguments", "invalid arguments for flutter init");
    }
  }

  void ZoomPlugin::init(const flutter::MethodCall<flutter::EncodableValue> &method_call,
                        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    std::wstring domain = L"";
    std::wstring jwt_token = L"";
    const auto *arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
    if (arguments)
    {
      auto domain_it = arguments->find(flutter::EncodableValue("domain"));
      if (domain_it != arguments->end())
      {
        domain = toString(std::get<std::string>(domain_it->second));
      }
      auto token_it = arguments->find(flutter::EncodableValue("jwtToken"));
      if (token_it != arguments->end())
      {
        jwt_token = toString(std::get<std::string>(token_it->second));
      }
      this->AuthorizeSDK(domain, jwt_token, std::move(result)); // TODO: wait for result from Zoom SDK if it is valid
    }
    else
    {
      result->Error("Bad arguments", "invalid arguments for flutter init");
    }
  }

  void ZoomPlugin::getMeetingStatus(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    auto meetingService = this->getMeetingService();
    if (meetingService)
    {
      auto status = meetingService->GetMeetingStatus();
      if (status == NULL)
      {
        auto out = flutter::EncodableList();
        out.push_back(flutter::EncodableValue("MEETING_STATUS_UNKNOWN"));
        out.push_back(flutter::EncodableValue("No status available"));
        result->Success(out);
      }
      else
      {
        auto statusName = mapStatusToName(status);
        // we just try to do the same as android, this structure is slightly weird
        auto out = flutter::EncodableList();

        out.push_back(flutter::EncodableValue(statusName));
        out.push_back(flutter::EncodableValue(""));
        result->Success(out);
      }
    }
    else
    {
      auto out = flutter::EncodableList();
      out.push_back(flutter::EncodableValue("MEETING_STATUS_UNKNOWN"));
      out.push_back(flutter::EncodableValue("No status available"));
      result->Success(out);
    }
  }

  void ZoomPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    std::cout << "Got message on channel " << method_call.method_name() << "\n";
    if (method_call.method_name().compare("join") == 0)
    {
      this->joinMeeting(method_call, std::move(result));
    }
    else if (method_call.method_name().compare("init") == 0)
    {
      this->init(method_call, std::move(result));
    }
    else if (method_call.method_name().compare("meeting_status") == 0)
    {
      this->getMeetingStatus(std::move(result));
    }
    else if (method_call.method_name().compare("getPlatformVersion") == 0)
    {
      std::ostringstream version_stream;
      version_stream << "Windows ";
      if (IsWindows10OrGreater())
      {
        version_stream << "10+";
      }
      else if (IsWindows8OrGreater())
      {
        version_stream << "8";
      }
      else if (IsWindows7OrGreater())
      {
        version_stream << "7";
      }
      flutter::EncodableValue res = flutter::EncodableValue(version_stream.str());
      result->Success(res);
    }
    else
    {
      result->NotImplemented();
    }
  }
} // namespace

void ZoomPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar)
{
  ZoomPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

#include "include/zoom/zoom_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include "./zoom_sdk/h/zoom_sdk.h"
#include "./zoom_sdk/h/zoom_sdk_ext.h"
#include "./zoom_sdk/h/meeting_service_interface.h"
// #include "./zoom_sdk/h/meeting_service_components/meeting_ui_ctrl_interface.h"
#include "./zoom_sdk/h/auth_service_interface.h"

#include <map>
#include <memory>
#include <sstream>
#include <iostream>
#include <string>

namespace
{

  class ZoomPlugin : public flutter::Plugin
  {
  public:
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);
    ZOOM_SDK_NAMESPACE::IMeetingService *_meetingService = nullptr;
    ZOOM_SDK_NAMESPACE::IMeetingService *getMeetingService();

    ZoomPlugin();

    virtual ~ZoomPlugin();
    void ZoomPlugin::joinMeeting(const flutter::MethodCall<flutter::EncodableValue> &method_call,
                                 std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

  private:
    // Called when a method is called on this plugin's channel from Dart.
    void HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue> &method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
    void AuthorizeSDK(const std::wstring domain, const std::wstring jwt_token, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
    void init(const flutter::MethodCall<flutter::EncodableValue> &method_call,
              std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

    class ZoomPluginAuthListener : public ZOOM_SDK_NAMESPACE::IAuthServiceEvent
    {
    public:
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> init_result;
      ZoomPluginAuthListener(std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> res)
      {
        this->init_result = std::move(res);
      }
      void onAuthenticationReturn(ZOOM_SDK_NAMESPACE::AuthResult result)
      {
        // flutter::EncodableValue res = flutter::EncodableValue(result); dart expects List<dynamic>
        this->init_result->Success();
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
  };

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

    registrar->AddPlugin(std::move(plugin));
  }

  ZoomPlugin::ZoomPlugin() {}

  ZoomPlugin::~ZoomPlugin() {}

  ZOOM_SDK_NAMESPACE::IMeetingService *ZoomPlugin::getMeetingService()
  {
    if (!this->_meetingService)
    {

      ZOOM_SDK_NAMESPACE::IMeetingService *meetingService;
      ZOOM_SDK_NAMESPACE::SDKError meetingServiceInitReturnVal = ZOOM_SDK_NAMESPACE::CreateMeetingService(&meetingService);
      if (meetingServiceInitReturnVal == ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS)
      {
        // MeetingService was created successfully, it is recommended to store this value for use across the application
        this->_meetingService = meetingService;
      }
    }
    return this->_meetingService;
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
        ZoomPluginAuthListener *yourAuthServiceEventListener = new ZoomPluginAuthListener(std::move(result));
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
    return nullptr;
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
      joinParams.isAudioOff = false;
      joinParams.isDirectShareDesktop = false;

      joinMeetingParam.userType = ZOOM_SDK_NAMESPACE::SDK_UT_NORMALUSER;
      joinMeetingParam.param.normaluserJoin = joinParams;

      ZOOM_SDK_NAMESPACE::SDKError joinMeetingCallReturnValue(ZOOM_SDK_NAMESPACE::SDKERR_UNKNOWN);
      auto meetingService = this->getMeetingService();

      // TODO: support more sharing options
      // meetingService->GetUIController()->ShowSharingToolbar(!disableShare);

      joinMeetingCallReturnValue = meetingService->Join(joinMeetingParam);
      if (joinMeetingCallReturnValue == ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS)
      {
        // Join meeting call succeeded, listen for join meeting result using the onMeetingStatusChanged callback
        flutter::EncodableValue res = flutter::EncodableValue(joinMeetingCallReturnValue);
        result->Success(res);
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

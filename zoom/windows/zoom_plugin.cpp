#include "include/zoom/zoom_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include "./zoom_sdk/h/zoom_sdk.h"
#include <map>
#include <memory>
#include <sstream>

namespace
{

  class ZoomPlugin : public flutter::Plugin
  {
  public:
    static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

    ZoomPlugin();

    virtual ~ZoomPlugin();

  private:
    // Called when a method is called on this plugin's channel from Dart.
    void HandleMethodCall(
        const flutter::MethodCall<flutter::EncodableValue> &method_call,
        std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
  };

  // static
  void ZoomPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarWindows *registrar)
  {
    auto channel =
        std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
            registrar->messenger(), "zoom",
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

  void createMeetingService()
  {
    // Create IMeetingService object to perform meeting actions
    ZOOM_SDK_NAMESPACE::IMeetingService *meetingService;
    ZOOM_SDK_NAMESPACE::SDKError meetingServiceInitReturnVal = ZOOM_SDK_NAMESPACE::CreateMeetingService(&meetingService);
    if (meetingServiceInitReturnVal == ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS)
    {
      // MeetingService was created successfully, it is recommended to store this value for use across the application
        }
  }
  void joinMeetingForEndUser()
  {
    // Join meeting for end user with JoinParam object
    //ZOOM_SDK_NAMESPACE::JoinParam joinMeetingParam;
    // Provide meeting credentials for end user using JoinParam4NormalUser
    // ZOOM_SDK_NAMESPACE::JoinParam4NormalUser joinMeetingForNormalUserLoginParam;

    // joinMeetingParam.userType = ZOOM_SDK_NAMESPACE::SDK_UT_NORMALUSER;

    // joinMeetingForNormalUserLoginParam.meetingNumber = 1234567890;
    // joinMeetingForNormalUserLoginParam.psw = L"Meeting password";
    // joinMeetingForNormalUserLoginParam.userName = L"Display name for user";

    // joinMeetingParam.param.normaluserJoin = joinMeetingForNormalUserLoginParam;

    // ZOOM_SDK_NAMESPACE::SDKError joinMeetingCallReturnValue(ZOOM_SDK_NAMESPACE::SDKERR_UNKNOWN);
    // joinMeetingCallReturnValue = yourMeetingServiceInstance->Join(joinMeetingParam);
    // if (joinMeetingCallReturnValue == ZOOM_SDK_NAMESPACE::SDKError::SDKERR_SUCCESS)
    // {
    //   // Join meeting call succeeded, listen for join meeting result using the onMeetingStatusChanged callback
    // }
  }

  void ZoomPlugin::HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result)
  {
    if (method_call.method_name().compare("joinMeeting") == 0)
    {
      joinMeetingForEndUser();
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
      result->Success(flutter::EncodableValue(version_stream.str()));
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

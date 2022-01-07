import Cocoa
import FlutterMacOS
import ZoomSDK

public class SwiftZoomPlugin: NSObject, FlutterPlugin,FlutterStreamHandler, ZoomSDKMeetingServiceDelegate {
  var authenticationDelegate: AuthenticationDelegate
  var eventSink: FlutterEventSink?
   
  public static func register(with registrar: FlutterPluginRegistrar) {
    let messenger = registrar.messenger
    let channel = FlutterMethodChannel(name: "plugins.webcare/zoom_channel", binaryMessenger: messenger)
    let instance = SwiftZoomPlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)

    let eventChannel = FlutterEventChannel(name: "plugins.webcare/zoom_event_stream", binaryMessenger: messenger)
    eventChannel.setStreamHandler(instance)
  }

  override init(){
    authenticationDelegate = AuthenticationDelegate()
  }


 
  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
        switch call.method {
        case "init":
            self.initZoom(call: call, result: result)
        case "join":
            self.joinMeeting(call: call, result: result)
        case "start":
            self.startMeeting(call: call, result: result)
        case "meeting_status":
            self.meetingStatus(call: call, result: result)
        default:
            result(FlutterMethodNotImplemented)
        }
  }

    
    public func onMethodCall(call: FlutterMethodCall, result: @escaping FlutterResult) {
        
        switch call.method {
        case "init":
            self.initZoom(call: call, result: result)
        case "join":
            self.joinMeeting(call: call, result: result)
        case "start":
            self.startMeeting(call: call, result: result)
        case "meeting_status":
            self.meetingStatus(call: call, result: result)
        default:
            result(FlutterMethodNotImplemented)
        }
    }
    private static func printStatus(_ err: ZoomSDKError?) {
        print("Got status: \(String(reflecting: err!))")
    }
    
    public func initZoom(call: FlutterMethodCall, result: @escaping FlutterResult)  {
        let arguments = call.arguments as! Dictionary<String, String>
        
        let params = ZoomSDKInitParams()
        
        let sharedSDK = ZoomSDK.shared()
        sharedSDK?.zoomDomain = arguments["domain"]!
        SwiftZoomPlugin.printStatus(sharedSDK?.initSDK(with: params))
        
        let auth = ZoomSDKAuthContext()
        if let jwtToken = arguments["jwtToken"] {
            auth.jwtToken = jwtToken
        }
        
        let authService = sharedSDK?.getAuthService()
        authService?.delegate = authenticationDelegate.onAuth(result)
        let err = authService!.sdkAuth(auth)
        SwiftZoomPlugin.printStatus(err)
    }
    
    public func meetingStatus(call: FlutterMethodCall, result: FlutterResult) {
        
        let meetingService = ZoomSDK.shared().getMeetingService()
        if meetingService != nil {
            
            let meetingState = meetingService?.getMeetingStatus()
            result(getStateMessage(meetingState))
        } else {
            result(["MEETING_STATUS_UNKNOWN", ""])
        }
    }
    
    public func joinMeeting(call: FlutterMethodCall, result: FlutterResult) {
        
        let meetingService = ZoomSDK.shared().getMeetingService()
        
        if meetingService != nil {
            
            let arguments = call.arguments as! Dictionary<String, String?>
            /*
            meetingSettings?.disableDriveMode(parseBoolean(data: arguments["disableDrive"]!, defaultValue: false))
            meetingSettings?.disableCall(in: parseBoolean(data: arguments["disableDialIn"]!, defaultValue: false))
            meetingSettings?.setAutoConnectInternetAudio(parseBoolean(data: arguments["noDisconnectAudio"]!, defaultValue: false))
            meetingSettings?.setMuteAudioWhenJoinMeeting(parseBoolean(data: arguments["noAudio"]!, defaultValue: false))
            meetingSettings?.meetingShareHidden = parseBoolean(data: arguments["disableShare"]!, defaultValue: false)
            meetingSettings?.meetingInviteHidden = parseBoolean(data: arguments["disableDrive"]!, defaultValue: false)
             */
            let joinMeetingParameters = ZoomSDKJoinMeetingElements()
            joinMeetingParameters.displayName = arguments["userId"]!!
            joinMeetingParameters.meetingNumber = Int64(arguments["meetingId"]!!)!
            joinMeetingParameters.isDirectShare = false
            
            let hasPassword = arguments["meetingPassword"]! != nil
            if hasPassword {
                joinMeetingParameters.password = arguments["meetingPassword"]!!
            }
            
            let response = meetingService?.joinMeeting(joinMeetingParameters)
            
            if let response = response {
                print("Got response from join: \(response)")
            }
            result(true)
        } else {
            result(false)
        }
    }

    public func startMeeting(call: FlutterMethodCall, result: FlutterResult) {
//
//        let meetingService = MobileRTC.shared().getMeetingService()
//        let meetingSettings = MobileRTC.shared().getMeetingSettings()
//
//        if meetingService != nil {
//
//            let arguments = call.arguments as! Dictionary<String, String?>
//
//            meetingSettings?.disableDriveMode(parseBoolean(data: arguments["disableDrive"]!, defaultValue: false))
//            meetingSettings?.disableCall(in: parseBoolean(data: arguments["disableDialIn"]!, defaultValue: false))
//            meetingSettings?.setAutoConnectInternetAudio(parseBoolean(data: arguments["noDisconnectAudio"]!, defaultValue: false))
//            meetingSettings?.setMuteAudioWhenJoinMeeting(parseBoolean(data: arguments["noAudio"]!, defaultValue: false))
//            meetingSettings?.meetingShareHidden = parseBoolean(data: arguments["disableShare"]!, defaultValue: false)
//            meetingSettings?.meetingInviteHidden = parseBoolean(data: arguments["disableDrive"]!, defaultValue: false)
//
//            let user: MobileRTCMeetingStartParam4WithoutLoginUser = MobileRTCMeetingStartParam4WithoutLoginUser.init()
//
//            user.userType = .apiUser
//            user.meetingNumber = arguments["meetingId"]!!
//            user.userName = arguments["displayName"]!!
//           // user.userToken = arguments["zoomToken"]!!
//            user.userID = arguments["userId"]!!
//            user.zak = arguments["zoomAccessToken"]!!
//
//            let param: MobileRTCMeetingStartParam = user
//
//            let response = meetingService?.startMeeting(with: param)
//
//            if let response = response {
//                print("Got response from start: \(response)")
//            }
//            result(true)
//        } else {
//            result(false)
//        }
        result(false)
    }
    
    private func parseBoolean(data: String?, defaultValue: Bool) -> Bool {
        var result: Bool
        
        if let unwrappeData = data {
            result = NSString(string: unwrappeData).boolValue
        } else {
            result = defaultValue
        }
        return result
    }
    
    
    
    
    public func onMeetingError(_ error: ZoomSDKMeetingError, message: String?) {
        
    }
    
    public func getMeetErrorMessage(_ errorCode: ZoomSDKMeetingError) -> String {
        
        let message = ""
        return message
    }
    
    public func onMeetingStatusChange(_ state: ZoomSDKMeetingStatus, meetingError error: ZoomSDKMeetingError, end reason: EndMeetingReason) {
        guard let eventSink = eventSink else {
            return
        }
        
        eventSink(getStateMessage(state))
    }
    public func onMeetingStateChange(_ state: ZoomSDKMeetingStatus) {
        
        guard let eventSink = eventSink else {
            return
        }
        
        eventSink(getStateMessage(state))
    }
    
    public func onListen(withArguments arguments: Any?, eventSink events: @escaping FlutterEventSink) -> FlutterError? {
        self.eventSink = events
        
        let meetingService = ZoomSDK.shared().getMeetingService()
        if meetingService == nil {
            return FlutterError(code: "Zoom SDK error", message: "ZoomSDK is not initialized", details: nil)
        }
        meetingService?.delegate = self
        
        return nil
    }
     
    public func onCancel(withArguments arguments: Any?) -> FlutterError? {
        eventSink = nil
        return nil
    }
    
    private func getStateMessage(_ state: ZoomSDKMeetingStatus?) -> [String] {
        
        var message: [String]
        switch state {
        case ZoomSDKMeetingStatus_Idle:
            message = ["MEETING_STATUS_IDLE", "No meeting is running"]
            break
        case ZoomSDKMeetingStatus_Connecting:
            message = ["MEETING_STATUS_CONNECTING", "Connect to the meeting server"]
            break
        case ZoomSDKMeetingStatus_InMeeting:
            message = ["MEETING_STATUS_INMEETING", "Meeting is ready and in process"]
            break
        case ZoomSDKMeetingStatus_Webinar_Promote:
            message = ["MEETING_STATUS_WEBINAR_PROMOTE", "Upgrade the attendees to panelist in webinar"]
            break
        case ZoomSDKMeetingStatus_Webinar_Depromote:
            message = ["MEETING_STATUS_WEBINAR_DEPROMOTE", "Demote the attendees from the panelist"]
            break
        case ZoomSDKMeetingStatus_Disconnecting:
            message = ["MEETING_STATUS_DISCONNECTING", "Disconnect the meeting server, leave meeting status"]
            break;
        case ZoomSDKMeetingStatus_Ended:
            message = ["MEETING_STATUS_ENDED", "Meeting ends"]
            break;
        case ZoomSDKMeetingStatus_Failed:
            message = ["MEETING_STATUS_FAILED", "Failed to connect the meeting server"]
            break;
        case ZoomSDKMeetingStatus_Reconnecting:
            message = ["MEETING_STATUS_RECONNECTING", "Reconnecting meeting server status"]
            break;
        case ZoomSDKMeetingStatus_WaitingForHost:
            message = ["MEETING_STATUS_WAITINGFORHOST", "Waiting for the host to start the meeting"]
            break;
        case ZoomSDKMeetingStatus_InWaitingRoom:
            message = ["MEETING_STATUS_IN_WAITING_ROOM", "Participants who join the meeting before the start are in the waiting room"]
            break;
        default:
            message = ["MEETING_STATUS_UNKNOWN", "\(state?.rawValue ?? 9999)"]
        }
        
        return message
    }
}


public class AuthenticationDelegate: NSObject, ZoomSDKAuthDelegate {
    private var result: FlutterResult?
    
    
    public func onAuth(_ result: FlutterResult?) -> AuthenticationDelegate {
        self.result = result
        return self
    }
    
    public func onZoomSDKLoginResult(_ loginStatus: ZoomSDKLoginStatus, failReason reason: ZoomSDKLoginFailReason) {
        print("onZoomSDKLoginResult")
    }
    
    public func onZoomAuthIdentityExpired() {
        print("onZoomAuthIdentityExpired")
    }
    
    public func onZoomSDKAuthReturn(_ returnValue: ZoomSDKAuthError) {
        print("onZoomSDKAuthReturn")
        
        if returnValue == ZoomSDKAuthError_Success {
            self.result?([0, 0])
        } else {
            self.result?([1, 0])
        }
        
        self.result = nil
    }
    
    public func getAuthErrorMessage(_ errorCode: ZoomSDKAuthError) -> String {
        
        let message = ""
         
        return message
    }
}


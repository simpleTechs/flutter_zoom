#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint zoom.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'zoom'
  s.version          = '0.0.1'
  s.summary          = 'A new flutter plugin project.'
  s.description      = <<-DESC
A new flutter plugin project.
                       DESC
  s.homepage         = 'http://example.com'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Your Company' => 'email@example.com' }
  s.source           = { :path => '.' }
  s.source_files     = 'Classes/**/*'
  s.dependency 'FlutterMacOS'

  s.platform = :osx, '10.11'
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES' }
  s.swift_version = '5.0'

  s.preserve_paths = 'zoom_sdk/Universal-Intel+M1/ZoomSDK'
  s.vendored_libraries = 'zoom_sdk/Universal-Intel+M1/ZoomSDK/libcrypto.dylib', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/libjson.dylib', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/libminizip.dylib', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/libssl.dylib'
  s.vendored_frameworks = 'zoom_sdk/Universal-Intel+M1/ZoomSDK/ZoomKit.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/ZoomMeetingBridge.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/ZoomSDK.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/ZoomSDKChatUI.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/ZoomSDKVideoUI.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/ZoomUnit.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/asproxy.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/cmmlib.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/curl64.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/nydus.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/protobuf.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/tp.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/util.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/viper.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/vtAdapter.framework', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/xmpp_framework.framework'
  s.resource = 'zoom_sdk/Universal-Intel+M1/ZoomSDK/RingtoneRes.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/annoter.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/aomagent.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/fdkaac2.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/mcm.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/ssb_sdk.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/viperex.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/zChatApp.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/zData.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/zKBCrypto.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/zSDKRes.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/zVideoApp.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/zVideoUIEx.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/zWebService.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/zlt.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/zmLoader.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/zmb.bundle', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/CptHost.app', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/SDK_Transcode.app', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/airhost.app', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/aomhost.app', 'zoom_sdk/Universal-Intel+M1/ZoomSDK/capHost.app'

end

#include <string>
#include <iostream>
#include <node/node_api.h>
#include <obs/obs.h>
#include <obs/obs-frontend-api.h>
#include <obs/obs-module.h>
#include <obs/obs-service.h>

namespace demo {

using namespace std;

static obs_output_t * g_rtmp_output = nullptr;

static obs_video_info get_video_settings() {
  obs_video_info ovi = {};
  ovi.adapter = 0;
  ovi.graphics_module = "libobs-opengl.so";
  ovi.output_format = VIDEO_FORMAT_NV12;
  ovi.fps_num = 30;
  ovi.fps_den = 1;
  ovi.base_width = 1920;
  ovi.base_height = 1080;
  ovi.output_width = 1920;
  ovi.output_height = 1080;
  return ovi;
}

static obs_audio_info get_audio_settings() {
  obs_audio_info oai = {};
  oai.samples_per_sec = 48000;
  oai.speakers = SPEAKERS_STEREO;
  return oai;
}

static obs_encoder_t * create_video_encoder() {
  obs_encoder_t * video_encoder = obs_video_encoder_create("obs_x264", "Video Encoder", nullptr, nullptr);
  return video_encoder;
}

static obs_encoder_t * create_audio_encoder() {
  obs_encoder_t * audio_encoder = obs_audio_encoder_create("ffmpeg_aac", "Audio Encoder", nullptr, 0, nullptr);
  return audio_encoder;
}

static obs_service_t * create_stream_service(const std::string & server_url, const std::string & stream_key) {
  obs_service_t * stream_service = obs_service_create("rtmp_common", "Twitch Service", nullptr, nullptr);
  if (not stream_service) {
    cout << "could not create rtmp_common service" << endl;
    return nullptr;
  }
  obs_data_t * settings = obs_data_create();
  obs_data_set_string(settings, "server", server_url.c_str());
  obs_data_set_string(settings, "key", stream_key.c_str());
  obs_service_update(stream_service, settings);
  obs_data_release(settings);
  return stream_service;
}

static obs_output_t * create_rtmp_output(obs_encoder_t * audio_encoder, obs_encoder_t * video_encoder, obs_service_t * stream_service) {
  obs_output_t * rtmp_output = obs_output_create("rtmp_output", "RTMP Output", nullptr, nullptr);
  if (not rtmp_output) {
      return nullptr;
  }
  obs_output_set_video_encoder(rtmp_output, video_encoder);
  obs_output_set_audio_encoder(rtmp_output, audio_encoder, 0);
  obs_output_set_service(rtmp_output, stream_service);
  return rtmp_output;
}

obs_source_t * create_media_source(const std::string & file_path, const std::string & source_name) {
  obs_data_t * settings = obs_data_create();
  obs_data_set_string(settings, "local_file", file_path.c_str());
  obs_data_set_bool(settings, "is_local_file", true);
  obs_data_set_bool(settings, "looping", true);
  obs_source_t * source = obs_source_create("ffmpeg_source", source_name.c_str(), settings, nullptr);
  obs_data_release(settings);
  return source;
}

obs_sceneitem_t * add_source_to_scene(obs_scene_t * scene, obs_source_t * source) {
  if (scene && source) {
    obs_sceneitem_t * item = obs_scene_add(scene, source);
    return item;
  }
  return nullptr;
}

static bool start_output(obs_output_t * rtmp_output) {
  return rtmp_output && obs_output_start(rtmp_output);
}

static bool stop_output(obs_output_t * rtmp_output) {
  if(rtmp_output && obs_output_active(rtmp_output)) {
    obs_output_stop(rtmp_output);
    return true;
  }
  return false;
}

napi_value TestObsStartup(napi_env env, napi_callback_info info) {
  napi_value status;
  if(obs_startup(nullptr, nullptr, nullptr)) {
    obs_shutdown();
    napi_create_string_utf8(env, "True", NAPI_AUTO_LENGTH, &status);
  }
  else {
    napi_create_string_utf8(env, "False", NAPI_AUTO_LENGTH, &status);
  }
  return status;
}

napi_value ObsVersion(napi_env env, napi_callback_info info) {
  napi_value version;
  napi_create_string_utf8(env, obs_get_version_string(), NAPI_AUTO_LENGTH, &version);
  return version;
}

napi_value ObsInitialized(napi_env env, napi_callback_info info) {
  napi_value status;
  if(obs_initialized()) {
    napi_create_string_utf8(env, "True", NAPI_AUTO_LENGTH, &status);
  }
  else {
    napi_create_string_utf8(env, "False", NAPI_AUTO_LENGTH, &status);
  }
  return status;
}

napi_value ObsStartStream(napi_env env, napi_callback_info info) {
  napi_value status;
  const std::string video_file_path = "/home/nik/Videos/Screencasts/obs-stream-test.mp4"; // Update this to your video path
  const std::string scene_name = "Video Scene";
  const std::string source_name = "Media Source";
  cout << "starting up..." << endl;
  if(obs_startup(nullptr, nullptr, nullptr)) {
    cout << "loading all modules..." << endl;
    obs_load_all_modules();
    cout << "posting load modules..." << endl;
    obs_post_load_modules();
    auto video_settings = get_video_settings();
    auto audio_settings = get_audio_settings();
    cout << "resetting audio and video settings..." << endl;
    if (obs_reset_video(&video_settings) == OBS_VIDEO_SUCCESS and obs_reset_audio(&audio_settings)) {
      cout << "creating scene..." << endl;
      auto * scene = obs_scene_create(scene_name.c_str());
      if(scene) {
        cout << "creating media source..." << endl;
        auto source = create_media_source(video_file_path, source_name);
        if(source) {
          cout << "adding source to scene..." << endl;
          auto scene_item = add_source_to_scene(scene, source);
          if(scene_item) {
            cout << "creating audio and video encoders..." << endl;
            auto video_encoder = create_video_encoder();
            auto audio_encoder = create_audio_encoder();
            if(video_encoder and audio_encoder) {
              cout << "creating stream service..." << endl;
              auto stream_service = create_stream_service("rtmp://live.twitch.tv/app", "");
              if(stream_service) {
                cout << "creating rtmp output..." << endl;
                g_rtmp_output = create_rtmp_output(audio_encoder, video_encoder, stream_service);
                cout << "starting stream..." << endl;
                if(g_rtmp_output and start_output(g_rtmp_output)) {
                  napi_create_string_utf8(env, "True", NAPI_AUTO_LENGTH, &status);
                  return status;
                }
              }
            }
          }
        }
      }
    }
  }
  napi_create_string_utf8(env, "False", NAPI_AUTO_LENGTH, &status);
  return status;
}

napi_value ObsStopStream(napi_env env, napi_callback_info info) {
  napi_value status;
  cout << "stopping stream..." << endl;
  if(stop_output(g_rtmp_output)) {
    napi_create_string_utf8(env, "True", NAPI_AUTO_LENGTH, &status);
  }
  else {
    napi_create_string_utf8(env, "False", NAPI_AUTO_LENGTH, &status);
  }
  return status;
}

napi_value init(napi_env env, napi_value exports) {
  napi_value nv_test_startup;
  napi_create_function(env, nullptr, 0, TestObsStartup, nullptr, &nv_test_startup);
  napi_set_named_property(env, exports, "test_obs_startup", nv_test_startup);

  napi_value nv_version;
  napi_create_function(env, nullptr, 0, ObsVersion, nullptr, &nv_version);
  napi_set_named_property(env, exports, "obs_version", nv_version);

  napi_value nv_initialized;
  napi_create_function(env, nullptr, 0, ObsInitialized, nullptr, &nv_initialized);
  napi_set_named_property(env, exports, "obs_initialized", nv_initialized);

  napi_value nv_start_stream;
  napi_create_function(env, nullptr, 0, ObsStartStream, nullptr, &nv_start_stream);
  napi_set_named_property(env, exports, "start_stream", nv_start_stream);

  napi_value nv_stop_stream;
  napi_create_function(env, nullptr, 0, ObsStopStream, nullptr, &nv_stop_stream);
  napi_set_named_property(env, exports, "stop_stream", nv_stop_stream);

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init)

}  // namespace demo

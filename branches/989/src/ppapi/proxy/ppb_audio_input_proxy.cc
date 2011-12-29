// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ppapi/proxy/ppb_audio_input_proxy.h"

#include "base/compiler_specific.h"
#include "base/threading/simple_thread.h"
#include "ppapi/c/dev/ppb_audio_input_dev.h"
#include "ppapi/c/pp_errors.h"
#include "ppapi/c/ppb_audio_config.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/trusted/ppb_audio_trusted.h"
#include "ppapi/proxy/enter_proxy.h"
#include "ppapi/proxy/plugin_dispatcher.h"
#include "ppapi/proxy/ppapi_messages.h"
#include "ppapi/shared_impl/api_id.h"
#include "ppapi/shared_impl/platform_file.h"
#include "ppapi/shared_impl/ppapi_globals.h"
#include "ppapi/shared_impl/ppb_audio_input_shared.h"
#include "ppapi/shared_impl/resource.h"
#include "ppapi/thunk/enter.h"
#include "ppapi/thunk/ppb_audio_config_api.h"
#include "ppapi/thunk/resource_creation_api.h"
#include "ppapi/thunk/thunk.h"

using ppapi::IntToPlatformFile;
using ppapi::thunk::EnterResourceNoLock;
using ppapi::thunk::PPB_AudioInput_API;
using ppapi::thunk::PPB_AudioConfig_API;

namespace ppapi {
namespace proxy {

class AudioInput : public Resource, public PPB_AudioInput_Shared {
 public:
  AudioInput(const HostResource& audio_input_id,
             PP_Resource config_id,
             PPB_AudioInput_Callback callback,
             void* user_data);
  virtual ~AudioInput();

  // Resource overrides.
  virtual PPB_AudioInput_API* AsPPB_AudioInput_API() OVERRIDE;

  // PPB_AudioInput_API implementation.
  virtual PP_Resource GetCurrentConfig() OVERRIDE;
  virtual PP_Bool StartCapture() OVERRIDE;
  virtual PP_Bool StopCapture() OVERRIDE;

  virtual int32_t OpenTrusted(PP_Resource config_id,
                              PP_CompletionCallback create_callback) OVERRIDE;
  virtual int32_t GetSyncSocket(int* sync_socket) OVERRIDE;
  virtual int32_t GetSharedMemory(int* shm_handle, uint32_t* shm_size) OVERRIDE;

 private:
  // Owning reference to the current config object. This isn't actually used,
  // we just dish it out as requested by the plugin.
  PP_Resource config_;

  DISALLOW_COPY_AND_ASSIGN(AudioInput);
};

AudioInput::AudioInput(const HostResource& audio_input_id,
                       PP_Resource config_id,
                       PPB_AudioInput_Callback callback,
                       void* user_data)
    : Resource(audio_input_id),
      config_(config_id) {
  SetCallback(callback, user_data);
  PpapiGlobals::Get()->GetResourceTracker()->AddRefResource(config_);
}

AudioInput::~AudioInput() {
  PpapiGlobals::Get()->GetResourceTracker()->ReleaseResource(config_);
}

PPB_AudioInput_API* AudioInput::AsPPB_AudioInput_API() {
  return this;
}

PP_Resource AudioInput::GetCurrentConfig() {
  // AddRef for the caller.
  PpapiGlobals::Get()->GetResourceTracker()->AddRefResource(config_);
  return config_;
}

PP_Bool AudioInput::StartCapture() {
  if (capturing())
    return PP_TRUE;
  SetStartCaptureState();
  PluginDispatcher::GetForResource(this)->Send(
      new PpapiHostMsg_PPBAudioInput_StartOrStop(
          API_ID_PPB_AUDIO_INPUT_DEV, host_resource(), true));
  return PP_TRUE;
}

PP_Bool AudioInput::StopCapture() {
  if (!capturing())
    return PP_TRUE;
  PluginDispatcher::GetForResource(this)->Send(
      new PpapiHostMsg_PPBAudioInput_StartOrStop(
          API_ID_PPB_AUDIO_INPUT_DEV, host_resource(), false));
  SetStopCaptureState();
  return PP_TRUE;
}

int32_t AudioInput::OpenTrusted(PP_Resource config_id,
                                PP_CompletionCallback create_callback) {
  return PP_ERROR_NOTSUPPORTED;  // Don't proxy the trusted interface.
}

int32_t AudioInput::GetSyncSocket(int* sync_socket) {
  return PP_ERROR_NOTSUPPORTED;  // Don't proxy the trusted interface.
}

int32_t AudioInput::GetSharedMemory(int* shm_handle, uint32_t* shm_size) {
  return PP_ERROR_NOTSUPPORTED;  // Don't proxy the trusted interface.
}

PPB_AudioInput_Proxy::PPB_AudioInput_Proxy(Dispatcher* dispatcher)
    : InterfaceProxy(dispatcher),
      callback_factory_(ALLOW_THIS_IN_INITIALIZER_LIST(this)) {
}

PPB_AudioInput_Proxy::~PPB_AudioInput_Proxy() {
}

// static
PP_Resource PPB_AudioInput_Proxy::CreateProxyResource(
    PP_Instance instance_id,
    PP_Resource config_id,
    PPB_AudioInput_Callback audio_input_callback,
    void* user_data) {
  PluginDispatcher* dispatcher = PluginDispatcher::GetForInstance(instance_id);
  if (!dispatcher)
    return 0;

  EnterResourceNoLock<PPB_AudioConfig_API> config(config_id, true);
  if (config.failed())
    return 0;

  if (!audio_input_callback)
    return 0;

  HostResource result;
  dispatcher->Send(new PpapiHostMsg_PPBAudioInput_Create(
      API_ID_PPB_AUDIO_INPUT_DEV, instance_id,
      config.object()->GetSampleRate(), config.object()->GetSampleFrameCount(),
      &result));
  if (result.is_null())
    return 0;

  return (new AudioInput(result, config_id, audio_input_callback,
                         user_data))->GetReference();
}

bool PPB_AudioInput_Proxy::OnMessageReceived(const IPC::Message& msg) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(PPB_AudioInput_Proxy, msg)
    IPC_MESSAGE_HANDLER(PpapiHostMsg_PPBAudioInput_Create, OnMsgCreate)
    IPC_MESSAGE_HANDLER(PpapiHostMsg_PPBAudioInput_StartOrStop,
                        OnMsgStartOrStop)
    IPC_MESSAGE_HANDLER(PpapiMsg_PPBAudioInput_NotifyAudioStreamCreated,
                        OnMsgNotifyAudioStreamCreated)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  // TODO(brettw) handle bad messages!

  return handled;
}

void PPB_AudioInput_Proxy::OnMsgCreate(PP_Instance instance_id,
                                       int32_t sample_rate,
                                       uint32_t sample_frame_count,
                                       HostResource* result) {
  thunk::EnterFunction<thunk::ResourceCreationAPI> resource_creation(
      instance_id, true);
  if (resource_creation.failed())
    return;

  // Make the resource and get the API pointer to its trusted interface.
  result->SetHostResource(
      instance_id,
      resource_creation.functions()->CreateAudioInputTrusted(instance_id));
  if (result->is_null())
    return;

  // At this point, we've set the result resource, and this is a sync request.
  // Anything below this point must issue the AudioInputChannelConnected
  // callback to the browser. Since that's an async message, it will be issued
  // back to the plugin after the Create function returns (which is good
  // because it would be weird to get a connected message with a failure code
  // for a resource you haven't finished creating yet).
  //
  // The ...ForceCallback class will help ensure the callback is always called.
  // All error cases must call SetResult on this class.

  EnterHostFromHostResourceForceCallback<PPB_AudioInput_API> enter(
      *result, callback_factory_,
      &PPB_AudioInput_Proxy::AudioInputChannelConnected, *result);
  if (enter.failed())
    return;  // When enter fails, it will internally schedule the callback.

  // Make an audio config object.
  PP_Resource audio_config_res =
      resource_creation.functions()->CreateAudioConfig(
          instance_id, static_cast<PP_AudioSampleRate>(sample_rate),
          sample_frame_count);
  if (!audio_config_res) {
    enter.SetResult(PP_ERROR_FAILED);
    return;
  }

  // Initiate opening the audio object.
  enter.SetResult(enter.object()->OpenTrusted(audio_config_res,
                                              enter.callback()));

  // Clean up the temporary audio config resource we made.
  const PPB_Core* core = static_cast<const PPB_Core*>(
      dispatcher()->local_get_interface()(PPB_CORE_INTERFACE));
  core->ReleaseResource(audio_config_res);
}

void PPB_AudioInput_Proxy::OnMsgStartOrStop(
    const HostResource& resource,
    bool capture) {
  EnterHostFromHostResource<PPB_AudioInput_API> enter(resource);
  if (enter.failed())
    return;
  if (capture)
    enter.object()->StartCapture();
  else
    enter.object()->StopCapture();
}

// Processed in the plugin (message from host).
void PPB_AudioInput_Proxy::OnMsgNotifyAudioStreamCreated(
    const HostResource& audio_id,
    int32_t result_code,
    IPC::PlatformFileForTransit socket_handle,
    base::SharedMemoryHandle handle,
    uint32_t length) {
  EnterPluginFromHostResource<PPB_AudioInput_API> enter(audio_id);
  if (enter.failed() || result_code != PP_OK) {
    // The caller may still have given us these handles in the failure case.
    // The easiest way to clean these up is to just put them in the objects
    // and then close them. This failure case is not performance critical.
    base::SyncSocket temp_socket(
        IPC::PlatformFileForTransitToPlatformFile(socket_handle));
    base::SharedMemory temp_mem(handle, false);
  } else {
    static_cast<AudioInput*>(enter.object())->SetStreamInfo(
        handle, length,
        IPC::PlatformFileForTransitToPlatformFile(socket_handle));
  }
}

void PPB_AudioInput_Proxy::AudioInputChannelConnected(
    int32_t result,
    const HostResource& resource) {
  IPC::PlatformFileForTransit socket_handle =
      IPC::InvalidPlatformFileForTransit();
  base::SharedMemoryHandle shared_memory = IPC::InvalidPlatformFileForTransit();
  uint32_t shared_memory_length = 0;

  int32_t result_code = result;
  if (result_code == PP_OK) {
    result_code = GetAudioInputConnectedHandles(resource, &socket_handle,
                                           &shared_memory,
                                           &shared_memory_length);
  }

  // Send all the values, even on error. This simplifies some of our cleanup
  // code since the handles will be in the other process and could be
  // inconvenient to clean up. Our IPC code will automatically handle this for
  // us, as long as the remote side always closes the handles it receives
  // (in OnMsgNotifyAudioStreamCreated), even in the failure case.
  dispatcher()->Send(new PpapiMsg_PPBAudioInput_NotifyAudioStreamCreated(
      API_ID_PPB_AUDIO_INPUT_DEV, resource, result_code, socket_handle,
      shared_memory, shared_memory_length));
}

int32_t PPB_AudioInput_Proxy::GetAudioInputConnectedHandles(
    const HostResource& resource,
    IPC::PlatformFileForTransit* foreign_socket_handle,
    base::SharedMemoryHandle* foreign_shared_memory_handle,
    uint32_t* shared_memory_length) {
  // Get the audio interface which will give us the handles.
  EnterHostFromHostResource<PPB_AudioInput_API> enter(resource);
  if (enter.failed())
    return PP_ERROR_NOINTERFACE;

  // Get the socket handle for signaling.
  int32_t socket_handle;
  int32_t result = enter.object()->GetSyncSocket(&socket_handle);
  if (result != PP_OK)
    return result;

  // socket_handle doesn't belong to us: don't close it.
  *foreign_socket_handle = dispatcher()->ShareHandleWithRemote(
      IntToPlatformFile(socket_handle), false);
  if (*foreign_socket_handle == IPC::InvalidPlatformFileForTransit())
    return PP_ERROR_FAILED;

  // Get the shared memory for the buffer.
  int shared_memory_handle;
  result = enter.object()->GetSharedMemory(&shared_memory_handle,
                                           shared_memory_length);
  if (result != PP_OK)
    return result;

  // shared_memory_handle doesn't belong to us: don't close it.
  *foreign_shared_memory_handle = dispatcher()->ShareHandleWithRemote(
      IntToPlatformFile(shared_memory_handle), false);
  if (*foreign_shared_memory_handle == IPC::InvalidPlatformFileForTransit())
    return PP_ERROR_FAILED;

  return PP_OK;
}

}  // namespace proxy
}  // namespace ppapi

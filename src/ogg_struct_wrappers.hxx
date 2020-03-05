#ifndef OGGSTRUCTWRAPPER_HXX
#define OGGSTRUCTWRAPPER_HXX

#include <napi.h>
#include "ogg/ogg.h"

namespace nodeogg
{
class OggSyncState : public Napi::ObjectWrap<OggSyncState>
{
public:
  static void Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(Napi::Value arg);

  OggSyncState(const Napi::CallbackInfo &info);
  ~OggSyncState();

  ogg_sync_state oy;

private:
  static Napi::FunctionReference constructor;
};

class OggStreamState : public Napi::ObjectWrap<OggStreamState>
{
public:
  static void Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(Napi::Value arg);

  OggStreamState(const Napi::CallbackInfo &info);
  ~OggStreamState();

  ogg_stream_state os;

private:
  static Napi::FunctionReference constructor;
};

class OggPage : public Napi::ObjectWrap<OggPage>
{
public:
  static void Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(Napi::Value arg);

  OggPage(const Napi::CallbackInfo &info);
  ~OggPage();

  void setHeader(const Napi::CallbackInfo &info, const Napi::Value &value);
  Napi::Value getHeader(const Napi::CallbackInfo &info);

  void setBody(const Napi::CallbackInfo &info, const Napi::Value &value);
  Napi::Value getBody(const Napi::CallbackInfo &info);
  Napi::Value toBuffer(const Napi::CallbackInfo &info);

  ogg_page op;

private:
  static Napi::FunctionReference constructor;
};

class OggPacket : public Napi::ObjectWrap<OggPacket>
{
public:
  static void Init(Napi::Env env, Napi::Object exports);
  static Napi::Object NewInstance(Napi::Value arg);

  OggPacket(const Napi::CallbackInfo &info);
  ~OggPacket();

  Napi::Value packet(const Napi::CallbackInfo &info);
  void setPacket(const Napi::CallbackInfo &info, const Napi::Value &value);

  Napi::Value bytes(const Napi::CallbackInfo &info);

  Napi::Value b_o_s(const Napi::CallbackInfo &info);
  void setB_o_s(const Napi::CallbackInfo &info, const Napi::Value &value);

  Napi::Value e_o_s(const Napi::CallbackInfo &info);
  void setE_o_s(const Napi::CallbackInfo &info, const Napi::Value &value);

  Napi::Value granulepos(const Napi::CallbackInfo &info);
  void setGranulepos(const Napi::CallbackInfo &info, const Napi::Value &value);

  Napi::Value packetno(const Napi::CallbackInfo &info);
  void setPacketno(const Napi::CallbackInfo &info, const Napi::Value &value);

  ogg_packet op;

private:
  static Napi::FunctionReference constructor;
};

} // namespace nodeogg

#endif

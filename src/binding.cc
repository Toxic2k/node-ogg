/*
 * Copyright (c) 2012, Nathan Rajlich <nathan@tootallnate.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * libogg API reference:
 * http://xiph.org/ogg/doc/libogg/reference.html
 */

#include <napi.h>
#include "ogg/ogg.h"
#include "ogg_struct_wrappers.hxx"

namespace nodeogg
{

OggPage::OggPage(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<OggPage>(info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
};

Napi::FunctionReference OggPage::constructor;

OggPage::~OggPage()
{
}

void OggPage::Init(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  std::vector<PropertyDescriptor> methods = {
      InstanceAccessor("header", &OggPage::getHeader, &OggPage::setHeader),
      InstanceAccessor("body", &OggPage::getBody, &OggPage::setBody),
      InstanceMethod("toBuffer", &OggPage::toBuffer)};

  Napi::Function func = DefineClass(env, "OggPage", methods);

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("OggPage", func);
}

void OggPage::setHeader(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  if (!value.IsTypedArray())
  {
    Napi::TypeError::New(env, "Expected a TypedArray").ThrowAsJavaScriptException();
    return;
  }

  Napi::TypedArrayOf<uint8_t> header = value.As<Napi::TypedArrayOf<uint8_t>>();
  op.header = header.Data();
  op.header_len = header.ByteLength();
}

Napi::Value OggPage::getHeader(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return Napi::Buffer<uint8_t>::New(env, op.header, op.header_len);
}

void OggPage::setBody(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  if (!value.IsTypedArray())
  {
    Napi::TypeError::New(env, "Expected a TypedArray").ThrowAsJavaScriptException();
    return;
  }

  Napi::TypedArrayOf<uint8_t> body = value.As<Napi::TypedArrayOf<uint8_t>>();
  op.body = body.Data();
  op.body_len = body.ByteLength();
}

Napi::Value OggPage::getBody(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return Napi::Buffer<uint8_t>::New(env, op.body, op.body_len);
}

Napi::Value OggPage::toBuffer(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  Napi::Buffer<uint8_t> buff = Napi::Buffer<uint8_t>::New(env, op.body_len + op.header_len);
  memcpy(buff.Data(), op.header, op.header_len);
  memcpy(buff.Data() + op.header_len, op.body, op.body_len);
  return buff;
}

Napi::Object OggPage::NewInstance(Napi::Value arg)
{
  Napi::Object obj = constructor.New({arg});
  return obj;
}

//
// --------------
//

OggPacket::OggPacket(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<OggPacket>(info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
};

Napi::FunctionReference OggPacket::constructor;

OggPacket::~OggPacket()
{
}

void OggPacket::Init(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  std::vector<PropertyDescriptor> methods = {
      InstanceAccessor("packet", &OggPacket::packet, &OggPacket::setPacket),
      InstanceAccessor("bytes", &OggPacket::bytes, nullptr),
      InstanceAccessor("b_o_s", &OggPacket::b_o_s, &OggPacket::setB_o_s),
      InstanceAccessor("e_o_s", &OggPacket::e_o_s, &OggPacket::setE_o_s),
      InstanceAccessor("granulepos", &OggPacket::granulepos, &OggPacket::setGranulepos),
      InstanceAccessor("packetno", &OggPacket::packetno, &OggPacket::setPacketno)};
  Napi::Function func = DefineClass(env, "OggPacket", methods);

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("OggPacket", func);
}

Napi::Value OggPacket::packet(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return Napi::Number::New(env, op.bytes);
}

void OggPacket::setPacket(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  if (!value.IsTypedArray())
  {
    Napi::TypeError::New(env, "Expected a TypedArray").ThrowAsJavaScriptException();
    return;
  }
  Napi::TypedArrayOf<uint8_t> packet = info[0].As<Napi::TypedArrayOf<uint8_t>>();
  op.packet = packet.Data();
  op.bytes = packet.ByteLength();
}

Napi::Value OggPacket::bytes(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return Napi::Number::New(env, op.bytes);
}

Napi::Value OggPacket::b_o_s(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return Napi::Number::New(env, op.b_o_s);
}

void OggPacket::setB_o_s(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  if (!value.IsNumber())
  {
    Napi::TypeError::New(env, "Expected a Number").ThrowAsJavaScriptException();
    return;
  }
  op.b_o_s = value.As<Napi::Number>();
}

Napi::Value OggPacket::e_o_s(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return Napi::Number::New(env, op.e_o_s);
}
void OggPacket::setE_o_s(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();

  if (!value.IsNumber())
  {
    Napi::TypeError::New(env, "Expected a Number").ThrowAsJavaScriptException();
    return;
  }
  op.e_o_s = value.As<Napi::Number>();
}

Napi::Value OggPacket::granulepos(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return Napi::Number::New(env, op.granulepos);
}

void OggPacket::setGranulepos(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  if (!value.IsNumber())
  {
    Napi::TypeError::New(env, "Expected a Number").ThrowAsJavaScriptException();
    return;
  }
  op.granulepos = value.As<Napi::Number>();
}

Napi::Value OggPacket::packetno(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  return Napi::Number::New(env, op.packetno);
}

void OggPacket::setPacketno(const Napi::CallbackInfo &info, const Napi::Value &value)
{
  Napi::Env env = info.Env();
  if (!value.IsNumber())
  {
    Napi::TypeError::New(env, "Expected a Number").ThrowAsJavaScriptException();
    return;
  }
  op.packetno = value.As<Napi::Number>();
}

Napi::Object OggPacket::NewInstance(Napi::Value arg)
{
  Napi::Object obj = constructor.New({arg});
  return obj;
}

//
// -----------
//

OggSyncState::OggSyncState(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<OggSyncState>(info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);

  ogg_sync_init(&oy);
};

Napi::FunctionReference OggSyncState::constructor;

OggSyncState::~OggSyncState()
{
  ogg_sync_clear(&oy);
}

void OggSyncState::Init(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "OggSyncState", {});

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("OggSyncState", func);
}

Napi::Object OggSyncState::NewInstance(Napi::Value arg)
{
  Napi::Object obj = constructor.New({arg});
  return obj;
}

class OggSyncWriteWorker : public Napi::AsyncWorker
{
public:
  OggSyncWriteWorker(ogg_sync_state *oy, Napi::TypedArrayOf<uint8_t> buffer, Napi::Function &callback)
      : Napi::AsyncWorker(callback), oy(oy), buffer(buffer), rtn(0) {}
  ~OggSyncWriteWorker() {}
  void Execute()
  {
    size_t byteLength = buffer.ByteLength();
    char *localBuffer = ogg_sync_buffer(oy, byteLength);
    memcpy(localBuffer, buffer.Data(), byteLength);
    rtn = ogg_sync_wrote(oy, byteLength);
  }

  void OnOK()
  {
    Napi::Env env = Env();

    Callback().Call({Napi::Number::New(env, rtn)});
  }

private:
  ogg_sync_state *oy;
  Napi::TypedArrayOf<uint8_t> buffer;
  int rtn;
};

void node_ogg_sync_write(const Napi::CallbackInfo &info)
{
  OggSyncState *syncState =
      Napi::ObjectWrap<OggSyncState>::Unwrap(info[0].As<Napi::Object>());
  Napi::TypedArrayOf<uint8_t> data = info[1].As<Napi::TypedArrayOf<uint8_t>>();
  Napi::Function cb = info[2].As<Napi::Function>();

  (new OggSyncWriteWorker(&syncState->oy, data, cb))->Queue();
}

// NAN_METHOD(node_ogg_sync_init)
// {
//   Nan::HandleScope scope;
//   ogg_sync_state *oy = reinterpret_cast<ogg_sync_state *>(UnwrapPointer(info[0]));
//   Local<Value> rtn = Nan::New<Integer>(ogg_sync_init(oy));
//   info.GetReturnValue().Set(rtn);
// }

/* combination of "ogg_sync_buffer", "memcpy", and "ogg_sync_wrote" on the thread
 * pool.
 */

// NAN_METHOD(node_ogg_sync_write)
// {
//   Nan::HandleScope scope;

//   ogg_sync_state *oy = reinterpret_cast<ogg_sync_state *>(UnwrapPointer(info[0]));
//   char *buffer = reinterpret_cast<char *>(UnwrapPointer(info[1]));
//   long size = static_cast<long>(info[2]->NumberValue(v8::Isolate::GetCurrent()->GetCurrentContext()).ToChecked());
//   Nan::Callback *callback = new Nan::Callback(info[3].As<Function>());

//   Nan::AsyncQueueWorker(new OggSyncWriteWorker(oy, buffer, size, callback));
// }

/* Reads out an `ogg_page` struct. */
class OggSyncPageoutWorker : public Napi::AsyncWorker
{
public:
  OggSyncPageoutWorker(ogg_sync_state *oy, Napi::Function &callback)
      : Napi::AsyncWorker(callback), oy(oy), page(), serialno(-1), packets(-1), rtn(0)
  {
  }
  ~OggSyncPageoutWorker() {}
  void Execute()
  {
    rtn = ogg_sync_pageout(oy, &page);
    if (rtn == 1)
    {
      serialno = ogg_page_serialno(&page);
      packets = ogg_page_packets(&page);
    }
  }
  void OnOK()
  {
    Napi::Env env = Env();

    Callback().Call({Napi::Number::New(env, rtn), Napi::Number::New(env, serialno), Napi::Number::New(env, packets)});
  }

private:
  ogg_sync_state *oy;
  ogg_page page;
  int serialno;
  int packets;
  int rtn;
};

void node_ogg_sync_pageout(const Napi::CallbackInfo &info)
{
  OggSyncState *syncState =
      Napi::ObjectWrap<OggSyncState>::Unwrap(info[0].As<Napi::Object>());
  Napi::Function cb = info[1].As<Napi::Function>();
  (new OggSyncPageoutWorker(&syncState->oy, cb))->Queue();
}

// NAN_METHOD(node_ogg_sync_pageout)
// {
//   Nan::HandleScope scope;

//   ogg_sync_state *oy = reinterpret_cast<ogg_sync_state *>(UnwrapPointer(info[0]));
//   ogg_page *page = reinterpret_cast<ogg_page *>(UnwrapPointer(info[1]));
//   Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());

//   Nan::AsyncQueueWorker(new OggSyncPageoutWorker(oy, page, callback));
// }

static int serial = 1;
OggStreamState::OggStreamState(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<OggStreamState>(info)
{
  Napi::Env env = info.Env();
  Napi::HandleScope scope(env);
  int serialno = info.Length() < 1 ? serial++ : info[0].ToNumber();
  if (0 != ogg_stream_init(&os, serialno))
  {
    Napi::TypeError::New(env, "ogg_stream_init() failed").ThrowAsJavaScriptException();
  }
}

Napi::FunctionReference OggStreamState::constructor;

OggStreamState::~OggStreamState()
{
  ogg_stream_clear(&os);
}

void OggStreamState::Init(Napi::Env env, Napi::Object exports)
{
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "OggStreamState", {});

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("OggStreamState", func);
}

Napi::Object OggStreamState::NewInstance(Napi::Value arg)
{
  Napi::Object obj = constructor.New({arg});
  return obj;
}

// NAN_METHOD(node_ogg_stream_init)
// {
//   Nan::HandleScope scope;
//   ogg_stream_state *os = reinterpret_cast<ogg_stream_state *>(UnwrapPointer(info[0]));
//   int serialno = static_cast<int>(info[1]->IntegerValue(v8::Isolate::GetCurrent()->GetCurrentContext()).ToChecked());
//   info.GetReturnValue().Set(Nan::New<Integer>(ogg_stream_init(os, serialno)));
// }

/* Writes a `ogg_page` struct into a `ogg_stream_state`. */
class OggStreamPageinWorker : public Napi::AsyncWorker
{
public:
  OggStreamPageinWorker(ogg_stream_state *os, Napi::Function &callback)
      : Napi::AsyncWorker(callback), os(os), page(), rtn(0) {}
  void Execute()
  {
    rtn = ogg_stream_pagein(os, &page);
  }

  void OnOK()
  {
    Napi::Env env = Env();

    Callback().Call({Napi::Number::New(env, rtn)});
  }

private:
  ogg_stream_state *os;
  ogg_page page;
  int rtn;
};

void node_ogg_stream_pagein(const Napi::CallbackInfo &info)
{
  OggStreamState *streamState =
      Napi::ObjectWrap<OggStreamState>::Unwrap(info[0].As<Napi::Object>());
  Napi::Function cb = info[1].As<Napi::Function>();
  (new OggStreamPageinWorker(&streamState->os, cb))->Queue();
}

// NAN_METHOD(node_ogg_stream_pagein)
// {
//   Nan::HandleScope scope;

//   ogg_stream_state *os = reinterpret_cast<ogg_stream_state *>(UnwrapPointer(info[0]));
//   ogg_page *page = reinterpret_cast<ogg_page *>(UnwrapPointer(info[1]));
//   Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());

//   Nan::AsyncQueueWorker(new OggStreamPageinWorker(os, page, callback));
// }

/* Reads a `ogg_packet` struct from a `ogg_stream_state`. */
class OggStreamPacketoutWorker : public Napi::AsyncWorker
{
public:
  OggStreamPacketoutWorker(ogg_stream_state *os, ogg_packet *packet, Napi::Function &callback)
      : Napi::AsyncWorker(callback), os(os), packet(packet), rtn(0) {}
  ~OggStreamPacketoutWorker() {}
  void Execute()
  {
    rtn = ogg_stream_packetout(os, packet);
  }
  void HandleOKCallback()
  {
    Napi::Env env = Env();
    if (rtn == 1)
    {
      Callback().Call({Napi::Number::New(env, rtn),
                       Napi::Number::New(env, packet->bytes),
                       Napi::Number::New(env, packet->b_o_s),
                       Napi::Number::New(env, packet->e_o_s),
                       Napi::Number::New(env, static_cast<double>(packet->granulepos)),
                       Napi::Number::New(env, static_cast<double>(packet->packetno))});
    }
    else
    {
      Callback().Call({Env().Null(),
                       Env().Null(),
                       Env().Null(),
                       Env().Null(),
                       Env().Null(),
                       Env().Null()});
    }
  }

private:
  ogg_stream_state *os;
  ogg_packet *packet;
  int rtn;
};

void node_ogg_stream_packetout(const Napi::CallbackInfo &info)
{
  OggStreamState *streamState =
      Napi::ObjectWrap<OggStreamState>::Unwrap(info[0].As<Napi::Object>());
  OggPacket *oggPacket =
      Napi::ObjectWrap<OggPacket>::Unwrap(info[1].As<Napi::Object>());
  Napi::Function cb = info[2].As<Napi::Function>();
  (new OggStreamPacketoutWorker(&streamState->os, &oggPacket->op, cb))->Queue();
}

// NAN_METHOD(node_ogg_stream_packetout)
// {
//   Nan::HandleScope scope;

//   ogg_stream_state *os = reinterpret_cast<ogg_stream_state *>(UnwrapPointer(info[0]));
//   ogg_packet *packet = reinterpret_cast<ogg_packet *>(UnwrapPointer(info[1]));
//   Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());

//   Nan::AsyncQueueWorker(new OggStreamPacketoutWorker(os, packet, callback));
// }

/* Writes a `ogg_packet` struct to a `ogg_stream_state`. */
class OggStreamPacketinWorker : public Napi::AsyncWorker
{
public:
  OggStreamPacketinWorker(ogg_stream_state *os, ogg_packet *packet, Napi::Function &callback)
      : Napi::AsyncWorker(callback), os(os), packet(packet), rtn(0) {}
  ~OggStreamPacketinWorker() {}

  void Execute()
  {
    rtn = ogg_stream_packetin(os, packet);
  }

  void OnOK()
  {
    Napi::Env env = Env();

    Callback().Call({Napi::Number::New(env, rtn)});
  }

private:
  ogg_stream_state *os;
  ogg_packet *packet;
  int rtn;
};

void node_ogg_stream_packetin(const Napi::CallbackInfo &info)
{
  OggStreamState *streamState =
      Napi::ObjectWrap<OggStreamState>::Unwrap(info[0].As<Napi::Object>());
  OggPacket *oggPacket =
      Napi::ObjectWrap<OggPacket>::Unwrap(info[1].As<Napi::Object>());
  Napi::Function cb = info[2].As<Napi::Function>();
  (new OggStreamPacketinWorker(&streamState->os, &oggPacket->op, cb))->Queue();
}

// NAN_METHOD(node_ogg_stream_packetin)
// {
//   Nan::HandleScope scope;

//   ogg_stream_state *os = reinterpret_cast<ogg_stream_state *>(UnwrapPointer(info[0]));
//   ogg_packet *packet = reinterpret_cast<ogg_packet *>(UnwrapPointer(info[1]));
//   Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());

//   Nan::AsyncQueueWorker(new OggStreamPacketinWorker(os, packet, callback));
// }

// Since both StreamPageout and StreamFlush have the same HandleOKCallback,
// this base class deals with both.
class StreamWorker : public Napi::AsyncWorker
{
public:
  StreamWorker(ogg_stream_state *os, ogg_page *page, Napi::Function &callback)
      : Napi::AsyncWorker(callback), os(os), page(page), rtn(0) {}
  void OnOK()
  {
    Napi::Env env = Env();
    if (rtn == 1)
    {
      Callback().Call({Napi::Number::New(env, rtn),
                       Napi::Number::New(env, page->header_len),
                       Napi::Number::New(env, page->body_len),
                       Napi::Number::New(env, ogg_page_eos(page))});
    }
    else
    {
      Callback().Call({Napi::Number::New(env, rtn),
                       Env().Null(),
                       Env().Null(),
                       Env().Null()});
    };
  }

protected:
  ogg_stream_state *os;
  ogg_page *page;
  int rtn;
};

class StreamPageoutWorker : public StreamWorker
{
public:
  StreamPageoutWorker(ogg_stream_state *os, ogg_page *page, Napi::Function &callback)
      : StreamWorker(os, page, callback) {}
  ~StreamPageoutWorker() {}
  void Execute()
  {
    rtn = ogg_stream_pageout(os, page);
  }
};

void node_ogg_stream_pageout(const Napi::CallbackInfo &info)
{
  OggStreamState *streamState =
      Napi::ObjectWrap<OggStreamState>::Unwrap(info[0].As<Napi::Object>());
  OggPage *page = Napi::ObjectWrap<OggPage>::Unwrap(info[1].As<Napi::Object>());
  Napi::Function cb = info[2].As<Napi::Function>();

  (new StreamPageoutWorker(
       &streamState->os,
       &page->op,
       cb))
      ->Queue();
}
/* Reads out a `ogg_page` struct from an `ogg_stream_state`. */
// NAN_METHOD(node_ogg_stream_pageout)
// {
//   Nan::HandleScope scope;
//   Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());

//   Nan::AsyncQueueWorker(
//       new StreamPageoutWorker(
//           reinterpret_cast<ogg_stream_state *>(UnwrapPointer(info[0])),
//           reinterpret_cast<ogg_page *>(UnwrapPointer(info[1])),
//           callback));
// }

class StreamFlushWorker : public StreamWorker
{
public:
  StreamFlushWorker(ogg_stream_state *os, ogg_page *page, Napi::Function &callback)
      : StreamWorker(os, page, callback) {}
  ~StreamFlushWorker() {}
  void Execute()
  {
    rtn = ogg_stream_flush(os, page);
  }
};

/* Forces an `ogg_page` struct to be flushed from an `ogg_stream_state`. */
void node_ogg_stream_flush(const Napi::CallbackInfo &info)
{
  OggStreamState *streamState =
      Napi::ObjectWrap<OggStreamState>::Unwrap(info[0].As<Napi::Object>());
  OggPage *page = Napi::ObjectWrap<OggPage>::Unwrap(info[1].As<Napi::Object>());
  Napi::Function cb = info[2].As<Napi::Function>();

  (new StreamFlushWorker(
       &streamState->os,
       &page->op,
       cb))
      ->Queue();
}

// NAN_METHOD(node_ogg_stream_flush)
// {
//   Nan::HandleScope scope;
//   Nan::Callback *callback = new Nan::Callback(info[2].As<Function>());

//   Nan::AsyncQueueWorker(
//       new StreamFlushWorker(
//           reinterpret_cast<ogg_stream_state *>(UnwrapPointer(info[0])),
//           reinterpret_cast<ogg_page *>(UnwrapPointer(info[1])),
//           callback));
// }

/* Converts an `ogg_page` instance to a node Buffer instance */
// NAN_METHOD(node_ogg_page_to_buffer)
// {
//   Nan::HandleScope scope;

//   ogg_page *op = reinterpret_cast<ogg_page *>(UnwrapPointer(info[0]));
//   unsigned char *buf = reinterpret_cast<unsigned char *>(UnwrapPointer(info[1]));
//   memcpy(buf, op->header, op->header_len);
//   memcpy(buf + op->header_len, op->body, op->body_len);
// }

/* packet->packet = ... */
// NAN_METHOD(node_ogg_packet_set_packet)
// {
//   Nan::HandleScope scope;
//   ogg_packet *packet = UnwrapPointer<ogg_packet *>(info[0]);
//   packet->packet = UnwrapPointer<unsigned char *>(info[1]);
// }

/* packet->packet */
// NAN_METHOD(node_ogg_packet_get_packet)
// {
//   Nan::HandleScope scope;
//   ogg_packet *packet = reinterpret_cast<ogg_packet *>(UnwrapPointer(info[0]));
//   Nan::MaybeLocal<v8::Object> wrapper = WrapPointer(packet->packet, packet->bytes);
//   info.GetReturnValue().Set(wrapper.ToLocalChecked());
// }

/* packet->bytes */
// NAN_METHOD(node_ogg_packet_bytes)
// {
//   Nan::HandleScope scope;
//   ogg_packet *packet = reinterpret_cast<ogg_packet *>(UnwrapPointer(info[0]));
//   info.GetReturnValue().Set(Nan::New<Number>(packet->bytes));
// }

/* packet->b_o_s */
// NAN_METHOD(node_ogg_packet_b_o_s)
// {
//   Nan::HandleScope scope;
//   ogg_packet *packet = reinterpret_cast<ogg_packet *>(UnwrapPointer(info[0]));
//   info.GetReturnValue().Set(Nan::New<Number>(packet->b_o_s));
// }

/* packet->e_o_s */
// NAN_METHOD(node_ogg_packet_e_o_s)
// {
//   Nan::HandleScope scope;
//   ogg_packet *packet = reinterpret_cast<ogg_packet *>(UnwrapPointer(info[0]));
//   info.GetReturnValue().Set(Nan::New<Number>(packet->e_o_s));
// }

/* packet->granulepos */
// NAN_METHOD(node_ogg_packet_granulepos)
// {
//   Nan::HandleScope scope;
//   ogg_packet *packet = reinterpret_cast<ogg_packet *>(UnwrapPointer(info[0]));
//   info.GetReturnValue().Set(Nan::New<Number>(static_cast<double>(packet->granulepos)));
// }

/* packet->packetno */
// NAN_METHOD(node_ogg_packet_packetno)
// {
//   Nan::HandleScope scope;
//   ogg_packet *packet = reinterpret_cast<ogg_packet *>(UnwrapPointer(info[0]));
//   info.GetReturnValue().Set(Nan::New<Number>(static_cast<double>(packet->packetno)));
// }

/* Replaces the `ogg_packet` "packet" pointer with a Node.js buffer instance */
// NAN_METHOD(node_ogg_packet_replace_buffer)
// {
//   Nan::HandleScope scope;

//   ogg_packet *packet = reinterpret_cast<ogg_packet *>(UnwrapPointer(info[0]));
//   unsigned char *buf = reinterpret_cast<unsigned char *>(UnwrapPointer(info[1]));
//   memcpy(buf, packet->packet, packet->bytes);
//   packet->packet = buf;
// }

// NAN_MODULE_INIT(Initialize)
// {
//   Nan::HandleScope scope;

//   /* sizeof's */
// #define SIZEOF(value)                                                        \
//   Nan::ForceSet(target, Nan::New<String>("sizeof_" #value).ToLocalChecked(), \
//                 Nan::New<Integer>(static_cast<int32_t>(sizeof(value))),      \
//                 static_cast<PropertyAttribute>(ReadOnly | DontDelete))
//   SIZEOF(ogg_sync_state);
//   SIZEOF(ogg_stream_state);
//   SIZEOF(ogg_page);
//   SIZEOF(ogg_packet);

//   Nan::SetMethod(target, "ogg_sync_init", node_ogg_sync_init);
//   Nan::SetMethod(target, "ogg_sync_write", node_ogg_sync_write);
//   Nan::SetMethod(target, "ogg_sync_pageout", node_ogg_sync_pageout);

//   Nan::SetMethod(target, "ogg_stream_init", node_ogg_stream_init);
//   Nan::SetMethod(target, "ogg_stream_pagein", node_ogg_stream_pagein);
//   Nan::SetMethod(target, "ogg_stream_packetout", node_ogg_stream_packetout);
//   Nan::SetMethod(target, "ogg_stream_packetin", node_ogg_stream_packetin);
//   Nan::SetMethod(target, "ogg_stream_pageout", node_ogg_stream_pageout);
//   Nan::SetMethod(target, "ogg_stream_flush", node_ogg_stream_flush);

//   /* custom functions */
//   Nan::SetMethod(target, "ogg_page_to_buffer", node_ogg_page_to_buffer);

//   Nan::SetMethod(target, "ogg_packet_set_packet", node_ogg_packet_set_packet);
//   Nan::SetMethod(target, "ogg_packet_get_packet", node_ogg_packet_get_packet);
//   Nan::SetMethod(target, "ogg_packet_bytes", node_ogg_packet_bytes);
//   Nan::SetMethod(target, "ogg_packet_b_o_s", node_ogg_packet_b_o_s);
//   Nan::SetMethod(target, "ogg_packet_e_o_s", node_ogg_packet_e_o_s);
//   Nan::SetMethod(target, "ogg_packet_granulepos", node_ogg_packet_granulepos);
//   Nan::SetMethod(target, "ogg_packet_packetno", node_ogg_packet_packetno);
//   Nan::SetMethod(target, "ogg_packet_replace_buffer", node_ogg_packet_replace_buffer);
// }

} // namespace nodeogg

// NODE_MODULE(ogg, nodeogg::Initialize)

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  using namespace nodeogg;
  OggSyncState::Init(env, exports);
  OggStreamState::Init(env, exports);
  OggPage::Init(env, exports);
  OggPacket::Init(env, exports);

  // exports.Set(Napi::String::New(env, "ogg_sync_init"), Napi::Function::New(env, node_ogg_sync_init));
  exports.Set(Napi::String::New(env, "ogg_sync_write"), Napi::Function::New(env, node_ogg_sync_write));
  exports.Set(Napi::String::New(env, "ogg_sync_pageout"), Napi::Function::New(env, node_ogg_sync_pageout));

  // exports.Set(Napi::String::New(env, "ogg_stream_init"), Napi::Function::New(env, node_ogg_stream_init));
  exports.Set(Napi::String::New(env, "ogg_stream_pagein"), Napi::Function::New(env, node_ogg_stream_pagein));
  exports.Set(Napi::String::New(env, "ogg_stream_packetout"), Napi::Function::New(env, node_ogg_stream_packetout));
  exports.Set(Napi::String::New(env, "ogg_stream_packetin"), Napi::Function::New(env, node_ogg_stream_packetin));
  exports.Set(Napi::String::New(env, "ogg_stream_pageout"), Napi::Function::New(env, node_ogg_stream_pageout));
  exports.Set(Napi::String::New(env, "ogg_stream_flush"), Napi::Function::New(env, node_ogg_stream_flush));

  /* custom functions */
  // exports.Set(Napi::String::New(env, "ogg_page_to_buffer"), Napi::Function::New(env, node_ogg_page_to_buffer));

  // exports.Set(Napi::String::New(env, "ogg_packet_set_packet"), Napi::Function::New(env, node_ogg_packet_set_packet));
  // exports.Set(Napi::String::New(env, "ogg_packet_get_packet"), Napi::Function::New(env, node_ogg_packet_get_packet));
  // exports.Set(Napi::String::New(env, "ogg_packet_bytes"), Napi::Function::New(env, node_ogg_packet_bytes));
  // exports.Set(Napi::String::New(env, "ogg_packet_b_o_s"), Napi::Function::New(env, node_ogg_packet_b_o_s));
  // exports.Set(Napi::String::New(env, "ogg_packet_e_o_s"), Napi::Function::New(env, node_ogg_packet_e_o_s));
  // exports.Set(Napi::String::New(env, "ogg_packet_granulepos"), Napi::Function::New(env, node_ogg_packet_granulepos));
  // exports.Set(Napi::String::New(env, "ogg_packet_packetno"), Napi::Function::New(env, node_ogg_packet_packetno));
  // exports.Set(Napi::String::New(env, "ogg_packet_replace_buffer"), Napi::Function::New(env, node_ogg_packet_replace_buffer));

  return exports;
}
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)

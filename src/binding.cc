/*
 * Copyright (c) 2020, Valyant AI
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

namespace nodeogg {

napi_property_attributes property_writable_enumerable =
    static_cast<napi_property_attributes>(napi_enumerable | napi_writable);
OggPage::OggPage(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<OggPage>(info) {
  memset(&op, 0, sizeof(op));
}

Napi::FunctionReference OggPage::constructor;

OggPage::~OggPage() {}

void OggPage::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  std::vector<PropertyDescriptor> methods = {
      InstanceAccessor("header", &OggPage::getHeader, &OggPage::setHeader,
                       property_writable_enumerable),
      InstanceAccessor("body", &OggPage::getBody, &OggPage::setBody,
                       property_writable_enumerable),
      InstanceMethod("toBuffer", &OggPage::toBuffer)};

  Napi::Function func = DefineClass(env, "ogg_page", methods);

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("ogg_page", func);
}

void OggPage::setHeader(const Napi::CallbackInfo &info,
                        const Napi::Value &value) {
  Napi::Env env = info.Env();
  if (!value.IsTypedArray()) {
    Napi::TypeError::New(env, "Expected a TypedArray")
        .ThrowAsJavaScriptException();
    return;
  }

  Napi::TypedArrayOf<uint8_t> header = value.As<Napi::TypedArrayOf<uint8_t>>();
  jsBufferHeaderRef = Napi::Reference<Napi::TypedArrayOf<uint8_t>>::New(header);

  op.header = header.Data();
  op.header_len = header.ByteLength();
}

Napi::Value OggPage::getHeader(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Buffer<uint8_t>::New(env, op.header, op.header_len);
}

void OggPage::setBody(const Napi::CallbackInfo &info,
                      const Napi::Value &value) {
  Napi::Env env = info.Env();
  if (!value.IsTypedArray()) {
    Napi::TypeError::New(env, "Expected a TypedArray")
        .ThrowAsJavaScriptException();
    return;
  }

  Napi::TypedArrayOf<uint8_t> body = value.As<Napi::TypedArrayOf<uint8_t>>();
  jsBufferBodyRef = Napi::Reference<Napi::TypedArrayOf<uint8_t>>::New(body);
  op.body = body.Data();
  op.body_len = body.ByteLength();
}

Napi::Value OggPage::getBody(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Buffer<uint8_t>::New(env, op.body, op.body_len);
}

Napi::Value OggPage::toBuffer(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  Napi::Buffer<uint8_t> buff =
      Napi::Buffer<uint8_t>::New(env, op.body_len + op.header_len);
  memcpy(buff.Data(), op.header, op.header_len);
  memcpy(buff.Data() + op.header_len, op.body, op.body_len);
  return buff;
}

Napi::Object OggPage::NewInstance(Napi::Value arg) {
  Napi::Object obj = constructor.New({arg});
  return obj;
}

//
// --------------
//

OggPacket::OggPacket(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<OggPacket>(info) {
  memset(&op, 0, sizeof(op));
}

Napi::FunctionReference OggPacket::constructor;

OggPacket::~OggPacket() {}

void OggPacket::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  std::vector<PropertyDescriptor> methods = {
      InstanceAccessor("packet", &OggPacket::packet, &OggPacket::setPacket,
                       property_writable_enumerable),
      InstanceAccessor("bytes", &OggPacket::bytes, nullptr, napi_enumerable),
      InstanceAccessor("b_o_s", &OggPacket::b_o_s, &OggPacket::setB_o_s,
                       property_writable_enumerable),
      InstanceAccessor("e_o_s", &OggPacket::e_o_s, &OggPacket::setE_o_s,
                       property_writable_enumerable),
      InstanceAccessor("granulepos", &OggPacket::granulepos,
                       &OggPacket::setGranulepos, property_writable_enumerable),
      InstanceAccessor("packetno", &OggPacket::packetno,
                       &OggPacket::setPacketno, property_writable_enumerable)};
  Napi::Function func = DefineClass(env, "ogg_packet", methods);

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("ogg_packet", func);
}

Napi::Value OggPacket::packet(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Buffer<uint8_t>::New(env, op.packet, op.bytes);
}

void OggPacket::setPacket(const Napi::CallbackInfo &info,
                          const Napi::Value &value) {
  Napi::Env env = info.Env();
  if (!value.IsTypedArray()) {
    Napi::TypeError::New(env, "Expected a TypedArray")
        .ThrowAsJavaScriptException();
    return;
  }
  Napi::TypedArrayOf<uint8_t> packet =
      info[0].As<Napi::TypedArrayOf<uint8_t>>();
  jsBufferRef = Napi::Reference<Napi::TypedArrayOf<uint8_t>>::New(packet);
  op.packet = packet.Data();
  op.bytes = packet.ByteLength();
}

Napi::Value OggPacket::bytes(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::New(env, op.bytes);
}

Napi::Value OggPacket::b_o_s(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::New(env, op.b_o_s);
}

void OggPacket::setB_o_s(const Napi::CallbackInfo &info,
                         const Napi::Value &value) {
  Napi::Env env = info.Env();
  if (!value.IsNumber()) {
    Napi::TypeError::New(env, "Expected a Number").ThrowAsJavaScriptException();
    return;
  }
  op.b_o_s = value.As<Napi::Number>().Int32Value();
}

Napi::Value OggPacket::e_o_s(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::New(env, op.e_o_s);
}
void OggPacket::setE_o_s(const Napi::CallbackInfo &info,
                         const Napi::Value &value) {
  Napi::Env env = info.Env();

  if (!value.IsNumber()) {
    Napi::TypeError::New(env, "Expected a Number").ThrowAsJavaScriptException();
    return;
  }
  op.e_o_s = value.As<Napi::Number>().Int32Value();
}

Napi::Value OggPacket::granulepos(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::New(env, op.granulepos);
}

void OggPacket::setGranulepos(const Napi::CallbackInfo &info,
                              const Napi::Value &value) {
  Napi::Env env = info.Env();
  if (!value.IsNumber()) {
    Napi::TypeError::New(env, "Expected a Number").ThrowAsJavaScriptException();
    return;
  }
  op.granulepos = value.As<Napi::Number>();
}

Napi::Value OggPacket::packetno(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();
  return Napi::Number::New(env, op.packetno);
}

void OggPacket::setPacketno(const Napi::CallbackInfo &info,
                            const Napi::Value &value) {
  Napi::Env env = info.Env();
  if (!value.IsNumber()) {
    Napi::TypeError::New(env, "Expected a Number").ThrowAsJavaScriptException();
    return;
  }
  op.packetno = value.As<Napi::Number>();
}

Napi::Object OggPacket::NewInstance(Napi::Value arg) {
  Napi::Object obj = constructor.New({arg});
  return obj;
}

//
// -----------
//

OggSyncState::OggSyncState(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<OggSyncState>(info) {
  ogg_sync_init(&oy);
};

Napi::FunctionReference OggSyncState::constructor;

OggSyncState::~OggSyncState() { ogg_sync_clear(&oy); }

void OggSyncState::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "ogg_sync_state", {});

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("ogg_sync_state", func);
}

Napi::Object OggSyncState::NewInstance(Napi::Value arg) {
  Napi::Object obj = constructor.New({arg});
  return obj;
}

class OggSyncWriteWorker : public Napi::AsyncWorker {
 public:
  OggSyncWriteWorker(ogg_sync_state *oy, Napi::TypedArrayOf<uint8_t> buffer,
                     Napi::Function &callback)
      : Napi::AsyncWorker(callback), oy(oy), buffer(buffer), rtn(0) {}
  ~OggSyncWriteWorker() {}
  void Execute() {
    size_t byteLength = buffer.ByteLength();
    char *localBuffer = ogg_sync_buffer(oy, byteLength);
    memcpy(localBuffer, buffer.Data(), byteLength);
    rtn = ogg_sync_wrote(oy, byteLength);
  }

  void OnOK() {
    Napi::Env env = Env();

    Callback().Call({Napi::Number::New(env, rtn)});
  }

 private:
  ogg_sync_state *oy;
  Napi::TypedArrayOf<uint8_t> buffer;
  int rtn;
};

void node_ogg_sync_write(const Napi::CallbackInfo &info) {
  OggSyncState *syncState =
      Napi::ObjectWrap<OggSyncState>::Unwrap(info[0].As<Napi::Object>());
  Napi::TypedArrayOf<uint8_t> data = info[1].As<Napi::TypedArrayOf<uint8_t>>();
  Napi::Function cb = info[2].As<Napi::Function>();

  (new OggSyncWriteWorker(&syncState->oy, data, cb))->Queue();
}

/* combination of "ogg_sync_buffer", "memcpy", and "ogg_sync_wrote" on the
 * thread pool.
 */

/* Reads out an `ogg_page` struct. */
class OggSyncPageoutWorker : public Napi::AsyncWorker {
 public:
  OggSyncPageoutWorker(ogg_sync_state *oy, ogg_page *page,
                       Napi::Function &callback)
      : Napi::AsyncWorker(callback),
        oy(oy),
        page(page),
        serialno(-1),
        packets(-1),
        rtn(0) {}
  ~OggSyncPageoutWorker() {}
  void Execute() {
    rtn = ogg_sync_pageout(oy, page);
    if (rtn == 1) {
      serialno = ogg_page_serialno(page);
      packets = ogg_page_packets(page);
    }
  }
  void OnOK() {
    Napi::Env env = Env();

    Callback().Call({Napi::Number::New(env, rtn),
                     Napi::Number::New(env, serialno),
                     Napi::Number::New(env, packets)});
  }

 private:
  ogg_sync_state *oy;
  ogg_page *page;
  int serialno;
  int packets;
  int rtn;
};

void node_ogg_sync_pageout(const Napi::CallbackInfo &info) {
  OggSyncState *syncState =
      Napi::ObjectWrap<OggSyncState>::Unwrap(info[0].As<Napi::Object>());
  OggPage *page = Napi::ObjectWrap<OggPage>::Unwrap(info[1].As<Napi::Object>());
  Napi::Function cb = info[2].As<Napi::Function>();
  (new OggSyncPageoutWorker(&syncState->oy, &page->op, cb))->Queue();
}

static int serial = 1;
OggStreamState::OggStreamState(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<OggStreamState>(info) {
  Napi::Env env = info.Env();
  int serialno = info.Length() < 1 ? serial++ : info[0].ToNumber();
  if (0 != ogg_stream_init(&os, serialno)) {
    Napi::TypeError::New(env, "ogg_stream_init() failed")
        .ThrowAsJavaScriptException();
  }
}

Napi::FunctionReference OggStreamState::constructor;

OggStreamState::~OggStreamState() { ogg_stream_clear(&os); }

void OggStreamState::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "ogg_stream_state", {});

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("ogg_stream_state", func);
}

Napi::Object OggStreamState::NewInstance(Napi::Value arg) {
  Napi::Object obj = constructor.New({arg});
  return obj;
}

/* Writes a `ogg_page` struct into a `ogg_stream_state`. */
class OggStreamPageinWorker : public Napi::AsyncWorker {
 public:
  OggStreamPageinWorker(ogg_stream_state *os, ogg_page *page,
                        Napi::Function &callback)
      : Napi::AsyncWorker(callback), os(os), page(page), rtn(0) {}
  void Execute() { rtn = ogg_stream_pagein(os, page); }

  void OnOK() {
    Napi::Env env = Env();

    Callback().Call({Napi::Number::New(env, rtn)});
  }

 private:
  ogg_stream_state *os;
  ogg_page *page;
  int rtn;
};

void node_ogg_stream_pagein(const Napi::CallbackInfo &info) {
  OggStreamState *streamState =
      Napi::ObjectWrap<OggStreamState>::Unwrap(info[0].As<Napi::Object>());
  OggPage *page = Napi::ObjectWrap<OggPage>::Unwrap(info[1].As<Napi::Object>());

  Napi::Function cb = info[2].As<Napi::Function>();
  (new OggStreamPageinWorker(&streamState->os, &page->op, cb))->Queue();
}

/* Reads a `ogg_packet` struct from a `ogg_stream_state`. */
class OggStreamPacketoutWorker : public Napi::AsyncWorker {
 public:
  OggStreamPacketoutWorker(ogg_stream_state *os, ogg_packet *packet,
                           Napi::Function &callback)
      : Napi::AsyncWorker(callback), os(os), packet(packet), rtn(0) {}
  ~OggStreamPacketoutWorker() {}
  void Execute() { rtn = ogg_stream_packetout(os, packet); }
  void OnOK() {
    Napi::Env env = Env();
    if (rtn == 1) {
      Callback().Call(
          {Napi::Number::New(env, rtn), Napi::Number::New(env, packet->bytes),
           Napi::Number::New(env, packet->b_o_s),
           Napi::Number::New(env, packet->e_o_s),
           Napi::Number::New(env, static_cast<double>(packet->granulepos)),
           Napi::Number::New(env, static_cast<double>(packet->packetno))});
    } else {
      Callback().Call({Env().Null(), Env().Null(), Env().Null(), Env().Null(),
                       Env().Null(), Env().Null()});
    }
  }

 private:
  ogg_stream_state *os;
  ogg_packet *packet;
  int rtn;
};

void node_ogg_stream_packetout(const Napi::CallbackInfo &info) {
  OggStreamState *streamState =
      Napi::ObjectWrap<OggStreamState>::Unwrap(info[0].As<Napi::Object>());
  OggPacket *oggPacket =
      Napi::ObjectWrap<OggPacket>::Unwrap(info[1].As<Napi::Object>());
  Napi::Function cb = info[2].As<Napi::Function>();
  (new OggStreamPacketoutWorker(&streamState->os, &oggPacket->op, cb))->Queue();
}

/* Writes a `ogg_packet` struct to a `ogg_stream_state`. */
class OggStreamPacketinWorker : public Napi::AsyncWorker {
 public:
  OggStreamPacketinWorker(ogg_stream_state *os, ogg_packet *packet,
                          Napi::Function &callback)
      : Napi::AsyncWorker(callback), os(os), packet(packet), rtn(0) {}
  ~OggStreamPacketinWorker() {}

  void Execute() { rtn = ogg_stream_packetin(os, packet); }

  void OnOK() {
    Napi::Env env = Env();

    Callback().Call({Napi::Number::New(env, rtn)});
  }

 private:
  ogg_stream_state *os;
  ogg_packet *packet;
  int rtn;
};

void node_ogg_stream_packetin(const Napi::CallbackInfo &info) {
  OggStreamState *streamState =
      Napi::ObjectWrap<OggStreamState>::Unwrap(info[0].As<Napi::Object>());
  OggPacket *oggPacket =
      Napi::ObjectWrap<OggPacket>::Unwrap(info[1].As<Napi::Object>());
  Napi::Function cb = info[2].As<Napi::Function>();
  (new OggStreamPacketinWorker(&streamState->os, &oggPacket->op, cb))->Queue();
}

// Since both StreamPageout and StreamFlush have the same HandleOKCallback,
// this base class deals with both.
class StreamWorker : public Napi::AsyncWorker {
 public:
  StreamWorker(ogg_stream_state *os, ogg_page *page, Napi::Function &callback)
      : Napi::AsyncWorker(callback), os(os), page(page), rtn(0) {}
  void OnOK() {
    Napi::Env env = Env();
    if (rtn == 1) {
      Callback().Call({Napi::Number::New(env, rtn),
                       Napi::Number::New(env, page->header_len),
                       Napi::Number::New(env, page->body_len),
                       Napi::Number::New(env, ogg_page_eos(page))});
    } else {
      Callback().Call({Napi::Number::New(env, rtn), Env().Null(), Env().Null(),
                       Env().Null()});
    };
  }

 protected:
  ogg_stream_state *os;
  ogg_page *page;
  int rtn;
};

class StreamPageoutWorker : public StreamWorker {
 public:
  StreamPageoutWorker(ogg_stream_state *os, ogg_page *page,
                      Napi::Function &callback)
      : StreamWorker(os, page, callback) {}
  ~StreamPageoutWorker() {}
  void Execute() { rtn = ogg_stream_pageout(os, page); }
};

void node_ogg_stream_pageout(const Napi::CallbackInfo &info) {
  OggStreamState *streamState =
      Napi::ObjectWrap<OggStreamState>::Unwrap(info[0].As<Napi::Object>());
  OggPage *page = Napi::ObjectWrap<OggPage>::Unwrap(info[1].As<Napi::Object>());
  Napi::Function cb = info[2].As<Napi::Function>();

  (new StreamPageoutWorker(&streamState->os, &page->op, cb))->Queue();
}

/* Reads out a `ogg_page` struct from an `ogg_stream_state`. */
class StreamFlushWorker : public StreamWorker {
 public:
  StreamFlushWorker(ogg_stream_state *os, ogg_page *page,
                    Napi::Function &callback)
      : StreamWorker(os, page, callback) {}
  ~StreamFlushWorker() {}
  void Execute() { rtn = ogg_stream_flush(os, page); }
};

/* Forces an `ogg_page` struct to be flushed from an `ogg_stream_state`. */
void node_ogg_stream_flush(const Napi::CallbackInfo &info) {
  OggStreamState *streamState =
      Napi::ObjectWrap<OggStreamState>::Unwrap(info[0].As<Napi::Object>());
  OggPage *page = Napi::ObjectWrap<OggPage>::Unwrap(info[1].As<Napi::Object>());
  Napi::Function cb = info[2].As<Napi::Function>();

  (new StreamFlushWorker(&streamState->os, &page->op, cb))->Queue();
}

}  // namespace nodeogg

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  using namespace nodeogg;
  OggSyncState::Init(env, exports);
  OggStreamState::Init(env, exports);
  OggPage::Init(env, exports);
  OggPacket::Init(env, exports);

  exports.Set(Napi::String::New(env, "ogg_sync_write"),
              Napi::Function::New(env, node_ogg_sync_write));
  exports.Set(Napi::String::New(env, "ogg_sync_pageout"),
              Napi::Function::New(env, node_ogg_sync_pageout));

  exports.Set(Napi::String::New(env, "ogg_stream_pagein"),
              Napi::Function::New(env, node_ogg_stream_pagein));
  exports.Set(Napi::String::New(env, "ogg_stream_packetout"),
              Napi::Function::New(env, node_ogg_stream_packetout));
  exports.Set(Napi::String::New(env, "ogg_stream_packetin"),
              Napi::Function::New(env, node_ogg_stream_packetin));
  exports.Set(Napi::String::New(env, "ogg_stream_pageout"),
              Napi::Function::New(env, node_ogg_stream_pageout));
  exports.Set(Napi::String::New(env, "ogg_stream_flush"),
              Napi::Function::New(env, node_ogg_stream_flush));

  return exports;
}
NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)

#include <libcryxml.h>
#include <nan.h>
#include <sstream>
#include <string>
#include <map>

using namespace Nan;
using namespace v8;

Local<String> operator "" _n(const char *input, size_t) {
  return Nan::New(input).ToLocalChecked();
}

NAN_METHOD(decodeFileWrap) {
  Local<Context> context = Nan::GetCurrentContext();
  Isolate *isolate = context->GetIsolate();

  if (info.Length() != 2) {
      Nan::ThrowError("Expected two paths (source, destination)");
      return;
  }

  String::Utf8Value sourceV8(isolate, info[0]->ToString(context).ToLocalChecked());
  String::Utf8Value destinationV8(isolate, info[1]->ToString(context).ToLocalChecked());

  int res = cryxml_decode_file(*sourceV8, *destinationV8);

  if (res != 0) {
    Local<Object> err = Nan::Error(cryxml_error_to_string(res)).As<Object>();
    err->Set(context, "errorCode"_n, Nan::New(res));
    Nan::ThrowError(err.As<Value>());
  }
}

#include <iostream>

NAN_METHOD(decodeBufferWrap) {
  Local<Context> context = Nan::GetCurrentContext();
  Isolate *isolate = context->GetIsolate();

  if (info.Length() != 1) {
      Nan::ThrowError("Expected one parameter (buffer)");
      std::cout << "got parameters " << info.Length() << std::endl;
      return;
  }

  Local<Uint8ClampedArray> bufferU8 = Local<Uint8ClampedArray>::Cast(info[0]);

  std::cout << "buffer length " << bufferU8->ByteLength() << std::endl;

  char *decoded;

  char *bufferC = reinterpret_cast<char*>(bufferU8->Buffer()->GetContents().Data());

  if (bufferC == nullptr) {
    return Nan::ThrowError("Invalid buffer");
  }

  int res = cryxml_decode_buffer(bufferC, bufferU8->ByteLength(), &decoded);

  if (res != 0) {
    Local<Object> err = Nan::Error(cryxml_error_to_string(res)).As<Object>();
    err->Set(context, "errorCode"_n, Nan::New(res));
    Nan::ThrowError(err.As<Value>());
  }
}

NAN_MODULE_INIT(Init) {
  Nan::Set(target, "decodeFile"_n,
    GetFunction(New<FunctionTemplate>(decodeFileWrap)).ToLocalChecked());
  Nan::Set(target, "decodeBuffer"_n,
    GetFunction(New<FunctionTemplate>(decodeBufferWrap)).ToLocalChecked());
}

NODE_MODULE(cryxml, Init)

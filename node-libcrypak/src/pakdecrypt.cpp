#include <libpakdecrypt.h>
#include "../extern/headers/scopeguard.h"
#include <nan.h>
#include <sstream>

using namespace Nan;
using namespace v8;

Local<String> operator "" _n(const char *input, size_t) {
  return Nan::New(input).ToLocalChecked();
}

NAN_METHOD(decryptWrap) {
  Local<Context> context = Nan::GetCurrentContext();
  Isolate *isolate = context->GetIsolate();

  if (info.Length() != 3) {
      Nan::ThrowError("Expected three paths (source, destination, key)");
      return;
  }

  String::Utf8Value sourceV8(isolate, info[0]->ToString(context).ToLocalChecked());
  String::Utf8Value destinationV8(isolate, info[1]->ToString(context).ToLocalChecked());
  String::Utf8Value keyV8(isolate, info[2]->ToString(context).ToLocalChecked());

  int res = pak_decrypt(*sourceV8, *destinationV8, *keyV8);

  if (res != 0) {
    Local<Object> err = Nan::Error(pak_error_to_string(res)).As<Object>();
    err->Set(context, "errorCode"_n, Nan::New(res));
    Nan::ThrowError(err.As<Value>());
  }
}

NAN_METHOD(decryptFilesWrap) {
  Local<Context> context = Nan::GetCurrentContext();
  Isolate *isolate = context->GetIsolate();

  if (info.Length() != 3) {
      Nan::ThrowError("Expected 3 parameters (source, key, fileNames)");
      return;
  }

  String::Utf8Value sourceV8(isolate, info[0]->ToString(context).ToLocalChecked());
  String::Utf8Value keyV8(isolate, info[1]->ToString(context).ToLocalChecked());
  Local<Array> files = Local<Array>::Cast(info[2]);

  std::vector<const char*> fileNames;

  ON_BLOCK_EXIT([&]() {
    for (const char *buf : fileNames) {
      delete[] buf;
    }
    });

  for (uint32_t i = 0; i < files->Length(); ++i) {
    Local<String> fileName = files->Get(context, i).ToLocalChecked()->ToString(context).ToLocalChecked();
    int len = fileName->Utf8Length(isolate);
    char *buffer = new char[len + 1];
    fileName->WriteUtf8(isolate, buffer);
    buffer[len] = '\0';

    fileNames.push_back(buffer);
  }

  char **buffers = nullptr;
  int *bufferSizes = nullptr;

  int res = pak_decrypt_files(*sourceV8, *keyV8, &fileNames[0], fileNames.size(), &buffers, &bufferSizes);

  ON_BLOCK_EXIT([&]() {
    pak_free_array((void**)buffers, fileNames.size());
    pak_free(bufferSizes);
    });

  if (res != 0) {
    Local<Object> err = Nan::Error(pak_error_to_string(res)).As<Object>();
    err->Set(context, "errorCode"_n, Nan::New(res));
    Nan::ThrowError(err.As<Value>());
    return;
  }

  Local<Array> result = Nan::New<Array>();
  for (uint32_t i = 0; i < files->Length(); ++i) {
    auto buffer = v8::ArrayBuffer::New(isolate, bufferSizes[i]);
    memcpy(buffer->GetContents().Data(), buffers[i], bufferSizes[i]);
    result->Set(context, i, v8::Uint8ClampedArray::New(buffer, 0, buffer->ByteLength()));
  }

  info.GetReturnValue().Set(result);
}

NAN_METHOD(listFilesWrap) {
  Local<Context> context = Nan::GetCurrentContext();
  Isolate *isolate = context->GetIsolate();

  if (info.Length() != 2) {
      Nan::ThrowError("Expected two paths (source, key)");
      return;
  }

  String::Utf8Value sourceV8(isolate, info[0]->ToString(context).ToLocalChecked());
  String::Utf8Value keyV8(isolate, info[1]->ToString(context).ToLocalChecked());

  char *fileNames;
  int res = pak_list_files(*sourceV8, *keyV8, &fileNames);

  if (res != 0) {
    Local<Object> err = Nan::Error(pak_error_to_string(res)).As<Object>();
    err->Set(context, "errorCode"_n, Nan::New(res));
    Nan::ThrowError(err.As<Value>());
  }

  Local<Array> result = Nan::New<Array>();
  int i = 0;
  const char *iter = fileNames;
  while (iter[0] != '\0') {
    result->Set(context, i++, Nan::New<String>(iter).ToLocalChecked());
    iter += strlen(iter) + 1;
  }

  info.GetReturnValue().Set(result);
}

NAN_MODULE_INIT(Init) {
  Nan::Set(target, "decrypt"_n, GetFunction(New<FunctionTemplate>(decryptWrap)).ToLocalChecked());
  Nan::Set(target, "decryptFiles"_n, GetFunction(New<FunctionTemplate>(decryptFilesWrap)).ToLocalChecked());
  Nan::Set(target, "listFiles"_n, GetFunction(New<FunctionTemplate>(listFilesWrap)).ToLocalChecked());
}

NODE_MODULE(pakdecrypt, Init)


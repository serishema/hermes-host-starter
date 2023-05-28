#pragma once
#include <jsi/jsi.h>
#include <iostream>
#include <map>

class NativeObject : public facebook::jsi::HostObject {
            protected:
              std::map<std::string,facebook::jsi::Value> props;
              static facebook::jsi::Value print(facebook::jsi::Runtime& rt,const facebook::jsi::Value& thisVal,const facebook::jsi::Value* args,size_t count);
               public:
               NativeObject(facebook::jsi::Runtime &rt);

               static facebook::jsi::Object makeNativeObject(facebook::jsi::Runtime& rt);

               virtual ~NativeObject();

               virtual facebook::jsi::Value get(facebook::jsi::Runtime& rt, const facebook::jsi::PropNameID& name);
               virtual void set(facebook::jsi::Runtime& rt,const facebook::jsi::PropNameID& name,const facebook::jsi::Value& value);
               virtual std::vector<facebook::jsi::PropNameID> getPropertyNames(facebook::jsi::Runtime& rt);
};
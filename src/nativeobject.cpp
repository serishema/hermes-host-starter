#include "nativeobject.hpp"

#include <stdexcept>
#include <map>
#include <utility>
#include <iostream>

using namespace facebook::jsi;

NativeObject::NativeObject(Runtime &rt) :
  props()
{
    props["print"] = Function::createFromHostFunction(rt,PropNameID::forAscii(rt, "print"), 2, print);
    props["a"] = Value(3);
    props["b"] = Value(5);
}

facebook::jsi::Object NativeObject::makeNativeObject(facebook::jsi::Runtime& rt) {
    auto obj = std::make_shared<NativeObject>(rt);
    auto rval = rt.global().createFromHostObject(rt,obj);
    
    return rval;
}

Value NativeObject::get(Runtime& rt, const PropNameID& name)
{
    try
    {
        return Value(rt, props.at(name.utf8(rt)));
    }
    catch (std::out_of_range e)
    {
        return facebook::jsi::Value::undefined();
    }
}
std::vector<PropNameID> NativeObject::getPropertyNames(Runtime& rt)
{
    std::vector<PropNameID> rval;
    for (auto& [key, value] : props)
    {
        rval.push_back(PropNameID::forUtf8(rt, key));
    }
    return rval;
}

void NativeObject::set(Runtime& rt, const PropNameID& name, const Value& value)
{
    props.insert_or_assign(name.utf8(rt), &value);
}

NativeObject::~NativeObject() {

}

facebook::jsi::Value NativeObject::print(facebook::jsi::Runtime& rt,const facebook::jsi::Value& thisVal,const facebook::jsi::Value* args,size_t count) {
     if (count == 0) {
         std::cout << std::endl;
         std::cout.flush();
     }

     std::cout << args[0].toString(rt).utf8(rt) << std::endl;
     std::cout.flush();

     return facebook::jsi::Value::undefined();
}
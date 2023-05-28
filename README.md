# Hermes host starter

This is a simple demo project to show how to host the hermes javascript 
runtime in a C++ application. Plain C is not supported as the API is C++

## Building
These instructions assume a 64 bit platform. I do not know if a 32 bit build will work.
### Linux amd64 or aarch64 
these instructions are for either **Debian 11.7.0** or the **current** chromebook Linux environment, you may need to update your linux environment first. 
Ubuntu may work, unless you are cross compiling for ARM64 (see below)

I assume you have set up sudo (Debian), so if you have not done so do that first.

(note: if you have an x64 chromebook i'd apprechiate feedback as to whether this works. Mine is ARM64)

    sudo apt-get install cmake gcc g++ libicu-dev ninja-build
    cd ~/code (or wherever you want to put it)
    git clone --recurse-submodules https://github.com/serishema/hermes-host-starter
    cd hermes-host-starter
    cd third_party/hermes
    cmake -DCMAKE_INSTALL_PREFIX=../../lib/hermes -DHERMES_ENABLE_TEST_SUITE=false  -S . -B build -G Ninja
for debian

    ninja -C build 
    ninja -C build install (do not skip this step)
and for chromebook (with the default options ld will run out of memory and crash)
    
    ninja -j 2 -C build 
    ninja -C build install
This will build hermes from source code as the hermes project does not distribute precompiled binaries. 

On a Ryzen 5 5600 this takes around 5 minutes. 
On ARM64 (mediatek) it takes nearly 2 hours. 

If you have access to a PC or cloud service such as GCP or codespaces you can cross compile (see below). If your chromebook has 8gb of ram or more 
you can do -j 4 but on 4gb models the compiler will run out of memory and crash.
you only need to do the above once. 

now return to the root of the project and type 

    cmake -S . -B build -G Ninja

then run ninja again

    ninja -C build
or (chromebook)

    ninja -j 2 -C build 

type 
    ninja -C build install
    ./build/hermes-host-starter ./js/index.js 

to run the program

### Windows (x64)
Make sure you have the following installed and in your path (where applicable)

you must have at least Windows 10 to build this.

* Visual Studio 2022 Community
* git
* cmake 
* python
* ninja

git, python and cmake must be in your path

if you are installing VS, the "Desktop application development with C/C++" workload will install most of the stuff you need. If you install cmake from VS it installs ninja as well
open the Visual Studio x64 Native tools command prompt (not the standard terminal)
and type 

    cd D:\code (or wherever you want to put it)
    git clone --recurse-submodules https://github.com/serishema/hermes-host-starter
    cd hermes-host-starter
    cd third_party\hermes
    cmake -DCMAKE_INSTALL_PREFIX=..\..\lib\hermes -DHERMES_ENABLE_TEST_SUITE=false -DHERMESVM_ALLOW_INLINE_ASM=false -DHERMES_BUILD_SHARED_JSI=true  -S . -B build
    ninja -C build

then return to the top level project directory
decide wether you want to use Visual Studio or not
if so type
    cmake -S . -B build -G "Visual Studio 17 2022"

or if not 
type 
    cmake -S . -B build -G Ninja

if using ninja type

    ninja -C build 
    ninja -C build install 

if using visual studio be sure to build the "install" project after building the application
these steps copy a needed DLL file into the build directory and the program will not run without it.

type
    
    build\hermes-host-starter.exe js\index.js 

to run the project

### Cross Compiling for aarch64 
see [doc/arm64-cross.md](doc/arm64-cross-linux.md)

### A note about static linking on windows
This doesn't currently work, be sure to build hermes as a DLL (the default option)

### A note about ARM64 Windows 11
I do not have access to this hardware, but would like to support it. In theory the Windows build
should work as normal if you open the ARM64 native tools command prompt.

It is also possible to cross compile for ARM64 windows on x64 windows. Open an issue if you would like to know how to do this.

### Android
I'm working on it

### iOS
I don't have a mac, sorry. PRs adding iOS support are welcome

## How this works
the basic interface between javascript and native code in hermes is facebook::jsi::HostObject

here's the relevant the relevant bit of hermes include files (this can be found in lib/hermes/include/jsi/jsi.h in your project directory if you have followed the build instructions) 

```c++
/// A function which has this type can be registered as a function
/// callable from JavaScript using Function::createFromHostFunction().
/// When the function is called, args will point to the arguments, and
/// count will indicate how many arguments are passed.  The function
/// can return a Value to the caller, or throw an exception.  If a C++
/// exception is thrown, a JS Error will be created and thrown into
/// JS; if the C++ exception extends std::exception, the Error's
/// message will be whatever what() returns. Note that it is undefined whether
/// HostFunctions may or may not be called in strict mode; that is `thisVal`
/// can be any value - it will not necessarily be coerced to an object or
/// or set to the global object.
using HostFunctionType = std::function<
    Value(Runtime& rt, const Value& thisVal, const Value* args, size_t count)>;

/// An object which implements this interface can be registered as an
/// Object with the JS runtime.
class JSI_EXPORT HostObject {
 public:
  // The C++ object's dtor will be called when the GC finalizes this
  // object.  (This may be as late as when the Runtime is shut down.)
  // You have no control over which thread it is called on.  This will
  // be called from inside the GC, so it is unsafe to do any VM
  // operations which require a Runtime&.  Derived classes' dtors
  // should also avoid doing anything expensive.  Calling the dtor on
  // a jsi object is explicitly ok.  If you want to do JS operations,
  // or any nontrivial work, you should add it to a work queue, and
  // manage it externally.
  virtual ~HostObject();

  // When JS wants a property with a given name from the HostObject,
  // it will call this method.  If it throws an exception, the call
  // will throw a JS \c Error object. By default this returns undefined.
  // \return the value for the property.
  virtual Value get(Runtime&, const PropNameID& name);

  // When JS wants to set a property with a given name on the HostObject,
  // it will call this method. If it throws an exception, the call will
  // throw a JS \c Error object. By default this throws a type error exception
  // mimicking the behavior of a frozen object in strict mode.
  virtual void set(Runtime&, const PropNameID& name, const Value& value);

  // When JS wants a list of property names for the HostObject, it will
  // call this method. If it throws an exception, the call will throw a
  // JS \c Error object. The default implementation returns empty vector.
  virtual std::vector<PropNameID> getPropertyNames(Runtime& rt);
};

/// Native state (and destructor) that can be attached to any JS object
/// using setNativeState.
class JSI_EXPORT NativeState {
 public:
  virtual ~NativeState();
};
```
If you implement this interface, you can pass the object to javascript

it should be noted that the only javascript APIs hermes provides itself are
* Promises and async functions (this is a compile time option IIRC)
* the members of the basic types eg if n is a number n.toString() will work

That is it. 
Every API you need in javascript you will need to either program
yourself or link an appropriate open source library with a suitable license for your project.

`require()` is not provided, i'd suggest using a packer to combine your javascript into a single 
minified file.

I hope this is enough to get you started :-)







#include <hermes/hermes.h>
#include <jsi/jsi.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <stdexcept>
#include <memory>
#include "nativeobject.hpp"

using namespace facebook::hermes;

void printUsage() {
#ifdef WIN32
    std::cout << "usage: hermes-host-starter.exe <jsfile>" << std::endl;
#else 
    std::cout << "usage: hermes-host-starter <jsfile>" << std::endl;
#endif 
}

int main(int argc, char **argv) {
    auto rt = makeHermesRuntime();
    if (argc != 2) { printUsage(); return -1;}
    try {
           rt->global().setProperty(*rt,"NativeObject",NativeObject::makeNativeObject(*rt));    
           auto f = std::ifstream(argv[1],std::ios::binary);
           f.seekg(0,std::ios_base::end);
           size_t scriptSize = f.tellg();
           f.seekg(0, std::ios_base::beg);
           char *script = new char[scriptSize+1]; // null terminator
           f.read(script,scriptSize);
           script[scriptSize] = 0; // null terminator
           if (!f.good()) throw std::runtime_error("failed to read the script from disk");
           auto scriptBuf = std::make_unique<facebook::jsi::StringBuffer>(std::string(script));
           auto compiledScript = rt->prepareJavaScript(std::move(scriptBuf),argv[1]); // in real application you would set this to the url you loaded the script from
           auto returnValue = rt->evaluatePreparedJavaScript(compiledScript);
           std::cout << "Script finished. Return Value was: " << returnValue.toString(*rt).utf8(*rt) << std::endl;
           std::cout.flush();
    } catch(std::runtime_error e) {
        std::cout << e.what() << std::endl;
        std::cout.flush(); // C++ does not have finally
        return -1;
    } catch(facebook::jsi::JSError e) {
        std::cout << "javascript error " << std::endl;
        std::cout << e.getMessage() << std::endl;
        std::cout << e.getStack() << std::endl;
        std::cout.flush(); // C++ does not have finally
        return -1;
    } 

    return 0;
}
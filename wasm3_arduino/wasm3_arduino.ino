
#define d_m3MaxFunctionStackHeight 128

#include <wasm3.h>

#include <m3_api_defs.h>
#include <m3_env.h>

#include "app_wasm.h"


#define FATAL(func, msg) { Serial.print("Fatal: " func " "); Serial.println(msg); return; }



#define LED_PIN             13

#define WASM_STACK_SLOTS    2048
#define NATIVE_STACK_SIZE   (32*1024)

#define WASM_MEMORY_LIMIT   4096


 m3ApiRawFunction(m3_debug)
{
    m3ApiReturnType(uint32_t)
    m3ApiGetArgMem(const uint8_t *, str_buf)
    m3ApiGetArg(uint32_t, str_len)
    Serial.printf("Rune::DEBUG %s\r\n", str_buf);
}


M3Result  LinkArduino  (IM3Runtime runtime)
{
    IM3Module module = runtime->modules;
    m3_LinkRawFunction(module, "env", "_debug", "i(ii)", &m3_debug);


    return m3Err_none;
}

/*
 * Engine start, liftoff!
 */

void wasm_task(void*)
{
    M3Result result = m3Err_none;

    IM3Environment env = m3_NewEnvironment ();
    if (!env) FATAL("NewEnvironment", "failed");

    IM3Runtime runtime = m3_NewRuntime (env, WASM_STACK_SLOTS, NULL);
    if (!runtime) FATAL("NewRuntime", "failed");

#ifdef WASM_MEMORY_LIMIT
    runtime->memoryLimit = WASM_MEMORY_LIMIT;
#endif

    IM3Module module;
    result = m3_ParseModule (env, &module, app_wasm, app_wasm_len-1);
    if (result) FATAL("ParseModule", result);

    result = m3_LoadModule (runtime, module);
    if (result) FATAL("LoadModule", result);

    
    Serial.println("Loaded WASM");

   result = LinkArduino (runtime);
   if (result) FATAL("LinkArduino", result);
    IM3Function f;

     IM3Function _call;

    result = m3_FindFunction (&_call, runtime, "_call");
    if (result) FATAL("FindFunction", result);
      const char* x_argv[2] = { "1", "1"};
      result = m3_CallWithArgs(_call, 2, x_argv);

    long ptr = *(uint64_t*)(runtime->stack);
    Serial.print("_call: ");
    Serial.println(ptr);

    if (result) {
        M3ErrorInfo info;
        m3_GetErrorInfo (runtime, &info);
        Serial.print("Error: ");
        Serial.print(result);
        Serial.print(" (");
        Serial.print(info.message);
        Serial.println(")");
    }
}

void setup()
{
    Serial.begin(115200);
    delay(100);

    // Wait for serial port to connect
    // Needed for native USB port only
    while(!Serial) {}

    Serial.println("\nWasm3 Demo " __DATE__ " " __TIME__ " " );

#ifdef ESP32
    // On ESP32, we can launch in a separate thread
    xTaskCreate(&wasm_task, "runic", NATIVE_STACK_SIZE, NULL, 5, NULL);
#else
    wasm_task(NULL);
#endif
}

void loop()
{
    delay(100);
}


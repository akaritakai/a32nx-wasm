# FBW_A32NX_WASM_SYSTEM
Core systems implementation for flybywire_A32NX as wasm module.

## Steps to add new system
- Create all your system implementation(as classes) in its own header file with a master class so that you don't flood `wasm_sys.cpp`
  - Expose only init(), update() and updateSimVars().
  - Create a class instance under `wasm_sys.cpp` and init, update and updateSimVars under proper panel_service_case(wasm_gauge callback handle); 
    this will become obsolete when porting to a wasm_module, build accordingly.
    - Current implementation uses wasm module run absTime to calulate _deltaTime instead of directly using deltaTime from pData structure, this is done keeping the above
      point under consideration, since _deltaTime is bound to gauge refresh rate.(Note: currently the difference between currentAbsTime and lastAbsTime will be equal to            REFRESH_RATE defined under common_sys.h)
    
- `#include "common_sys.h"` in your header file. Use and update enum definition and PCSTRINGZ definition in common_sys.h as required(do not delete/change order of any existing ones)
  - In order to add new ones, first add its enum to the enum definitions and follow suit with its string at the same index as enum definition
  - Ensure to provide a comment of local var name to be used while defining its enum(yes, one may scroll down to PCSTRINGZ array and lookup,
    but this makes things easier for others to work with.)
    
- Thread support is currently unavailable so expect sync delay issues with update function(worst case should be no more than one update cycle desync).
- Once thread support is available all the functions can then run async.
    

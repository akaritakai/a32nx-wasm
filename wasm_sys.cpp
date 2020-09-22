#include "systems/elec_sys.h"
#include "systems/packs_sys.h"
#include "systems/bleed_sys.h"
#include "systems/press_sys.h"
#include "systems/eng_sys.h"

#include <SimConnect.h>
#include <ratio>
#include <chrono>
#include <sys/time.h>

class WasmSys {
    private:
        ElecSys ELEC_SYSTEM;
        PacksSys PACK_SYSTEM;
        BleedSys BLEED_SYSTEM;
        PressSys PRESS_SYSTEM;
        EngSys ENG_SYSTEM;
    public:
        void init() {
            initUnitEnums();
            initLocalSimVarsIDs();
            srand(time(nullptr));
            ELEC_SYSTEM.init();
            PACK_SYSTEM.init();
            BLEED_SYSTEM.init();
            PRESS_SYSTEM.init();
            ENG_SYSTEM.init();
        }
        void update(double const currentAbsTime) {
            updateASimVars();

            ELEC_SYSTEM.update(currentAbsTime);
            PACK_SYSTEM.update();
            BLEED_SYSTEM.update();
            PRESS_SYSTEM.update();
            ENG_SYSTEM.update(currentAbsTime);

            ELEC_SYSTEM.updateSimVars();
            PACK_SYSTEM.updateSimVars();
            BLEED_SYSTEM.updateSimVars();
            PRESS_SYSTEM.updateSimVars();
            ENG_SYSTEM.updateSimVars();

            lastAbsTime = currentAbsTime;
        }
        void destroy() {
            unregister_all_named_vars();
            free(ENUM_UNITS);
            free(ID_LSIMVAR);
        }
}WASM_SYS;

class ServiceDef {
private:
    HANDLE hSimConnect = 0;
    const char* stopState = "SimStop";
    SIMCONNECT_DATA_REQUEST_ID  RequestID;
public:
    bool handleSimConnect(FsContext ctx, int service_id, void* pData) {
        switch (service_id)
        {
        case PANEL_SERVICE_PRE_INSTALL: {
            HRESULT hr = SimConnect_Open(&hSimConnect, "A32NX_wasm_sys", nullptr, 0, 0, 0);
            if (SUCCEEDED(hr)) {
                return true;
            }
            return false;
        }
        default:
            return true;
        }
    }
    bool simStopCheck() {
        HRESULT kill = SimConnect_RequestSystemState(hSimConnect, RequestID, stopState);
        if (SUCCEEDED(kill)) {
            return kill;
        }
        return 0;
    }
    bool handleSimDisconnect() {
        WASM_SYS.destroy();
        HRESULT hr = SimConnect_Close(hSimConnect);
        if (SUCCEEDED(hr)) {
            return true;
        }
        return false;
    }
}service;

// Callbacks
extern "C" {

    MSFS_CALLBACK bool wasm_sys_callback(FsContext ctx, int service_id, void* pData)
    {
        if(service.handleSimConnect(ctx, service_id, pData)){
            
            bool initialized = 0;
            bool kill = 0;
            struct timeval time;
            double lastRefresh = 0;

            while (!(kill)) {
                gettimeofday(&time, 0);
                double currentAbsTime = time.tv_usec * 1000;
                
                if (!(initialized)) {
                    WASM_SYS.init();
                    initialized = 1;
                } else {
                    if (lastAbsTime == 0) {
                        lastAbsTime = currentAbsTime;
                    }
                    lastRefresh = currentAbsTime - lastAbsTime;
                    if (lastRefresh >= REFRESH_RATE) {
                        WASM_SYS.update(currentAbsTime);
                    }
                }
                kill = service.simStopCheck();
                if (kill) {
                    service.handleSimDisconnect();
                }
            }
        }
        return true;
    }
}

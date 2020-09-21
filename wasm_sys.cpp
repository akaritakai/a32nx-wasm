#include "systems/elec_sys.h"
#include "systems/packs_sys.h"
#include "systems/bleed_sys.h"
#include "systems/press_sys.h"
#include "systems/eng_sys.h"
#include <SimConnect.h>
#include <ratio>
#include <chrono>
#include <sys/time.h>

HANDLE hSimConnect = 0;
class ServiceDef {
private:
public:
    bool handleSimConnect(FsContext ctx, int service_id, void* pData) {
        switch (service_id)
        {
        case PANEL_SERVICE_PRE_INSTALL: {
            HRESULT hr = SimConnect_Open(&hSimConnect, "A32NX_wasm_sys", nullptr, 0, 0, 0);
            if (hr) {
                return true;
            }
            return false;
        }
        default:
            return true;
        }
    }

    bool handleSimDisconnect(FsContext ctx, int service_id, void* pData) {
        switch (service_id)
        {
        case PANEL_SERVICE_PRE_KILL: {
            unregister_all_named_vars();
            free(ENUM_UNITS);
            free(ID_LSIMVAR);

            HRESULT hr = SimConnect_Close(hSimConnect);
            if (hr) {
                return true;
            }
            return false;
        }
        default:
            return true;
        }
    }

    bool handlePostInstallInitSimVarEnumsIDs(FsContext ctx, int service_id, void* pData) {
        switch (service_id)
        {
        case PANEL_SERVICE_POST_INSTALL: {
            return true;
        }
        default:
            return true;
        }
    }

    bool handlePostInitSimVar(FsContext ctx, int service_id, void* pData) {
        switch (service_id)
        {
        case PANEL_SERVICE_POST_INITIALIZE: {
            return true;
        }
        default:
            return true;
        }
    }

    bool handleUpdateSimVar(FsContext ctx, int service_id, void* pData, const double currentAbsTime) {
        switch (service_id)
        {
        case PANEL_SERVICE_PRE_UPDATE: {
            return true;
        }
        default:
            return true;
        }
    }

    bool handleUpdatepDataVar(FsContext ctx, int service_id, void* pData) {
        switch (service_id)
        {
        case PANEL_SERVICE_PRE_DRAW: {
            return true;
        }
        default:
            return true;
        }
    }
}service;

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
            srand(time(NULL));
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
            ENG_SYSTEM.update();

            ELEC_SYSTEM.updateSimVars();
            PACK_SYSTEM.updateSimVars();
            BLEED_SYSTEM.updateSimVars();
            PRESS_SYSTEM.updateSimVars();
            ENG_SYSTEM.updateSimVars();

            lastAbsTime = currentAbsTime;
        }
}WASM_SYS;

// Callbacks
extern "C" {

    MSFS_CALLBACK bool wasm_sys_callback(FsContext ctx, int service_id, void* pData)
    {
        if(service.handleSimConnect(ctx, service_id, pData)){
            
            bool initialized = 0;
            bool kill = 0;
            struct timeval time;
            double refreshRate = 500;                               //refresh rate in milliseconds
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
                    if (lastRefresh >= refreshRate) {
                        WASM_SYS.update(currentAbsTime);
                    }
                }
                //TODO:: implement KILL trigger
            }
            /*
                service.handlePostInstallInitSimVarEnumsIDs(ctx, service_id, pData);
                service.handlePostInitSimVar(ctx, service_id, pData);
                service.handleUpdateSimVar(ctx, service_id, pData, currAbsTime);
                service.handleUpdatepDataVar(ctx, service_id, pData);
                service.handleSimDisconnect(ctx, service_id, pData);
            */
        }
        return 0;
    }
}

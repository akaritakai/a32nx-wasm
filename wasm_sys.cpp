#include "elec_sys.h"
#include "packs_sys.h"
#include "bleed_sys.h"
#include "press_sys.h"
#include "eng_sys.h"

#include <SimConnect.h>

ElecSys ELEC_SYSTEM;
PacksSys PACK_SYSTEM;
BleedSys BLEED_SYSTEM;
PressSys PRESS_SYSTEM;
EngSys ENG_SYSTEM;


HANDLE hSimConnect = NULL;
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
            initUnitEnums();
            initLocalSimVarsIDs();
            srand(time(NULL));
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
            ELEC_SYSTEM.init();
            PACK_SYSTEM.init();
            BLEED_SYSTEM.init();
            PRESS_SYSTEM.init();
            ENG_SYSTEM.init();
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
            sGaugeDrawData* pData_raw = static_cast<sGaugeDrawData*>(pData);
            currAbsTime = pData_raw->t;
            if (lastAbsTime == 0) {
                lastAbsTime = currAbsTime;
            }
            return true;
        }
        default:
            return true;
        }
    }
}service;


// Callbacks
extern "C" {

    MSFS_CALLBACK bool wasm_sys_callback(FsContext ctx, int service_id, void* pData)
    {
        if(service.handleSimConnect(ctx, service_id, pData)){
            service.handlePostInstallInitSimVarEnumsIDs(ctx, service_id, pData);
            service.handlePostInitSimVar(ctx, service_id, pData);
            service.handleUpdateSimVar(ctx, service_id, pData, currAbsTime);
            service.handleUpdatepDataVar(ctx, service_id, pData);
            service.handleSimDisconnect(ctx, service_id, pData);
        }
    }
}

#include "elec_sys.h"
#include <SimConnect.h>

elecSys ELEC_SYSTEM;

HANDLE hSimConnect = NULL;
class ServiceDef {
private:
public:
    bool handleSimConnect(FsContext ctx, int service_id, void* pData) {
        switch (service_id)
        {
        case PANEL_SERVICE_PRE_INSTALL: {
            HRESULT hr = SimConnect_Open(&hSimConnect, "A32NX_elec_sys", nullptr, 0, 0, 0);
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
            ELEC_SYSTEM.updateSimVars();
            lastAbsTime = currentAbsTime;
        }
        default:
            break;
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
        }
        default:
            break;
        }
    }
}service;


// Callbacks
extern "C" {

    MSFS_CALLBACK bool elec_sys_callback(FsContext ctx, int service_id, void* pData)
    {
        service.handleSimConnect(ctx, service_id, pData);
        service.handlePostInstallInitSimVarEnumsIDs(ctx, service_id, pData);
        service.handlePostInitSimVar(ctx, service_id, pData);
        //lasAbsTime is unavailable for the first draw call, start updating simVars only after first draw call to avoid discrepancy
        if(lastAbsTime){
            service.handleUpdateSimVar(ctx, service_id, pData, currAbsTime);
        }
        service.handleUpdatepDataVar(ctx, service_id, pData);
        service.handleSimDisconnect(ctx, service_id, pData);

    }

}

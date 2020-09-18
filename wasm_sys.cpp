#include "elec_sys.h"
#include <SimConnect.h>

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

            updateLocalSimVars();
            return true;
        }
        default:
            return true;
        }
    }

    bool handleUpdateSimVar(FsContext ctx, int service_id, void* pData) {
        switch (service_id)
        {
        case PANEL_SERVICE_PRE_UPDATE: {

        }
        default:
            break;
        }
    }

    bool handleUpdatepDataVar(FsContext ctx, int service_id, void* pData) {
        switch (service_id)
        {
        case PANEL_SERVICE_PRE_DRAW: {

        }
        default:
            break;
        }
    }
};

// Callbacks
extern "C" {

    ServiceDef Service;
    MSFS_CALLBACK bool elec_sys_callback(FsContext ctx, int service_id, void* pData)
    {
        
    }

}

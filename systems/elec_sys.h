#pragma once
#include "../common_sys.h"

/*
* =============== *
* CLASSES DEFINED *
* =============== *
* ELEC MASTER CLASS
* Batteries
* EXTPWR
* APU => includes start sequence
* Engine
* RAT => includes start sequence
* Buses
* Convertors
* 
*/


/*
* ========= *
* BATTERIES *
* ========= *
*/
class Batteries {
private:
    bool apu_starterOn;
    double lastAbsTime;
    const double apu_VoltageDrop = 3 + (rand() % 20) * 0.1;
    const double bcl_charging_current = 150; //150A
    const double bcl_charging_voltage = 28.5 + rand() % 5 / 10;;
    double batt1_diff_voltage;
    double batt2_diff_voltage;
    void updateBattState() {
        if (aSimVarsValue[BATT1_SW] && lSimVarsValue[BATT1_CAPACITY] >= 5) {
            lSimVarsValue[BATT1_ONLINE] = 1;
        }
        else {
            lSimVarsValue[BATT1_ONLINE] = 0;
        }
        if (aSimVarsValue[BATT2_SW] && lSimVarsValue[BATT2_CAPACITY] >= 5) {
            lSimVarsValue[BATT2_ONLINE] = 1;
        }
        else {
            lSimVarsValue[BATT2_ONLINE] = 0;
        }
    }
    void APUStart() {
        apu_starterOn = 1;
        if (lSimVarsValue[BATT1_ONLINE]) {
            lSimVarsValue[BATT1_VOLTAGE] -= apu_VoltageDrop;
        }
        if (lSimVarsValue[BATT2_ONLINE]) {
            lSimVarsValue[BATT2_VOLTAGE] -= apu_VoltageDrop;
        }
    }
    void BATCharging(const double currentAbsTime) {
        if (lSimVarsValue[BATT1_CAPACITY] < 23) {
            batt1_diff_voltage = bcl_charging_voltage - lSimVarsValue[BATT1_VOLTAGE];
            lSimVarsValue[BATT1_CAPACITY] += (bcl_charging_current / 3600) * ((currentAbsTime - lastAbsTime) * 0.0001);
            lSimVarsValue[BATT1_VOLTAGE] += ((23 - lSimVarsValue[BATT1_CAPACITY]) / 23) * batt1_diff_voltage;

        }
        if (lSimVarsValue[BATT2_CAPACITY] < 23) {
            batt2_diff_voltage = (bcl_charging_voltage - lSimVarsValue[BATT2_VOLTAGE]) / 10;
            lSimVarsValue[BATT2_CAPACITY] += (bcl_charging_current / 3600) * ((currentAbsTime - lastAbsTime) * 0.001);
            lSimVarsValue[BATT2_VOLTAGE] += ((23 - lSimVarsValue[BATT1_CAPACITY]) / 23) * batt1_diff_voltage;
        }
    }
    void BATDischarging(const double currentAbsTime) {
        float batt1_load = 0;
        float batt2_load = 0;
        if (lSimVarsValue[BATT1_ONLINE] && lSimVarsValue[BATT2_ONLINE]) {
            batt1_load = lSimVarsValue[BATT_BUS_LOAD] / 2;
            batt2_load = lSimVarsValue[BATT_BUS_LOAD] / 2;
            lSimVarsValue[BATT1_ONLINE] = 1;
            lSimVarsValue[BATT2_ONLINE] = 1;
        }
        else if (lSimVarsValue[BATT1_ONLINE]) {
            batt1_load = lSimVarsValue[BATT_BUS_LOAD];
        }
        else if (lSimVarsValue[BATT2_ONLINE]) {
            batt2_load = lSimVarsValue[BATT_BUS_LOAD];
        }
        if (lSimVarsValue[BATT1_ONLINE]) {
            lSimVarsValue[BATT1_CAPACITY] -= batt1_load * ((currentAbsTime - lastAbsTime) * 0.001) * (1 / 3600);
        }
        if (lSimVarsValue[BATT2_ONLINE]) {
            lSimVarsValue[BATT2_CAPACITY] -= batt2_load * ((currentAbsTime - lastAbsTime) * 0.001) * (1 / 3600);
        }
    }

public:
    void init() {
        apu_starterOn = 0;
        lSimVarsValue[BATT1_ONLINE] = 0;
        lSimVarsValue[BATT2_ONLINE] = 0;
        lSimVarsValue[BATT1_CAPACITY] = 23 * (rand() % 10 + 90) / 100;
        lSimVarsValue[BATT2_CAPACITY] = 23 * (rand() % 10 + 90) / 100;
        lSimVarsValue[BATT1_VOLTAGE] = 25.5 + (rand() % 30) / 10;
        lSimVarsValue[BATT2_VOLTAGE] = 25.5 + (rand() % 30) / 10;
        lSimVarsValue[BATT1_AMPERAGE] = 0;
        lSimVarsValue[BATT2_AMPERAGE] = 0;
    }
    void update(const double currentAbsTime) {

        updateBattState();

        if (lSimVarsValue[APU_START] && apu_starterOn == 0) {
            APUStart();
        }
        if (lSimVarsValue[DC_BUS1] || lSimVarsValue[DC_BUS2]) {
            BATCharging(currentAbsTime);
        }
        else {
            BATDischarging(currentAbsTime);
        }
        updateSimVars();
    }
    void updateSimVars() {
        for (int i = BATT1_ONLINE; i <= BATT2_AMPERAGE; i++) {
            set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
        }
    }
};

/*
* ========== *
* EXTPWR/GPU *
* ========== *
*/
class EXTGen {
public:
    void init() {
        lSimVarsValue[EXT_GEN_VOLTAGE] = 0;
        lSimVarsValue[EXT_GEN_AMPERAGE] = 0;
        lSimVarsValue[EXT_GEN_FREQ] = 0;
    }
    void update(const double currentAbsTime) {
        if (aSimVarsValue[EXT_POWER]) {
            lSimVarsValue[EXT_GEN_ONLINE] = 1;
            lSimVarsValue[EXT_GEN_VOLTAGE] = 114 + rand() % 1;
            lSimVarsValue[EXT_GEN_AMPERAGE] = 145 + rand() % 5;
            lSimVarsValue[EXT_GEN_FREQ] = 398 + rand() % 2;
        }
        else {
            lSimVarsValue[EXT_GEN_ONLINE] = 0;
            lSimVarsValue[EXT_GEN_VOLTAGE] = 0;
            lSimVarsValue[EXT_GEN_AMPERAGE] = 0;
            lSimVarsValue[EXT_GEN_FREQ] = 0;
        }
    }
    void updateSimVars() {
        for (int i = EXT_GEN_VOLTAGE; i <= EXT_GEN_FREQ; i++) {
            set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
        }
    }
};

/*
* ======= *
* APU GEN *
* ======= *
* 
*/
class APUGen {
private:

public:
    void init() {
        for (int i = APU_GEN_ONLINE; i <= APU_LOAD_PERCENT; i++) {
            lSimVarsValue[i] = 0;
        }
    }
    void update(const double currentAbsTime) {
        if (lSimVarsValue[APU_N1] >= 87) {
            lSimVarsValue[APU_GEN_VOLTAGE] = round(114.3 + (rand() % 10 / 10));
            lSimVarsValue[APU_GEN_AMPERAGE] = 150;
            lSimVarsValue[APU_GEN_FREQ] = round(4.46 * lSimVarsValue[APU_N1] - 46 + rand() % 1);
        }
        if (lSimVarsValue[APU_GEN_FREQ] > 390 && lSimVarsValue[APU_GEN_FREQ] < 410) {
            lSimVarsValue[APU_GEN_ONLINE] = 1;
        } else {
            lSimVarsValue[APU_GEN_ONLINE] = 0;
            lSimVarsValue[APU_GEN_VOLTAGE] = 0;
            lSimVarsValue[APU_GEN_AMPERAGE] = 0;
            lSimVarsValue[APU_GEN_FREQ] = 0;
        }

    }
    void updateSimVars() {
        for (int i = APU_GEN_ONLINE; i <= APU_LOAD_PERCENT; i++) {
            set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
        }
    }
};

/*
* =========== *
* ENGINE GENS *
* =========== *
*/

class EngineGen {
private:
    const double stableTime = 0.5;
    double timeElapsedGen1;
    double timeElapsedGen2;
    const double IDGHeatingCoeff = 1.4;
    const double IDGCoolingCoeff = 0.4;

    void updateGen1(const double currentAbsTime, const double ambient) {
        if (aSimVarsValue[ENG1_N2] >= 59.5 && timeElapsedGen1 < stableTime) {
            timeElapsedGen1 += (currentAbsTime - lastAbsTime) * 0.001;
        }
        else if (aSimVarsValue[ENG1_N2] <= 56 && timeElapsedGen1 > 0) {
            timeElapsedGen1 -= (currentAbsTime - lastAbsTime) * 0.001;
        }

        if (timeElapsedGen1 >= stableTime && aSimVarsValue[GEN2_SW] && !(lSimVarsValue[IDG1_DISC_SW]) && !(lSimVarsValue[IDG1_FAULT])) {
            lSimVarsValue[GEN1_ONLINE] = 1;
            lSimVarsValue[GEN1_VOLTAGE] = 115;
            lSimVarsValue[GEN1_AMPERAGE] = 782.60;
            lSimVarsValue[GEN1_FREQ] = 400;
        }
        else {
            lSimVarsValue[GEN1_ONLINE] = 0;
            lSimVarsValue[GEN1_VOLTAGE] = 0;
            lSimVarsValue[GEN1_AMPERAGE] = 0;
            lSimVarsValue[GEN1_FREQ] = 0;
        }

        updateIDGTEMP(ENG1_N2, GEN1_IDG, IDG1_DISC_SW, ambient, currentAbsTime);
    }
    void updateGen2(const double currentAbsTime, const double ambient) {
        if (aSimVarsValue[ENG2_N2] >= 59.5 && timeElapsedGen2 < stableTime) {
            timeElapsedGen2 += (currentAbsTime - lastAbsTime) * 0.001;
        }
        else if (aSimVarsValue[ENG2_N2] <= 56.3 && timeElapsedGen1 > 0) {
            timeElapsedGen2 -= (currentAbsTime - lastAbsTime) * 0.001;
        }
        if (timeElapsedGen1 >= stableTime && aSimVarsValue[GEN2_SW] && !(lSimVarsValue[IDG2_DISC_SW]) && !(lSimVarsValue[IDG2_FAULT])) {
            lSimVarsValue[GEN2_ONLINE] = 1;
            lSimVarsValue[GEN2_VOLTAGE] = 115;
            lSimVarsValue[GEN2_AMPERAGE] = 782.60;
            lSimVarsValue[GEN2_FREQ] = 400;
        }
        else {
            lSimVarsValue[GEN2_ONLINE] = 0;
            lSimVarsValue[GEN2_VOLTAGE] = 0;
            lSimVarsValue[GEN2_AMPERAGE] = 0;
            lSimVarsValue[GEN2_FREQ] = 0;
        }
        updateIDGTEMP(ENG2_N2, GEN2_IDG, IDG2_DISC_SW, ambient, currentAbsTime);
    }
    void updateIDGTEMP(ENUM ENG_N2, ENUM GEN_IDG, ENUM IDG_DISC, const double ambient, const double currentAbsTime) {

        double maxIDG = lSimVarsValue[ENG_N2] / 100 * 1.8;
        if (ambient > 0) {
            maxIDG += ambient;
            if (maxIDG > 115) {
                maxIDG = 115;
            }
        }
        else {
            maxIDG -= ambient;
            if (maxIDG < 70) {
                maxIDG = 70;
            }
        }
        if (lSimVarsValue[IDG_DISC]) {
            maxIDG = ambient;
        }
        if (aSimVarsValue[ENG_N2] > 0 && aSimVarsValue[GEN_IDG] <= maxIDG) {
            lSimVarsValue[GEN_IDG] += IDGHeatingCoeff * (currentAbsTime - lastAbsTime) * 0.001;
        }
        if (aSimVarsValue[GEN_IDG] > maxIDG) {
            lSimVarsValue[GEN_IDG] -= IDGCoolingCoeff * (currentAbsTime - lastAbsTime) * 0.001;
        }
    }
public:
    void init() {
        lSimVarsValue[GEN1_IDG] = aSimVarsValue[AMB_TEMP];
        lSimVarsValue[GEN2_IDG] = aSimVarsValue[AMB_TEMP];
        timeElapsedGen1 = 0;
        timeElapsedGen2 = 0;
        for (int i = GEN1_ONLINE; i <= GEN2_FREQ; i++) {
            lSimVarsValue[i] = 0;
        }
    }
    void update(const double currentAbsTime) {
        const double ambient = aSimVarsValue[AMB_TEMP];
        updateGen1(currentAbsTime, ambient);
        updateGen2(currentAbsTime, ambient);
    }
    void updateSimVars() {
        for (int i = GEN1_ONLINE; i <= GEN2_IDG; i++) {
            set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
        }
    }
};

/*
* ======== *
* EMER RAT *
* ======== *
* ============================TODO: separate RAT start sequence to engine module?======================
*/
class RATGen {
private:
    const int RATdelay = 8; //8sec
    int timeElapsed = 0;
public:
    void init() {
        for (int i = EMER_ONLINE; i <= EMER_FREQ; i++) {
            lSimVarsValue[i] = 0;
        }
    }
    void update(double const currentAbsTime) {
        if (!(lSimVarsValue[GEN1_ONLINE] && lSimVarsValue[GEN2_ONLINE])) {
            if (timeElapsed <= RATdelay) {
                timeElapsed += (currentAbsTime - lastAbsTime) * 0.001;
            }
            if (timeElapsed >= RATdelay && aSimVarsValue[IAS] > 100) {
                lSimVarsValue[EMER_ONLINE] = 1;
                lSimVarsValue[EMER_VOLTAGE] = 115;
                lSimVarsValue[EMER_AMPERAGE] = 130.43; //15kVA RAT
                lSimVarsValue[EMER_FREQ] = 400;
            }
            if (aSimVarsValue[IAS] < 100) {
                lSimVarsValue[EMER_ONLINE] = 0;
                lSimVarsValue[EMER_VOLTAGE] = 0;
                lSimVarsValue[EMER_AMPERAGE] = 0; //15kVA RAT
                lSimVarsValue[EMER_FREQ] = 0;
            }
        }
    }
    void updateSimVars() {
        for (int i = EMER_ONLINE; i <= EMER_FREQ; i++) {
            set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
        }
    }
};

/*
* ========== *
* ELEC BUSES *
* ========== *
*/
class Buses {
private:

    void updateACBuses() {
        lSimVarsValue[AC_BUS1] = updateACBUS1();
        lSimVarsValue[AC_BUS2] = updateACBUS2();
        lSimVarsValue[AC_ESS] = updateACESS();
        lSimVarsValue[AC_SHED] = updateACSHED();
        lSimVarsValue[GALLEY_SHED] = updateGALLEYSHED();
    }
    void updateDCBuses() {
        lSimVarsValue[HOT_BUS1] = updateHOTBUS1();
        lSimVarsValue[HOT_BUS2] = updateHOTBUS2();
        lSimVarsValue[DC_BUS1] = updateDCBUS1();
        lSimVarsValue[DC_BUS2] = updateDCBUS2();
        lSimVarsValue[DC_BAT] = updateDCBAT();
        lSimVarsValue[DC_ESS] = updateDCESS();
        lSimVarsValue[DC_SHED] = updateDCSHED();
    }
    ENUM updateACBUS1() {
        if (lSimVarsValue[GEN1_ONLINE]) {
            return GEN1;
        }
        if(lSimVarsValue[BUSTIE_AUTO]){
            if (lSimVarsValue[EXT_GEN_ONLINE]) {
                return EXT;
            }
            if (lSimVarsValue[APU_GEN_ONLINE]) {
                return APU;
            }
            if (lSimVarsValue[GEN2_ONLINE]) {
                return GEN2;
            }
        }
        return NOPOWER;
    }
    ENUM updateACBUS2() {
        if (lSimVarsValue[GEN2_ONLINE]) {
            return GEN2;
        }
        if (lSimVarsValue[BUSTIE_AUTO]) {
            if (lSimVarsValue[EXT_GEN_ONLINE]) {
                return EXT;
            }
            if (lSimVarsValue[APU_GEN_ONLINE]) {
                return APU;
            }
            if (lSimVarsValue[GEN2_ONLINE]) {
                return GEN1;
            }
        }
        return NOPOWER;
    }
    ENUM updateACESS() {
        if (lSimVarsValue[AC_BUS1]) {
            return lSimVarsValue[AC_BUS1];
        }
        if(lSimVarsValue[ACESS_FEED_AUTO]){
            if (lSimVarsValue[AC_BUS2]) {
                return lSimVarsValue[AC_BUS2];
            }
        }
        if (lSimVarsValue[EMER_ONLINE]) {
            return EMER;
        }
        if (lSimVarsValue[STATICINV_ONLINE]) {
            return STATINV;
        }
        return NOPOWER;
    }
    ENUM updateACSHED() {
        if ((lSimVarsValue[AC_ESS] && lSimVarsValue[AC_ESS] != EMER) || lSimVarsValue[AC_ESS] != STATINV) {
            return lSimVarsValue[AC_ESS];
        }
        return NOPOWER;
    }
    ENUM updateGALLEYSHED() {
        if(lSimVarsValue[GALLY_CAB_SW] && lSimVarsValue[COMMERCIAL_SW]){
            return lSimVarsValue[AC_SHED];
        }
        return NOPOWER;
    }
    ENUM updateHOTBUS1() {
        if (lSimVarsValue[BATT1_CAPACITY] > 0) {
            return BATT1;
        }
        return NOPOWER;
    }
    ENUM updateHOTBUS2() {
        if (lSimVarsValue[BATT2_CAPACITY] > 0) {
            return BATT2;
        }
        return NOPOWER;
    }
    ENUM updateDCBUS1() {
        if (lSimVarsValue[TR1_ONLINE]) {
            return TR1;
        }
        if (lSimVarsValue[TR2_ONLINE]) {
            return TR2;
        }
        return NOPOWER;
    }
    ENUM updateDCBUS2() {
        if (lSimVarsValue[TR2_ONLINE]) {
            return TR2;
        }
        if (lSimVarsValue[TR1_ONLINE]) {
            return TR1;
        }
        return NOPOWER;
    }
    ENUM updateDCBAT() {
        if (lSimVarsValue[DC_BUS1]) {
            return lSimVarsValue[DC_BUS1];
        }
        if (lSimVarsValue[DC_BUS2]) {
            return lSimVarsValue[DC_BUS2];
        }
        if (lSimVarsValue[BATT1_ONLINE] && lSimVarsValue[BATT2_ONLINE]) {
            return BATT12;
        }
        if (lSimVarsValue[BATT1_ONLINE]) {
            return BATT1;
        }
        if (lSimVarsValue[BATT2_ONLINE]) {
            return BATT2;
        }
        return NOPOWER;
    }
    ENUM updateDCESS() {
        if (lSimVarsValue[DC_BAT]) {
            return lSimVarsValue[DC_BAT];
        }
        if (lSimVarsValue[TRESS_ONLINE]) {
            return TRESS;
        }
        if (lSimVarsValue[HOT_BUS2]) {
            return lSimVarsValue[HOT_BUS2];
        }
        return NOPOWER;
    }
    ENUM updateDCSHED() {
        if (lSimVarsValue[DC_ESS] && lSimVarsValue[DC_ESS] != HOT_BUS2) {
            return lSimVarsValue[DC_ESS];
        }
        return NOPOWER;
    }

public:
    void init() {
        for (int i = AC_BUS1; i <= DC_SHED; i++) {
            lSimVarsValue[i] = 0;
        }
    }
    void update(double const currentAbsTime) {
        updateACBuses();
        updateDCBuses();
    }
    void updateSimVars() {
        for (int i = AC_BUS1; i <= HOT_BUS2; i++) {
            set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
        }
    }
};

/*
* ================ *
* AC/DC CONVERTORS *
* ================ *
*/
class Convertors {
private:
    void updateTR1() {
        if (lSimVarsValue[AC_BUS1]) {
            lSimVarsValue[TR1_ONLINE] = 1;
            lSimVarsValue[TR1_VOLTAGE] = round(28.4 + rand() % 5 / 10);
            lSimVarsValue[TR1_AMPERAGE] = round(33 + rand() % 8);
        }
        else {
            lSimVarsValue[TR1_ONLINE] = 0;
            lSimVarsValue[TR1_VOLTAGE] = 0;
            lSimVarsValue[TR1_AMPERAGE] = 0;
        }
    }
    void updateTR2() {
        if (lSimVarsValue[AC_BUS1]) {
            lSimVarsValue[TR2_ONLINE] = 1;
            lSimVarsValue[TR2_VOLTAGE] = round(28.4 + rand() % 5 / 10);
            if (lSimVarsValue[TR1_ONLINE]) {
                lSimVarsValue[TR2_AMPERAGE] = round(25 + rand() % 8);
            }
            else {
                lSimVarsValue[TR2_AMPERAGE] = round(33 + rand() % 8);
            }
        }
        else {
            lSimVarsValue[TR2_ONLINE] = 0;
            lSimVarsValue[TR2_VOLTAGE] = 0;
            lSimVarsValue[TR2_AMPERAGE] = 0;
        }
    }
    void updateTRESS() {
        if (lSimVarsValue[EMER_ONLINE]) {
            lSimVarsValue[TRESS_ONLINE] = 1;
            lSimVarsValue[TRESS_VOLTAGE] = round(28.4 + rand() % 5 / 10);
            lSimVarsValue[TRESS_AMPERAGE] = 10 + rand() % 5;
        }
        else {
            lSimVarsValue[TRESS_ONLINE] = 0;
            lSimVarsValue[TRESS_VOLTAGE] = 0;
            lSimVarsValue[TRESS_AMPERAGE] = 0;
        }
    }
    void updateSTATINV() {
        if ((!(ACPowerState() && aSimVarsValue[TAS] > 0) || lSimVarsValue[STATICINV_ONLINE]) && lSimVarsValue[HOT_BUS1]) {
            lSimVarsValue[STATICINV_ONLINE] = 1;
            lSimVarsValue[STATICINV_VOLTAGE] = 115;
            lSimVarsValue[STATICINV_AMPERAGE] = 70 + rand() % 5;
            lSimVarsValue[STATICINV_FREQ] = 400;
        }
        else {
            lSimVarsValue[STATICINV_ONLINE] = 0;
        }
        if(!(lSimVarsValue[STATICINV_ONLINE]) || lSimVarsValue[AC_ESS] != STATINV) {
            lSimVarsValue[STATICINV_ONLINE] = 0;
            lSimVarsValue[STATICINV_VOLTAGE] = 0;
            lSimVarsValue[STATICINV_AMPERAGE] = 0;
            lSimVarsValue[STATICINV_FREQ] = 0;
        }
    }
    bool ACPowerState() {
        if (lSimVarsValue[AC_ESS]) {
            lSimVarsValue[AC_POWER_AVAIL] = 1;
            return 1;
        }
        lSimVarsValue[AC_POWER_AVAIL] = 0;
        return 0;
    }
    bool DCPowerState() {
        if (lSimVarsValue[DC_ESS]) {
            lSimVarsValue[DC_POWER_AVAIL] = 1;
            return 1;
        }
        lSimVarsValue[DC_POWER_AVAIL] = 0;
        return 0;
    }
public:
    void init() {
        for (int i = TR1_ONLINE; i <= STATICINV_FREQ; i++) {
            lSimVarsValue[i] = 0;
        }
    }
    void update(const double currentAbsTime) {
        updateTR1();
        updateTR2();
        updateTRESS();
        updateSTATINV();
    }
    void updateSimVars() {
        for (int i = TR1_ONLINE; i <= STATICINV_FREQ; i++) {
            set_named_variable_value(ID_LSIMVAR[i],lSimVarsValue[i]);
        }
    }
};

/*
*
* ================= *
* ELEC MASTER CLASS *
* ================= *
*
*/

class ElecSys {
private:
    Batteries battUnit;
    EXTGen gpuUnit;
    APUGen apuUnit;
    EngineGen engUnit;
    RATGen emerUnit;
    Buses  busUnit;
    Convertors convertorUnit;
public:
    void init() {
        battUnit.init();
        gpuUnit.init();
        apuUnit.init();
        engUnit.init();
        emerUnit.init();
        busUnit.init();
        convertorUnit.init();
    }
    void update(const double currentAbsTime) {
        battUnit.update(currentAbsTime);
        gpuUnit.update(currentAbsTime);
        apuUnit.update(currentAbsTime);
        engUnit.update(currentAbsTime);
        emerUnit.update(currentAbsTime);
        busUnit.update(currentAbsTime);
        convertorUnit.update(currentAbsTime);
    }
    void updateSimVars() {
        battUnit.updateSimVars();
        gpuUnit.updateSimVars();
        apuUnit.updateSimVars();
        engUnit.updateSimVars();
        emerUnit.updateSimVars();
        busUnit.updateSimVars();
        convertorUnit.updateSimVars();
    }
};
//TODO
void updateELECConf() {

}


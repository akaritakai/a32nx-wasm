#pragma once
#include "../common_sys.h"

class Engine {
private:
public:
    void init() {

    }
    void update() {

    }
    void updateSimVars() {

    }
};

class APUEngine {
private:
    const int apu_flap_delay = 3 + rand() % 14;
    const int bleed_pressure_drop = 2 + rand() % 2;

    void openFlap(const double currentAbsTime) {
        if (lSimVarsValue[APU_FLAP_OPEN] < 100) {
            lSimVarsValue[APU_FLAP_OPEN] += 100 * ((currentAbsTime - lastAbsTime) * 0.001) / apu_flap_delay;	//pct time finished for opening flap
        }
    }
    void closeFlap(const double currentAbsTime) {
        if (lSimVarsValue[APU_FLAP_OPEN] > 0) {
            lSimVarsValue[APU_FLAP_OPEN] -= 100 * ((currentAbsTime - lastAbsTime) * 0.001) / apu_flap_delay;	//pct time finished for opening flap
        }
    }
    void startup(const double currentAbsTime) {
        trigger_key_event(KEY_APU_STARTER, 1);      //activate apu starter only after full flap opening
        if (lSimVarsValue[APU_N1] <= 12) {
            lSimVarsValue[APU_N1] += ((currentAbsTime - lastAbsTime) * 0.001) * 3;
        }
        else if (lSimVarsValue[APU_N1] <= 60) {
            lSimVarsValue[APU_N1] += ((currentAbsTime - lastAbsTime) * 0.001) * 1.92;
        }
        else if (lSimVarsValue[APU_N1] < 100) {
            lSimVarsValue[APU_N1] += ((currentAbsTime - lastAbsTime) * 0.001) * 2.85;
        }
        if (lSimVarsValue[APU_N1] >= 87) {
            lSimVarsValue[APU_BLEED_PRESSURE] = 35 - rand() % 1;
        }
        if (lSimVarsValue[APU_N1] <= 95) {
            lSimVarsValue[APU_BLEED_PRESSURE] = 35 - rand() % 1;
        }
        updateEGT(true, currentAbsTime);
    }
    void shutdown(const double currentAbsTime) {
        trigger_key_event(KEY_APU_OFF_SWITCH, 1);
        if (lSimVarsValue[APU_N1] > 0) {
            lSimVarsValue[APU_N1] -= ((currentAbsTime - lastAbsTime) * 0.001) * 5;
        }
        updateEGT(false, currentAbsTime);
    }
    void updateEGT(bool startup, const double currentAbsTime) {
        const double N1 = lSimVarsValue[APU_N1];
        const double ambient = aSimVarsValue[AMB_TEMP];
        if (startup) {
            if (N1 < 10) {
                lSimVarsValue[APU_EGT] = ambient;
            }
            else if (N1 <= 14) {
                lSimVarsValue[APU_EGT] = round(90 / 6 * N1 - 140 + ambient);
            }
            else if (N1 <= 20) {
                lSimVarsValue[APU_EGT] = round((215 / 4 * N1) - 760 + ambient);
            }
            else if (N1 <= 32) {
                lSimVarsValue[APU_EGT] = round((420 / 11 * N1) - 481.8 + ambient);
            }
            else if (N1 <= 36) {
                lSimVarsValue[APU_EGT] = round((20 / 3 * N1) + 525 + ambient);
            }
            else if (N1 <= 43) {
                lSimVarsValue[APU_EGT] = round((-15 / 6 * N1) + 888.3 + ambient);
            }
            else if (N1 <= 50) {
                lSimVarsValue[APU_EGT] = round((3 * N1) + 618 + ambient);
            }
            else if (N1 <= 74) {
                lSimVarsValue[APU_EGT] = round((-100 / 13 * N1) + 1152.3 + ambient);
            }
            else {
                lSimVarsValue[APU_EGT] = round((-104 / 10 * N1) + 1430 + ambient - rand() % 2);
            }
        }
        else {
            if (lSimVarsValue[APU_EGT] >= ambient) {
                lSimVarsValue[APU_EGT] = round(lSimVarsValue[APU_EGT] - 2 * (currentAbsTime - lastAbsTime) * 0.001);
            }
        }
    }
    void updateEGTWarn() {
        const double n = lSimVarsValue[APU_N1];
        if (n < 11) {
            lSimVarsValue[APU_EGT_WARN] = 1200;
        }
        else if (n <= 15) {
            lSimVarsValue[APU_EGT_WARN] = (-50 * n) + 1750;
        }
        else if (n <= 65) {
            lSimVarsValue[APU_EGT_WARN] = (-3 * n) + 1045;
        }
        else {
            lSimVarsValue[APU_EGT_WARN] = (-30 / 7 * n) + 1128.6;
        }//ōīṇḥūiūṇō => I somehow managed to type this while I was asleep on the keyboard, any explanation would be nice.
    }
public:
    void init() {
        for (int i = APU_FLAP_OPEN; i <= APU_EGT_WARN; i++) {
            lSimVarsValue[i] = 0;
        }
    }
    void update(const double currentAbsTime) {
        if (aSimVarsValue[FUEL_VALUE_8]) {
            openFlap(currentAbsTime);
            if (lSimVarsValue[APU_FLAP_OPEN] == 100 && lSimVarsValue[APU_START]) {
                startup(currentAbsTime);
            }
        }
        else {
            closeFlap(currentAbsTime);
            if (lSimVarsValue[APU_FLAP_OPEN] < 100) {
                shutdown(currentAbsTime);
            }
        }
        updateEGTWarn();
    }
    void updateSimVars() {
        for (int i = APU_FLAP_OPEN; i <= APU_EGT_WARN; i++) {
            set_named_variable_value(ID_LSIMVAR[i], lSimVarsValue[i]);
        }
    }
};

class EngSys {
    private:
        Engine eng1;
        Engine eng2;
        APUEngine apuEng;
    public:
    void init() {
        eng1.init();
        eng2.init();
        apuEng.init();
    }
    void update(double const currentAbsTime) {
        eng1.update();
        eng2.update();
        apuEng.update(currentAbsTime);
    }
    void updateSimVars() {
        eng1.updateSimVars();
        eng2.updateSimVars();
        apuEng.updateSimVars();
    }
};
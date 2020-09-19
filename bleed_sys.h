#pragma once
#include "common_sys.h"

using namespace std;

class BleedSys {
private:
    EngineBleed engUnit;
    APUBleed apuUnit;
    GPUBleed gpuUnit;
    RATAir ratUnit;
    Valves valveUnit;
public:
    void init() {
        engUnit.init();
        apuUnit.init();
        gpuUnit.init();
        ratUnit.init();
        valveUnit.init();
    }
    void update() {
        engUnit.update();
        apuUnit.update();
        gpuUnit.update();
        ratUnit.update();
        valveUnit.update();

    }
    void updateSimVars() {
        engUnit.updateSimVars();
        apuUnit.updateSimVars();
        gpuUnit.updateSimVars();
        ratUnit.updateSimVars();
        valveUnit.updateSimVars();
    }
};

class EngineBleed {
private:
public:
    void init() {

    }
    void update() {

    }
    void updateSimVars() {

    }
};

class APUBleed {
private:
public:
    void init() {

    }
    void update() {

    }
    void updateSimVars() {

    }
};

class GPUBleed {
private:
public:
    void init() {

    }
    void update() {

    }
    void updateSimVars() {

    }
};

class RATAir {
private:
public:
    void init() {

    }
    void update() {

    }
    void updateSimVars() {

    }
};

class Valves {
private:
public:
    void init() {

    }
    void update() {

    }
    void updateSimVars() {

    }
};
#pragma once
#include "../common_sys.h"

using namespace std;

class PacksSys {
private:
    Packs packsUnit;
    Cabin cabinUnit;
public:
    void init() {
        packsUnit.init();
        cabinUnit.init();
    }
    void update() {
        packsUnit.update();
        cabinUnit.update();
    }
    void updateSimVars() {
        packsUnit.updateSimVars();
        cabinUnit.updateSimVars();
    }
};

class Packs {
private:
public:
    void init() {

    }
    void update() {

    }
    void updateSimVars() {

    }
};

class Cabin {
private:
public:
    void init() {

    }
    void update() {

    }
    void updateSimVars() {

    }
};
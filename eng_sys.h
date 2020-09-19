#pragma once
#include "common_sys.h"

using namespace std;

class engSys {
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
    void update() {
        eng1.update();
        eng2.update();
        apuEng.update();
    }
    void updateSimVars() {
        eng1.updateSimVars();
        eng2.updateSimVars();
        apuEng.updateSimVars();
    }
};

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
public:
    void init() {

    }
    void update() {

    }
    void updateSimVars() {

    }
};
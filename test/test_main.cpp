#include "simulatorImp.h"

int main(int argc, char** argv) {
    for (int i = 0; i < 1000; i++) {
        cppSimulatorImp engine(nullptr, nullptr);
        while (engine.get_time() < 200) {
            engine.loop();
        }
    }
    
    return 0;
}
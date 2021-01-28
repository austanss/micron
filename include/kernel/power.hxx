//
// Created by rizet on 11/27/20.
//

#ifndef MICRONET_POWER_HXX
#define MICRONET_POWER_HXX

extern "C"
{
namespace cpu {    
    namespace power {
        void shutdown();
        void restart_cold();
        void restart_warm();
    }
}
};

#endif //MICRONET_POWER_HXX

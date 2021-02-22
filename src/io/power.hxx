//
// Created by rizet on 11/27/20.
//
#pragma once

extern "C"
{
namespace cpu {    
    namespace power {
        void shutdown();
        void restart_cold();
        void restart_warm();
    }
}
}
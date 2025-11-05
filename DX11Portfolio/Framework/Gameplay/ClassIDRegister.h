#pragma once
#include "ClassID.h"

#define REGISTER_CLASS(CLASSNAME)\
    namespace { \
        struct CLASSNAME##_AutoRegister { \
            CLASSNAME##_AutoRegister() { \
                ClassID::Register(#CLASSNAME, \
                    [](UWorld* w) -> AActor* { return w->SpawnActor<CLASSNAME>(); }); \
            } \
        }; \
        static CLASSNAME##_AutoRegister s_##CLASSNAME##_AutoRegister; \
    }
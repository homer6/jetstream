#pragma once
#include "config/JetStreamConfig.h"

namespace jetstream{
namespace writer{

    class Writer {

        virtual void run( const bool& keep_running ) = 0;

    };

}
}

#pragma once
#include "data/DataAugmenter.h"

namespace jetstream{
namespace data{

    class GizmoDataAugmenter : public DataAugmenter {

    public:
        virtual void augment();

    };

}
}

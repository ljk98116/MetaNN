#pragma once

#include <METANN/Facility/VarTypeDict.hpp>
namespace METANN
{
    struct LayerIO : public VarTypeDict<LayerIO>{};
    struct CostLayerIn : public VarTypeDict<CostLayerIn, struct CostLayerLabel>{};
}
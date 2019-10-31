#pragma once

#include <cstdint>
#include <memory>

// We need the following WraithX classes
#include <WraithModel.h>

// We need the cod assets
#include "CoDXAssets.h"

// -- Structures for storing data

struct WeightsData
{
    // The weight value for each bone
    float WeightValues[8];
    // The bone ids for each value
    uint32_t BoneValues[8];
    // The count of weights this set contains
    uint8_t WeightCount;

    WeightsData()
    {
        // Defaults
        WeightCount = 1;
        // Clear memory
        WeightValues[0] = 1.0f;
        WeightValues[1] = 1.0f;
        WeightValues[2] = 1.0f;
        WeightValues[3] = 1.0f;
        WeightValues[4] = 1.0f;
        WeightValues[5] = 1.0f;
        WeightValues[6] = 1.0f;
        WeightValues[7] = 1.0f;
        BoneValues[0] = 0;
        BoneValues[1] = 0;
        BoneValues[2] = 0;
        BoneValues[3] = 0;
        BoneValues[4] = 0;
        BoneValues[5] = 0;
        BoneValues[6] = 0;
        BoneValues[7] = 0;
    }
};

// -- End storage structures

// A class that handles translating generic Call of Duty XModels to Wraith Models
class CoDXModelTranslator
{
public:
    // -- Translation functions

    // Translates an in-game XModel to a WraithModel
    static std::unique_ptr<WraithModel> TranslateXModel(const std::unique_ptr<XModel_t>& Model, uint32_t LodIndex, bool JustBones = false);

    // -- Utility functions

    // Calculates the index of the biggest lod, returns -1 if there are no lods
    static int32_t CalculateBiggestLodIndex(const std::unique_ptr<XModel_t>& Model);

private:
    // -- Translation utilities

    // Normal unpack method A
    static Vector3 UnpackNormalA(uint32_t Normal);

    // Prepares vertex weights A
    static void PrepareVertexWeightsA(std::vector<WeightsData>& Weights, const XModelSubmesh_t& Submesh);
};
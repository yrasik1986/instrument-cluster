#pragma once

#include <cstdint>

namespace VehicleCan
{

enum class FrameId : std::uint32_t
{
    DriveData  = 0x100,
    StatusData = 0x101
};

} // namespace VehicleCan
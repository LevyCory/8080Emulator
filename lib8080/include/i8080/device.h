#pragma once

#include <cstdint>
#include <memory>

namespace i8080
{
    struct Device
    {
        using sptr = std::shared_ptr<Device>;

        virtual ~Device() = default;

        virtual void write(uint8_t byte) {}
        virtual void read(uint8_t&) {}
    };
}

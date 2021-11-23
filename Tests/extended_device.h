#pragma once

#include <i8080cpu/include/cpu.h>
#include <i8080cpu/include/device.h>

// Crazy hack to allow hardware devices to access the CPU and Memory
// Will be used for testing
class ExtendedDevice : public i8080::Device
{
public:
    using sptr = std::shared_ptr<ExtendedDevice>;

    ExtendedDevice(const i8080::Cpu& cpu, const buffer& memory) :
        i8080::Device(),
        _cpu(cpu),
        _memory(memory)
    {}

protected:
    const i8080::Cpu& _cpu;
    const buffer& _memory;
};
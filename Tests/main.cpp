#include <i8080cpu\include\cpu.h>
#include <i8080cpu\include\bus.h>
#include <i8080cpu\include\device.h>

#include "extended_device.h"

#include <vector>
#include <numeric>
#include <fstream>
#include <iostream>
#include <filesystem>


namespace fs = std::filesystem;
using buffer = std::vector<uint8_t>;

static constexpr uint16_t MEMORY_NAMESPACE_SIZE = std::numeric_limits<uint16_t>::max();

struct TestStateDevice : public i8080::Device
{
    bool finished = false;

    void write(uint8_t) override
    {
        finished = true;
    }
};

class DebugDevice : public ExtendedDevice
{
public:
    DebugDevice(const i8080::Cpu& cpu, const buffer& memory) :
        ExtendedDevice(cpu, memory)
    {}

    void write(uint8_t byte) override
    {
        switch (byte)
        {
        case PRINT_E_REG_VALUE:
            std::cout << static_cast<char>(_cpu.state().e);
            break;
        case OUTPUT_DE_MEMORY:
            _print_de_memory();
            break;
        default:
            return;
        }
    }

private:
    void _print_de_memory()
    {
        uint16_t address = _cpu.state().de;

        for (; _memory[address] != '$'; address++)
        {
            std::cout << _memory[address];
        }
    }

    static constexpr uint8_t PRINT_E_REG_VALUE = 2;
    static constexpr uint8_t OUTPUT_DE_MEMORY = 9;
};

bool load_binary(const fs::path& path, buffer& memory, uint16_t offset)
{
    std::memset(memory.data(), 0, memory.capacity());
    std::ifstream test_file(path, std::ios::binary);
    if (!test_file.is_open())
    {
        return false;
    }

    if (test_file.read(reinterpret_cast<char*>(memory.data() + offset), memory.size()))
    {
        return false;
    }

    // Inject "OUT 0" at 0x0000 (signal to stop the test)
    memory[0x0000] = 0xD3;
    memory[0x0001] = 0x00;

    // Inject "OUT 1" at 0x0005 (signal to output some characters)
    memory[0x0005] = 0xD3;
    memory[0x0006] = 0x01;
    memory[0x0007] = 0xC9;

    return true;
}

int main()
{
    buffer memory(MEMORY_NAMESPACE_SIZE);
    load_binary(".\\Resources\\test8080.com", memory, 0x100);
    i8080::Bus bus(memory);

    auto test_device = std::make_shared<TestStateDevice>();
    bus.register_device(0, test_device);

    i8080::Cpu cpu(bus, 0x100, 0);
    cpu.set_debug(true);

    bus.register_device(1, std::make_shared<DebugDevice>(cpu, memory));

    while (!(test_device->finished || cpu.halt()))
    {
        cpu.tick();
    }

    return 0;
}
#include <i8080/device.h>
#include <i8080/bus.h>
#include <i8080/cpu.h>

#include <fmt/core.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>


namespace fs = std::filesystem;
using buffer = std::vector<uint8_t>;

static constexpr uint16_t MEMORY_NAMESPACE_SIZE = std::numeric_limits<uint16_t>::max();

class TestControlDevice : public i8080::Device
{
public:
    TestControlDevice(bool& finished) :
        _finished(finished)
    {}

    void write(uint8_t) override
    {
        _finished = true;
    }

private:
    bool& _finished;
};

class IODevice : public i8080::Device
{
    static constexpr uint8_t PRINT_STATUS_REG_E = 2;
    static constexpr uint8_t PRINT_MESSAGE = 9;

public:
    IODevice(const i8080::Cpu& cpu, const buffer& memory) :
        _cpu(cpu),
        _memory(memory)
    {}

    void write(uint8_t byte) override
    {
        switch (byte)
        {
        case PRINT_STATUS_REG_E:
            std::cout << fmt::format("{:c}", static_cast<char>(_cpu.state().e));
            break;
        case PRINT_MESSAGE:
            _print_message_in_de();
            break;
        default:
            return;
        }
    }

private:
    void _print_message_in_de()
    {
        uint16_t address = _cpu.state().de;

        for (; _memory[address] != '$'; address++)
        {
            std::cout << _memory[address];
        }

        std::cout << std::endl;
    }

    const i8080::Cpu & _cpu;
    const buffer& _memory;
};

bool load_binary(const fs::path& path, buffer& memory)
{
    std::memset(memory.data(), 0, memory.capacity());
    std::ifstream test_file(path, std::ios::binary);
    if (!test_file.is_open())
    {
        return false;
    }

    if (test_file.read(reinterpret_cast<char*>(memory.data() + i8080::Cpu::PROGRAM_START_OFFSET), memory.size()))
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

uint64_t run_test(const fs::path& test_rom, buffer& memory, bool debug)
{
    bool test_finished = false;

    load_binary(test_rom, memory);
    i8080::Bus bus(memory);

    i8080::Cpu cpu(bus);
    cpu.set_debug(debug);

    bus.register_device(0, std::make_shared<TestControlDevice>(test_finished));
    bus.register_device(1, std::make_shared<IODevice>(cpu, memory));

    while (!test_finished && !cpu.halt())
    {
        cpu.tick();
    }

    return cpu.state().cycle;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Usage: i8080tests <test_rom>" << std::endl;
        return 1;
    }

    buffer memory(MEMORY_NAMESPACE_SIZE);

    run_test(argv[1], memory, true);
    return 0;
}

/*
02d0    CPE 02d9h
02d3    ADI 0002h
02d5    RPO
02c5    CPI 00d9h
02c7    JZ 032ah
*/

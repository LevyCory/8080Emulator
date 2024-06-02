#include <i8080/bus.h>
#include <i8080/cpu.h>
#include <i8080/device.h>

#include <fmt/core.h>

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace fs = std::filesystem;
using buffer = std::vector<uint8_t>;

static constexpr uint16_t PROGRAM_START_OFFSET = 0x100;

class TestControlDevice : public i8080::Device
{
public:
    TestControlDevice(bool& finished) :
        _finished(finished)
    {}

    void write(uint8_t) override { _finished.get() = true; }

private:
    std::reference_wrapper<bool> _finished;
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
        switch (byte) {
        case PRINT_STATUS_REG_E:
            std::cout << fmt::format("{:c}", static_cast<char>(_cpu.get().state().e));
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
        uint16_t address = _cpu.get().state().de;
        for (; _memory.get()[address] != '$'; address++) {
            std::cout << _memory.get()[address];
        }

        std::cout << std::endl;
    }

    std::reference_wrapper<const i8080::Cpu> _cpu;
    std::reference_wrapper<const buffer> _memory;
};

bool load_binary(const fs::path& path, buffer& memory)
{
    std::memset(memory.data(), 0, memory.capacity());
    std::ifstream test_file(path, std::ios::binary);
    if (!test_file.is_open()) {
        throw std::runtime_error(fmt::format("Could not open file: {}", path.string()));
    }

    if (test_file.read(reinterpret_cast<char*>(memory.data() + PROGRAM_START_OFFSET),
                       memory.size())) {
        throw std::runtime_error(fmt::format("Could not read file: {}", path.string()));
    }

    // Inject "OUT 0" at 0 (stop test)
    memory[0] = 0xD3;
    memory[1] = 0x00;

    // Inject "OUT 1" at 5 (print characters)
    memory[5] = 0xD3;
    memory[6] = 0x01;
    memory[7] = 0xC9;

    return true;
}

uint64_t run_test(const fs::path& test_rom, bool debug)
{
    buffer memory(i8080::Cpu::NAMESPACE_SIZE);
    load_binary(test_rom, memory);

    i8080::Bus bus(memory);
    i8080::Cpu cpu(bus, PROGRAM_START_OFFSET);

    cpu.set_debug(debug);

    bool test_finished = false;
    bus.register_device(0, std::make_shared<TestControlDevice>(test_finished));
    bus.register_device(1, std::make_shared<IODevice>(cpu, memory));

    while (!test_finished && !cpu.halt()) {
        cpu.tick();
    }

    return cpu.state().cycle;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Usage: tester <test_rom>" << std::endl;
        return 1;
    }

    try {
        run_test(argv[1], true);
    } catch (const std::exception& e) {
        std::cerr << fmt::format("Test failed: {}\n", e.what());
        return 2;
    }

    return 0;
}

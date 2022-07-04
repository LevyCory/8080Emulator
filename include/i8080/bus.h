#pragma once

#include <array>

#include "asm.h"
#include "common.h"
#include "device.h"

namespace i8080
{
    class Bus final
    {
    public:
        Bus(buffer& memory) :
            _memory(memory)
        {}

        ~Bus() = default;
        Bus(const Bus&) = delete;
        Bus& operator= (const Bus&) = delete;
        Bus(Bus&&) noexcept = default;
        Bus& operator= (Bus&&) noexcept = default;
        
        const Opcode& fetch(uint16_t pc) const;

        void register_device(uint8_t id, Device::sptr device);
        void write(uint8_t device, uint8_t byte);
        void read(uint8_t device, uint8_t& byte);

        void mem_write(uint16_t address, uint8_t byte);
        void mem_write(uint16_t address, uint16_t word);

        void mem_read(uint16_t address, uint8_t& byte);
        void mem_read(uint16_t address, uint16_t& word);
        uint8_t& mem_read_u8_ref(uint16_t address);

    private:
        buffer& _memory;
        std::array<Device::sptr, 8> _devices;
    };
}

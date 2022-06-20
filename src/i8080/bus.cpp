#include "bus.h"

namespace i8080
{
    const Opcode& Bus::fetch(uint16_t pc) const
    {
        return *reinterpret_cast<const Opcode*>(padd(_memory.data(), pc));
    }

    void Bus::register_device(uint8_t id, Device::sptr device)
    {
        _devices[id] = std::move(device);
    }

    void Bus::write(uint8_t device, uint8_t byte)
    {
        if (_devices[device])
        {
            _devices[device]->write(byte);
        }
    }

    void Bus::read(uint8_t device, uint8_t& byte)
    {
        if (_devices[device])
        {
            _devices[device]->read(byte);
        }
    }

    void Bus::mem_write(uint16_t address, uint8_t byte)
    {
        _memory.data()[address] = byte;
    }

    void Bus::mem_write(uint16_t address, uint16_t word)
    {
        *reinterpret_cast<uint16_t*>(padd(_memory.data(), address)) = word;
    }

    void Bus::mem_read(uint16_t address, uint8_t& byte)
    {
        byte = _memory[address];
    }

    void Bus::mem_read(uint16_t address, uint16_t& word)
    {
        word = *reinterpret_cast<uint16_t*>(padd(_memory.data(), address));
    }

    uint8_t& Bus::mem_read_u8_ref(uint16_t address)
    {
        return _memory[address];
    }
}
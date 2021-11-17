#include <i8080cpu\include\cpu.h>

#include <vector>
#include <numeric>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
using buffer = std::vector<uint8_t>;

static constexpr uint16_t MEMORY_NAMESPACE_SIZE = std::numeric_limits<uint16_t>::max();

bool load_binary(const fs::path& path, buffer& memory, uint16_t offset)
{
    std::ifstream test_file(path, std::ios::binary);
    if (!test_file.is_open())
    {
        return false;
    }

    if (!test_file.read(reinterpret_cast<char*>(memory.data() + offset), memory.size()))
    {
        return false;
    }

    return true;
}

int main()
{
    buffer memory(MEMORY_NAMESPACE_SIZE);
    load_binary(".\\Resources\\test8080.com", memory, 0x100);
    i8080::Cpu cpu(memory.data());
    return 0;
}
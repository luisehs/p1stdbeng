#include "PartGenerator.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <string>

namespace bufman {
namespace {

constexpr std::array<const char*, 6> kMaterials = {
    "Steel", "Wood", "Iron","Glass","Copper","Plastic"
};

}

std::vector<Part> generate_parts(std::size_t count, int first_part_id) {
    std::vector<Part> parts;
    parts.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        Part part{};
        part.part_id = first_part_id + static_cast<int>(i);
        //en el caso que no salga decimal podemos usar static_cast como el profesor

        part.part_weight = 1.0 + (part.part_id % 100) / 100.0;
        part.part_color = (part.part_id % 6);
        part.part_price = 10.0 + part.part_id % 100;

        const std::string name = "P" + std::to_string(part.part_id);
        name.copy(part.part_name, std::min(name.size(), sizeof(part.part_name) - 1));

        const char* material = kMaterials[i % kMaterials.size()];
        std::memcpy(part.part_material, material, std::min(strlen(material), sizeof(part.part_material) - 1));

        parts.push_back(part);
    }
    return parts;
}

}

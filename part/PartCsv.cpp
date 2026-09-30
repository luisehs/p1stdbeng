#include "PartCsv.h"
#include <charconv>
#include <fstream>
#include <sstream>

namespace bufman {
namespace {


bool parse_integer(const std::string& text, int& value) {
    if (text.empty()) {
        return false;
    }
    const char* first = text.data();
    const char* last = first + text.size();
    const auto result = std::from_chars(first, last, value);
    return result.ec == std::errc{} && result.ptr == last;
}
bool parse_float(const std::string& text, float& value) {
    if (text.empty()) {
        return false;
    }
    const char* first = text.data();
    const char* last = first + text.size();
    const auto result = std::from_chars(first, last, value);
    return result.ec == std::errc{} && result.ptr == last;
}

bool parse_line(const std::string& line, Part& part, std::string& error) {
    std::stringstream input(line);
    std::string part_id_text;
    std::string part_name;
    std::string part_weight_text;
    std::string part_color_text;
    std::string part_price_text;
    std::string part_material;
    std::string extra;
    
    if (!std::getline(input, part_id_text, ',') ||
        !std::getline(input, part_name, ',') ||
        !std::getline(input, part_weight_text, ',') ||
        !std::getline(input, part_color_text, ',') ||
        !std::getline(input, part_price_text, ',') ||
        !std::getline(input, part_material, ',') ||
        std::getline(input, extra, ',')) {
        error = "expected exactly six comma-separated fields";
        return false;
    }

    int part_id = 0;
    float part_weight = 0.0f;
    int part_color = 0;
    float part_price = 0.0f;
    
    if (!parse_integer(part_id_text, part_id) || part_id <= 0) {
        error = "part_id must be a positive integer";
        return false;
    }
    if (!parse_float(part_weight_text, part_weight) || part_weight < 0) {
        error = "part_weight must be a nonnegative float";
        return false;
    }
    if (!parse_integer(part_color_text, part_color) || part_color < 0 || part_color > 5) {
        error = "part_color must be an integer between 0 and 5";
        return false;
    }
    if (!parse_float(part_price_text, part_price) || part_price < 0) {
        error = "part_price must be a nonnegative float";
        return false;
    }
    if (part_name.size() > 9) {
        error = "part_name must contain at most 9 characters";
        return false;
    }
    if (part_material.size() > 9) {
        error = "part_material must contain at most 9 characters";
        return false;
    }


    part = Part{};
    part.part_id = part_id;
    part.part_weight = part_weight;
    part.part_color = part_color;
    part.part_price = part_price;
    part_name.copy(part.part_name, part_name.size());
    part_material.copy(part.part_material, part_material.size());
    return true;
}

}


PartLoadResult load_parts(const std::string& path, std::ostream& diagnostics) {
    PartLoadResult result;
    std::ifstream input(path);
    if (!input) {
        diagnostics << "cannot open CSV file: " << path << '\n';
        result.skipped = 1;
        return result;
    }

    std::string line;
    std::size_t line_number = 0;
    while (std::getline(input, line)) {
        ++line_number;
        if (line.empty()) {
            ++result.skipped;
            diagnostics << "line " << line_number << ": blank line\n";
            continue;
        }
        Part part{};
        std::string error;
        if (!parse_line(line, part, error)) {
            ++result.skipped;
            diagnostics << "line " << line_number << ": " << error << '\n';
            continue;
        }
        result.parts.push_back(part);
    }
    return result;
}

}
#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <variant>
#include <stdexcept>

using property_type = std::variant<int, float, bool, std::string>;

class Material
{
public:
	Material();
private:
	std::unordered_map<std::string, property_type> properties;
};


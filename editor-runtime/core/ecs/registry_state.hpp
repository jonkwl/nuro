#pragma once

#include <entt.hpp>
#include <cstdint>
#include <vector>

struct RegistryState {
	std::vector<std::byte> buffer;
	size_t offset = 0;

	void operator()(std::underlying_type_t<entt::entity> value) {
		store(value);
	}

	void operator()(entt::entity value) {
		store(value);
	}

	template<typename T>
	void operator()(const T& value) {
		store(value);
	}

	template<typename T>
	void operator()(T& value) {
		retrieve(value);
	}

private:
	template<typename T>
	void store(const T& value) {
		auto ptr = reinterpret_cast<const std::byte*>(&value);
		buffer.insert(buffer.end(), ptr, ptr + sizeof(T));
	}

	template<typename T>
	void retrieve(T& value) {
		auto ptr = reinterpret_cast<std::byte*>(&value);
		std::copy(buffer.begin() + offset, buffer.begin() + offset + sizeof(T), ptr);
		offset += sizeof(T);
	}
};
#pragma once

#include <string>
#include <utility>
#include <optional>

#include <rfl.hpp>
#include <rfl/yaml.hpp>
#include <yaml-cpp/yaml.h>

#include <utils/fsutil.h>
#include <utils/console.h>

using AssetGUID = std::string;

namespace AssetMeta {

	// Returns the guid from the header of a metadata file
	AssetGUID parseGUID(const FS::Path& path);

	// Create header for metadata file
	std::string createHeader(AssetGUID guid);

	// Remove header from metadata file source
	void removeHeader(std::string& source);

	// Serializes a metadata object into a metadata file, returns success
	template <typename T>
	bool serialize(const T& obj, const FS::Path& path) {
		// Try to serialize object
		std::string serialized;
		try {
			serialized = rfl::yaml::write(obj);
		}
		catch (const std::exception& e) {
			std::string err = std::string(e.what());
			Console::out::warning("Asset Meta", "Failed to serialize some assets metadata: " + err);
			return false;
		}

		// Add guid header to serialized string
		std::string header = createHeader(parseGUID(path));
		serialized.insert(0, header);

		// Write serialized object to metadata file
		FS::writeFile(path, serialized);
	}

	// Deserializes a metadata file, returns optional of deserialized object depending on success
	template <typename T>
	std::optional<T> deserialize(const FS::Path& path) {
		// Read meta file source
		std::string source = FS::readFile(path);

		// Remove header from metadata source
		removeHeader(source);

		// Ensure metadata source is available
		if (source.empty())
			return std::nullopt;

		// Try to deserialize metadata source
		try {
			const T obj = rfl::yaml::read<T>(source).value();
			return obj;
		}
		catch (const std::exception& e) {
			std::string err = std::string(e.what());
			Console::out::warning("Asset Meta", "Failed to deserialize some assets metadata: " + err);
			return std::nullopt;
		}
	}

	// Loads an assets metadata into its slot
	template <typename T>
	void loadInto(T* slot, const FS::Path& path) {
		auto meta = deserialize<T>(path);
		if (!meta) {
			*slot = T();
			AssetMeta::serialize(*slot, path);
		}
		else {
			*slot = meta.value();
		}
	}

}
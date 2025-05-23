#pragma once

#include <string>
#include <utility>
#include <optional>

#include <rfl.hpp>
#include <rfl/json.hpp>
#include <nlohmann/json.hpp>

#include <utils/guid.h>
#include <utils/fsutil.h>
#include <utils/console.h>

// Global asset id
using AssetGUID = XG::GUID;

// Session-only runtime id of an asset
using AssetSID = uint32_t;

struct AssetKey {
	AssetGUID guid;
	AssetSID sessionID = 0;
};

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
			serialized = rfl::json::write(obj);
			auto json = nlohmann::json::parse(serialized);
			serialized = json.dump(4);
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

		return true;
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
			const T obj = rfl::json::read<T>(source).value();
			return obj;
		}
		catch (const std::exception& e) {
			std::string err = std::string(e.what());
			Console::out::warning("Asset Meta", "Failed to deserialize some assets metadata: " + err);
			return std::nullopt;
		}
	}

	// Loads an assets metadata from a metadata file
	template <typename T>
	void loadMeta(T* slot, const FS::Path& path) {
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
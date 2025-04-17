#include "asset_meta.h"

namespace AssetMeta {

	AssetGUID parseGUID(const FS::Path& path)
    {
		try {
			// Ensure metadata file is valid
			if (!FS::exists(path))
				return "INVALID-GUID";

			// Open metadata file
			std::ifstream file(path);
			if (!file.is_open()) {
				return "INVALID-GUID";
			}

			// Read first line of metadata file (containing guid)
			std::string line;
			if (!std::getline(file, line))
				return "INVALID-GUID";

			const std::string guidPrefix = "guid: ";
			if (line.rfind(guidPrefix, 0) != 0)
				return "INVALID-GUID";

			std::string guid = line.substr(guidPrefix.length());

			// Optional guid validation
			// if (guid.length() != 32 || guid.find_first_not_of("0123456789abcdef") != std::string::npos) return "INVALID-GUID";

			return guid;
		}
		catch (...) {
			return "INVALID-GUID";
		}
    }

	std::string createHeader(AssetGUID guid)
	{
		return "guid: " + guid + "\n";
	}

	void removeHeader(std::string& source)
	{
		// Removes the first line of metadata source containing guid
		size_t newlinePos = source.find('\n');
		if (newlinePos == std::string::npos) return;
		source = source.substr(newlinePos + 1);
	}

}
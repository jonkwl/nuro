#include "asset_meta.h"

namespace AssetMeta {

	AssetGUID parseGUID(const FS::Path& path)
    {
		try {
			// Ensure metadata file is valid
			if (!FS::exists(path))
				return AssetGUID();

			// Open metadata file
			std::ifstream file(path);
			if (!file.is_open()) {
				return AssetGUID();
			}

			// Read first line of metadata file (containing guid)
			std::string line;
			if (!std::getline(file, line))
				return AssetGUID();

			// Extract guid string
			const std::string guidPrefix = "guid: ";
			if (line.rfind(guidPrefix, 0) != 0)
				return AssetGUID();
			std::string guidStr = line.substr(guidPrefix.length());

			return AssetGUID(guidStr);
		}
		catch (...) {
			return AssetGUID();
		}
    }

	std::string createHeader(AssetGUID guid)
	{
		return "guid: " + guid.str() + "\n---\n";
	}

	void removeHeader(std::string& source)
	{
		try {
			// Find header document separator
			size_t separatorPos = source.find("---");
			if (separatorPos == std::string::npos) return;

			size_t newlineAfterSeparator = source.find('\n', separatorPos);
			if (newlineAfterSeparator == std::string::npos) return;

			// Remove header
			source = source.substr(newlineAfterSeparator + 1);
		}
		catch (const std::exception& e) {
			Console::out::warning("Asset Meta", "Failed to remove header: " + std::string(e.what()));
		}
	}

}
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <efsw/efsw.hpp>
#include <utils/event.h>
#include <utils/concurrent_queue.h>

#include <filesystem>
namespace fs = std::filesystem;

class ProjectObserver {
public:
	struct IONode {
		inline static uint32_t idCounter;

		// Session-unique identifier of io node
		uint32_t ioId;

		// Name of node, e.g. filename
		std::string name;

		// Path to io node, relative to project root
		fs::path path;

		explicit IONode(std::string name, fs::path path) : ioId(++idCounter), name(name), path(path) {}

		virtual bool isFolder() = 0;
		virtual ~IONode() = default;
	};

	struct File : public IONode {
		explicit File(std::string name, fs::path path) : IONode(name, path) {}

		bool isFolder() override { return false; }
	};

	struct Folder : public IONode {
		std::vector<std::shared_ptr<File>> files;
		std::vector<std::shared_ptr<Folder>> subfolders;
		bool expanded;

		explicit Folder(std::string name, fs::path path) : IONode(name, path), files(), subfolders(), expanded(false) {}
		~Folder() override { folderRegistry.erase(ioId); }

		bool isFolder() override { return true; }

		// Returns if the folder has any subfolders
		bool hasSubfolders();
		
		// Adds a file
		void addFile(const std::string& fileName);

		// Adds a subfolder
		void addFolder(const std::string& folderName);

		// Removes a file by its name if existing
		void removeFile(const std::string& fileName);

		// Removes a subfolder by its name if existing
		void removeFolder(const std::string& folderName);

		// Removes either a file or subfolder by its name if existing
		void removeAny(const std::string& name);

		// Prints the folders contents recursively
		void print(uint32_t depth = 0);
	};

	struct IOEvent {
		efsw::Action action;
		std::string directory;
		std::string filename;
		std::string oldFilename;

		IOEvent(efsw::Action action, std::string directory, std::string filename, std::string oldFilename) :
			action(action), directory(directory), filename(filename), oldFilename(oldFilename)
		{
		};
	};

	ProjectObserver();

	// Dispatches any observer related events
	void pollEvents();

	// Updates the path of the project to observe
	void setTarget(const fs::path& path);

	// Returns the project structures root folder node
	const std::shared_ptr<Folder>& rootNode();

	// Returns a folder from the folder registry by its io id
	std::optional<const std::shared_ptr<Folder>> fetchFolder(uint32_t ioId);

private:
	// Listens to file watcher events
	struct IOListener : public efsw::FileWatchListener {
		void handleFileAction(efsw::WatchID watchId, const std::string& directory, const std::string& filename, efsw::Action action, std::string oldFilename) override;
		ConcurrentQueue<std::unique_ptr<IOEvent>> eventQueue;
	};

	// Creates the root folder structure for a given path recursively
	std::shared_ptr<Folder> createFolder(const fs::path& path);

	// Finds a folder node given a relative path
	std::shared_ptr<ProjectObserver::Folder> ProjectObserver::findFolder(const fs::path& relativePath, const std::shared_ptr<Folder>& currentFolder);

	// Project root path being observed
	fs::path target;

	// Root node of project structure representation
	std::shared_ptr<Folder> projectStructure;

	// Registry for all folders in project structure representation
	inline static std::unordered_map<uint32_t, const std::shared_ptr<Folder>> folderRegistry;

	std::unique_ptr<efsw::FileWatcher> watcher;
	std::unique_ptr<IOListener> listener;
	efsw::WatchID watchId;
};
#include "project_observer.h"

#include <utils/console.h>

bool ProjectObserver::Folder::hasSubfolders()
{
	return !subfolders.empty();
}

void ProjectObserver::Folder::addFile(const std::string& fileName)
{
	fs::path newPath = path / fileName;
	auto file = std::make_shared<File>(fileName, newPath);
	auto it = std::lower_bound(files.begin(), files.end(), file,
		[](const std::shared_ptr<File>& a, const std::shared_ptr<File>& b) {
			return a->name < b->name;
		});
	files.insert(it, file);
}

void ProjectObserver::Folder::addFolder(const std::string& folderName)
{
	fs::path newPath = path / folderName;
	auto folder = std::make_shared<Folder>(folderName, newPath);
	auto it = std::lower_bound(subfolders.begin(), subfolders.end(), folder,
		[](const std::shared_ptr<Folder>& a, const std::shared_ptr<Folder>& b) {
			return a->name < b->name;
		});
	subfolders.insert(it, folder);
	folderRegistry.emplace(folder->ioId, folder);
}

void ProjectObserver::Folder::removeFile(const std::string& fileName)
{
	files.erase(std::remove_if(files.begin(), files.end(),
		[&](const std::shared_ptr<File>& file) { return file->name == fileName; }),
		files.end());
}

void ProjectObserver::Folder::removeFolder(const std::string& folderName)
{
	subfolders.erase(std::remove_if(subfolders.begin(), subfolders.end(),
		[&](const std::shared_ptr<Folder>& folder) { return folder->name == folderName; }),
		subfolders.end());
}

void ProjectObserver::Folder::removeAny(const std::string& name)
{
	removeFile(name);
	removeFolder(name);
}

void ProjectObserver::Folder::print(uint32_t depth)
{
	std::string indent(depth * 3, ' ');
	std::string line(name.size(), '-');

	Console::print >> indent >> "--" >> line >> "--" >> Console::endl;
	Console::print >> indent >> "| " >> name >> " |" >> Console::endl;
	Console::print >> indent >> "--" >> line >> "--" >> Console::endl;

	for (const auto& file : files) {
		Console::print >> indent >> "- " >> file->name >> Console::endl;
	}

	for (const auto& subfolder : subfolders) {
		subfolder->print(depth + 1);
	}
}


ProjectObserver::ProjectObserver() : target(),
projectStructure(nullptr),
watcher(std::make_unique<efsw::FileWatcher>()),
listener(std::make_unique<IOListener>()),
watchId(0)
{
}

void ProjectObserver::pollEvents()
{
	// Only dispatch one io listener event per poll

	// Fetch next io listener event
	std::unique_ptr<IOEvent> event;
	bool success = listener->eventQueue.try_dequeue(event);
	if (!success) return;

	// Get event related data
	std::string filename = event->filename;
	fs::path absolutePath = fs::path(event->directory) / event->filename;
	fs::path relativePath = fs::relative(absolutePath, target);

	switch (event->action) {
	case efsw::Action::Add:
	{
		fs::path parentRelative = relativePath.parent_path();

		auto parentFolder = findFolder(parentRelative, projectStructure);
		if (!parentFolder) break;

		if (fs::is_directory(absolutePath))
			parentFolder->addFolder(filename);
		else
			parentFolder->addFile(filename);

		break;
	}
	case efsw::Action::Delete:
	{
		fs::path parentRelative = relativePath.parent_path();

		auto parentFolder = findFolder(parentRelative, projectStructure);
		if (!parentFolder) break;

		parentFolder->removeAny(filename);

		break;
	}
	case efsw::Action::Modified:
		break;
	case efsw::Action::Moved:
	{
		fs::path oldRelativePath(event->oldFilename);
		fs::path oldParentRelative = oldRelativePath.parent_path();
		auto oldParentFolder = findFolder(oldParentRelative, projectStructure);
		if (!oldParentFolder) break;

		std::string oldBaseName = oldRelativePath.filename().string();
		oldParentFolder->removeAny(oldBaseName);

		std::string newBaseName = absolutePath.filename().string();
		fs::path newParentRelative = relativePath.parent_path();
		auto newParentFolder = findFolder(newParentRelative, projectStructure);
		if (!newParentFolder) break;

		if (fs::is_directory(absolutePath))
			newParentFolder->addFolder(newBaseName);
		else
			newParentFolder->addFile(newBaseName);
		
		break;
	}
	}
}

void ProjectObserver::setTarget(const fs::path& path)
{
	target = path;

	folderRegistry.clear();
	projectStructure = createFolder(target);

	if (watchId)
		watcher->removeWatch(watchId);

	watchId = watcher->addWatch(target.string(), listener.get(), true);

	if (!watchId)
		Console::out::warning("Project Observer", "Could not initiate file watcher process");

	watcher->watch();
}

const std::shared_ptr<ProjectObserver::Folder>& ProjectObserver::rootNode()
{
	return projectStructure;
}

std::optional<const std::shared_ptr<ProjectObserver::Folder>> ProjectObserver::fetchFolder(uint32_t ioId)
{
	auto it = folderRegistry.find(ioId);

	if (it != folderRegistry.end())
		return it->second;

	return std::nullopt;
}

void ProjectObserver::IOListener::handleFileAction(efsw::WatchID watchId, const std::string& directory, const std::string& filename, efsw::Action action, std::string oldFilename)
{
	auto event = std::make_unique<IOEvent>(action, directory, filename, oldFilename);
	bool success = eventQueue.try_enqueue(std::move(event));
	if (!success)
		Console::out::warning("Project Observer", "Failed to enqueue an io event");
}

std::shared_ptr<ProjectObserver::Folder> ProjectObserver::createFolder(const fs::path& path)
{
	auto root = std::make_shared<Folder>(path.filename().string(), path);
	folderRegistry.emplace(root->ioId, root);

	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.is_directory())
			root->subfolders.push_back(createFolder(entry.path()));
		else if (entry.is_regular_file())
			root->addFile(entry.path().filename().string());
	}

	return root;
}

std::shared_ptr<ProjectObserver::Folder> ProjectObserver::findFolder(const fs::path& relativePath, const std::shared_ptr<Folder>& currentFolder)
{
	if (!currentFolder) {
		Console::out::warning("Project Observer", "Attempted to search from an invalid folder");
		return nullptr;
	}

	// No path left, return root folder
	if (relativePath.empty() || relativePath == ".")
		return currentFolder;

	// First component in relative path is immediate subfolder name
	auto it = relativePath.begin();
	std::string targetName = it->string();

	// Find subfolder with target name
	for (const auto& subfolder : currentFolder->subfolders) {
		if (subfolder->name == targetName) {
			// Compute remaining path by advancing iterator
			fs::path remaining;
			for (++it; it != relativePath.end(); ++it)
				remaining /= *it;
			// Recurse finding target folder
			return findFolder(remaining, subfolder);
		}
	}

	// Folder not found
	Console::out::warning("Project Observer", "Some parent folder wasn't found");
	return nullptr;
}
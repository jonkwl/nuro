#pragma once

#include <queue>
#include <atomic>
#include <cstdint>
#include <optional>
#include <functional>

class Resource;
class ResourceManager;

enum class TaskFlags : uint32_t {
	None = 0,
	UseContextThread = 1 << 0 // Task will be synchronously executed on the rendering context thread
};

constexpr TaskFlags operator|(TaskFlags a, TaskFlags b) {
	return static_cast<TaskFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

constexpr bool operator&(TaskFlags a, TaskFlags b) {
	return (static_cast<uint32_t>(a) & static_cast<uint32_t>(b)) != 0;
}

// Constructs a resource task by binding a member function of the current class
#define BIND_TASK(_class, _member) ResourceTask(std::bind(&_class::_member, this))

// Constructs a resource task by binding a member function of the current class with specified task flags
#define BIND_TASK_WITH_FLAGS(_class, _member, _flags) ResourceTask(std::bind(&_class::_member, this), _flags)

class ResourceTask {
public:
	using TaskFunc = std::function<bool()>;

	// The function defining the task needs to return a bool indicating success and can't take any arguments
	template <typename Func>
	ResourceTask(Func func, TaskFlags flags = TaskFlags::None) : func(func), flags(flags) {}

private:
	// Only the resource manager should be able to access and execute the function associated with a task
	friend class ResourceManager;

	// Function that defines the task to be executed
	TaskFunc func;

	// Configuration of the task
	TaskFlags flags;
};

using NextTask = std::optional<ResourceTask>;

class ResourcePipe {
public:
	ResourcePipe& operator>>(ResourceTask&& task) {
		tasks.emplace(task);
		return *this;
	}

	// Returns the next task in the pipe, if it exists
	NextTask next() {
		if (tasks.empty()) return std::nullopt;
		NextTask task = std::move(tasks.front());
		tasks.pop();
		return task;
	}

	// Returns the id of the resource owning the pipe
	uint32_t owner() {
		return ownerId;
	}

private:
	// Resource pipe should always be related to a resource and thus is only constructible by a resource base
	friend class Resource;
	explicit ResourcePipe(uint32_t ownerId) : ownerId(ownerId), tasks() {};

	// Resource id of the resource owning this pipe
	uint32_t ownerId;

	// Queue of tasks to be executed
	std::queue<ResourceTask> tasks;
};
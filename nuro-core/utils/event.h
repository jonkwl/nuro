#pragma once

#include <memory>
#include <vector>

#include <utils/callback.h>

template <typename... Args>
class Event {
public:
	using FuncType = std::function<void(Args...)>;
	using CallbackType = Callback<Args...>;
	using CallbackPointer = std::shared_ptr<CallbackType>;

	CallbackPointer subscribe(CallbackType callback) {
		auto callbackPointer = std::make_shared<CallbackType>(std::move(callback));
		callbacks.emplace_back(callbackPointer);
		return callbackPointer;
	}

	CallbackPointer subscribe(FuncType func) {
		auto callbackPointer = std::make_shared<CallbackType>(std::move(func));
		callbacks.emplace_back(callbackPointer);
		return callbackPointer;
	}

	void unsubscribe(CallbackPointer callback) {
		callbacks.erase(std::remove(callbacks.begin(), callbacks.end(), callback), callbacks.end());
	}

	void operator()(Args... args) const {
		for (const auto& callback : callbacks) {
			if (callback) {
				(*callback)(std::forward<Args>(args)...);
			}
		}
	}

private:
	std::vector<CallbackPointer> callbacks;
};
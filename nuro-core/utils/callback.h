#include <functional>

template <typename... Args>
class Callback
{
public:
	using FuncType = std::function<void(Args...)>;

	Callback() = default;
	Callback(FuncType func) : callback(std::move(func)) {};

	void operator()(Args... args) const 
	{
		if (callback) callback(std::forward<Args>(args)...);
	}

	explicit operator bool() const noexcept {
		return static_cast<bool>(callback);
	}

private:
	FuncType callback;
};
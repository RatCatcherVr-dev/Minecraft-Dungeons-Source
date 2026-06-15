#pragma once

#include <memory>

template <typename Func>
Func makeSharedFunc1(Func f) {
	return [shared = std::make_shared<Func>(std::move(f))](const auto& arg) {
		return (*shared)(arg);
	};
}

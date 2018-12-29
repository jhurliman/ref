#pragma once

#include <third_party/optional.hpp>

namespace ref {

template<typename T>
using Optional = std::experimental::optional<T>;

static constexpr auto Nullopt = std::experimental::nullopt;

}  // namespace ref

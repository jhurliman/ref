#pragma once

#include "Assert.h"

#include <third_party/expected.hpp>

namespace ref {

template<typename T>
class [[nodiscard]] Result {
public:
    Result(const T& value) : _result(value) {}
    Result(T && value) : _result(std::move(value)) {}
    Result(std::exception error) : _result(tl::unexpected<std::exception>(std::move(error))) {}

    bool isOk() const {
        return _result.has_value();
    }

    constexpr const T& value() const& {
        return _result.value();
    }

    constexpr T& value()& {
        return _result.value();
    }

    constexpr const T&& value() const&& {
        return std::move(_result.value());
    }

    constexpr T&& value()&& {
        return std::move(_result.value());
    }

    T&& moveValue() {
        return std::move(*_result);
    }

    constexpr const std::exception& error() const& {
        return _result.error();
    }

    constexpr std::exception& error()& {
        return _result.error();
    }

    constexpr const std::exception&& error() const&& {
        return std::move(_result.error());
    }

    constexpr std::exception&& error()&& {
        return std::move(_result.error());
    }

private:
    tl::expected<T, std::exception> _result;
};

/// Template specialization for void, a Result that does not wrap a return value.
template<>
class [[nodiscard]] Result<void> {
public:
    Result() : _result() {}
    Result(std::exception error) : _result(tl::unexpected<std::exception>(std::move(error))) {}

    ~Result() {
#ifdef REF_DEBUG
        REF_ASSERT(_checked, "Result was never checked");
#endif
    }

    bool isOk() const {
        markChecked();
        return _result.has_value();
    }

    void ignoreError() {
        markChecked();
    }

    constexpr const std::exception& error() const& {
        return _result.error();
    }

    constexpr std::exception& error()& {
        return _result.error();
    }

    constexpr const std::exception&& error() const&& {
        return std::move(_result.error());
    }

    constexpr std::exception&& error()&& {
        return std::move(_result.error());
    }

private:
    tl::expected<void, std::exception> _result;
#ifdef REF_DEBUG
    mutable bool _checked = false;
#endif

    void markChecked() const {
#ifdef REF_DEBUG
        _checked = true;
#endif
    }
};

}  // namespace ref

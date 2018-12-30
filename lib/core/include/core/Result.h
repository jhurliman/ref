#pragma once

#include "Assert.h"

#include <third_party/expected.hpp>

namespace std {

template<class T, class E>
using expected = tl::expected<T, E>;

template<class E>
using unexpected = tl::unexpected<E>;

}  // namespace std

namespace ref {

template<typename T>
class [[nodiscard]] Result {
public:
    Result(const T& value) : _result(value) {}
    Result(T && value) : _result(std::move(value)) {}
    Result(std::exception error) : _result(std::unexpected<std::exception>(std::move(error))) {}

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

    constexpr const T& value() const& {
#ifdef REF_DEBUG
        REF_ASSERT(_checked, "Unchecked value access");
#endif
        return _result.value();
    }

    constexpr T& value()& {
#ifdef REF_DEBUG
        REF_ASSERT(_checked, "Unchecked value access");
#endif
        return _result.value();
    }

    constexpr T&& moveValue() {
#ifdef REF_DEBUG
        REF_ASSERT(_checked, "Unchecked value access");
#endif
        return std::move(_result.value());
    }

    constexpr const std::exception& error() const& {
        return _result.error();
    }

    constexpr std::exception& error()& {
        return _result.error();
    }

private:
    std::expected<T, std::exception> _result;
#ifdef REF_DEBUG
    mutable bool _checked = false;
#endif

    void markChecked() const {
#ifdef REF_DEBUG
        _checked = true;
#endif
    }
};

/// Template specialization for void, a Result that does not wrap a return value.
template<>
class [[nodiscard]] Result<void> {
public:
    Result() : _result() {}
    Result(std::exception error) : _result(std::unexpected<std::exception>(std::move(error))) {}

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

private:
    std::expected<void, std::exception> _result;
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

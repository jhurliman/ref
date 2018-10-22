#include "core/WildcardMatch.h"

#include <third_party/wildcards.hpp>

namespace ref {

bool WildcardMatch(const std::string& haystack, const std::string& pattern) {
    return wildcards::match(haystack, pattern);
}

}  // namespace ref

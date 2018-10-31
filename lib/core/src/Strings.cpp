#include "core/Strings.h"

namespace ref {

bool StartsWith(const std::string& haystack, const std::string& prefix) {
    if (!prefix.size() || prefix.size() > haystack.size()) {
        return false;
    }
    return std::equal(prefix.begin(), prefix.end(), haystack.begin());
}

}

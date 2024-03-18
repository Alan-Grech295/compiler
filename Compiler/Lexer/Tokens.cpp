#include "Tokens.h"

namespace Tokens
{
#define X(keyword, _) #keyword,
    const std::unordered_set<std::string> Keyword::keywords = {
        KEYWORDS
    };
#undef X

#define X(name, _) #name,
    const std::unordered_set<std::string> Builtin::builtinTypes = {
        BUILTIN_TYPES
    };
#undef X
}
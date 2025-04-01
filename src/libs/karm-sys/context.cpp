#include "context.h"

namespace marK::Sys {

Context& globalContext() {
    static Context ctx;
    return ctx;
}

} // namespace marK::Sys

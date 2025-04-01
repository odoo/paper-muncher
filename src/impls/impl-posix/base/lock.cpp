#include <karm-base/_embed.h>

namespace marK::_Embed {

void relaxe() {
#if defined(__x86_64__)
    asm volatile("pause");
#endif
}

void enterCritical() {
    // NOTE: We don't do any thread so we don't need to do anything here.
}

void leaveCritical() {
    // NOTE: We don't do any thread so we don't need to do anything here.
}

} // namespace marK::_Embed

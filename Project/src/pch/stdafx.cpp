#include "stdafx.h"

#ifdef TRACY_ENABLE
void* operator new(std::size_t sz) {
    auto ptr = malloc(sz);
    TracyAlloc(ptr, sz);   // <-- tells Tracy about the allocation
    return ptr;
}

void operator delete(void* ptr) noexcept {
    TracyFree(ptr);        // <-- tells Tracy about the free
    free(ptr);
}
#endif

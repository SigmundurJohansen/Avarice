#pragma once

#define VMA_IMPLEMENTATION

#ifdef __clang__
    // This file is riddled with compiler warnings that we want to suppress. So we move our vulkan includes into here.
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Weverything"
#endif

#include <vk_mem_alloc.h>

#ifdef __clang__
    #pragma clang diagnostic pop
#endif
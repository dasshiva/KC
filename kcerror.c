#include "kcprivate.h"
#include "kc.h"

#define VK_ERROR_VALIDATION_FAILED (-1000011001)

int KC_Error(VkResult result) {
    switch (result) {
        case VK_ERROR_OUT_OF_HOST_MEMORY: return KC_ERROR_OUT_OF_MEMORY;
        case VK_ERROR_VALIDATION_FAILED: return KC_IMPLEMENTATION_ERROR;
        case VK_ERROR_UNKNOWN: 
        default: return KC_UNKNOWN_ERROR;
    }

}
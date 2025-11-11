#include <vulkan/vulkan.h>
#include <stdio.h>

#include "kc.h"

int main(int argc, const char* argv[]) {
    KC_Handle handle;
    int ret = KC_Init(&handle);

    if (ret != KC_SUCCESS) {
        printf("Failed to initialize library = %d", ret);
        return ret;
    }

    KC_Destroy(handle);
    return 0;
}
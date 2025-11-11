#include "kc.h"
#include "kcprivate.h"
#include <stdlib.h>
#include <vulkan/vulkan_core.h>


typedef struct KC_Internal_Handle {
    VkInstance instance;
    uint32_t version;
    VkDevice device;
} KC_Internal_Handle;

int KC_Init(KC_Handle* handle) {
    VkInstance instance;
    uint32_t version;
    VkResult result;

    result = vkEnumerateInstanceVersion(&version);
    if (result != VK_SUCCESS) 
        return KC_Error(result);

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "KC",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName = "KCENGINE",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = version
    };

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = NULL
    };

    result = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
    if (result != VK_SUCCESS)
        return KC_Error(result);

    KC_Internal_Handle* ret = malloc(sizeof(KC_Internal_Handle));
    ret->instance = instance;
    ret->version = version;

    *handle = ret;
    return KC_SUCCESS;
}

void KC_Destroy(KC_Handle h) {
    KC_Internal_Handle* handle = h;
    if (!h)
        return;

    vkDestroyInstance(handle->instance, NULL);
}
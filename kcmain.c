#include "kc.h"
#include "kcprivate.h"
#include <stdint.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

typedef struct vkstruct {
    VkStructureType type;
    void* pNext;
} vkstruct;

typedef struct KC_Internal_Handle {
    VkInstance instance;
    uint32_t version;
    uint32_t flags;
    uint32_t qf;
    VkDevice device;
    VkPhysicalDeviceLimits limits;
    VkQueue queue;
} KC_Internal_Handle;

int KC_Init(KC_Handle* handle) {
    VkInstance instance;
    uint32_t version;
    uint32_t flags;
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

    uint32_t nphysicalDevices = 0;
    result = vkEnumeratePhysicalDevices(instance, &nphysicalDevices, NULL);
    if (result != VK_SUCCESS)
        return KC_Error(result);

    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * nphysicalDevices);
    vkEnumeratePhysicalDevices(instance, &nphysicalDevices, devices);

    uint32_t selectedDevice = UINT32_MAX;

    for (uint32_t idx = 0; idx < nphysicalDevices; idx++) {
        VkPhysicalDeviceProperties tmp = {0};
        vkGetPhysicalDeviceProperties(devices[idx], &tmp);
        uint32_t deviceApiVersion = tmp.apiVersion;
        if (tmp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || 
                tmp.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
            selectedDevice = idx;
            break;
        }
    }

    if (selectedDevice == UINT32_MAX)
        return KC_NO_SUITABLE_DEVICE;

    VkPhysicalDeviceProperties devProps = {0};
    vkGetPhysicalDeviceProperties(devices[selectedDevice], &devProps);
    VkPhysicalDeviceLimits limits = devProps.limits;

    
    KC_Internal_Handle* ret = malloc(sizeof(KC_Internal_Handle));
    ret->instance = instance;
    ret->version = version;
    memcpy(&ret->limits, &limits, sizeof(VkPhysicalDeviceLimits));

    float prio = 1.0f;
    uint32_t queues = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(devices[selectedDevice], &queues, NULL);
    if (!queues)
        return KC_DEAD_DEVICE;

    VkQueueFamilyProperties* families = malloc(queues * sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(devices[selectedDevice], &queues, families);

    uint32_t selectedQueueFamily = UINT32_MAX;
    for (uint32_t idx = 0; idx < queues; idx++) {
        VkQueueFamilyProperties family = families[idx];
        if (family.queueFlags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT)) {
            selectedQueueFamily = idx;
            break;
        }
    }

    if (selectedQueueFamily == UINT32_MAX)
        return KC_NO_COMPUTE_SUPPORT;

    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueFamilyIndex = selectedQueueFamily,
        .queueCount = 1,
        .pQueuePriorities = &prio
    };

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = NULL,
        .pEnabledFeatures = NULL
    };

    result = vkCreateDevice(devices[selectedDevice], &deviceCreateInfo, NULL, &ret->device);
    if (result != VK_SUCCESS)
        return KC_Error(result);

    ret->qf = selectedQueueFamily;
    vkGetDeviceQueue(ret->device, ret->qf, 0, &ret->queue);
    
    *handle = ret;
    return KC_SUCCESS;
}

void KC_Destroy(KC_Handle h) {
    KC_Internal_Handle* handle = h;
    if (!h)
        return;

    vkDestroyInstance(handle->instance, NULL);
}
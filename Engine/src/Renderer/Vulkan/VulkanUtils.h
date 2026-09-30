#pragma once
#include "VulkanDef.inl"

/**
 * Returns the string representation of result.
 * @param Result The result to get the string for.
 * @param GetExtended Indicates whether to also return an extended result.
 * @returns The error code and/or extended error message in string form. Defaults to success for unknown result types.
 */
const char* VulkanResultString(VkResult Result, Bool8 GetExtended);

/**
 * Indicates if the passed result is a success or an error as defined by the Vulkan spec.
 * @returns True if success; otherwise false. Defaults to true for unknown result types.
 */
Bool8 VulkanResultIsSuccess(VkResult Result);
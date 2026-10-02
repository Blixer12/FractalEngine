#pragma once
#include "VulkanDef.inl"

/**
 * Returns the string representation of Result.
 * @param Result The Result to get the string for.
 * @param GetExtended Indicates whether to also return an extended Result.
 * @returns The error code and/or extended error message in string form. Defaults to success for unknown Result types.
 */
const char* VulkanResultString(VkResult Result, Bool8 GetExtended);

/**
 * Indicates if the passed Result is a success or an error as defined by the Vulkan spec.
 * @returns True if success; otherwise false. Defaults to true for unknown Result types.
 */
Bool8 VulkanResultIsSuccess(VkResult Result);
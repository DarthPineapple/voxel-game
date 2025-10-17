# Code Style Guide

## File Organization

### Header Files (.h)
- Use include guards (#ifndef/#define/#endif)
- Forward declare classes when possible to reduce dependencies
- Order includes: system headers, then project headers
- Group public/private members logically

### Source Files (.cpp)
- Include corresponding header first
- Group related functionality together
- Keep functions focused and single-purpose

### File Naming
- Use snake_case for multi-word files (e.g., `vulkan_instance.h`)
- Match header and source file names exactly
- Place related files in the same directory

## Class Design

### Initialization and Cleanup
- Use RAII (Resource Acquisition Is Initialization)
- Pair init() with cleanup() methods explicitly
- Call cleanup() in destructors for safety
- Set handles to VK_NULL_HANDLE after destruction

### Modularity
- Keep classes focused on a single responsibility
- Prefer composition over inheritance
- Use dependency injection (pass dependencies to methods/constructors)

## Vulkan-Specific Guidelines

### Error Handling
- Check all Vulkan function return values
- Throw std::runtime_error with descriptive messages
- Clean up resources before throwing

### Resource Management
- Always clean up Vulkan objects in reverse order of creation
- Use vkDeviceWaitIdle() before destroying device resources
- Check for VK_NULL_HANDLE before destruction

### Platform Compatibility
- Use #ifdef __APPLE__ for macOS-specific code
- Add VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME for MoltenVK
- Define fallback macros for older Vulkan headers

## File Size Guidelines

- Keep individual files under 200 lines when possible
- Split large classes into multiple files if they exceed 300 lines
- Extract helper functions into utility files
- Consider creating separate classes for distinct functionality

## Code Quality

### Readability
- Use descriptive variable and function names
- Add comments only when code intent is not obvious
- Keep functions short and focused (under 50 lines preferred)

### Maintainability
- Avoid code duplication
- Extract common patterns into helper functions
- Document non-obvious design decisions

### Performance
- Prefer const references for read-only parameters
- Use std::move for transferring ownership
- Cache expensive calculations when appropriate

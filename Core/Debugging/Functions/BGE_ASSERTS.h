#pragma once
#include "ErrorHandling/ErrorMessage.h"
#include <format>
#include <string>
#include <source_location>
#include "GLOBALS.h"

#if DIRECTX11 == 1
	#include <comdef.h>
#endif

#include "GraphicsBackends.h"

#ifdef _DEBUG
	template<typename T>
	void BGE_ASSERT_PTR(T* ptr, const std::string& message,
		const std::source_location& location = std::source_location::current()) {
		if (ptr == nullptr) {
			std::string errorMsg = std::format("UGE_ASSERT failed: {} is nullptr!\nMessage: {}\nFile: {} (line: {})",
				ptr, message,
				location.file_name(), location.line());
			MakeAError(errorMsg);
		}
	}

	template<typename T>
	void BGE_ASSERT_CONDITION_IMPL(T&& condition, const std::string& message,
		const std::string& conditionStr,
		const std::source_location& location = std::source_location::current()) {
		if (!condition) {
			std::string errorMsg = std::format("BGE_ASSERT failed: {} is false!\nMessage: {}\nFile: {} (line: {})",
				conditionStr, message,
				location.file_name(), location.line());
			CreateError(errorMsg);
		}
	}


	template<typename Result>
	void BGE_ASSERT_VKRESULT_IMPL(Result result, const std::string& message,
		const std::string& conditionStr = "",
		const std::source_location& location = std::source_location::current()) {

		#if VULKAN == 1
				if (result != VK_SUCCESS) {
					std::string errorName;
					switch (result) {
					case VK_ERROR_OUT_OF_HOST_MEMORY: errorName = "VK_ERROR_OUT_OF_HOST_MEMORY"; break;
					case VK_ERROR_OUT_OF_DEVICE_MEMORY: errorName = "VK_ERROR_OUT_OF_DEVICE_MEMORY"; break;
					case VK_ERROR_INITIALIZATION_FAILED: errorName = "VK_ERROR_INITIALIZATION_FAILED"; break;
					case VK_ERROR_DEVICE_LOST: errorName = "VK_ERROR_DEVICE_LOST"; break;
					case VK_ERROR_OUT_OF_DATE_KHR: errorName = "VK_ERROR_OUT_OF_DATE_KHR"; break;
					case VK_ERROR_SURFACE_LOST_KHR: errorName = "VK_ERROR_SURFACE_LOST_KHR"; break;
					default: errorName = std::format("VkResult code: {}", static_cast<int>(result)); break;
					}

					std::string errorMsg = std::format(
						"BGE_ASSERT_VKRESULT failed: {}\n"
						"Result: {}\n"
						"Message: {}\n"
						"Location: {} (line: {})",
						conditionStr.empty() ? errorName : conditionStr,
						errorName,
						message,
						location.file_name(),
						location.line()
					);

					CreateError(errorMsg);
				}
		#endif
	}

	template<typename HResult>
	void BGE_ASSERT_HRESULT_IMPL(
		HResult result,
		const std::string& message,
		const std::string& conditionStr = "",
		const std::source_location& location = std::source_location::current())
	{
		#if DIRECTX11 == 1

			if (FAILED(result))
			{
				_com_error err(result);
				std::string errorName = err.ErrorMessage();

				std::string errorMsg = std::format(
					"BGE_ASSERT_HRESULT failed: {}\n"
					"Result: {}\n"
					"Message: {}\n"
					"Location: {} (line: {})",
					conditionStr.empty() ? errorName : conditionStr,
					errorName,
					message,
					location.file_name(),
					location.line()
				);

				CreateError(errorMsg);
			}

		#endif
	}
	template<typename T>
	void BGE_ASSERT_VK_HANDLE_IMPL(T handle, const std::string& message,
		const std::source_location& location = std::source_location::current()) {
		#if VULKAN == 1
				if (handle == VK_NULL_HANDLE) {
					std::string errorMsg = std::format("BGE_ASSERT failed: Vulkan handle is VK_NULL_HANDLE!\nMessage: {}\nFile: {} (line: {})",
						message,
						location.file_name(), location.line());
					CreateError(errorMsg);
				}
		#endif
	}

	#define BGE_VK_ASSERT(handle, message) \
		BGE_ASSERT_VK_HANDLE_IMPL(handle, message)

	#define BGE_ASSERT(condition, message) \
		BGE_ASSERT_CONDITION_IMPL(condition, message, #condition)

	#define BGE_ASSERT_VKRESULT(VkResult, message) \
		BGE_ASSERT_VKRESULT_IMPL(VkResult, message, #VkResult)

	#define BGE_ASSERT_HRESULT(HResult, message) \
		BGE_ASSERT_HRESULT_IMPL(HResult, message, #HResult)
	
#else
	#define BGE_ASSERT_VKRESULT(VkResult, message) (VkResult)
	#define BGE_VK_ASSERT(handle, message) ((void)(handle))
	#define BGE_ASSERT(condition, message) ((void)0)
	#define BGE_ASSERT_HRESULT(expr, message) ((void)(expr))
#endif
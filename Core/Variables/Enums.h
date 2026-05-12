#pragma once

#include <cstdint>

namespace Boron {
	namespace Enums {
		enum class InstanceType : uint8_t {
			None = 0,
			Object,
			Instance,
			World
		};
	}
}
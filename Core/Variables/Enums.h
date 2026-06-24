#pragma once

#include <cstdint>

namespace Boron {
	namespace Enums {
		extern enum class InstanceType : uint8_t {
			None = 0,
			Object,
			Instance,
			World
		};
	}
	namespace Editor {
		extern enum class ThemeType : uint8_t {
			None = 0,
			Dark,
			Light
		};
	}
}
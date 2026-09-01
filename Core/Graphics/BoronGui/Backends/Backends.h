#pragma once

#include "GLOBALS.h"
#include "BoronGuiTypes.h"

namespace BoronGuiBackends {
	class Backends {
	public:
		virtual ~Backends() = default;
		virtual void RenderAFrame() = 0;
		virtual void Init() = 0;
		virtual void SetBoronGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) = 0;
		virtual void UpdatePerFrameOBJ(PerFrameStuct& p_perFrameStuct) = 0;
	private:

	};
};


#include "BoronGui.h"
#include "BGE_ASSERTS.h"
#include "Logger/Logger.h"
#include "Backends/Backends.h"

std::unique_ptr<BoronGuiBackends::Backends> BoronGui::m_backend{};
std::vector<Borongui::Widget*> BoronGui::widgets{};

std::vector<Vertex2d> BoronGui::m_vertices{};
std::vector<uint32_t> BoronGui::m_indicies{};

void BoronGui::UpdatePerFrameOBJ(PerFrameStuct& p_perFrameStuct) {
	m_backend->UpdatePerFrameOBJ(p_perFrameStuct);
}

void BoronGui::InitBoronGui(BoronGuiNeeds& p_boronGuiNeeds) {
	bool Inited = false;

	#if VULKAN == 1
		m_backend = std::make_unique<BoronGui_implVulkan>();
		Inited = true;
	#endif

	#if DIRECTX11 == 1

	#endif

	if (Inited) {
		m_backend->SetBoronGuiNeeds(p_boronGuiNeeds);
		m_backend->Init();

		CreateInfo("Initing BoronGui");
	}
}

void BoronGui::SubmitWidget(Borongui::Widget& p_widget) {
	BGE_ASSERT_PTR(&p_widget, "Widget cannot be nullptr!");

	widgets.push_back(&p_widget);
}

void BoronGui::EndFrame() {
	m_indicies.clear();
	m_vertices.clear();

	widgets.clear();
}

void BoronGui::ReSizeViewport(GPUVector2 p_newSize) {
	m_backend->ReSizeViewport(p_newSize);
}

void BoronGui::RenderAFrame(Borongui::Frame& frame) {
	m_backend->RenderAFrame(frame);
}

void BoronGui::DrawWidgets() {
	for (Borongui::Widget* widget : widgets) {
		uint32_t vertexOffset = static_cast<uint32_t>(m_vertices.size());
		
		if (auto frame = dynamic_cast<Borongui::Frame*>(widget)) {
			GPUVector3 color = GPUVector3(frame->getColor().x() / 255.0f, frame->getColor().y() / 255.0f, frame->getColor().z() / 255.0f);

			for (auto& vertex : frame->getVertices()) {
				Vertex2d guiVertex{};
				guiVertex.color = color;
				guiVertex.pos = vertex.pos;
				guiVertex.size = GPUVector2(frame->getSize().x(), frame->getSize().y());
				guiVertex.rounding = frame->getRounding();

				m_vertices.push_back(guiVertex);
			}

			for (auto& index : frame->getIndices()) {
				m_indicies.push_back(vertexOffset + index);
			}

			widget->Render();
		}
	}

	m_backend->UploadBatch(m_vertices, m_indicies);
	m_backend->DrawBatch();
}

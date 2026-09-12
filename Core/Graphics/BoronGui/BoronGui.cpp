#include "BoronGui.h"
#include "BGE_ASSERTS.h"
#include "Logger/Logger.h"
#include "Backends/Backends.h"

#include "Mouse/Mouse.h"

std::unique_ptr<BoronGuiBackends::Backends> BoronGui::m_backend{};
std::vector<Borongui::Widget*> BoronGui::widgets{};

std::vector<Vertex2d> BoronGui::m_vertices{};
std::vector<uint32_t> BoronGui::m_indicies{};
bool BoronGui::m_inited = false;

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
    if (m_inited) {
        m_backend->ReSizeViewport(p_newSize);
    }
}

bool checkAABB(BML::Vec2 p_a, Borongui::Widget& p_widget) {
	return (
		p_a.x() >= p_widget.m_position.x() &&
		p_a.x() <= p_widget.m_position.x() + p_widget.m_size.x() &&
		p_a.y() >= p_widget.m_position.y() &&
		p_a.y() <= p_widget.m_position.y() + p_widget.m_size.y()
	); //Ty google <:
}

void BoronGui::DrawWidgets() {
    static bool isDragging = false;
    static bool isUsing = false;

    isUsing = false;

    std::sort(widgets.begin(), widgets.end(), //z-Index sorting
        [](Borongui::Widget* a, Borongui::Widget* b) {
            return a->m_zIndex < b->m_zIndex;
        }
    );

    for (auto it = widgets.rbegin(); it != widgets.rend(); ++it) { //input loop
        Borongui::Widget* widget = *it;

        widget->m_isHovered = false;

        BML::Vec2 mousePos = Mouse::getMousePos();

        mousePos = BML::Vec2(
            mousePos.x(),
            screen_height - mousePos.y()
        );

        if (checkAABB(mousePos, *widget) && !isUsing) {
            widget->m_isHovered = true;
            isUsing = true;
            if (Mouse::isLeftClicked() && !isDragging) {
                isDragging = true;
                CreateInfo("ss");
                widget->m_previousZIndex = widget->m_zIndex;
                widget->m_zIndex = 9999;
                widget->m_isDragging = true;
                widget->m_isClicked = true;
            }
        }

        if (widget->m_isDragging) {
            widget->m_position += BML::Vec2(
                Mouse::getDelta().x(),
                -Mouse::getDelta().y()
            );

            if (!Mouse::isLeftClicked()) {
                widget->m_zIndex = widget->m_previousZIndex;
                widget->m_isDragging = false;
                isDragging = false;
                widget->m_isClicked = false;
            }
        }
    }

    std::sort(widgets.begin(), widgets.end(), //z-Index sorting
        [](Borongui::Widget* a, Borongui::Widget* b) {
            return a->m_zIndex < b->m_zIndex;
        }
    );

    for (auto it = widgets.begin(); it != widgets.end(); ++it) { //rendering loop
        Borongui::Widget* widget = *it;

        uint32_t vertexOffset =
            static_cast<uint32_t>(m_vertices.size());

        GPUVector4 color = GPUVector4(widget->getColor().x() / 255.0f, widget->getColor().y() / 255.0f,
            widget->getColor().z() / 255.0f, widget->getColor().w()
        );
        if (widget->isClicked()) {
            color = GPUVector4(widget->getClickColor().x() / 255.0f, widget->getClickColor().y() / 255.0f,
                widget->getClickColor().z() / 255.0f, widget->getClickColor().w()
            );
        }
        else if (widget->isHovered()) {
            color = GPUVector4(widget->getHoverColor().x() / 255.0f, widget->getHoverColor().y() / 255.0f,
                widget->getHoverColor().z() / 255.0f, widget->getHoverColor().w()
            );
        }

        for (auto& vertex : widget->getVertices()) {
            Vertex2d guiVertex{};

            guiVertex.color = color;
            guiVertex.pos = vertex.pos;
            guiVertex.size = GPUVector2(
                widget->getSize().x(),
                widget->getSize().y()
            );
            guiVertex.rounding = widget->getRounding();
            guiVertex.guiPos = GPUVector2(
                widget->getPosition().x(),
                widget->getPosition().y()
            );

            m_vertices.push_back(guiVertex);
        }

        for (auto index : widget->getIndices()) {
            m_indicies.push_back(vertexOffset + index);
        }
    }
    
    m_backend->UploadBatch(m_vertices, m_indicies);
    m_backend->DrawBatch();
}
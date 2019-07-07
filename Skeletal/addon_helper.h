#pragma once

namespace ImGui
{

// dummy functions
void BeginHorizontal(const void *, ImVec2 = ImVec2(0, 0), float x = 0)
{
}
void EndHorizontal()
{
}
void Spring()
{
}
void Spring(float)
{
}
void Spring(float, float)
{
}
void BeginVertical(const void *, ImVec2 = ImVec2(0, 0), float x = 0)
{
}
void EndVertical()
{
}
} // namespace ImGui

class Builder
{
    ax::NodeEditor::NodeId CurrentNodeId = 0;

public:
    Builder()
    // : m_w(w), m_h(h)
    {
    }

    void Begin(ax::NodeEditor::NodeId id)
    {
        ax::NodeEditor::BeginNode(id);
        ImGui::PushID(id.AsPointer());
        CurrentNodeId = id;
    }

    void End()
    {
        ax::NodeEditor::EndNode();
        CurrentNodeId = 0;
        ImGui::PopID();
    }

    void Header(const ImColor &color)
    {
    }

    void EndHeader()
    {
    }

    void Input(ax::NodeEditor::PinId id)
    {
        Pin(id, ax::NodeEditor::PinKind::Input);
    }

    void EndInput()
    {
        EndPin();
    }

    void Output(ax::NodeEditor::PinId id)
    {
        Pin(id, ax::NodeEditor::PinKind::Output);
    }

    void EndOutput()
    {
        EndPin();
    }

    void Pin(ax::NodeEditor::PinId pin, ax::NodeEditor::PinKind kind)
    {
        ax::NodeEditor::BeginPin(pin, kind);
    }

    void EndPin()
    {
        ax::NodeEditor::EndPin();
    }

    void Middle()
    {
    }
};

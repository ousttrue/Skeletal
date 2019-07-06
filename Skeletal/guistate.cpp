#include "guistate.h"
#include <scene.h>
#include <gles3renderer.h>
#include <imgui.h>
#include <imgui_widgets.cpp>
#include <ImGuizmo.h>
#include <memory>
#include <Windows.h>

static std::wstring OpenDialog()
{
    OPENFILENAME ofn;        // common dialog box structure
    TCHAR szFile[260] = {0}; // if using TCHAR macros

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (!GetOpenFileName(&ofn) == TRUE)
    {
        // use ofn.lpstrFile
        return L"";
    }

    return szFile;
}

struct NodeTreeDrawer
{
    // int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
    std::shared_ptr<agv::scene::Node> m_root;
    std::unordered_map<uint32_t, std::shared_ptr<agv::scene::Node>> m_selection;
    std::shared_ptr<agv::scene::Node> m_clicked;

    void DrawRecursive(const std::shared_ptr<agv::scene::Node> &node)
    {
        // Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (m_selection.find(node->get().GetID()) != m_selection.end())
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        auto hasChild = node->GetChildCount();
        if (!hasChild)
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
        }
        bool isOpen = ImGui::TreeNodeEx((void *)(int64_t)node->get().GetID(),
                                        node_flags, "%s", node->get().GetName().c_str());
        if (ImGui::IsItemClicked())
        {
            m_clicked = node;
        }

        if (hasChild && isOpen)
        {
            for (auto &child : *node)
            {
                DrawRecursive(child);
            }

            ImGui::TreePop();
        }
    }

    void
    Draw(const std::shared_ptr<agv::scene::Node> &node)
    {
        // clear
        if (node != m_root)
        {
            // clear selection
            m_root = node;
            m_selection.clear();
        }
        m_clicked = nullptr;

        // indent
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize());
        DrawRecursive(node);
        ImGui::PopStyleVar();

        if (m_clicked)
        {
            // update selection
            if (!ImGui::GetIO().KeyCtrl)
            {
                m_selection.clear();
            }
            m_selection[m_clicked->get().GetID()] = m_clicked;
        }
    }
};
static NodeTreeDrawer m_tree;

static void EditTransform(
    const dxm::Matrix &projection,
    const dxm::Matrix &view,
    dxm::Matrix *pM)
{
    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
    if (ImGui::IsKeyPressed(90))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(69))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(82)) // r Key
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(pM->data(), matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation, 3);
    ImGui::InputFloat3("Rt", matrixRotation, 3);
    ImGui::InputFloat3("Sc", matrixScale, 3);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, pM->data());

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;
        ImGui::SameLine();
        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }
    static bool useSnap(false);
    if (ImGui::IsKeyPressed(83))
        useSnap = !useSnap;
    ImGui::Checkbox("", &useSnap);
    ImGui::SameLine();
    DirectX::XMFLOAT3 snap = {0, 0, 0};
    switch (mCurrentGizmoOperation)
    {
    case ImGuizmo::TRANSLATE:
        // snap = config.mSnapTranslation;
        ImGui::InputFloat3("Snap", &snap.x);
        break;
    case ImGuizmo::ROTATE:
        // snap = config.mSnapRotation;
        ImGui::InputFloat("Angle Snap", &snap.x);
        break;
    case ImGuizmo::SCALE:
        // snap = config.mSnapScale;
        ImGui::InputFloat("Scale Snap", &snap.x);
        break;
    }
    ImGuiIO &io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(view.data(), projection.data(),
                         mCurrentGizmoOperation, mCurrentGizmoMode, pM->data(), NULL,
                         useSnap ? &snap.x : NULL);
}

void GuiState::Update(agv::scene::Scene *scene, agv::renderer::GLES3Renderer *renderer)
{
    ImGuizmo::BeginFrame();
    ImGui::Begin("scene", nullptr, ImGuiWindowFlags_MenuBar);
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open"))
                {
                    auto path = OpenDialog();
                    if (!path.empty())
                    {
                        scene->Load(path);
                    }
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::Text("time: %d", scene->GetTime());
        ImGui::Text("fps: %d", scene->GetFps());

        auto model = scene->GetModel();
        if (model)
        {
            m_tree.Draw(model->Root);
        }

        ImGui::End();
    }

    auto projection = scene->GetCamera()->GetMatrix();
    auto view = scene->GetCameraNode()->GetWorldMatrix();
    for (auto kv : m_tree.m_selection)
    {
        ImGui::Begin("selected");
        {
            auto model = kv.second->GetWorldMatrix();
            EditTransform(projection, view, &model);
            kv.second->SetWorldMatrix(model);
            ImGui::End();
        }
        break;
    }

    ImGui::Begin("assets");
    {
        auto model = scene->GetModel();
        if (model)
        {
            //ImGui::Text("generator: %s", m_gltf->asset.generator.c_str());

            //ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_Once);
            if (ImGui::TreeNode("textures"))
            {
                auto &textures = model->Textures;
                for (int i = 0; i < textures.size(); ++i)
                {
                    auto &texture = textures[i];
                    ImGui::PushID(texture->GetID());
                    bool isOpen = ImGui::TreeNode("%s", texture->GetName().c_str());
                    if (isOpen)
                    {
                        // draw image
                        ImGui::Image(renderer->GetTexture(texture->GetID()), ImVec2(100, 100));
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("materials"))
            {
                auto &materials = model->Materials;
                for (int i = 0; i < materials.size(); ++i)
                {
                    auto &material = materials[i];
                    ImGui::PushID(material->GetID());
                    bool isOpen = ImGui::TreeNode("%s", material->GetName().c_str());
                    if (isOpen)
                    {
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("meshes"))
            {
                auto &meshes = model->Meshes;
                for (int i = 0; i < meshes.size(); ++i)
                {
                    auto &mesh = meshes[i];
                    ImGui::PushID(mesh->GetID());
                    bool isOpen = ImGui::TreeNode("%s", mesh->GetName().c_str());
                    if (isOpen)
                    {
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("nodes"))
            {

                ImGui::Columns(2);
                for (int i = 0; i < model->Nodes.size(); ++i)
                {
                    auto &node = model->Nodes[i];
                    ImGui::PushID(node->get().GetID());

                    bool isOpen = ImGui::TreeNode("%s", node->get().GetName().c_str());
                    ImGui::NextColumn();
                    if (isOpen)
                    {
                        ImGui::Text("%03d", i);
                        ImGui::TreePop();
                    }
                    ImGui::PopID();
                    ImGui::NextColumn();
                }
                ImGui::Columns(1);

                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    ImGui::ShowDemoWindow();

    ImGui::Begin("logger", &loggerOpen);
    {
        ImGui::BeginChild("scrolling");
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 1));
        //if (copy) ImGui::LogToClipboard();

        /*
				if (Filter.IsActive())
				{
					const char* buf_begin = Buf.begin();
					const char* line = buf_begin;
					for (int line_no = 0; line != NULL; line_no++)
					{
						const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
						if (Filter.PassFilter(line, line_end))
							ImGui::TextUnformatted(line, line_end);
						line = line_end && line_end[1] ? line_end + 1 : NULL;
					}
				}
				else
				*/
        {
            ImGui::TextUnformatted(logger.c_str());
        }

        ImGui::SetScrollHere(1.0f);

        ImGui::PopStyleVar();
        ImGui::EndChild();
        ImGui::End();
    }
}

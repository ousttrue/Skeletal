#include "guistate.h"
#include <scene.h>
#include <gles3renderer.h>
#include <imgui.h>
#include <imgui_widgets.cpp>
#include <memory>
#include <Windows.h>
#include "addon.h"

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
    std::shared_ptr<agv::scene::Node> m_clicked;

    void DrawRecursive(const agv::scene::Scene *pScene, const std::shared_ptr<agv::scene::Node> &node)
    {
        // Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
        ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
        if (pScene->IsSelected(node->GetID()))
        {
            node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        auto hasChild = node->GetChildCount();
        if (!hasChild)
        {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
        }
        bool isOpen = ImGui::TreeNodeEx((void *)(int64_t)node->GetID(),
                                        node_flags, "%s", node->GetName().c_str());
        if (ImGui::IsItemClicked())
        {
            m_clicked = node;
        }

        if (hasChild && isOpen)
        {
            for (auto &child : *node)
            {
                DrawRecursive(pScene, child);
            }

            ImGui::TreePop();
        }
    }

    void
    Draw(agv::scene::Scene *pScene, const std::shared_ptr<agv::scene::Node> &node)
    {
        // clear
        m_clicked = nullptr;

        // indent
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize());
        DrawRecursive(pScene, node);
        ImGui::PopStyleVar();

        if (m_clicked)
        {
            // update selection
            if (!ImGui::GetIO().KeyCtrl)
            {
                pScene->ClearSelection();
            }
            pScene->Select(m_clicked);
        }
    }
};
static NodeTreeDrawer m_tree;

void GuiState::Update(agv::scene::Scene *scene, agv::renderer::GLES3Renderer *renderer)
{
    ////////////////////////////////////////////////////////////

    ImGui::ShowDemoWindow();

    if(ImGui::Begin("scene", &modelOpen, ImGuiWindowFlags_MenuBar))
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
            m_tree.Draw(scene, model->Root);
        }
    }
    ImGui::End();

    if(ImGui::Begin("assets", &assetsOpen, ImGuiWindowFlags_MenuBar))
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
                    ImGui::PushID(node->GetID());

                    bool isOpen = ImGui::TreeNode("%s", node->GetName().c_str());
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
    }
    ImGui::End();

    if(ImGui::Begin("logger", &loggerOpen, ImGuiWindowFlags_MenuBar))
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
    }
    ImGui::End();

    AddOn();
}

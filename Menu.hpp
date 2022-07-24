#pragma once

#include "imgui.h"
#include "backend/imgui_impl_glfw.h"
#include "backend/imgui_impl_opengl2.h"
#include "Log.hpp"
#include "TreeNodePositioning.hpp"
#include "font/font.hpp"

class Menu
{
public:
	Menu() = default;
	~Menu() = default;

	//call this function once when initializing menu
	bool init(GLFWwindow* window);

	void mainloop();

	void destroy();

private:
    //render 
    inline void renderCanvas();

    inline void renderLog();

    inline void renderControls();

    inline void renderTree();

    inline void renderNode(TreeNodePositioning::Node<int>*);

    inline void printNodeList();

    //utils
    inline float ZOOM(const float val) const{
        return val * zoomScale;
    }

    inline void refreshNodeList() {
        for (int i = 0; i < nodeList.size(); i++) delete nodeList[i];
        TreeNodePositioning::Helper<int>::calculate(nodeList, tree.root, ZOOM(siblingSpacing), ZOOM(treeSpacing), ZOOM(nodeSize) * 2);
    }

    static void helpMarker(const char* desc) {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    inline void setupStyle();

    inline void updateMarker(ImVec2 mouse_pos_in_canvas) {
        for (int i = 0; i < nodeList.size(); i++)
        {
            auto node = nodeList[i];
            if (node->n == 1)
            {
                if (mouse_pos_in_canvas.x >= nodeList[i]->x && mouse_pos_in_canvas.x <= nodeList[i]->x + ZOOM(nodeSize) &&
                    mouse_pos_in_canvas.y >= node->y * ZOOM(levelSpacing) && mouse_pos_in_canvas.y <= node->y * ZOOM(levelSpacing) + ZOOM(nodeSize))
                {
                    node->isMarked = !node->isMarked;
                    return;
                }
            }
            else
            {
                if (mouse_pos_in_canvas.x >= nodeList[i]->x && mouse_pos_in_canvas.x <= nodeList[i]->x + ZOOM(nodeSize * 2) &&
                    mouse_pos_in_canvas.y >= node->y * ZOOM(levelSpacing) && mouse_pos_in_canvas.y <= node->y * ZOOM(levelSpacing) + ZOOM(nodeSize))
                {
                    node->isMarked = !node->isMarked;
                    return;
                }
            }
        }
    }

    inline void dragNode(ImVec2 mouse_pos_in_canvas) {
        for (int i = 0; i < nodeList.size(); i++)
        {
            auto node = nodeList[i];
            if (node->n == 1)
            {
                if (mouse_pos_in_canvas.x >= nodeList[i]->x && mouse_pos_in_canvas.x <= nodeList[i]->x + ZOOM(nodeSize) &&
                    mouse_pos_in_canvas.y >= node->y * ZOOM(levelSpacing) && mouse_pos_in_canvas.y <= node->y * ZOOM(levelSpacing) + ZOOM(nodeSize))
                {
                    node->x += ImGui::GetIO().MouseDelta.x;
                    node->y += ImGui::GetIO().MouseDelta.y / ZOOM(levelSpacing);
                    return;
                }
            }
            else
            {
                if (mouse_pos_in_canvas.x >= nodeList[i]->x && mouse_pos_in_canvas.x <= nodeList[i]->x + ZOOM(nodeSize * 2) &&
                    mouse_pos_in_canvas.y >= node->y * ZOOM(levelSpacing) && mouse_pos_in_canvas.y <= node->y * ZOOM(levelSpacing) + ZOOM(nodeSize))
                {
                    node->x += ImGui::GetIO().MouseDelta.x;
                    node->y += ImGui::GetIO().MouseDelta.y / ZOOM(levelSpacing);
                    return;
                }
            }
        }
    }

private:
    GLFWwindow* window{nullptr};

    ds::TwoThreeTree<int> tree;

    //tree node attribs
    ImVector<TreeNodePositioning::Node<int>*> nodeList;
    float siblingSpacing{50.f};
    float treeSpacing{ 50.f };
    float levelSpacing{ 100.f };
    float nodeSize{60};
    float zoomScale{ 1.0f };
    float fontSize{ 50.f };
    float nodeRounding{ 20.f };
    float nodeOuterWidth{ 3.f };
    int connectorStyle{ 0 };
    
    //render specs
    ImVec4 nodeOuterColor{ 0,0,0,1 };
    ImVec4 nodeInnerColor{ 1,1,1,.95f };
    ImVec4 connectorColor{ 1,1,1,.95f };
    ImVec4 textColor{ 0,0,0,1 };
    ImVec4 markColor{ 1,0,0,1 };

    //fonts
    ImFont* cousineRegular;
    ImFont* karlaRegular;

    //configs
    bool shouldDrawGrid{ true };
    float gridInterval{ 64.0 };

    //canvas specs
    ImVec2 canvasOrigin; //screen coordinate of the origin point in canvas
};

bool Menu::init(GLFWwindow* window)
{
	if (window == nullptr) return false;
	this->window = window;
	//setup imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
   
	//setup style
    setupStyle();

	//setup randerer backends
	if (!ImGui_ImplGlfw_InitForOpenGL(window, true)) return false;
	if (!ImGui_ImplOpenGL2_Init()) return false;

	return true;
}

void Menu::mainloop()
{
    //opengl clear color 
    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 1.00f);

    //loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Rendering
        ImGui::BeginMainMenuBar();
        helpMarker("This application is made by Tongji University CS student 2050250.");
        ImGui::EndMainMenuBar();
        renderCanvas();
        renderLog();
        renderControls();
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }
}

void Menu::destroy()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

inline void Menu::renderCanvas()
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowPos(ImVec2(557, 19), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(722, 698), ImGuiCond_FirstUseEver);
    ImGui::Begin("Canvas", 0, windowFlags);
    if (ImGui::Button("0.50x")) { zoomScale = 0.50f; refreshNodeList(); }
    ImGui::SameLine();
    if (ImGui::Button("1.00x")) { zoomScale = 1.00f; refreshNodeList(); }
    ImGui::SameLine();
    if (ImGui::Button("1.25x")) { zoomScale = 1.25; refreshNodeList(); }
    ImGui::SameLine();
    if (ImGui::Button("2.00x")) { zoomScale = 2.00; refreshNodeList(); }
    ImGui::SameLine();
    if(ImGui::SliderFloat("Zoom", &zoomScale, 0.25f, 5.00f)) refreshNodeList();
    ImVec2 canvas_tl = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    
    ImVec2 canvas_br = ImVec2(canvas_tl.x + canvas_sz.x, canvas_tl.y + canvas_sz.y);
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(canvas_tl, canvas_br, IM_COL32(50, 50, 50, 255));
    drawList->AddRect(canvas_tl, canvas_br, IM_COL32(255, 255, 255, 255));

    //add interaction logic
    ImGui::InvisibleButton(
        "canvas", canvas_sz,
        ImGuiButtonFlags_::ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_::ImGuiButtonFlags_MouseButtonRight);
    const bool isActive = ImGui::IsItemActive();
    const bool is_hovered = ImGui::IsItemHovered(); 

    //drag canvas
    static ImVec2 dragDistance = { 0, 0 }; //the x and y distance of dragging
    canvasOrigin = { canvas_tl.x + dragDistance.x + ZOOM(canvas_sz.x) / 2, canvas_tl.y + dragDistance.y + ZOOM(canvas_sz.y) / 2}; //the screen coord for canvas origin point
    const ImVec2 mouse_pos_in_canvas(io.MousePos.x - canvasOrigin.x, io.MousePos.y - canvasOrigin.y);
    //update drag distance
    if (isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Right, -1.0f))
    {
        dragDistance.x += io.MouseDelta.x;
        dragDistance.y += io.MouseDelta.y;
    }

    if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        updateMarker(mouse_pos_in_canvas);
    }

    if (isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f))
    {
        dragNode(mouse_pos_in_canvas);
    }

    drawList->PushClipRect(canvas_tl, canvas_br, true);
    if (shouldDrawGrid) //draw grids
    {
        for (float x = fmodf(dragDistance.x, ZOOM(gridInterval)); x < canvas_sz.x; x += ZOOM(gridInterval)) //draw cols
            drawList->AddLine(ImVec2(canvas_tl.x + x, canvas_tl.y), ImVec2(canvas_tl.x + x, canvas_br.y), IM_COL32(200, 200, 200, 40));
        for(float y = fmodf(dragDistance.y, ZOOM(gridInterval)); y < canvas_sz.y; y += ZOOM(gridInterval)) //draw rows
            drawList->AddLine(ImVec2(canvas_tl.x, canvas_tl.y + y), ImVec2(canvas_br.x, canvas_tl.y + y), IM_COL32(200, 200, 200, 40));
    }
    renderTree();

    drawList->PopClipRect();

    ImGui::End();
}

inline void Menu::renderLog()
{
    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowPos(ImVec2(0, 317), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(559, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Log output", 0, flags);
    ImGui::End();
    g_log->draw("Log output");
}

inline void Menu::renderControls()
{
    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    ImGui::SetNextWindowPos(ImVec2(0, 18), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(558, 297), ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls", 0, flags);
    ImGui::BeginTabBar("##TabBar");
    if (ImGui::BeginTabItem("Options"))
    {
        static int insert_value = 0;
        if (ImGui::Button("Insert"))
        {
            if (tree.insert(insert_value)) LOG("[%s] %s %d\n", "Info", "Inserted value:", insert_value);
            else LOG("[%s] %s %d\n", "Error", "Failed to insert value:", insert_value);
            refreshNodeList();
        }
        ImGui::SameLine();
        ImGui::InputInt("Insert value", &insert_value);
        static int remove_value = 0;
        if (ImGui::Button("Remove"))
        {
            if(tree.deleteNode(remove_value)) LOG("[%s] %s %d\n", "Info", "Removed value:", remove_value);
            else LOG("[%s] %s %d\n", "Error", "Failed to remove value:", remove_value);
            refreshNodeList();
        }
        ImGui::SameLine();
        ImGui::InputInt("Remove value", &remove_value);
        static int search_value = 0;
        if (ImGui::Button("Search"))
        {
            auto result = tree.searchFor(search_value);
            if (result != NULL)
            {
                LOG("[%s] Found node with value %d at memory block: 0x%08X\n", "Info", search_value, result);
                for (int i = 0; i < nodeList.size(); i++) 
                {
                    if (nodeList[i]->n == 1)
                    {
                        if (nodeList[i]->k1 == search_value) {
                            nodeList[i]->isMarked = true; 
                            break;
                        }
                    }
                    else
                    {
                        if (nodeList[i]->k1 == search_value || nodeList[i]->k2 == search_value) {
                            nodeList[i]->isMarked = true;
                            break;
                        }
                    }
                }
            }
            else LOG("[%s] %s %d\n", "Error", "Failed to find value:", search_value);
        }
        ImGui::SameLine();
        ImGui::InputInt("Search value", &search_value);
        ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Misc"))
    {
        if (ImGui::Button("Print tree")) printNodeList();
        ImGui::SameLine();
        ImGui::Checkbox("Grid view", &this->shouldDrawGrid);
        ImGui::SameLine();
        ImGui::RadioButton("Straight connector", &connectorStyle, 0); 
        ImGui::SameLine();
        ImGui::RadioButton("Right-Angle connector", &connectorStyle, 1);
        if (ImGui::SliderFloat("Node size", &this->nodeSize, 40.f, 100.f)) refreshNodeList();
        if (ImGui::SliderFloat("Level spacing", &this->levelSpacing, 50.f, 200.f)) refreshNodeList();
        if (ImGui::SliderFloat("Sibling spacing", &this->siblingSpacing, 40.f, 150.f)) refreshNodeList();
        if (ImGui::SliderFloat("Tree spacing", &this->treeSpacing, 10.f, 100.f)) refreshNodeList();
        ImGui::SliderFloat("Font size", &this->fontSize, 25.f, 75.f);
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::BeginChild("Color edit", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NavFlattened);
        ImGui::PushItemWidth(-160);
        ImGui::ColorEdit4("##NodeOuter", &nodeOuterColor.x, ImGuiColorEditFlags_AlphaBar);
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        ImGui::TextUnformatted("NodeOuter");
        ImGui::ColorEdit4("##NodeInner", &nodeInnerColor.x, ImGuiColorEditFlags_AlphaBar);
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        ImGui::TextUnformatted("NodeInner");
        ImGui::ColorEdit4("##Connector", &connectorColor.x, ImGuiColorEditFlags_AlphaBar);
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        ImGui::TextUnformatted("Connector");
        ImGui::ColorEdit4("##Text", &textColor.x, ImGuiColorEditFlags_AlphaBar);
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        ImGui::TextUnformatted("Text");
        ImGui::ColorEdit4("##Mark", &markColor.x, ImGuiColorEditFlags_AlphaBar);
        ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
        ImGui::TextUnformatted("Mark");
        ImGui::PopItemWidth();
        ImGui::EndChild();
        ImGui::EndTabItem();
    }

    ImGui::EndTabBar();

    ImGui::End();
}

inline void Menu::renderTree()
{
    for (int i = 0; i < this->nodeList.size(); i++)
    {
        auto node = this->nodeList[i];
        renderNode(node);
    }
}

inline void Menu::renderNode(TreeNodePositioning::Node<int>* node)
{
    if (node == nullptr) return;
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    if (node->n == 1)
    {
        drawList->AddRectFilled(
            ImVec2(canvasOrigin.x + node->x, canvasOrigin.y + node->y * ZOOM(levelSpacing)),
            ImVec2(canvasOrigin.x + node->x + ZOOM(nodeSize), canvasOrigin.y + node->y * ZOOM(levelSpacing) + ZOOM(nodeSize)),
            ImGui::ColorConvertFloat4ToU32(nodeInnerColor), ZOOM(nodeRounding * nodeSize / 60.f));
        drawList->AddRect(
            ImVec2(canvasOrigin.x + node->x, canvasOrigin.y + node->y * ZOOM(levelSpacing)),
            ImVec2(canvasOrigin.x + node->x + ZOOM(nodeSize), canvasOrigin.y + node->y * ZOOM(levelSpacing) + ZOOM(nodeSize)),
            ImGui::ColorConvertFloat4ToU32(node->isMarked ? markColor : nodeOuterColor), ZOOM(nodeRounding * nodeSize / 60.f), 0, ZOOM(nodeOuterWidth));
        char buf[256];
        sprintf_s(buf, "%d", node->k1);
        int modifier = abs(node->k1) < 10 ? 1 : (int)log10(node->k1) + 1;
        if (node->k1 < 0) modifier += 1;
        drawList->AddText(
            cousineRegular, ZOOM(fontSize * (1 + (node->k1 < 0)) / modifier),
            ImVec2(canvasOrigin.x + node->x + ZOOM((nodeSize - fontSize / 2) /2), canvasOrigin.y + node->y * ZOOM(levelSpacing)),
            ImGui::ColorConvertFloat4ToU32(textColor),
            buf);
    }
    else
    {
        drawList->AddRectFilled(
            ImVec2(canvasOrigin.x + node->x, canvasOrigin.y + node->y * ZOOM(levelSpacing)),
            ImVec2(canvasOrigin.x + node->x + 2 * ZOOM(nodeSize), canvasOrigin.y + node->y * ZOOM(levelSpacing) + ZOOM(nodeSize)),
            ImGui::ColorConvertFloat4ToU32(nodeInnerColor), ZOOM(nodeRounding * nodeSize / 60.f));
        drawList->AddRect(
            ImVec2(canvasOrigin.x + node->x, canvasOrigin.y + node->y * ZOOM(levelSpacing)),
            ImVec2(canvasOrigin.x + node->x + 2 * ZOOM(nodeSize), canvasOrigin.y + node->y * ZOOM(levelSpacing) + ZOOM(nodeSize)),
            ImGui::ColorConvertFloat4ToU32(node->isMarked ? markColor : nodeOuterColor), ZOOM(nodeRounding * nodeSize / 60.f), 0, ZOOM(nodeOuterWidth));
        char buf[256];
        sprintf_s(buf, "%d|%d", node->k1, node->k2);
        int modifier = (abs(node->k1) < 10 ? 1 : (int)log10(node->k1) + 1) + (abs(node->k2) < 10 ? 1 : (int)log10(node->k2) + 1) + 1;
        drawList->AddText(
            cousineRegular, ZOOM(fontSize * 3 / modifier),
            ImVec2(canvasOrigin.x + node->x + ZOOM((nodeSize * 2 - fontSize * 3 / 2) / 2), canvasOrigin.y + node->y * ZOOM(levelSpacing)),
            ImGui::ColorConvertFloat4ToU32(textColor),
            buf);
    }

    if (node->parent != nullptr)
    {
        ImVec2 src(0, 0);
        if (node->n == 1) src = { canvasOrigin.x + node->x + ZOOM(nodeSize / 2), canvasOrigin.y + node->y * ZOOM(levelSpacing) - ZOOM(nodeOuterWidth)};
        else src = { canvasOrigin.x + node->x + ZOOM(nodeSize), canvasOrigin.y + node->y * ZOOM(levelSpacing)};
        ImVec2 dst{ 0,0 };
        if (node->parent->n == 1) dst = { canvasOrigin.x + node->parent->x + ZOOM(nodeSize / 2), canvasOrigin.y + node->parent->y * ZOOM(levelSpacing) + ZOOM(nodeSize) + ZOOM(nodeOuterWidth)};
        else dst = { canvasOrigin.x + node->parent->x + ZOOM(nodeSize), canvasOrigin.y + node->parent->y * ZOOM(levelSpacing) + ZOOM(nodeSize) };
        if(this->connectorStyle == 0) //Straight connector
            drawList->AddLine(src, dst, ImGui::ColorConvertFloat4ToU32(connectorColor), 2.f);
        else //Right-Angle connector
        {
            drawList->AddLine(src, ImVec2(src.x, src.y + (dst.y - src.y) / 2), ImGui::ColorConvertFloat4ToU32(connectorColor), 2.f);
            drawList->AddLine(
                ImVec2(src.x, src.y + (dst.y - src.y) / 2),
                ImVec2(dst.x, src.y + (dst.y - src.y) / 2), ImGui::ColorConvertFloat4ToU32(connectorColor), 2.f);
            drawList->AddLine(ImVec2(dst.x, src.y + (dst.y - src.y) / 2), dst, ImGui::ColorConvertFloat4ToU32(connectorColor), 2.f);
        }
    }
}

inline void Menu::printNodeList()
{
    if (nodeList.empty()) 
    {
        LOG("[%s] %s", "Warning", "Tree is empty...\n"); 
        return;
    }
    LOG("[%s] %s", "Info", "Current tree nodes: \n");
    for (int i = 0; i < nodeList.size(); i++)
    {
        auto node = nodeList[i];
        if (node->n == 1)
            LOG("[%s] Type: %s, k1: %d, x: %f, y: %d\n", "Info", "2-node", node->k1, node->x, node->y);
        else 
            LOG("[%s] Type: %s, k1: %d, k2: %d, x: %f, y: %d\n", "Info", "3-node", node->k1, node->k2, node->x, node->y);
    }
}

inline void Menu::setupStyle()
{
    ImGuiStyle* style = &ImGui::GetStyle();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.WantSaveIniSettings = false;

    //roundings
    style->WindowRounding = 8.f;
    style->FrameRounding = 3.f;
    style->ScrollbarRounding = 12.f;

    //fonts
    io.Fonts->AddFontDefault();
    karlaRegular = io.Fonts->AddFontFromMemoryCompressedTTF((void*)karla_regular_data, karla_regular_size, 200.0f, NULL);
    cousineRegular = io.Fonts->AddFontFromMemoryCompressedTTF((void*)cousine_regular_data, cousine_regular_size, 200.0f, NULL);
    //color styles
    ImGui::StyleColorsDark();
}

inline auto g_menu = std::make_unique<Menu>();



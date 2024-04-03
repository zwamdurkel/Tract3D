#include <algorithm>
#include "ImGuiWrapper.h"
#include "logger.h"
#include "TractDataWrapper.h"
#include "path.h"
#include "nativefiledialog/src/include/nfd.hpp"
#include "RenderSettings.h"
#include "GLFWWrapper.h"
#include "IconFontCppHeaders/IconsFontAwesome6.h"
#include "imgui_internal.h"

void ImGuiWrapper::init() {
    settings.imgui = this;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    auto path = getPath();
    iniFile = (path + "imgui.ini");
    io.IniFilename = iniFile.c_str();
//    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.Fonts->AddFontFromFileTTF((path + "Ruda-Regular.ttf").c_str(), 16);
//    io.Fonts->AddFontFromFileTTF("Ruda-Bold.ttf", 10);
//    io.Fonts->AddFontFromFileTTF("Ruda-Bold.ttf", 14);
//    io.Fonts->AddFontFromFileTTF("Ruda-Bold.ttf", 18);
    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 16.0f; // Use if you want to make the icon monospaced
    static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    io.Fonts->AddFontFromFileTTF((path + "fa-solid-900.ttf").c_str(), 16.0f, &config, icon_ranges);

    ImGuiStyle* style = &ImGui::GetStyle();

    style->WindowPadding = ImVec2(15, 15);
    style->WindowRounding = 5.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 4.0f;
    style->ItemSpacing = ImVec2(12, 10);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 15.0f;
    style->ScrollbarRounding = 9.0f;
    style->GrabMinSize = 5.0f;
    style->GrabRounding = 3.0f;

    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.52f, 0.50f, 0.55f, 0.88f);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.29f, 0.29f, 0.35f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.29f, 0.29f, 0.35f, 1.00f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.29f, 0.29f, 0.35f, 1.00f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.29f, 0.29f, 0.35f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    const char* glsl_version = "#version 420";
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(350, 500));
}

static void HelpMarker(const char* desc, float offset = ImGui::GetColumnWidth() - 5) {
    ImGui::SameLine(offset);
    auto window = ImGui::GetCurrentWindow();
    window->DC.CurrLineTextBaseOffset += 1;
    ImGui::TextDisabled(ICON_FA_CIRCLE_INFO);
    window->DC.CurrLineTextBaseOffset -= 1;
    if (ImGui::BeginItemTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

static bool IconCollapsingHeader(const char* label, const char* icon, ImGuiTreeNodeFlags flags = 0) {
    auto state = ImGui::CollapsingHeader(label, flags);
    ImGui::SameLine(ImGui::GetColumnWidth() - 10);
    auto window = ImGui::GetCurrentWindow();
    window->DC.CurrLineTextBaseOffset += 1;
    ImGui::Text(icon);
    window->DC.CurrLineTextBaseOffset -= 1;
    return state;
}

static void IconSeparatorText(const char* label, const char* icon) {
    ImGui::AlignTextToFramePadding();
    ImGui::Text(icon);
    ImGui::SameLine();
    ImGui::SeparatorText(label);
}

void ImGuiWrapper::draw() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // windows settings
    static bool no_move = false;
    static bool no_resize = false;
    ImGuiWindowFlags window_flags = 0;
    //window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_move) window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize) window_flags |= ImGuiWindowFlags_NoResize;

    if (settings.show_demo_window)
        ImGui::ShowDemoWindow(&settings.show_demo_window);

    {
        ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
        ImGui::Begin("Tract 3D", nullptr, window_flags);

        ImGui::Text("Version 1.0.0");

        if (IconCollapsingHeader("Rendering Settings", ICON_FA_VECTOR_SQUARE, ImGuiTreeNodeFlags_DefaultOpen)) {
            IconSeparatorText("General Options", ICON_FA_SLIDERS);

            ImGui::ColorEdit3("Background Color", (float*) &settings.clear_color, ImGuiColorEditFlags_NoInputs);
            HelpMarker("The user can pick the desired background color to suite their preferences.");

            if (ImGui::Checkbox("Anti Aliasing", &settings.MSAA)) {
                settings.MSAA ? glEnable(GL_MULTISAMPLE) : glDisable(GL_MULTISAMPLE);
            }
            HelpMarker("When enabled, sharpen the image at the cost of some performance.");

            if (ImGui::Checkbox("V-Sync", &settings.vsync)) {
                glfwSwapInterval((int) settings.vsync);
            }
            HelpMarker(
                    "When enabled, lock the framerate to the refreshrate of the monitor to prevent screen tearing and to keep the GPU from doing unnecessary work.");

            if (ImGui::Checkbox("Full Screen", &settings.fullScreen)) {
                settings.glfw->setFullScreen(settings.fullScreen);
            }
            HelpMarker("Toggle the application between windowed mode and full screen, for better immersion.");

            IconSeparatorText("Tract Options", ICON_FA_DIAGRAM_PROJECT);

            const char* renderers[] = {"Unshaded Lines", "Shaded Lines", "Shaded Tubes", "Ray tracing"};
            ImGui::PushItemWidth(170);
            if (ImGui::Combo("Renderer", (int*) &settings.renderer, renderers, IM_ARRAYSIZE(renderers))) {
                switch (settings.renderer) {
                    case UNSHADED_LINES:
                        settings.shader = settings.defaultShader;
                        break;
                    case SHADED_LINES:
                        settings.shader = settings.lineShadingShader;
                        break;
                    case SHADED_TUBES:
                        settings.shader = settings.defaultShader;
                        break;
                }

                for (auto& dataset: settings.datasets) {
                    dataset->init();
                }
                for (auto& dataset: settings.examples) {
                    dataset->init();
                }
            }
            ImGui::PopItemWidth();
            HelpMarker("Select The desired renderer.\n\n"
                       "- Unshaded Lines: (default)\nThe tracts are rendered as lines. All colors will have the same brightness.\n\n"
                       "- Shaded Lines:\nThe tracts are rendered as lines. Lighting will be applied to the colors.\n\n"
                       "- Shaded Tubes:\nThe tracts are rendered as tubes. Tubes are heavier to render. The user can select how many sides the tubes have and how thick the tubes are. Lighting will be applied to the colors.");

            if (settings.renderer == UNSHADED_LINES || settings.renderer == SHADED_LINES) {
                ImGui::Checkbox("Draw Points", &settings.drawPoints);
                HelpMarker("When enabled, draw a point on every vertex of a tract.");
            }

            if (settings.renderer == SHADED_TUBES || settings.renderer == SHADED_LINES) {
                ImGui::Checkbox("Rotating Light", &settings.rotatingLight);
                HelpMarker("When enabled, the light direction will continuously rotate.");
            }

            if (settings.renderer == SHADED_TUBES) {
                ImGui::PushItemWidth(170);
                if (ImGui::SliderInt("Tube Sides", &settings.nrOfSides, 3, 8)) {
                    for (auto& dataset: settings.datasets) {
                        dataset->init();
                    }
                    for (auto& dataset: settings.examples) {
                        dataset->init();
                    }
                }
                HelpMarker("Specify the number of sides the tract tubes have. More sides means less performance.");
                if (ImGui::SliderFloat("Tube Diameter", &settings.tubeDiameter, 0.01, 0.3, "%.2f")) {
                    for (auto& dataset: settings.datasets) {
                        dataset->init();
                    }
                    for (auto& dataset: settings.examples) {
                        dataset->init();
                    }
                }
                HelpMarker(
                        "Specify the diameter of the tubes. Tubes that are too thick may not work well with some effects.");
                ImGui::PopItemWidth();
                ImGui::Checkbox("Smooth End Caps", &settings.smoothCap);
                HelpMarker("When enabled, the end caps of tubes are rendered to look smoother");
            }
            if (settings.renderer != RAY_TRACING) {
                if (ImGui::Checkbox("Highlight", &settings.highlightEnabled)) {
                    for (auto& dataset: settings.datasets) {
                        if (!settings.highlightEnabled) {
                            dataset->alpha = settings.generalAlpha;
                        } else {
                            if (dataset->name != settings.highlightedBundle) {
                                dataset->alpha = settings.highlightAlpha;
                            }
                        }
                    }
                    for (auto& dataset: settings.examples) {
                        if (!settings.highlightEnabled) {
                            dataset->alpha = settings.generalAlpha;
                        } else {
                            if (dataset->name != settings.highlightedBundle) {
                                dataset->alpha = settings.highlightAlpha;
                            }
                        }
                    }
                }
                HelpMarker("When enabled, allows the user to select a tract bundle to highlight.");
            } else {
                ImGui::Checkbox("Enable Blur", &settings.blurEnabled);
                HelpMarker("When enabled, blurs the image to attempt to remove noise");

                if (ImGui::Button("Reset Image")) {
                    settings.rt->cleanup();
                    settings.rt->init();
                }
            }
            if (settings.highlightEnabled) {
                ImGui::PushItemWidth(170);
                if (ImGui::SliderFloat("Highlight Alpha", &settings.highlightAlpha, 0.0f, 1.0f, "%.2f")) {
                    for (auto& data: settings.datasets) {
                        if (data->name != settings.highlightedBundle) {
                            data->alpha = settings.highlightAlpha;
                        }
                    }
                    for (auto& data: settings.examples) {
                        if (data->name != settings.highlightedBundle) {
                            data->alpha = settings.highlightAlpha;
                        }
                    }
                }
                HelpMarker("Sets the transparency of all bundles that are not highlighted.");

                if (ImGui::BeginCombo("Highlight Bundle", settings.highlightedBundle.c_str())) {
                    for (auto& dataset: settings.datasets) {
                        if (dataset->enabled) {
                            ImGui::PushID(&dataset);
                            if (ImGui::Selectable(dataset->name.c_str(), dataset->name == settings.highlightedBundle)) {
                                settings.highlightedBundle = dataset->name;
                                for (auto& data: settings.datasets) {
                                    if (data->name != settings.highlightedBundle) {
                                        data->alpha = settings.highlightAlpha;
                                    } else {
                                        data->alpha = settings.generalAlpha;
                                    }
                                }
                                for (auto& data: settings.examples) {
                                    if (data->name != settings.highlightedBundle) {
                                        data->alpha = settings.highlightAlpha;
                                    } else {
                                        data->alpha = settings.generalAlpha;
                                    }
                                }
                            }
                            ImGui::PopID();
                        }
                    }
                    for (auto& dataset: settings.examples) {
                        if (dataset->enabled) {
                            ImGui::PushID(&dataset);
                            if (ImGui::Selectable(dataset->name.c_str(), dataset->name == settings.highlightedBundle)) {
                                settings.highlightedBundle = dataset->name;
                                for (auto& data: settings.datasets) {
                                    if (data->name != settings.highlightedBundle) {
                                        data->alpha = settings.highlightAlpha;
                                    } else {
                                        data->alpha = settings.generalAlpha;
                                    }
                                }
                                for (auto& data: settings.examples) {
                                    if (data->name != settings.highlightedBundle) {
                                        data->alpha = settings.highlightAlpha;
                                    } else {
                                        data->alpha = settings.generalAlpha;
                                    }
                                }
                            }
                            ImGui::PopID();
                        }
                    }
                    ImGui::EndCombo();
                }
                HelpMarker("Choose which bundle to highlight.");
                ImGui::PopItemWidth();
            }

//            if (ImGui::SliderFloat("Alpha", &settings.generalAlpha, 0.0f, 1.0f, "%.2f")) {
//                for (auto& dataset: settings.datasets) {
//                    dataset->alpha = settings.generalAlpha;
//                }
//                for (auto& dataset: settings.examples) {
//                    dataset->alpha = settings.generalAlpha;
//                }
//            }

//            IconSeparatorText("Expanding Views", ICON_FA_ARROW_UP_RIGHT_DOTS);
//            ImGui::NewLine();
            if (ImGui::Checkbox("Expanding Views", &settings.expandingViewsEnabled)) {
                for (auto& dataset: settings.datasets) {
                    if (settings.expandingViewsEnabled && dataset->enabled) {
                        dataset->bindDB();
                    } else if (!settings.expandingViewsEnabled) {
                        dataset->clearDB();
                    }
                }
                for (auto& dataset: settings.examples) {
                    if (settings.expandingViewsEnabled && dataset->enabled) {
                        dataset->bindDB();
                    } else if (!settings.expandingViewsEnabled) {
                        dataset->clearDB();
                    }
                }
            }

            if (settings.expandingViewsEnabled) {
                ImGui::PushItemWidth(170);
                if(ImGui::SliderFloat("Expansion Factor", &settings.expansionFactor, -1.0f, 2.0f, "%.2f")){
                    for (auto& dataset: settings.datasets) {
                        if (settings.expandingViewsEnabled && dataset->enabled) {
                            dataset->bindDB();
                        }
                    }
                    for (auto& dataset: settings.examples) {
                        if (settings.expandingViewsEnabled && dataset->enabled) {
                            dataset->bindDB();
                        }
                    }
                }
            }


            IconSeparatorText("Tract Count", ICON_FA_ARROW_UP_RIGHT_DOTS);
            ImGui::NewLine();
            HelpMarker("Specify the number of tracts to render for each tract bundle.", 0);

            for (auto& dataset: settings.datasets) {
                ImGui::PushItemWidth(170);
                if (dataset->enabled) {
                    std::string name = dataset->name;
                    ImGui::SliderInt((name + " ").c_str(), &dataset->showTractCount, 1, dataset->tractCount);
                }
                ImGui::PopItemWidth();
            }

            if (ImGui::TreeNode("Examples Tract Count")) {
                ImGui::PushItemWidth(170);
                for (auto& dataset: settings.examples) {
                    if (dataset->enabled) {
                        std::string name = dataset->name;
                        ImGui::SliderInt((name + " ").c_str(), &dataset->showTractCount, 1, dataset->tractCount);
                    }
                }
                ImGui::PopItemWidth();

                ImGui::TreePop();
            }
        }

        if (IconCollapsingHeader("Dataset Management", ICON_FA_FILE_CIRCLE_PLUS)) {
            static std::string message = "Select Dataset";
            if (ImGui::Button("Browse")) {
                NFD::Init();
                char* outPath;
                nfdfilteritem_t filterItem[1] = {{"MRI files", "tck"}};
                nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 1, nullptr);
                if (result == NFD_OKAY) {
                    Info("Selected file: " << outPath);
                    NFD::FreePath(outPath);
                    auto name = std::string(outPath);
                    name.erase(0, name.find_last_of('\\') + 1);
                    bool duplicate = false;
                    for (auto& dataset: settings.datasets) {
                        if (dataset->name == name) {
                            duplicate = true;
                            message = "Duplicate Rejected";
                        }
                    }
                    for (auto& dataset: settings.examples) {
                        if (dataset->name == name) {
                            duplicate = true;
                            message = "Example Duplicate";
                        }
                    }
                    if (!duplicate) {
                        auto td = std::make_unique<TractDataWrapper>(name, std::string(outPath));
                        settings.datasets.push_back(std::move(td));
                        message = "Successfully Added";
                    }
                } else if (result == NFD_CANCEL) {
                    Info("User pressed cancel.");
                    message = "User Cancelled";
                } else {
                    Info("Error: %s\n" << NFD::GetError());
                    message = "Select Dataset";
                }
                NFD::Quit();

            }
            ImGui::SameLine();
            ImGui::Text(message.c_str());

            IconSeparatorText("Datasets", ICON_FA_FOLDER_TREE);

            for (auto it = settings.datasets.begin(); it != settings.datasets.end();) {
                if (ImGui::Checkbox((*it)->name.c_str(), &(*it)->enabled))
                    (*it)->init();
                ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 30);
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor(191, 78, 78));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) ImColor(211, 100, 100));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) ImColor(211, 100, 100));
                if (ImGui::Button(ICON_FA_FILE_CIRCLE_MINUS)) {
                    it = settings.datasets.erase(it);
                } else {
                    ++it;
                }
                ImGui::PopStyleColor(3);
            }

            if (ImGui::TreeNode("Examples")) {
                if (ImGui::Button("Enable All")) {
                    for (auto& dataset: settings.examples) {
                        dataset->enabled = true;
                        dataset->init();
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Disable All")) {
                    for (auto& dataset: settings.examples) {
                        dataset->enabled = false;
                    }
                }

                for (auto& dataset: settings.examples) {
                    if (ImGui::Checkbox(dataset->name.c_str(), &dataset->enabled))
                        dataset->init();
                }
                ImGui::TreePop();
            }
        }

        if (IconCollapsingHeader("Camera Settings", ICON_FA_VIDEO)) {
            ImGui::PushItemWidth(170);
            ImGui::SliderFloat("FOV", &settings.camera.FOV, 30.0f, 150.0f, "%.0f");
            ImGui::SliderFloat("Speed", &settings.camera.MovementSpeed, 1.0f, 100.0f, "%.0f");
            ImGui::SliderFloat("Draw Distance", &settings.camera.FarPlane, 1.0f, 500.0f, "%.0f");
            ImGui::PopItemWidth();
        }

        if (IconCollapsingHeader("Development Settings", ICON_FA_CODE)) {
            ImGui::TableNextColumn();
            ImGui::Checkbox("No Move", &no_move);
            ImGui::TableNextColumn();
            ImGui::Checkbox("No Resize", &no_resize);
            ImGui::Checkbox("Demo Window", &settings.show_demo_window);
        }

        if (IconCollapsingHeader("Guide", ICON_FA_BOOK_OPEN)) {
            if (ImGui::TreeNode("Keyboard Shortcuts##1")) {
                if (ImGui::BeginTable("table1", 2, ImGuiTableFlags_SizingStretchProp)) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Esc");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextWrapped("Exit the application");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("F11");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextWrapped("Toggle full screen");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("W");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextWrapped("Move camera forward");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("A");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextWrapped("Move camera left");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("S");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextWrapped("Move camera backward");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("D");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextWrapped("Move camera right");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Shift");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextWrapped("Move camera down");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Space");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextWrapped("Move camera up");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Ctrl + Scroll");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextWrapped("Change camera FOV");

                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Alt + Scroll");
                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextWrapped("Change movement speed");

                    ImGui::EndTable();
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Render Settings##1")) {
                ImGui::TextWrapped(
                        "The Render Settings are divided in two main categories, General options and tract options. General options affect the whole application while tract options are specific to rendering the tracts.");
                ImGui::TextWrapped(
                        "Details on the effects of each option can be found by hovering over the info marker %s next to each option.",
                        ICON_FA_CIRCLE_INFO);

                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Dataset Management##1")) {
                if (ImGui::TreeNode("Using your own .tck file")) {
                    ImGui::TextWrapped(
                            "Go to the Dataset Management tab and press Browse to select a local .tck file.");
                    ImGui::TextWrapped(
                            "If you wish to remove this file from Tract 3D, press the %s button to the right of the corresponding dataset.",
                            ICON_FA_FILE_CIRCLE_MINUS);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Enable/Disable datasets")) {
                    ImGui::TextWrapped(
                            "Go to the Dataset Management tab. Every dataset in the dataset list has a checkbox to enable or disable rendering the dataset. Example datasets can be found in the Examples dropdown. ");
                    ImGui::TextWrapped(
                            "It is possible to enable or disable all example files at once by pressing the Enable All or Disable All button in the Examples dropdown.");
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }
        }

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Application Framerate %.1f FPS", io.Framerate);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void ImGuiWrapper::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


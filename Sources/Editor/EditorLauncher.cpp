#include "EditorLauncher.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl2.h"
#include "imgui/backends/imgui_impl_opengl3.h"

#include "EditorWindow/EditorWindowManager.h"
#include "EditorSceneManager.h"

#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>

namespace BorderlessEditor
{
    void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;

    double targetFrameRate = 20.0;
    double realTime = 0.0;
    double frameTime = 0.0;
    double accumulator = 0.0;
    double timeScale = 1.0;
    double targetTime = 0.0;

    GLFWwindow *EditorLauncher::glfwWindow = 0;

    double GetFrameInterval()
    {
        return 1.0 / targetFrameRate;
    }

    GLFWwindow *EditorLauncher::GetGLFWWindow()
    {
        return EditorLauncher::glfwWindow;
    }

    void EditorLauncher::Launch()
    {
        std::cout << "BorderlessEditor launched" << std::endl;

        InitializeWindow();
        EditorWindowManager::Init();
        InitImgui(glfwWindow);

        Loop();

        DestroyImgui();
        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
    }

    bool EditorLauncher::InitializeWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
        if (glfwWindow == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(glfwWindow);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        glfwSetFramebufferSizeCallback(glfwWindow, framebuffer_size_callback);

        return 0;
    }

    void EditorLauncher::Loop()
    {
        while (!glfwWindowShouldClose(glfwWindow))
        {
            // ??????
            Input();
            // ??????
            Render();
            // ???????????????UI
            DrawImgui();
            // ???????????????
            glfwSwapBuffers(glfwWindow);
            // ????????????????????????????????????????????????????????????
            WaitForNextFrame();
        }
    }

    void EditorLauncher::Input()
    {
        // glfw???????????????
        glfwPollEvents();
        // processInput(window);
    }

    void EditorLauncher::Render()
    {
        // ??????????????????
        glEnable(GL_DEPTH_TEST);
        //// ????????????
        // glEnable(GL_CULL_FACE);
        //// ??????????????????
        // glCullFace(GL_BACK);
        //  ????????????
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // ??????
        glClearColor(0, 0, .7, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void EditorLauncher::InitImgui(GLFWwindow *window)
    {
        ImGui::CreateContext();                     // Setup Dear ImGui context
        ImGui::StyleColorsDark();                   // Setup Dear ImGui style
        ImGui_ImplGlfw_InitForOpenGL(window, true); // Setup Platform/Renderer backends
        ImGui_ImplOpenGL3_Init("#version 450");

        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // ??????docking
        ImGui::GetIO().ConfigFlags = ImGuiConfigFlags_DockingEnable;
    }

    void EditorLauncher::DrawImgui()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        bool isFrameRateOpen = false;
        ImGuiWindowFlags window_flags =
            // ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoDecoration |     // ??????????????????????????????????????????????????????????????????????????????
            ImGuiWindowFlags_AlwaysAutoResize | // ??????????????????
            // ImGuiWindowFlags_NoSavedSettings |				// ?????????????????????????????????
            ImGuiWindowFlags_NoFocusOnAppearing | // ??????????????????????????????
            ImGuiWindowFlags_NoNav;
        // if (ImGui::Begin("frame rate", &isFrameRateOpen, window_flags))
        // {
        //     ImGui::InputDouble("targetFrameRate", &BorderlessEditor::targetFrameRate, 1.0f, 5.0f, "%.2f");
        //     ImGui::Text("frame rate %.0lf", BorderlessEngine::GetFrameRate());
        //     ImGui::End();
        // }

        auto editorWindows = *EditorWindowManager::GetAllEditorwindows();
        // ?????????
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene"))
                {
                    BorderlessEditor::EditorSceneManager::NewScene();
                }
                if (ImGui::MenuItem("Open Scene"))
                {
                    BorderlessEditor::EditorSceneManager::OpenScene();
                }
                if (ImGui::MenuItem("Save Scene"))
                {
                    BorderlessEditor::EditorSceneManager::SaveScene();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                for (size_t i = 0; i < editorWindows.size(); i++)
                {
                    EditorWindow &editorWindow = *(editorWindows[i]);
                    if (ImGui::MenuItem(editorWindow.name))
                    {
                        editorWindow.Open();
                    }
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // ????????????window
        for (size_t i = 0; i < editorWindows.size(); i++)
        {
            EditorWindow &editorWindow = *(editorWindows[i]);
            if (editorWindow.isOpen)
            {
                editorWindow.BeginDraw();
                editorWindow.Draw();
                editorWindow.EndDraw();
            }
        }

        // for (vector<EditorWindow*>::iterator it = windows.begin(); it != windows.end(); it++)
        //{
        //	EditorWindow* window;
        //	window = &(it);
        //	EditorWindow w = (*it);
        //	w.BeginDraw();
        //	if (w.isOpen)
        //		w.Draw();
        //	w.EndDraw();
        // }

        // ImGui::ShowDemoWindow();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void EditorLauncher::DestroyImgui()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void EditorLauncher::WaitForNextFrame()
    {
        double currentTime = glfwGetTime();
        if (currentTime < targetTime)
            return;
        frameTime = currentTime - realTime;
        realTime = currentTime;

        // accumulator += frameTime;
        //// ??????????????????????????????????????????????????????????????????
        // while (accumulator < currentTime)
        //{
        //	InputSystem::FixedUpdate();
        //	accumulator += interval;
        // }

        targetTime += GetFrameInterval();
        // ????????????????????????Update?????????
        if (currentTime < targetTime)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds((long)((targetTime - currentTime) * 1000)));
        }
    }

}
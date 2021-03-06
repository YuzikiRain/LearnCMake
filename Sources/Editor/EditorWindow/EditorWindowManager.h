#pragma once

#include "EditorWindow/EditorWindow.h"
#include "EditorWindow/Inspector.h"
#include "EditorWindow/Hierarchy.h"
#include "EditorWindow/GameEditorWindow.h"
#include <vector>

namespace BorderlessEditor
{
    class EditorWindowManager
    {
    public:
        static std::vector<EditorWindow *> *GetAllEditorwindows();
        static void Init();

    private:
        static Inspector *inspector;
        static Hierarchy *hierarchy;
        static GameEditorWindow *game;
        static std::vector<EditorWindow *> *editorWindows;
    };
}
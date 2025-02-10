#include "raylib.h"
#include "raygui.h"
#include "course.h"
#include <stddef.h>
#include <string.h>
#include "gui.h"
#include "gui_func.h"
#include "def.h"
#include "schedule.h"

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scheduler with Raylib");
  SetTargetFPS(40);

  const char* courseFilePath = "resources/extracted_table.txt";

  
  Class** classes = ParseClasses(courseFilePath);
  StudentSchedule studentSchedule  = (StudentSchedule){0};
  GuiLayoutState guiState = InitGuiLayout(&studentSchedule);

  while (!WindowShouldClose()) {
    UpdateGUIState(&guiState, classes, &studentSchedule);
    
    BeginDrawing();
      ClearBackground(RAYWHITE);
      UpdateGuiLayout(&guiState);
    EndDrawing();
  }
  // Deinit of GUI must be before the classes
  // guiState is on stack, so no need to free
  DeinitGuiLayout(&guiState);
  ClearStudentSchedule(&studentSchedule);
  ClearClasses(classes);
  MemFree(classes);
  CloseWindow();
  return 0;
}

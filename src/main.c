#include "raylib.h"
#include "raygui.h"
#include "course.h"
#include <stddef.h>
#include <string.h>
#include "gui.h"
#include "def.h"
#include "schedule.h"

void SearchAndFilter(GuiLayoutState* guiState, Class** classes){
  if (guiState->searchSubmited || guiState->filterSubmited || guiState->matchCurrentCellToggled){
    if (guiState->searchResults){
      // Classses are referenced from the main classes array, only the array of classes need to be freed
      MemFree(guiState->searchResults);
      guiState->searchResults = NULL;
    }
    // Apply filter first, then search on those results
    Class** filteredClasses = classes;
    int filterPhase = 0;
    for (int i = 0; i < 2; i++)
    {
      Class** temp = NULL;
      int selectedOption = i == filterPhase ? guiState->filterOptionsDropdownSelected : guiState->searchOptionsDropdownSelected;
      char* searchText = i == filterPhase ? guiState->filterText : guiState->searchText;
      switch (selectedOption)
      {
      case PROPERTY_ALL:{
        temp = FilterByAll(filteredClasses, searchText);
        break;
      }
      case COURSE_ID_PROPERTY_INDEX:{
        temp = FilterByCourseId(filteredClasses, searchText);
        break;
      }
      case COURSE_NAME_PROPERTY_INDEX:{
        temp = FilterByCourseName(filteredClasses, searchText);
        break;
      }
      case CLASS_ID_PROPERTY_INDEX:{
        temp = FilterByClassId(filteredClasses, searchText);
        break;
      }
      default:
        break;
      }
      if (temp == NULL){
        break; // Something crazy happened
      }
      // If filtering, pass the classes to the search phase
      if (i == filterPhase){
        filteredClasses = temp;
      } else {
        // If searching, free the filtered classes (the intermediate result)
        MemFree(filteredClasses);
        guiState->searchResults = temp;
      }
    }
    // Wait there's more
    if (guiState->matchCurrentCellEnabled && guiState->emptyCellSelectedDay >= 0 && guiState->emptyCellSelectedPeriod >= 0){
      Class** temp = guiState->searchResults;
      guiState->searchResults = FilterByPeriod(guiState->searchResults, guiState->emptyCellSelectedDay, guiState->emptyCellSelectedPeriod);
      MemFree(temp);
    }
  }
}

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scheduler with Raylib");
  SetTargetFPS(40);

  GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE_DEFAULT);

  int classCount;
  Class** classes = ParseClasses("resources/extracted_table.txt", &classCount);
  GuiLayoutState guiState = InitGuiLayoutState();
  StudentSchedule* studentSchedule  = CreateStudentSchedule();
  guiState.studentSchedule = studentSchedule;


  while (!WindowShouldClose()) {
    SearchAndFilter(&guiState, classes);
    if (guiState.emptyCellSelected){
      guiState.searchResults = FilterByPeriod(classes, guiState.emptyCellSelectedDay, guiState.emptyCellSelectedPeriod);
    }
    if (guiState.searchResultSelected){
      AddClassToSchedule(studentSchedule, guiState.searchResultSelectedClass);
      guiState.searchResultSelected = false;
    }
    if (guiState.scheduleClassRemovedSelected){
      RemoveClassFromSchedule(studentSchedule, guiState.scheduleClassRemoved);
    }
    if (guiState.fontSizeDropdownSubmitted){
      switch (guiState.fontSizeSelected)
      {
      case FONT_SIZE_SMALL_INDEX:
        GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE_SMALL);
        break;
      case FONT_SIZE_NORMAL_INDEX:
        GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE_NORMAL);
        break;
      default:
        break;
      }
    }
    BeginDrawing();
      ClearBackground(RAYWHITE);
      UpdateGuiLayout(&guiState);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

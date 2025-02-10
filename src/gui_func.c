#include "gui.h"
#include "def.h"
#include <stddef.h>
#include "gui_func.h"

void SearchAndFilter(GuiLayoutState *guiState, Class **classes)
{
  if (guiState->searchSubmitted || guiState->filterSubmitted || guiState->matchCurrentCellToggled)
  {
    if (guiState->searchResults)
    
    {
      // Classses are referenced from the main classes array, only the array of classes need to be freed
      MemFree(guiState->searchResults);
      guiState->searchResults = NULL;
    }
    // Apply filter first, then search on those results
    Class **filteredClasses = classes;
    int filterPhase = 0;
    for (int i = 0; i < 2; i++)
    {
      Class **temp = NULL;
      int selectedOption = i == filterPhase ? guiState->filterOptionsDropdownSelected : guiState->searchOptionsDropdownSelected;
      char *searchText = i == filterPhase ? guiState->filterText : guiState->searchText;
      switch (selectedOption)
      {
      case PROPERTY_ALL:
      {
        temp = FilterByAll(filteredClasses, searchText);
        break;
      }
      case COURSE_ID_PROPERTY_INDEX:
      {
        temp = FilterByCourseId(filteredClasses, searchText);
        break;
      }
      case COURSE_NAME_PROPERTY_INDEX:
      {
        temp = FilterByCourseName(filteredClasses, searchText);
        break;
      }
      case CLASS_ID_PROPERTY_INDEX:
      {
        temp = FilterByClassId(filteredClasses, searchText);
        break;
      }
      default:
        break;
      }
      if (temp == NULL)
      {
        break; // Something crazy happened
      }
      // If filtering, pass the classes to the search phase
      if (i == filterPhase)
      {
        filteredClasses = temp;
      }
      else
      {
        // If searching, free the filtered classes (the intermediate result)
        MemFree(filteredClasses);
        guiState->searchResults = temp;
      }
    }
    // Wait there's more
    if (guiState->matchCurrentCellEnabled && guiState->emptyCellSelectedDay >= 0 && guiState->emptyCellSelectedPeriod >= 0)
    {
      Class **temp = guiState->searchResults;
      guiState->searchResults = FilterByPeriod(guiState->searchResults, guiState->emptyCellSelectedDay, guiState->emptyCellSelectedPeriod);
      MemFree(temp);
    }
  }
}

void SetFontStyle(GuiLayoutState *guiState)
{
  if (guiState->fontStyleSubmitted)
  {
    TraceLog(LOG_INFO, "Font style submitted");
    switch (guiState->fontStyleSelected)
    {
    case FONT_STYLE_NORMAL_INDEX:
      GuiSetFont(LoadFont(FONT_STYLE_NORMAL));
      break;
    case FONT_STYLE_BOLD_INDEX:
      GuiSetFont(LoadFont(FONT_STYLE_BOLD));
      break;
    case FONT_STYLE_MONO_INDEX:
      GuiSetFont(LoadFont(FONT_STYLE_MONO));
      break;
    case FONT_STYLE_MONO_BOLD_INDEX:
      GuiSetFont(LoadFont(FONT_STYLE_BOLD));
      break;
    default:
      break;
    }
  }
}

void MatchCurrentCell(GuiLayoutState *guiState, Class** classes){
  if (guiState->emptyCellSelected){
    guiState->searchResults = FilterByPeriod(classes, guiState->emptyCellSelectedDay, guiState->emptyCellSelectedPeriod);
  }
}

void AddToSchedule(GuiLayoutState *guiState, StudentSchedule *studentSchedule){
  if (guiState->searchResultSelected){
    AddClassToSchedule(studentSchedule, guiState->searchResultSelectedClass);
  }
}

void RemoveFromSchedule(GuiLayoutState *guiState, StudentSchedule *studentSchedule){
  if (guiState->scheduleClassRemovedSelected)
  {
    RemoveClassFromSchedule(studentSchedule, guiState->scheduleClassRemoved);
  }
}

GuiLayoutState InitGuiLayout(StudentSchedule *studentSchedule)
{
  GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE_DEFAULT);
  GuiSetFont(LoadFont(FONT_STYLE_DEFAULT));
  GuiLayoutState guiState = InitGuiLayoutState();
  guiState.studentSchedule = studentSchedule;
  return guiState;
}

void ExportSchedule(GuiLayoutState* state, StudentSchedule* studentSchedule)
{
  if (state->exportButtonPressed){
    const char* exportPath = "resources/schedule.txt";
    ExportStudentSchedule(*studentSchedule, exportPath);
  }
}

void ImportSchedule(GuiLayoutState* state, StudentSchedule* studentSchedule)
{
  if (IsFileDropped()){
    FilePathList filePaths = LoadDroppedFiles();
    if (filePaths.count > 0){
      ClearStudentSchedule(studentSchedule);
      *studentSchedule = ImportStudentSchedule(filePaths.paths[0]);
    }
    UnloadDroppedFiles(filePaths);
  }
}

void UpdateGUIState(GuiLayoutState *guiState, Class** classes, StudentSchedule *studentSchedule){
  SearchAndFilter(guiState, classes);
  SetFontStyle(guiState);
  MatchCurrentCell(guiState, classes);
  AddToSchedule(guiState, studentSchedule);
  RemoveFromSchedule(guiState, studentSchedule);
  ImportSchedule(guiState, studentSchedule);
  ExportSchedule(guiState, studentSchedule);
}

void DeinitGuiLayout(GuiLayoutState *guiState){
  if (guiState->searchResults){
    MemFree(guiState->searchResults);
    guiState->searchResults = NULL;
  }
}
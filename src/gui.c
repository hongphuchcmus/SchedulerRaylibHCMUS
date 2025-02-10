#include "gui.h"
#include "def.h"
#include <stddef.h>
#include <math.h>
#include "raymath.h"
#include <string.h>

Rectangle fCalculateCellRect(int i, int j){
  return (Rectangle){
    TABLE_PADDING + TABLE_MARGIN_LEFT + TABLE_CELL_WIDTH * j, TABLE_PADDING + TABLE_MARGIN_TOP + TABLE_HEADER_HEIGHT + TABLE_CELL_HEIGHT * i,
    TABLE_CELL_WIDTH - 2 * TABLE_PADDING, TABLE_CELL_HEIGHT - 2 * TABLE_PADDING
  };
}

Rectangle fCalculateClassSheduleRect(int day, int periodStart, int periodEnd){
  return (Rectangle){
    TABLE_PADDING + TABLE_MARGIN_LEFT + TABLE_CELL_WIDTH * (day - 2), TABLE_PADDING + TABLE_MARGIN_TOP + TABLE_HEADER_HEIGHT + TABLE_CELL_HEIGHT * periodStart,
    TABLE_CELL_WIDTH - 2 * TABLE_PADDING, TABLE_CELL_HEIGHT * (periodEnd - periodStart + 1) - 2 * TABLE_PADDING
  };
}

GuiLayoutState InitGuiLayoutState()
{
  GuiLayoutState state = {0};
  state.searchEditMode = false;
  state.searchSubmitted = false;
  state.searchResultsScrollOffset = (Vector2){0};
  state.searchResults = NULL;
  state.studentSchedule = NULL;
  state.searchOptionsDropdownSelected = 0;
  state.searchOptionsDropdownActive = false;
  state.emptyCellSelected = false;
  state.emptyCellSelectedPeriod = -1;
  state.emptyCellSelectedDay = -1;
  state.searchResultSelectedClass = NULL;
  state.scheduleClassRemovedSelected = false;
  state.scheduleClassRemoved = NULL;
  state.filterEditMode = false;
  state.filterSubmitted = false;
  state.filterOptionsDropdownSelected = 0;
  state.filterOptionsDropdownActive = false;
  state.matchCurrentCellEnabled = false;
  state.matchCurrentCellToggled = false;
  state.fontStyleSelected = FONT_STYLE_DEFAULT_INDEX;

  // (Search + Filter) 
  // Filter Label
  state.layoutRecs[25] = (Rectangle){SCREEN_WIDTH - 400, 10, 190, 30};
  // Filter By Dropdown
  state.layoutRecs[26] = (Rectangle){SCREEN_WIDTH - 200, 10, 190, 30};
  // Match Current Cell Checkbox
  state.layoutRecs[28] = (Rectangle){SCREEN_WIDTH - 400, 50, 30, 30};
  // Filter Text Box
  state.layoutRecs[27] = (Rectangle){SCREEN_WIDTH - 400, 90, 390, 30};
  // Search Label
  state.layoutRecs[0] = (Rectangle){SCREEN_WIDTH - 400, 130, 190, 30};
  // Search By Dropdown
  state.layoutRecs[3] = (Rectangle){SCREEN_WIDTH - 200, 130, 190, 30};
  // Search Text Box
  state.layoutRecs[1] = (Rectangle){SCREEN_WIDTH - 400, 170, 390, 30};
  // Search Results Scroll Panel Bounds
  state.layoutRecs[2] = (Rectangle){SCREEN_WIDTH - 400, 210, 390, 400};
  
  // Schedule Layout
  // Day Of Week Label
  for (int i = 0; i < 7; i++) {
    state.layoutRecs[4 + i] = (Rectangle){TABLE_MARGIN_LEFT + i * TABLE_CELL_WIDTH, TABLE_MARGIN_TOP, TABLE_CELL_WIDTH , TABLE_HEADER_HEIGHT};
  }

  // Period Labels
  for (int i = 0; i < 14; i++)
  {
    state.layoutRecs[11 + i] = (Rectangle){10, TABLE_MARGIN_TOP + TABLE_HEADER_HEIGHT + i * TABLE_CELL_HEIGHT, 100, TABLE_CELL_HEIGHT};
  }

  // Tooltip Label
  state.layoutRecs[34]  = (Rectangle){10, SCREEN_HEIGHT - 90, 110, 20};
  // Tooltip Text Box
  state.layoutRecs[30]  = (Rectangle){120, SCREEN_HEIGHT - 90, SCREEN_WIDTH - 130, 20};
  // Credit Counter Label
  state.layoutRecs[29]  = (Rectangle){10, SCREEN_HEIGHT - 60, 390, 20};
  // Controls Label
  state.layoutRecs[33]  = (Rectangle){10, SCREEN_HEIGHT - 30, SCREEN_WIDTH - 300, 20};
  // Font Style Label
  state.layoutRecs[32]  = (Rectangle){SCREEN_WIDTH - 310, SCREEN_HEIGHT - 30, 60, 20};
  // Font Style Edit Toggle Group
  state.layoutRecs[31]  = (Rectangle){SCREEN_WIDTH - 240, SCREEN_HEIGHT - 30, 70, 20};

  return state;
}

void UpdateGuiLayout(GuiLayoutState *state)
{
  const char* filterOptions = "All;Course ID;Course Name;Class ID";

  //Filter Label
  GuiLabel(state->layoutRecs[25], "Filter by:");
  // Filter Text Box
  state->filterSubmitted = false;
  if (GuiTextBox(state->layoutRecs[27], state->filterText, 128, state->filterEditMode)){
    state->filterEditMode = !state->filterEditMode;
    if (!state->filterEditMode){
      state->filterSubmitted = true;
    }
  }
  if (state->filterEditMode && GetKeyPressed()){
    state->filterSubmitted = true;
  }
  // Match Current Cell Checkbox
  bool preMatchCurrentCellEnabled = state->matchCurrentCellEnabled;
  GuiCheckBox(state->layoutRecs[28], "Match Selected Cell", &state->matchCurrentCellEnabled);
  if (preMatchCurrentCellEnabled != state->matchCurrentCellEnabled){
    state->matchCurrentCellToggled = true;
  }

  // Search Label
  GuiLabel(state->layoutRecs[0], "Search by:");
  // Search Text Box
  state->searchSubmitted = false;
  if (GuiTextBox(state->layoutRecs[1], state->searchText, 128, state->searchEditMode)){
    state->searchEditMode = !state->searchEditMode;
    if (!state->searchEditMode){
      state->searchSubmitted = true;
    }
  }
  if (state->searchEditMode && GetKeyPressed()){
    state->searchSubmitted = true;
  }
  // Search Results Scroll Panel
  // FIXME: Due to not implementing exclusive mode, if you click the dropdown, the search result below it will get clicked as well 
  state->searchResultSelected = false;
  state->searchResultSelectedClass = NULL;
  Class* selectedResult = NULL; // The class from the search result that is selected
  Class* hoveredSearchResult = NULL;
  if (state->searchResults) {
    Rectangle view = {0};
    int resultCount = 0;
    
    for (int i = 0; state->searchResults && state->searchResults[i]; i++) resultCount++;
    //// Change the content bounds to match the number of results
    Rectangle resultContentBounds = (Rectangle) {0, 0, 1000, resultCount * 30};
    GuiScrollPanel(state->layoutRecs[2], NULL, resultContentBounds, &state->searchResultsScrollOffset, &view);
    
    BeginScissorMode(view.x, view.y, view.width, view.height);
      for (int i = 0; state->searchResults && state->searchResults[i]; i++)
      {
        // Only draw what is visible
        Rectangle classInfoRec = { state->layoutRecs[2].x + 10 + state->searchResultsScrollOffset.x, state->layoutRecs[2].y + 10 + i * 30 + state->searchResultsScrollOffset.y, 800, 30};
        if (CheckCollisionRecs(classInfoRec, view)){
          char* classInfo = ClassToString(state->searchResults[i]);
          bool isHovered = CheckCollisionPointRec(GetMousePosition(), view) && CheckCollisionPointRec(GetMousePosition(), classInfoRec);
          bool isPressed = isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
          if (isHovered){
            if (isPressed){
              DrawRectangleRec(classInfoRec, COLOR_LIGHTBLUE);
              selectedResult = state->searchResults[i];
            } else {
              DrawRectangleRec(classInfoRec, COLOR_LIGHTBLUE_ALPHA);
            }
            hoveredSearchResult = state->searchResults[i];
          }
          GuiLabel(classInfoRec, classInfo);
          MemFree(classInfo);
        }
      }
    EndScissorMode();
  } else {
    GuiScrollPanel(state->layoutRecs[2], NULL, (Rectangle){0}, &state->searchResultsScrollOffset, NULL);
  }

  // Period Labels
  for (int i = 0; i < 14; i++)
  {
    if (i >= PERIOD_6){
      DrawRectangleRec((Rectangle){10, TABLE_MARGIN_TOP + TABLE_HEADER_HEIGHT + i * TABLE_CELL_HEIGHT, TABLE_MARGIN_LEFT + TABLE_CELL_WIDTH * 7, TABLE_CELL_HEIGHT}, COLOR_LIGHTGRAY_ALPHA);
    }
    GuiLabel(state->layoutRecs[11 + i], PeriodToString(i));
    GuiLine((Rectangle){10, TABLE_MARGIN_TOP + TABLE_HEADER_HEIGHT + i * TABLE_CELL_HEIGHT, TABLE_MARGIN_LEFT + TABLE_CELL_WIDTH * 7, 1}, NULL);
  }
  GuiLine((Rectangle){10, TABLE_MARGIN_TOP + TABLE_HEADER_HEIGHT + 14 * TABLE_CELL_HEIGHT, TABLE_MARGIN_LEFT + TABLE_CELL_WIDTH * 7, 1}, NULL);
  
  // Preview Class
  if (hoveredSearchResult != NULL){
    Rectangle classPreviewRect = fCalculateClassSheduleRect(hoveredSearchResult->classSchedule.dayOfWeek,
      hoveredSearchResult->classSchedule.periodStart, hoveredSearchResult->classSchedule.periodEnd);
    DrawRectangleRec(classPreviewRect, COLOR_LIGHTBLUE_ALPHA);
  }
  
  // Search Dropdown
  if (GuiDropdownBox(state->layoutRecs[3], filterOptions, &state->searchOptionsDropdownSelected, state->searchOptionsDropdownActive)){
    // Prevent both dropdowns from being active at the same time
    if (!state->filterOptionsDropdownActive){
      state->searchOptionsDropdownActive = !state->searchOptionsDropdownActive;
    } 
  }
  // Filter Dropdown
  if (GuiDropdownBox(state->layoutRecs[26], filterOptions, &state->filterOptionsDropdownSelected, state->filterOptionsDropdownActive)){
    state->filterOptionsDropdownActive = !state->filterOptionsDropdownActive;
  }
  // Always prioritize the dropdown, since the dropdown rect is overlapping the search results rect
  else if (selectedResult != NULL){
    state->searchResultSelected = true;
    TraceLog(LOG_INFO, "Selected class: %s", selectedResult->courseName);
    state->searchResultSelectedClass = selectedResult;
  }

  // Schedule Layout
  // Days Of Week  Label
  int tempTextAlign = GuiGetStyle(LABEL, TEXT_ALIGNMENT);
  GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
  GuiLabel(state->layoutRecs[4], "Mon");
  // Tuesday Label
  GuiLabel(state->layoutRecs[5], "Tue");
  // Wednesday Label
  GuiLabel(state->layoutRecs[6], "Wed");
  // Thursday Label
  GuiLabel(state->layoutRecs[7], "Thu");
  // Friday Label
  GuiLabel(state->layoutRecs[8], "Fri");
  // Saturday Label
  GuiLabel(state->layoutRecs[9], "Sat");
  // Sunday Label
  GuiLabel(state->layoutRecs[10], "Sun");
  GuiSetStyle(LABEL, TEXT_ALIGNMENT, tempTextAlign);

  // Schedule Cells 
  for (int i = 0; i < 14; i++){
    for (int j = 0; j < 7; j++){
      state->scheduleCellOccupation[i][j] = false;
    }
  }
  
  Class* hoveredClass = NULL;
  state->scheduleClassRemovedSelected = false;
  if (state->studentSchedule != NULL && state->studentSchedule->classes != NULL) {
    int tempTextWrapMode = GuiGetStyle(DEFAULT, TEXT_WRAP_MODE);
    int tempTextPadding = GuiGetStyle(DEFAULT, TEXT_PADDING);
    GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, TEXT_WRAP_WORD);
    GuiSetStyle(LABEL, TEXT_PADDING, 5);
    
    for (int i = 0; state->studentSchedule->classes[i]; i++)
    {
      Class* classData = state->studentSchedule->classes[i];
      int dayOfWeek = classData->classSchedule.dayOfWeek;
      int periodStart = classData->classSchedule.periodStart;
      int periodEnd = classData->classSchedule.periodEnd;
      char* classInfo = (char*) TextFormat("%s - %s", classData->classId, classData->courseName);
      
      Rectangle classRect = fCalculateClassSheduleRect(dayOfWeek, periodStart, periodEnd);
      
      bool isHovered = CheckCollisionPointRec(GetMousePosition(), classRect);
      bool isPressed = isHovered && IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
      DrawRectangleRec(classRect, RAYWHITE);
      DrawRectangleLinesEx(classRect, 1, GRAY);
      if (isHovered){
        hoveredClass = classData;
        if (isPressed){
          DrawRectangleRec(classRect, COLOR_LIGHTBLUE);
          state->scheduleClassRemovedSelected = true;
          state->scheduleClassRemoved = classData;
        } else {
          DrawRectangleRec(classRect, COLOR_LIGHTBLUE_ALPHA);
        }
      }
      GuiLabel(classRect, classInfo);
      
      for (int p = periodStart; p <= periodEnd; p++)
      {
        state->scheduleCellOccupation[p][dayOfWeek-2] = true;
      }
    }

    GuiSetStyle(LABEL, TEXT_PADDING, tempTextPadding);
    GuiSetStyle(DEFAULT, TEXT_WRAP_MODE, tempTextWrapMode);
  }

  // Empty Cell Selection
  GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
  state->emptyCellSelected = false;
  for (int i = 0; i < 14; i++){
    for (int j = 0; j < 7; j++){
      if (!state->scheduleCellOccupation[i][j]){
        Rectangle cellRect = fCalculateCellRect(i, j);
        bool hovered = CheckCollisionPointRec(GetMousePosition(), cellRect);
        if (hovered){
          DrawRectangleRec(cellRect, COLOR_LIGHTBLUE_ALPHA);
        }
        char* text = NULL;
        if (j == 6){ // No one studies on Sunday
          text = "x";
        }
        if (GuiLabelButton(cellRect, text)){
          state->emptyCellSelected = true;
          state->emptyCellSelectedPeriod = i;
          state->emptyCellSelectedDay = j + 2;
        }
      }
    }
  }

  // Let user use arrow keys to navigate the empty cells
  if (!state->emptyCellSelected && state->emptyCellSelectedPeriod >= 0 && state->emptyCellSelectedDay >= 0){
    int moveX = 0;
    int moveY = 0;
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)){
      moveY = 1;
    } else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)){
      moveY = -1;
    } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)){
      moveX = -1;
    } else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)){
      moveX = 1;
    }
    if (moveX != 0 || moveY != 0){
      int targetPeriod = state->emptyCellSelectedPeriod;
      int targetDay = state->emptyCellSelectedDay;
      for (int i = 0; i < (moveX != 0 ? 6 : 13); i++)
      {
        targetPeriod = (targetPeriod + moveY + 14) % 14;
        targetDay = (targetDay + moveX + 7 - 2) % 7 + 2;
        if (!state->scheduleCellOccupation[targetPeriod][targetDay-2]) {
          state->emptyCellSelectedPeriod = targetPeriod;
          state->emptyCellSelectedDay = targetDay;
          break;
        }
      }
      if (state->scheduleCellOccupation[targetPeriod][targetDay-2]){
        state->emptyCellSelectedPeriod = -1;
        state->emptyCellSelectedDay = -1;
      }
    }
  }

  GuiSetStyle(LABEL, TEXT_ALIGNMENT, tempTextAlign);

  //// Remain selection on the preivously selected cell
  if (!state->emptyCellSelected && state->emptyCellSelectedPeriod >= 0 && state->emptyCellSelectedDay >= 0){
    Rectangle cellRect = fCalculateCellRect(state->emptyCellSelectedPeriod, state->emptyCellSelectedDay - 2);
    DrawRectangleRec(cellRect, COLOR_LIGHTBLUE_ALPHA);
  }
  //// If the cell is collapsed the remove the selection
  if (state->searchResultSelected){
    state->emptyCellSelected = false;
    state->emptyCellSelectedPeriod = -1;
    state->emptyCellSelectedDay = -1;
  }
  
  // A credit counter
  GuiLabel(state->layoutRecs[29], TextFormat("Total Credits: %d", GetCreditCount(state->studentSchedule)));
  
  // Font Style Label
  GuiLabel(state->layoutRecs[32], "Font:");
  // Font Style Edit Dropdown
  const char* fontStyleOptions = "Normal;Bold;Mono";
  
  int tempFontStyleSelected = state->fontStyleSelected;
  GuiToggleGroup(state->layoutRecs[31], fontStyleOptions, &state->fontStyleSelected);
  state->fontStyleSubmitted = false;
  if (tempFontStyleSelected != state->fontStyleSelected){
    state->fontStyleSubmitted = true;
  }

  // Tooltip Label
  GuiLabel(state->layoutRecs[34], "Class Details:");
  // Tooltip holding class details
  if (hoveredClass != NULL){
    char* tooltipText = ClassToString(hoveredClass);
    GuiTextBox(state->layoutRecs[30], tooltipText, strlen(tooltipText), false);
    MemFree(tooltipText);
  } else {
    GuiTextBox(state->layoutRecs[30], "", 128, false);
  }

  state->exportButtonPressed = false;
  if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_E)){
    state->exportButtonPressed = true;
  }

  // Controls Label
  GuiLabel(state->layoutRecs[33], "ARROWS - Navigate    LMB - Add Class    RMB - Remove Class    ESC - Exit    CTRL+E - Export    DRAG & DROP - Import");
}

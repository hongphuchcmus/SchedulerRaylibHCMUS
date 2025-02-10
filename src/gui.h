#ifndef GUI_H_
#define GUI_H_
#include "raylib.h"
#include "raygui.h"
#include "course.h"
#include "schedule.h"

#define MAX_LAYOUT_RECS 100

#define TABLE_CELL_WIDTH 108
#define TABLE_CELL_HEIGHT 40
#define TABLE_HEADER_HEIGHT 40
#define TABLE_MARGIN_LEFT 100
#define TABLE_MARGIN_TOP 10
#define TABLE_PADDING 4

typedef struct GuiLayoutState {
  bool filterEditMode;
  char filterText[128];
  bool filterSubmited;
  bool searchEditMode;
  bool searchSubmited;
  bool matchCurrentCellEnabled;
  bool matchCurrentCellToggled;
  Vector2 searchResultsScrollOffset;
  char searchText[128];
  int filterOptionsDropdownSelected;
  int filterOptionsDropdownActive;
  int searchOptionsDropdownSelected;
  int searchOptionsDropdownActive;
  Class** searchResults;
  const StudentSchedule* studentSchedule;
  Rectangle layoutRecs[MAX_LAYOUT_RECS];
  bool scheduleCellOccupation[14][7];
  bool emptyCellSelected;
  int emptyCellSelectedPeriod;
  int emptyCellSelectedDay;
  bool searchResultSelected;
  const Class* searchResultSelectedClass;
  bool scheduleClassRemovedSelected; // A class is right clicked
  const Class* scheduleClassRemoved;
  bool fontSizeDropdownActive;
  int fontSizeSelected;
  bool fontSizeDropdownSubmitted;
} GuiLayoutState;

GuiLayoutState InitGuiLayoutState();
void UpdateGuiLayout(GuiLayoutState* state);

#endif
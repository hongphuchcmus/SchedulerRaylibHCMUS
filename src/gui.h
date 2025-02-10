#ifndef GUI_H_
#define GUI_H_
#include "raylib.h"
#include "raygui.h"
#include "course.h"
#include "schedule.h"

#define MAX_LAYOUT_RECS 100

#define TABLE_CELL_WIDTH 110
#define TABLE_CELL_HEIGHT 40
#define TABLE_HEADER_HEIGHT 40
#define TABLE_MARGIN_LEFT 50
#define TABLE_MARGIN_TOP 10
#define TABLE_PADDING 4

#define FONT_SIZE_DEFAULT 16
#define FONT_STYLE_DEFAULT "resources/PixelOperator.ttf"
#define FONT_STYLE_NORMAL "resources/PixelOperator.ttf"
#define FONT_STYLE_BOLD "resources/PixelOperator-Bold.ttf"
#define FONT_STYLE_MONO "resources/PixelOperatorMono.ttf"
#define FONT_STYLE_MONO_BOLD "resources/PixelOperatorMono-Bold.ttf"
#define FONT_STYLE_DEFAULT_INDEX 0
#define FONT_STYLE_NORMAL_INDEX 0
#define FONT_STYLE_BOLD_INDEX 1
#define FONT_STYLE_MONO_INDEX 2
#define FONT_STYLE_MONO_BOLD_INDEX 3

#define COLOR_LIGHTBLUE CLITERAL(Color){129, 230, 247, 255}
#define COLOR_LIGHTBLUE_ALPHA CLITERAL(Color){129, 230, 247, 100}
#define COLOR_LIGHTGRAY CLITERAL(Color){240, 240, 240, 255}
#define COLOR_LIGHTGRAY_ALPHA CLITERAL(Color){220, 220, 220, 80}


typedef struct GuiLayoutState {
  bool filterEditMode;
  char filterText[128];
  bool filterSubmitted;
  bool searchEditMode;
  bool searchSubmitted;
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
  bool scheduleClassRemovedSelected; // A class that's right clicked
  const Class* scheduleClassRemoved;
  int fontStyleSelected;
  bool fontStyleSubmitted;
  bool exportButtonPressed;
} GuiLayoutState;

GuiLayoutState InitGuiLayoutState();
void UpdateGuiLayout(GuiLayoutState* state);

#endif
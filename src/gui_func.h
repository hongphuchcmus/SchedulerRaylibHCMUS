#ifndef GUI_FUNC_H_
#define GUI_FUNC_H_
#include "raylib.h"
#include "gui.h"

void SearchAndFilter(GuiLayoutState* guiState, Class** classes);
void SetFontStyle(GuiLayoutState* guiState);
void MatchCurrentCell(GuiLayoutState *guiState, Class** classes);
void AddToSchedule(GuiLayoutState *guiState, StudentSchedule *studentSchedule);
void RemoveFromSchedule(GuiLayoutState *guiState, StudentSchedule *studentSchedule);
void UpdateGUIState(GuiLayoutState *guiState, Class** classes, StudentSchedule *studentSchedule);
GuiLayoutState InitGuiLayout(StudentSchedule *studentSchedule);
void ExportSchedule(GuiLayoutState *guiState, StudentSchedule* studentSchedule);
void ImportSchedule(GuiLayoutState *guiState, StudentSchedule* studentSchedule);

void DeinitGuiLayout(GuiLayoutState* guiState);

#endif
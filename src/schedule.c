#include "schedule.h"
#include "raylib.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void RemoveClassFromSchedule(StudentSchedule *schedule, const Class * classData)
{
  if (schedule->classes == NULL){
    return;
  }
  int removePoint = -1;
  for (int i = 0; schedule->classes[i]; i++)
  {
    if (schedule->classes[i] == classData){
      removePoint = i;
      break;
    }
  }
  if (removePoint < 0){
    return;
  }
  for (int i = removePoint; schedule->classes[i]; i++)
  {
    schedule->classes[i] = schedule->classes[i+1];
  }
}

void AddClassToSchedule(StudentSchedule *schedule, const Class * classData)
{
  if (schedule->classes == NULL){
    schedule->classes = MemAlloc(sizeof(Class*) * 2); //1 more for terminator
    schedule->classes[0] = classData;
    return;
  }
  bool conflicted = false;
  int count;
  for (count = 0; schedule->classes[count]; count++){
    // Check for conflicts
    const Class* currentClass = schedule->classes[count];
    if (classData->classSchedule.dayOfWeek == currentClass->classSchedule.dayOfWeek
      && classData->classSchedule.periodStart <= currentClass->classSchedule.periodEnd
      && currentClass->classSchedule.periodStart <= classData->classSchedule.periodEnd){
        conflicted = true;
        break;
    }
  }
  if (conflicted){
    return;
  }
  schedule->classes = MemRealloc(schedule->classes, sizeof(Class*) * (count + 2)); //1 more for terminator
  schedule->classes[count] = classData;
  schedule->classes[count+1] = NULL;
}


int GetCreditCount(const StudentSchedule *studentSchedule)
{
  if (studentSchedule == NULL || studentSchedule->classes == NULL){
    return 0;
  }
  int creditCount = 0;
  for (int i = 0; studentSchedule->classes[i]; i++)
  {
    creditCount += studentSchedule->classes[i]->creditCount;
  }
  return creditCount;
}

void ClearStudentSchedule(StudentSchedule *studentSchedule)
{
  if (studentSchedule == NULL || studentSchedule->classes == NULL){
    return;
  }
  MemFree(studentSchedule->classes);
  studentSchedule->classes = NULL;
}

StudentSchedule ImportStudentSchedule(const char *filePath)
{
  StudentSchedule schedule = {0};
  FILE* file = fopen(filePath, "r");
  if (!file) return schedule;
  char lineBuffer[1024];
  while (fgets(lineBuffer, 1024, file)){
    Class* classData = ParseClassFormatedString(lineBuffer);
    AddClassToSchedule(&schedule, classData);
  }
  fclose(file);
  return schedule;
}

void ExportStudentSchedule(StudentSchedule studentSchedule, const char* filePath)
{
  if (studentSchedule.classes == NULL){
    return;
  }
  int currentCapacity = 1024;
  int incrementalCapacity = 512;
  char* exportString = MemAlloc(sizeof(char) * currentCapacity);
  char* exportStringPtr = exportString;
  int curSize = 0;
  for (int i = 0; studentSchedule.classes[i]; i++)
  {
    char* classString = ClassToFormatedString(studentSchedule.classes[i]);
    if (curSize + strlen(classString) + 1 > currentCapacity){
      currentCapacity += incrementalCapacity;
      exportString = MemRealloc(exportString, sizeof(char) * currentCapacity);
      exportStringPtr = exportString + curSize;
    } else {
      char* classStringPtr = classString;
      while (*classStringPtr != '\0'){
        *exportStringPtr = *classStringPtr;
        exportStringPtr++;
        classStringPtr++;
        curSize++;
      }
      *exportStringPtr = '\n';
      exportStringPtr++;
      curSize++;
    }
    MemFree(classString);
  }
  *exportStringPtr = '\0';

  FILE* file = fopen(filePath, "w");  
  if (!file){
    MemFree(exportString);
  }
  fputs(exportString, file);
  fclose(file);
  MemFree(exportString);
  return;
}

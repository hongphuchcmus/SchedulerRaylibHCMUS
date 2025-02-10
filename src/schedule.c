#include "schedule.h"
#include "raylib.h"
#include <stddef.h>
#include <stdio.h>

StudentSchedule *CreateStudentSchedule()
{
  StudentSchedule* studentSchedule = MemAlloc(sizeof(StudentSchedule));
  studentSchedule->classes = NULL;
  return studentSchedule;
}

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

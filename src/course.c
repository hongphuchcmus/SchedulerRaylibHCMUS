#include "course.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "language.h"

static const char *fTextToLower(const char *text)
{
  // Remember to free the memory
  char *lowerText = strdup(text);
  for (int i = 0; lowerText[i]; i++)
  {
    if (lowerText[i] >= 'A' && lowerText[i] <= 'Z')
    {
      lowerText[i] = lowerText[i] + 'a' - 'A';
    }
  }
  return lowerText;
}

static const char *fTextToUpper(const char *text)
{
  // Remember to free the memory
  char *upperText = strdup(text);
  for (int i = 0; upperText[i]; i++)
  {
    if (upperText[i] >= 'a' && upperText[i] <= 'z')
    {
      upperText[i] = upperText[i] + 'A' - 'a';
    }
  }
  return upperText;
}

char *ClassToString(const Class *c)
{
  return strdup(TextFormat("%s -- %s -- %s -- %s -- %d/%d -- TC:%d -- T%d %s-%s -- %s -- %d",
    c->courseId, c->courseName, c->classId, c->location, c->enrolledCount, c->classSize, c->creditCount, c->classSchedule.dayOfWeek, PeriodToString(c->classSchedule.periodStart), PeriodToString(c->classSchedule.periodEnd), c->classSchedule.room, c->year));
}

// String that can be saved to file
char *ClassToFormatedString(const Class *c)
{
  return strdup(TextFormat("%s|%s|%s|%s|%d/%d|TC:%d|T%d %s-%s|%s|%d",
    c->courseId, c->courseName, c->classId, c->location, c->enrolledCount, c->classSize, c->creditCount, c->classSchedule.dayOfWeek, PeriodToString(c->classSchedule.periodStart), PeriodToString(c->classSchedule.periodEnd), c->classSchedule.room, c->year));
}

// Parse from saved string
Class *ParseClassFormatedString(const char * classString){
  Class* classData = MemAlloc(sizeof(Class));
  float periodStartFloat;
  float periodEndFloat;
  sscanf(classString, "%[^|]|%[^|]|%[^|]|%[^|]|%d/%d|TC:%d|T%d %f-%f|%[^|]|%d",
    classData->courseId, classData->courseName, classData->classId, classData->location, &classData->enrolledCount, &classData->classSize, &classData->creditCount, &classData->classSchedule.dayOfWeek, &periodStartFloat, &periodEndFloat, classData->classSchedule.room, &classData->year);
  classData->classSchedule.periodStart = ParsePeriod(periodStartFloat);
  classData->classSchedule.periodEnd = ParsePeriod(periodEndFloat);
}


Class *ParseClass(char *dataLine)
{
  // Reading class data from tab separated line
  // Class itself and its string data are heap allocated
  // Remember to free the memory 
  Class* classData = MemAlloc(sizeof(Class));
  *classData = (Class){0};

  // Note: strcpy() copy the dataLine into char[] fields
  // so no need to free the memory for these fields
  int colIndex = 0;  
  
  for (const char * token = strtok(dataLine, "\t"); token != NULL; token = strtok(NULL, "\t\n"))
  {
    switch (colIndex)
    {
    case COURSE_ID_COLUMN:
      strcpy(classData->courseId, token);
      break;
    case COURSE_NAME_COLUMN:
      strcpy(classData->courseName, token);
      break;
    case CLASS_ID_COLUMN:
      strcpy(classData->classId, token);
      break;
    case CLASS_SIZE_COLUMN:
      classData->classSize = TextToInteger(token);
      break;
    case ENROLLED_COUNT_COLUMN:
      classData->enrolledCount = TextToInteger(token);
      break;
    case CREDIT_COUNT_COLUMN:
      classData->creditCount = TextToInteger(token);
      break;
    case CLASS_SCHEDULE_COLUMN:
      classData->classSchedule = ParseClassSchedule(token);
      break;
    case YEAR_COLUMN:
      classData->year = TextToInteger(token);
    case LOCATION_COLUMN:
      strcpy(classData->location, token);
      break;
    default:
      break;
    }
    colIndex++;
  }

  return classData;
}

Class **ParseClasses(const char *filePath)
{
  FILE* file = fopen(filePath, "r");
  if (!file) return NULL;
  
  int currentSize = 100;
  int incrementalCapacity = 50;
  Class** classes = MemAlloc(sizeof(Class*) * currentSize);

  char lineBuffer[1024];
  int count = 0;

  int** resolveTable = ReadResolveTable("resources/vnchar.txt");
  bool firstLine = true;
  while (fgets(lineBuffer, 1024, file)){
    if (firstLine){
      firstLine = false;
      continue;
    }
    if (count + 2 > currentSize){ // 1 more slot for the terminator
      currentSize += incrementalCapacity;
      classes = MemRealloc(classes, sizeof(Class*) * currentSize);
    }
    
    char* normalizedLine = NormalizeVietnamese(lineBuffer, resolveTable);
    Class* classData = ParseClass(normalizedLine);
    MemFree(normalizedLine);
    classes[count] = classData;
    count++;
  }
  classes[count] = NULL;
  fclose(file);
  return classes;
}

ClassSchedule ParseClassSchedule(const char *scheduleStr)
{
  ClassSchedule schedule = (ClassSchedule) {0};
  
  int dayOfWeek;
  float periodStartFloat;
  float periodEndFloat;
  char roomBuffer[50];
  sscanf(scheduleStr, "T%d(%f-%f)%s", &dayOfWeek, &periodStartFloat, &periodEndFloat, roomBuffer);
  //With Room:    T2(1-4)-P.cs2:E202
  //Without Room: T6(1-4)
  if (roomBuffer[0] != '-'){
    strcpy(schedule.room, "N/A");
  } else {
    strcpy(schedule.room, roomBuffer + 1);
  }

  schedule.dayOfWeek = dayOfWeek;
  schedule.periodStart = ParsePeriod(periodStartFloat);
  schedule.periodEnd = ParsePeriod(periodEndFloat);

  return schedule;
}

Period ParsePeriod(float periodFloat)
{
  const float epsilon = 0.01;
  if (fabs(periodFloat - 1.0) < epsilon) return PERIOD_1;
  else if (fabs(periodFloat - 2.0) < epsilon) return PERIOD_2;
  else if (fabs(periodFloat - 2.5) < epsilon) return PERIOD_2_5;
  else if (fabs(periodFloat - 3.0) < epsilon) return PERIOD_3;
  else if (fabs(periodFloat - 3.5) < epsilon) return PERIOD_3_5;
  else if (fabs(periodFloat - 4.0) < epsilon) return PERIOD_4;
  else if (fabs(periodFloat - 5.0) < epsilon) return PERIOD_5;
  else if (fabs(periodFloat - 6.0) < epsilon) return PERIOD_6;
  else if (fabs(periodFloat - 7.0) < epsilon) return PERIOD_7;
  else if (fabs(periodFloat - 7.5) < epsilon) return PERIOD_7_5;
  else if (fabs(periodFloat - 8.0) < epsilon) return PERIOD_8;
  else if (fabs(periodFloat - 8.5) < epsilon) return PERIOD_8_5;
  else if (fabs(periodFloat - 9.0) < epsilon) return PERIOD_9;
  else if (fabs(periodFloat - 10.0) < epsilon) return PERIOD_10;
  else return -1; // Invalid period float
}

float PeriodToFloat(Period period)
{
  switch (period)
  {
    case PERIOD_1: return 1.0f;
    case PERIOD_2: return 2.0f;
    case PERIOD_2_5: return 2.5f;
    case PERIOD_3: return 3.0f;
    case PERIOD_3_5: return 3.5f;
    case PERIOD_4: return 4.0f;
    case PERIOD_5: return 5.0f;
    case PERIOD_6: return 6.0f;
    case PERIOD_7: return 7.0f;
    case PERIOD_7_5: return 7.5f;
    case PERIOD_8: return 8.0f;
    case PERIOD_8_5: return 8.5f;
    case PERIOD_9: return 9.0f;
    case PERIOD_10: return 10.0f;
    default: return -1.0f; // Invalid period
  }
}

char *PeriodToString(Period period)
{
  // This used static buffer, which won't last forever
  float periodFloat = PeriodToFloat(period);
  float epsilon = 0.01f;
  if (fabs(periodFloat - (int)periodFloat) < epsilon){
    return (char*) TextFormat("%d", (int)periodFloat);
  } else {
    return (char*) TextFormat("%.1f", periodFloat);
  }
}
Period ParsePeriodFromString(const char *periodStr)
{
  // This function is not used
  float periodFloat;
  sscanf(periodStr, "%f", &periodFloat);
  return ParsePeriod(periodFloat);
}

void ClearClasses(Class **classes)
{
  if (classes == NULL){
    return;
  }
  for (int i = 0;classes[i]; i++)
  {
    MemFree(classes[i]);
  }
}

Class **FilterByCourseId(Class** classes, const char *courseId)
{
  // Remember to free the memory
  if (classes == NULL){
    return NULL;
  }
  int currentSize = 20;
  int incrementalCapacity = 10;
  Class** filteredClasses = MemAlloc(sizeof(Class*) * currentSize);
  int count = 0;
  for (int i = 0; classes && classes[i]; i++)
  {
    // Don't use TextToLower since it will modify the original string
    const char* lowerCourseId = fTextToLower(classes[i]->courseId);
    const char* lowerSearchText = fTextToLower(courseId);
    if (strstr(lowerCourseId, lowerSearchText))
    {
      if (count + 2 > currentSize){ // 1 more slot for the terminator
        currentSize += incrementalCapacity;
        filteredClasses = MemRealloc(filteredClasses, sizeof(Class*) * currentSize);
      }
      filteredClasses[count] = classes[i];
      count++;
    }
    // Free the memory
    MemFree(lowerCourseId);
    MemFree(lowerSearchText);
  }
  filteredClasses[count] = NULL;
  return filteredClasses;
}

Class **FilterByCourseName(Class** classes, const char *courseName)
{
  // Remember to free the memory
  if (classes == NULL){
    return NULL;
  }
  int currentSize = 20;
  int incrementalCapacity = 10;
  Class** filteredClasses = MemAlloc(sizeof(Class*) * currentSize);
  int count = 0;
  for (int i = 0; classes && classes[i]; i++)
  {
    // Don't use TextToLower since it will modify the original string
    const char* lowerCourseName = fTextToLower(classes[i]->courseName);
    const char* lowerSearchText = fTextToLower(courseName);
    if (strstr(lowerCourseName, lowerSearchText))
    {
      if (count + 2 > currentSize){ // 1 more slot for the terminator
        currentSize += incrementalCapacity;
        filteredClasses = MemRealloc(filteredClasses, sizeof(Class*) * currentSize);
      }
      filteredClasses[count] = classes[i];
      count++;
    }
    // Free the memory
    MemFree(lowerCourseName);
    MemFree(lowerSearchText);
  }
  filteredClasses[count] = NULL;
  return filteredClasses;
}

Class **FilterByClassId(Class** classes, const char *classId)
{
  // Remember to free the memory
  if (classes == NULL){
    return NULL;
  }
  int currentSize = 20;
  int incrementalCapacity = 10;
  Class** filteredClasses = MemAlloc(sizeof(Class*) * currentSize);
  int count = 0;
  for (int i = 0; classes && classes[i]; i++)
  {
    // Don't use TextToLower since it will modify the original string
    const char* lowerClassId = fTextToLower(classes[i]->classId);
    const char* lowerSearchText = fTextToLower(classId);
    if (strstr(lowerClassId, lowerSearchText))
    {
      if (count + 2 > currentSize){ // 1 more slot for the terminator
        currentSize += incrementalCapacity;
        filteredClasses = MemRealloc(filteredClasses, sizeof(Class*) * currentSize);
      }
      filteredClasses[count] = classes[i];
      count++;
    }
    // Free the memory
    MemFree(lowerClassId);
    MemFree(lowerSearchText);
  }
  filteredClasses[count] = NULL;
  return filteredClasses;
}

Class **FilterByAll(Class** classes, const char *searchText)
{
  // Remember to free the memory
  if (classes == NULL){
    return NULL;
  }
  int currentSize = 20;
  int incrementalCapacity = 10;
  Class** filteredClasses = MemAlloc(sizeof(Class*) * currentSize);
  int count = 0;
  for (int i = 0; classes[i]; i++)
  {
    const char* lowerPropertyText;
    int props[3] = {COURSE_ID_PROPERTY_INDEX, COURSE_NAME_PROPERTY_INDEX, CLASS_ID_PROPERTY_INDEX};
    for (int j = 0; j < 3; j++)
    {
      switch (props[j])
      {
      case COURSE_ID_PROPERTY_INDEX:
        lowerPropertyText = fTextToLower(classes[i]->courseId);
        break;
      case COURSE_NAME_PROPERTY_INDEX:
        lowerPropertyText = fTextToLower(classes[i]->courseName);
        break;
      case CLASS_ID_PROPERTY_INDEX:
        lowerPropertyText = fTextToLower(classes[i]->classId);
        break;
      default:
        continue;;
      }

      const char* lowerSearchText = fTextToLower(searchText);
      if (strstr(lowerPropertyText, lowerSearchText))
      {
        if (count + 2 > currentSize){ // 1 more slot for the terminator
          currentSize += incrementalCapacity;
          filteredClasses = MemRealloc(filteredClasses, sizeof(Class*) * currentSize);
        }
        filteredClasses[count] = classes[i];
        count++;
        // Free the memory
        MemFree(lowerPropertyText);
        MemFree(lowerSearchText);
        break;
      }
      // Free the memory
      MemFree(lowerPropertyText);
      MemFree(lowerSearchText);
    }
  }
  filteredClasses[count] = NULL;
  return filteredClasses;
}

Class **FilterByPeriod(Class** classes, int dayOfWeek, Period period)
{
  // Remember to free the memory
  int currentSize = 20;
  int incrementalCapacity = 10;
  Class** filteredClasses = MemAlloc(sizeof(Class*) * currentSize);
  int count = 0;
  for (int i = 0; classes && classes[i]; i++)
  {
    if (classes[i]->classSchedule.dayOfWeek == dayOfWeek && classes[i]->classSchedule.periodStart <= period && classes[i]->classSchedule.periodEnd >= period)
    {
      if (count + 2 > currentSize){ // 1 more slot for the terminator
        currentSize += incrementalCapacity;
        filteredClasses = MemRealloc(filteredClasses, sizeof(Class*) * currentSize);
      }
      filteredClasses[count] = classes[i];
      count++;
    }
  }
  filteredClasses[count] = NULL;
  return filteredClasses;
}


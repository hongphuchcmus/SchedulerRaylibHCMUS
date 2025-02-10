#ifndef SCHEDULE_H_
#define SCHEDULE_H_
#include "course.h"

typedef struct StudentSchedule {
  Class** classes; // Make the classes read-only
} StudentSchedule;

void RemoveClassFromSchedule(StudentSchedule* schedule, const Class* classData);
void AddClassToSchedule(StudentSchedule* schedule, const Class* classData);
int GetCreditCount(const StudentSchedule* studentSchedule);
void ClearStudentSchedule(StudentSchedule* studentSchedule);
StudentSchedule ImportStudentSchedule(const char* filePath);
void ExportStudentSchedule(StudentSchedule studentSchedule, const char* filePath);

#endif
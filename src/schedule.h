#ifndef SCHEDULE_H_
#define SCHEDULE_H_
#include "course.h"

typedef struct StudentSchedule {
  Class** classes; // Make the classes read-only
} StudentSchedule;

StudentSchedule* CreateStudentSchedule();
void RemoveClassFromSchedule(StudentSchedule* schedule, const Class* classData);
void AddClassToSchedule(StudentSchedule* schedule, const Class* classData);
int GetCreditCount(const StudentSchedule* studentSchedule);

#endif
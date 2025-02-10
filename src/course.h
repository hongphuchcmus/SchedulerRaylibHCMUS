#ifndef COURSE_H_
#define COURSE_H_

#define COURSE_ID_COLUMN 0
#define COURSE_NAME_COLUMN 1
#define CLASS_ID_COLUMN 2
#define CREDIT_COUNT_COLUMN 3
#define CLASS_SIZE_COLUMN 4
#define ENROLLED_COUNT_COLUMN 5
#define YEAR_COLUMN 6
#define CLASS_SCHEDULE_COLUMN 7
#define LOCATION_COLUMN 10
#define MAX_CLASSES_PER_DAY 4

typedef enum Period{
  PERIOD_1,
  PERIOD_2,
  PERIOD_2_5,
  PERIOD_3,
  PERIOD_3_5,
  PERIOD_4,
  PERIOD_5,
  PERIOD_6,
  PERIOD_7,
  PERIOD_7_5,
  PERIOD_8,
  PERIOD_8_5,
  PERIOD_9,
  PERIOD_10
} Period;

#define PROPERTY_ALL 0
#define COURSE_ID_PROPERTY_INDEX 1
#define COURSE_NAME_PROPERTY_INDEX 2
#define CLASS_ID_PROPERTY_INDEX 3

typedef struct ClassSchedule {
  int dayOfWeek;
  int periodStart;
  int periodEnd;
  char room[50];
} ClassSchedule ;

typedef struct Class {
  char courseId[10];
  char courseName[100];
  char courseAlias[10];
  char classId[10];
  int classSize;
  int enrolledCount;
  int creditCount;
  ClassSchedule classSchedule;
  int year;
  char location[50];
} Class;

char* ClassToString(const Class* c);
char* ClassToFormatedString(const Class* c);
Class* ParseClassFormatedString(const char* classString);
Class* ParseClass(char* dataLine);
Class** ParseClasses(const char* filePath);
ClassSchedule ParseClassSchedule(const char* scheduleStr);
Period ParsePeriod(float periodFloat);
float PeriodToFloat(Period period);
char *PeriodToString(Period period);
void ClearClasses(Class** classes);

Class** FilterByCourseId(Class** classes, const char* courseId);
Class** FilterByCourseName(Class** classes, const char* courseName);
Class** FilterByClassId(Class** classes, const char* classId);
Class** FilterByAll(Class** classes, const char* seachText);
Class** FilterByPeriod(Class** classes, int dayOfWeek, Period period);

#endif
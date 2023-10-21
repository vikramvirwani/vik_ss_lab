#ifndef COURSE_RECORD
#define COURSE_RECORD

#define MAX_ENROLLMENT 10

struct Course
{
    int id;     // CourseIDs
    char name[35];
    char department[30];
    int noOfSeats;
    int courseCredit;
    bool active;
    int enrollment;
};

#endif
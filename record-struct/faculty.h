#ifndef FACULTY_RECORD
#define FACULTY_RECORD
#include "../record-struct/course.h"
#define MAX_ENROLLMENT 10

struct Faculty 
{
    int id;     // Faculty IDs
    char name[35];
    char email[30];
    char address[50];
    char designation[30];
    char password[1000];
    int course_id[100];
    int noOfCourse;
};

#endif
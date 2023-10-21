#ifndef STUDENT_RECORD
#define STUDENT_RECORD

#define MAX_ENROLLMENT 10

struct Student
{
    int id;     // Student IDs
    char name[25];
    char email[30];
    char address[50];
    int age;
    char password[1000];
    int active;
};

#endif
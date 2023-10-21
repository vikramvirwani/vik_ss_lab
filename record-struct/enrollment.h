#ifndef ENROLLMENT_RECORD
#define ENROLLMENT_RECORD

#define MAX_ENROLLMENT 10

struct Enrollment
{
    int student_id;     // Student IDs
    int course_id;
    bool active;
};

#endif
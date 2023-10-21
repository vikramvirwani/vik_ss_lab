#ifndef FACULTY_FUNCTIONS
#define FACULTY_FUNCTIONS

#include "common.h"
struct Student loggedInStudent;

// Function Prototypes =================================

bool student_operation_handler(int connFD);
bool change_student_password(int connFD);
bool view_all_course(int connFD);
bool enroll_course(int connFD);
bool view_enroll_course(int connFD);
bool unenroll_course(int connFD);
// =====================================================

// Function Definition =================================

bool student_operation_handler(int connFD)
{
    if (login_handler_student(connFD, &loggedInStudent))
    {
        ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
        char readBuffer[1000], writeBuffer[1000]; // A buffer used for reading & writing to the client
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, STUDENT_MENU);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing STUDENT_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for STUDENT_MENU");
                return false;
            }
            
            // printf("READ BUFFER : %s\n", readBuffer);
            int choice = atoi(readBuffer);
            // printf("CHOICE : %d\n", choice);
            switch (choice)
            {
            case 1:
                view_all_course(connFD);
                break;
            case 2:
                enroll_course(connFD);
                break;
            case 3:
                unenroll_course(connFD);
                break;
            case 4:
                view_enroll_course(connFD);
                break;
            case 5:
                change_student_password(connFD);
                break;
            case 6:
                writeBytes = write(connFD, CUSTOMER_LOGOUT, strlen(CUSTOMER_LOGOUT));
                return false;
                break;
            default:
                
            }
        }
    }
    else
    {
        // CUSTOMER LOGIN FAILED
        return false;
    }
    return true;
}

// =====================================================
bool change_student_password(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];


    int studentFileDescriptor;

    off_t offset;
    int lockingStatus;

    
    writeBytes = write(connFD, STUDENT_MOD_PASSWORD, strlen(STUDENT_MOD_PASSWORD));
    if (writeBytes == -1)
    {
        perror("Error while writing STUDENT_MOD_PASSWORD message to client!");
        return false;
    }
    readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while getting response for STUDENT's new name from client!");
        return false;
    }
    char hashedPassword[1000];
    strcpy(hashedPassword, crypt(readBuffer, SALT_BAE));
    strcpy(loggedInStudent.password, hashedPassword);

    
    studentFileDescriptor = open(STUDENT_FILE, O_WRONLY);
    if (studentFileDescriptor == -1)
    {
        perror("Error while opening Student file");
        return false;
    }
    offset = lseek(studentFileDescriptor, loggedInStudent.id * sizeof(struct Student), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required Student record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Student), getpid()};
    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(studentFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on student record!");
        return false;
    }

    writeBytes = write(studentFileDescriptor, &loggedInStudent, sizeof(struct Student));
    if (writeBytes == -1)
    {
        perror("Error while writing update student info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(studentFileDescriptor, F_SETLKW, &lock);

    close(studentFileDescriptor);

    writeBytes = write(connFD, ADMIN_MOD_STUDENT_SUCCESS, strlen(ADMIN_MOD_STUDENT_SUCCESS));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_STUDENT_SUCCESS message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return true;
}
bool view_all_course(int connFD)
{
    ssize_t readBytes, writeBytes;             // Number of bytes read from / written to the socket
    char readBuffer[1000], writeBuffer[10000]; // A buffer for reading from / writing to the socket
    char tempBuffer[1000];
    int CourseID = -1;
    struct Course course;
    int courseFileDescriptor;
    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Course), getpid()};

  

    courseFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (courseFileDescriptor == -1)
    {
        // Customer File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, FACULTY_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    
    lock.l_start = 0;

    int lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining read lock on the course file!");
        return false;
    }
    
    bzero(writeBuffer, sizeof(writeBuffer));
    while (read(courseFileDescriptor, &course, sizeof(struct Course)) == sizeof(struct Course)){
        if(course.active==true){
            bzero(tempBuffer, sizeof(tempBuffer));
            sprintf(tempBuffer, "-------------------------------------------------\nCourse Details - \n\tCourse ID : %d\n\tCourse Name : %s\n\tCourse Department : %s\n\tCourse Seats : %d\n\tCourse Credit : %d", course.id, course.name, course.department,course.noOfSeats,course.courseCredit);
            strcat(writeBuffer, tempBuffer);
            strcat(writeBuffer, "\n");
        }
    }

    strcat(writeBuffer, "\n^");
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));   
    if (writeBytes == -1)
    {
        perror("Error writing faculty info to client!");
        return false;
    }
    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLK, &lock);
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    return true;
    
}
bool enroll_course(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Course course;
    struct Enrollment newEnrollment;
    
    int courseID;

    off_t offset;
    int lockingStatus;

    writeBytes = write(connFD, STUDENT_ENROL_COURSE, strlen(STUDENT_ENROL_COURSE));
    if (writeBytes == -1)
    {
        perror("Error while writing STUDENT_ENROL_COURSE message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading student ID from client!");
        return false;
    }

    courseID = atoi(readBuffer);

    int courseFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (courseFileDescriptor == -1)
    {
        // Student File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    
    offset = lseek(courseFileDescriptor, courseID * sizeof(struct Course), SEEK_SET);
    if (errno == EINVAL)
    {
        // Student record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required Course record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Course), getpid()};

    // Lock the record to be read
    lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on Course record!");
        return false;
    }
    readBytes = read(courseFileDescriptor, &course, sizeof(struct Course));
    if (readBytes == -1 )
    {
        perror("Error while reading Student record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLK, &lock);

    close(courseFileDescriptor);

    newEnrollment.active=true;
    newEnrollment.course_id=course.id;
    newEnrollment.student_id=loggedInStudent.id;

    int EnrollFileDescriptor = open(ENROLLMENT_FILE, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);

    if (EnrollFileDescriptor == -1)
    {
        perror("Error while creating / opening Student file!");
        return false;
    }
    writeBytes = write(EnrollFileDescriptor, &newEnrollment, sizeof(newEnrollment));

    if (writeBytes == -1)
    {
        perror("Error while writing Student record to file!");
        return false;
    }

    close(EnrollFileDescriptor);
    
    struct flock lock1 = {F_RDLCK, SEEK_SET, offset, sizeof(struct Course), getpid()};
 
    courseFileDescriptor = open(COURSE_FILE, O_WRONLY);
    if (courseFileDescriptor == -1)
    {
        perror("Error while opening Course file");
        return false;
    }
    offset = lseek(courseFileDescriptor, courseID * sizeof(struct Course), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required Course record!");
        return false;
    }

    lock1.l_type = F_WRLCK;
    lock1.l_start = offset;
    lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock1);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on Faculty record!");
        return false;
    }
    course.enrollment=course.enrollment+1;
    writeBytes = write(courseFileDescriptor, &course, sizeof(struct Course));
    if (writeBytes == -1)
    {
        perror("Error while writing update Faculty info into file");
    }

    lock1.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLKW, &lock1);

    close(courseFileDescriptor);

    sprintf(writeBuffer, "%s%d", ENROLLMENT_CREATED,courseID);
    strcat(writeBuffer, "\n^");
    writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing ENROLLMENT_CREATED message to client!");
        return false;
    }


    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    return true;

}
bool view_enroll_course(int connFD){
    ssize_t readBytes, writeBytes;             // Number of bytes read from / written to the socket
    char readBuffer[1000], writeBuffer[10000]; // A buffer for reading from / writing to the socket
    char tempBuffer[1000];
    
    struct Course course;
    int courseFileDescriptor;
    struct flock lock = {F_RDLCK, SEEK_SET, 0, sizeof(struct Course), getpid()};

    struct Enrollment enroll;
    int enrollFileDescriptor;
    struct flock lock1 = {F_RDLCK, SEEK_SET, 0, sizeof(struct Enrollment), getpid()};


    courseFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (courseFileDescriptor == -1)
    {
        // Customer File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, FACULTY_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    enrollFileDescriptor = open(ENROLLMENT_FILE, O_RDONLY);
    if (enrollFileDescriptor == -1)
    {
        // Enrollment File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, FACULTY_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    
    lock.l_start = 0;
    lock1.l_start = 0;
    int lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
    int lockingStatus1 = fcntl(enrollFileDescriptor, F_SETLKW, &lock1);
    if (lockingStatus == -1 || lockingStatus1==-1)
    {
        perror("Error while obtaining read lock on the courseorenrollment file!");
        return false;
    }
    
    bzero(writeBuffer, sizeof(writeBuffer));
    while (read(enrollFileDescriptor, &enroll, sizeof(struct Enrollment)) == sizeof(struct Enrollment))
    {   
        if(enroll.student_id==loggedInStudent.id){
            lseek(courseFileDescriptor, 0, SEEK_SET);
            while (read(courseFileDescriptor, &course, sizeof(struct Course)) == sizeof(struct Course)){
                if(course.id==enroll.course_id && enroll.active==true && course.active==true){
                    bzero(tempBuffer, sizeof(tempBuffer));
                    sprintf(tempBuffer, "-------------------------------------------------\nCourse Details - \n\tCourse ID : %d\n\tCourse Name : %s\n\tCourse Department : %s\n\tCourse Seats : %d\n\tCourse Credit : %d", course.id, course.name, course.department,course.noOfSeats,course.courseCredit);
                    strcat(writeBuffer, tempBuffer);
                    strcat(writeBuffer, "\n");
                }
            }
        }
    }
    strcat(writeBuffer, "\n^");
    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));   
    if (writeBytes == -1)
    {
        perror("Error writing faculty info to client!");
        return false;
    }
    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLK, &lock);
    lock1.l_type = F_UNLCK;
    fcntl(enrollFileDescriptor, F_SETLK, &lock1);
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    return true;
}
bool unenroll_course(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Course course;
    
    int courseID;

    off_t offset;
    int lockingStatus;

    writeBytes = write(connFD, STUDENT_UNENROL_COURSE, strlen(STUDENT_UNENROL_COURSE));
    if (writeBytes == -1)
    {
        perror("Error while writing STUDENT_UNENROL_COURSE message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading student ID from client!");
        return false;
    }

    courseID = atoi(readBuffer);

    int courseFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (courseFileDescriptor == -1)
    {
        // Student File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    
    offset = lseek(courseFileDescriptor, courseID * sizeof(struct Course), SEEK_SET);
    if (errno == EINVAL)
    {
        // Student record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing STUDENT_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required Course record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Course), getpid()};

    // Lock the record to be read
    lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on Course record!");
        return false;
    }
    if(course.active==true && course.noOfSeats>course.enrollment)
    readBytes = read(courseFileDescriptor, &course, sizeof(struct Course));
    if (readBytes == -1 )
    {
        perror("Error while reading Student record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLK, &lock);

    close(courseFileDescriptor);

    

    struct Enrollment enroll;

    int EnrollFileDescriptor = open(ENROLLMENT_FILE, O_RDWR, S_IRWXU);
    if (EnrollFileDescriptor == -1)
    {
        perror("Error while creating / opening Student file!");
        return false;
    }
    struct flock lock2 = {F_WRLCK, SEEK_SET, offset, sizeof(struct Enrollment), getpid()};
    lock2.l_start = 0;
    int lockingStatus2 = fcntl(EnrollFileDescriptor, F_SETLKW, &lock2);
    if (lockingStatus2==-1)
    {
        perror("Error while obtaining read lock on the courseorenrollment file!");
        return false;
    }
    
    bzero(writeBuffer, sizeof(writeBuffer));
    while (read(EnrollFileDescriptor, &enroll, sizeof(struct Enrollment)) == sizeof(struct Enrollment))
    {   
        if(enroll.student_id==loggedInStudent.id && enroll.course_id==courseID){
            enroll.active=false;
            writeBytes = write(EnrollFileDescriptor, &enroll, sizeof(struct Enrollment));
            if (writeBytes == -1)
            {
                perror("Error while writing update Faculty info into file");
                return false;
            }
            break;
        }
    }
    
    lock2.l_type = F_UNLCK;
    fcntl(EnrollFileDescriptor, F_SETLK, &lock2);
    

    close(EnrollFileDescriptor);
    
    struct flock lock1 = {F_RDLCK, SEEK_SET, offset, sizeof(struct Course), getpid()};
 
    courseFileDescriptor = open(COURSE_FILE, O_WRONLY);
    if (courseFileDescriptor == -1)
    {
        perror("Error while opening Course file");
        return false;
    }
    offset = lseek(courseFileDescriptor, courseID * sizeof(struct Course), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required Course record!");
        return false;
    }

    lock1.l_type = F_WRLCK;
    lock1.l_start = offset;
    lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock1);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on Faculty record!");
        return false;
    }
    course.enrollment=course.enrollment-1;
    writeBytes = write(courseFileDescriptor, &course, sizeof(struct Course));
    if (writeBytes == -1)
    {
        perror("Error while writing update Faculty info into file");
    }

    lock1.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLKW, &lock1);

    close(courseFileDescriptor);

    sprintf(writeBuffer, "%s%d", ENROLLMENT_DELETED,courseID);
    strcat(writeBuffer, "\n^");
    writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing ENROLLMENT_DELETED message to client!");
        return false;
    }


    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    return true;

}
#endif
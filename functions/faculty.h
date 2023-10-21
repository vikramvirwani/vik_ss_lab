#ifndef STUDENT_FUNCTIONS
#define STUDENT_FUNCTIONS

#include "common.h"
struct Faculty loggedInFaculty;

// Function Prototypes =================================

bool faculty_operation_handler(int connFD);
bool change_faculty_password(int connFD);
bool add_course(int connFD);
bool view_course(int connFD);
bool remove_course(int connFD);
bool update_course(int connFD);
// =====================================================

// Function Definition =================================

bool faculty_operation_handler(int connFD)
{
    if (login_handler_faculty(connFD, &loggedInFaculty))
    {
        ssize_t writeBytes, readBytes;            // Number of bytes read from / written to the client
        char readBuffer[1000], writeBuffer[1000]; // A buffer used for reading & writing to the client
        
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, STUDENT_LOGIN_SUCCESS);
        while (1)
        {
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, FACULTY_MENU);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));

            if (writeBytes == -1)
            {
                perror("Error while writing FACULTY_MENU to client!");
                return false;
            }
            bzero(writeBuffer, sizeof(writeBuffer));

            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1)
            {
                perror("Error while reading client's choice for FACULTY_MENU");
                return false;
            }
            
            int choice = atoi(readBuffer);
            switch (choice)
            {
            case 1:
                view_course(connFD);
                break;
            case 2:
                add_course(connFD);
                break;
            case 3:
                remove_course(connFD);
                break;
            case 4:
                update_course(connFD);
                break;
            case 5:
                change_faculty_password(connFD);
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
bool update_course(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Course course;

    int courseID;

    off_t offset;
    int lockingStatus;

    writeBytes = write(connFD, FACULTY_MOD_COURSE_ID, strlen(FACULTY_MOD_COURSE_ID));
    if (writeBytes == -1)
    {
        perror("Error while writing FACULTY_MOD_COURSE_ID message to client!");
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
    if (readBytes == -1 && course.active==false)
    {
        perror("Error while reading Student record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLK, &lock);

    close(courseFileDescriptor);

    writeBytes = write(connFD, FACULTY_MOD_COURSE_MENU, strlen(FACULTY_MOD_COURSE_MENU));
    if (writeBytes == -1)
    {
        perror("Error while writing FACULTY_MOD_COURSE_MENU message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while getting student modification menu choice from client!");
        return false;
    }

    int choice = atoi(readBuffer);
    if (choice == 0)
    { // A non-numeric string was passed to atoi
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    switch (choice)
    {
    case 1:
        writeBytes = write(connFD, FACULTY_MOD_COURSE_NAME, strlen(FACULTY_MOD_COURSE_NAME));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_MOD_COURSE_NAME message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for Course's new name from client!");
            return false;
        }
        strcpy(course.name, readBuffer);

        break;
    case 2:
        writeBytes = write(connFD, FACULTY_MOD_COURSE_DEPARTMENT, strlen(FACULTY_MOD_COURSE_DEPARTMENT));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_MOD_COURSE_DEPARTMENT message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for customer's new gender from client!");
            return false;
        }
        strcpy(course.department, readBuffer);
        break;

    case 3:
        writeBytes = write(connFD, FACULTY_MOD_COURSE_SEATS, strlen(FACULTY_MOD_COURSE_SEATS));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_MOD_COURSE_SEATS message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for customer's new age from client!");
            return false;
        }
        int seats = atoi(readBuffer);
        if (seats == 0)
        {
            // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
            bzero(writeBuffer, sizeof(writeBuffer));
            strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
                return false;
            }
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
            return false;
        }
        course.noOfSeats = seats;
        break;
        
    case 4:
        writeBytes = write(connFD, FACULTY_MOD_COURSE_CREDITS, strlen(FACULTY_MOD_COURSE_CREDITS));
        if (writeBytes == -1)
        {
            perror("Error while writing FACULTY_MOD_COURSE_CREDITS message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for customer's new age from client!");
            return false;
        }
        int updatedCredits = atoi(readBuffer);
        if (updatedCredits == 0)
        {
            // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
            bzero(writeBuffer, sizeof(writeBuffer));
            strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
                return false;
            }
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
            return false;
        }
        course.courseCredit = updatedCredits;
        break;
    default:
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, INVALID_MENU_CHOICE);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing INVALID_MENU_CHOICE message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    
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

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on Course record!");
        return false;
    }

    writeBytes = write(courseFileDescriptor, &course, sizeof(struct Course));
    if (writeBytes == -1)
    {
        perror("Error while writing update student info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLKW, &lock);

    close(courseFileDescriptor);

    writeBytes = write(connFD, COURSE_MOD_SUCCESS, strlen(COURSE_MOD_SUCCESS));
    if (writeBytes == -1)
    {
        perror("Error while writing COURSE_MOD_SUCCESS message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return true;
}
bool change_faculty_password(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];


    int FacultyFileDescriptor;

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
        perror("Error while getting response for Faculty's new name from client!");
        return false;
    }
    char hashedPassword[1000];
    strcpy(hashedPassword, crypt(readBuffer, SALT_BAE));
    strcpy(loggedInFaculty.password, hashedPassword);

    
    FacultyFileDescriptor = open(FACULTY_FILE, O_WRONLY);
    if (FacultyFileDescriptor == -1)
    {
        perror("Error while opening Faculty file");
        return false;
    }
    offset = lseek(FacultyFileDescriptor, loggedInFaculty.id * sizeof(struct Faculty), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required Faculty record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Faculty), getpid()};
    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(FacultyFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on Faculty record!");
        return false;
    }

    writeBytes = write(FacultyFileDescriptor, &loggedInFaculty, sizeof(struct Faculty));
    if (writeBytes == -1)
    {
        perror("Error while writing update Faculty info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(FacultyFileDescriptor, F_SETLKW, &lock);

    close(FacultyFileDescriptor);

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
bool add_course(int connFD){
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Course newCourse, previousCourse;

    int CourseFileDescriptor = open(COURSE_FILE, O_RDONLY);
    if (CourseFileDescriptor == -1 && errno == ENOENT)
    {
        // Course file was never created
        newCourse.id = 0;
    }
    else if (CourseFileDescriptor == -1)
    {
        perror("Error while opening Course file");
        return -1;
    }
    else
    {
        int offset = lseek(CourseFileDescriptor, -sizeof(struct Course), SEEK_END);
        if (offset == -1)
        {
            perror("Error seeking to last Course record!");
            return false;
        }

        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Course), getpid()};
        int lockingStatus = fcntl(CourseFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1)
        {
            perror("Error obtaining read lock on Course record!");
            return false;
        }

        readBytes = read(CourseFileDescriptor, &previousCourse, sizeof(struct Course));
        if (readBytes == -1)
        {
            perror("Error while reading Course record from file!");
            return false;
        }

        lock.l_type = F_UNLCK;
        fcntl(CourseFileDescriptor, F_SETLK, &lock);

        close(CourseFileDescriptor);

        newCourse.id = previousCourse.id + 1;
    }

    sprintf(writeBuffer, "%s", FACULTY_ADD_COURSE_NAME);
    writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));

    if (writeBytes == -1)
    {
        perror("Error writing FACULTY_ADD_COURSE_NAME message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading FACULTY_ADD_COURSE_NAME response from client!");
        ;
        return false;
    }
    
    strcpy(newCourse.name, readBuffer);

    sprintf(writeBuffer, "%s", FACULTY_ADD_COURSE_DEPARTMENT);
    writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));

    if (writeBytes == -1)
    {
        perror("Error writing FACULTY_ADD_COURSE_DEPARTMENT message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading FACULTY_ADD_COURSE_DEPARTMENT response from client!");
        ;
        return false;
    }

    strcpy(newCourse.department, readBuffer);

    sprintf(writeBuffer, "%s", FACULTY_ADD_COURSE_SEAT);
    writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));

    if (writeBytes == -1)
    {
        perror("Error writing FACULTY_ADD_COURSE_SEAT message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading SEATS response from client!");
        return false;
    }

    newCourse.noOfSeats = atoi(readBuffer);
    newCourse.enrollment=0;
    newCourse.active=true;
    sprintf(writeBuffer, "%s", FACULTY_ADD_COURSE_CREDIT);
    writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));

    if (writeBytes == -1)
    {
        perror("Error writing FACULTY_ADD_COURSE_CREDIT message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading SEATS response from client!");
        return false;
    }

    newCourse.courseCredit = atoi(readBuffer);

    CourseFileDescriptor = open(COURSE_FILE, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (CourseFileDescriptor == -1)
    {
        perror("Error while creating / opening Student file!");
        return false;
    }
    writeBytes = write(CourseFileDescriptor, &newCourse, sizeof(newCourse));
    if (writeBytes == -1)
    {
        perror("Error while writing Student record to file!");
        return false;
    }

    close(CourseFileDescriptor);
    off_t offset;
    int lockingStatus;
    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Faculty), getpid()};

    loggedInFaculty.course_id[loggedInFaculty.noOfCourse++]=newCourse.id;
    int facultyFileDescriptor = open(FACULTY_FILE, O_WRONLY);
    if (facultyFileDescriptor == -1)
    {
        perror("Error while opening Faculty file");
        return false;
    }
    offset = lseek(facultyFileDescriptor, loggedInFaculty.id * sizeof(struct Faculty), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required Faculty record!");
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(facultyFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on Faculty record!");
        return false;
    }

    writeBytes = write(facultyFileDescriptor, &loggedInFaculty, sizeof(struct Faculty));
    if (writeBytes == -1)
    {
        perror("Error while writing update Faculty info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(facultyFileDescriptor, F_SETLKW, &lock);

    close(facultyFileDescriptor);

    sprintf(writeBuffer, "%s%d", COURSE_CREATED,newCourse.id);
    strcat(writeBuffer, "\n^");
    writeBytes = write(connFD, writeBuffer, sizeof(writeBuffer));
    if (writeBytes == -1)
    {
        perror("Error writing COURSE_CREATED message to client!");
        return false;
    }


    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    return true;



}
bool view_course(int connFD)
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
    for(int i=0;i<loggedInFaculty.noOfCourse;i++)
    {   
        lseek(courseFileDescriptor, 0, SEEK_SET);
        while (read(courseFileDescriptor, &course, sizeof(struct Course)) == sizeof(struct Course)){
            if(course.id==loggedInFaculty.course_id[i] && course.active==true){
                bzero(tempBuffer, sizeof(tempBuffer));
                sprintf(tempBuffer, "-------------------------------------------------\nCourse Details - \n\tCourse ID : %d\n\tCourse Name : %s\n\tCourse Department : %s\n\tCourse Seats : %d\n\tCourse Credit : %d", course.id, course.name, course.department,course.noOfSeats,course.courseCredit);
                strcat(writeBuffer, tempBuffer);
                strcat(writeBuffer, "\n");
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
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    return true;
    
}
bool remove_course(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Course course;

    int courseID;

    off_t offset;
    int lockingStatus;

    writeBytes = write(connFD, FACULTY_DEL_COURSE_ID, strlen(FACULTY_DEL_COURSE_ID));
    if (writeBytes == -1)
    {
        perror("Error while writing FACULTY_DEL_COURSE_ID message to client!");
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
    if (readBytes == -1)
    {
        perror("Error while reading Student record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLK, &lock);

    close(courseFileDescriptor);

    course.active=false;
    loggedInFaculty.noOfCourse--;

    int FacultyFileDescriptor = open(FACULTY_FILE, O_WRONLY);
    if (FacultyFileDescriptor == -1)
    {
        perror("Error while opening Faculty file");
        return false;
    }
    offset = lseek(FacultyFileDescriptor, loggedInFaculty.id * sizeof(struct Faculty), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required Faculty record!");
        return false;
    }

    struct flock lock1 = {F_RDLCK, SEEK_SET, offset, sizeof(struct Faculty), getpid()};
    lock1.l_type = F_WRLCK;
    lock1.l_start = offset;
    lockingStatus = fcntl(FacultyFileDescriptor, F_SETLKW, &lock1);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on Faculty record!");
        return false;
    }

    writeBytes = write(FacultyFileDescriptor, &loggedInFaculty, sizeof(struct Faculty));
    if (writeBytes == -1)
    {
        perror("Error while writing update Faculty info into file");
    }

    lock1.l_type = F_UNLCK;
    fcntl(FacultyFileDescriptor, F_SETLKW, &lock1);

    close(FacultyFileDescriptor);    

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

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(courseFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on Course record!");
        return false;
    }

    writeBytes = write(courseFileDescriptor, &course, sizeof(struct Course));
    if (writeBytes == -1)
    {
        perror("Error while writing update student info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(courseFileDescriptor, F_SETLKW, &lock);

    close(courseFileDescriptor);

    writeBytes = write(connFD, COURSE_MOD_DELETE_SUCCESS, strlen(COURSE_MOD_DELETE_SUCCESS));
    if (writeBytes == -1)
    {
        perror("Error while writing COURSE_MOD_DELETE_SUCCESS message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return true;
}
#endif
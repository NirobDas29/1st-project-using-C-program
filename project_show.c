#define _POSIX_C_SOURCE 200809L // Define before includes for POSIX functions like remove, rename
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Needed for system(), exit()
#include <ctype.h>  // For isspace
#include <errno.h>  // For errno with file operations

// --- Data Structures ---

// Structure for common routine details
struct RoutineInfo
{
    char day[30];      // e.g., "Sunday", "Monday-Wednesday"
    char date[30];     // e.g., "25/12/2023", "TBA"
    char time[30];     // e.g., "10:00 AM - 11:30 AM", "02:00 PM"
    char room[20];     // e.g., "Room 501", "Online", "TBA"
    char faculty[100]; // e.g., "Dr. Smith (DS)", "TBA"
};

// Structure for academic schedule (Revised Again)
struct AcademicSchedule
{
    char intake[20];
    char section[20];
    char scheduleType[50];          // "Class Routine", "Mid Exam Routine", "Final Exam Routine", "Result Publication Date", "Other Event"
    struct RoutineInfo routineData; // Use for routine types
    char otherDetails[500];         // Use for non-routine types or extra notes
    // int uniqueID; // Optional: Add later if needed for precise updates/deletes
};

// Structure for student result (Keep as is)
struct StudentResult
{
    char studentID[20];
    char name[100];
    char intake[20];
    char section[20];
    float gpa;
    char grade[5];
};

// --- Function Prototypes ---

// Admission Phase
void handleAdmissionPhase(FILE **P_ptr);
void searchStudentByName(FILE **P_ptr, const char *departmentName);
void viewStudentsByIntakeSection(FILE **P_ptr, const char *departmentName);
void deleteStudentById(FILE **P_ptr, const char *departmentName);
void updateStudentById(FILE **P_ptr, const char *departmentName);
void addStudent(FILE **P_ptr, const char *departmentName);
void viewAllStudents(FILE **P_ptr, const char *departmentName);
void searchStudentById(FILE **P_ptr, const char *departmentName);

// Academic Schedule
void manageAcademicSchedules(FILE **scheduleFile_ptr);
void addSchedule(FILE **scheduleFile_ptr);
void viewSchedule(FILE **scheduleFile_ptr); // Enhanced display
void updateSchedule(FILE **scheduleFile_ptr);
void deleteSchedule(FILE **scheduleFile_ptr);
int isRoutineType(const char *scheduleType); // Helper

// Result Management
void manageResults(FILE **resultFile_ptr);
void addResult(FILE **resultFile_ptr);
void viewIndividualResult(FILE **resultFile_ptr);
void viewResultList(FILE **resultFile_ptr);
void updateResult(FILE **resultFile_ptr);
void deleteResult(FILE **resultFile_ptr);

// Helper Functions
void calculateGrade(float gpa, char *grade);
void pressEnterToContinue();
void trimWhitespace(char *str);
void clearInputBuffer();

// --- Main Function ---
int main()
{
    FILE *P = NULL;
    FILE *scheduleFile = NULL;
    FILE *resultFile = NULL;

    // Open student file (text mode)
    P = fopen("test.txt", "a+"); // Append plus (read/write, create if needed, append writes)
    if (P == NULL)
    {
        perror("FATAL: Error opening student data file (test.txt)");
        return 1;
    }

    // Open schedule file (binary mode)
    scheduleFile = fopen("schedules.dat", "rb+"); // Read/write binary, must exist
    if (scheduleFile == NULL)
    {
        if (errno == ENOENT)
        {                                                 // If file doesn't exist, create it
            scheduleFile = fopen("schedules.dat", "wb+"); // Write/read binary, create/truncate
            if (scheduleFile == NULL)
            {
                perror("FATAL: Error creating schedule data file (schedules.dat)");
                fclose(P);
                return 1;
            }
        }
        else
        { // Other error opening with rb+
            perror("FATAL: Error opening schedule data file (schedules.dat)");
            fclose(P);
            return 1;
        }
    }

    // Open result file (binary mode)
    resultFile = fopen("results.dat", "rb+"); // Read/write binary, must exist
    if (resultFile == NULL)
    {
        if (errno == ENOENT)
        {                                             // If file doesn't exist, create it
            resultFile = fopen("results.dat", "wb+"); // Write/read binary, create/truncate
            if (resultFile == NULL)
            {
                perror("FATAL: Error creating result data file (results.dat)");
                fclose(P);
                if (scheduleFile)
                    fclose(scheduleFile);
                return 1;
            }
        }
        else
        { // Other error opening with rb+
            perror("FATAL: Error opening result data file (results.dat)");
            fclose(P);
            if (scheduleFile)
                fclose(scheduleFile);
            return 1;
        }
    }

    // Optional: Disable buffering if experiencing output delays, especially on some systems
    // setvbuf(stdout, NULL, _IONBF, 0);
    // setvbuf(stderr, NULL, _IONBF, 0);

    int select;
    long long int pass;
    int running = 1;

    system("clear || cls"); // Use || cls for Windows compatibility
    printf("Enter Password: ");

    // Use long long specifier %lld
    if (scanf("%lld", &pass) != 1)
    {
        printf("Invalid password input format.\n");
        clearInputBuffer(); // Clear the invalid input
        if (P)
            fclose(P);
        if (scheduleFile)
            fclose(scheduleFile);
        if (resultFile)
            fclose(resultFile);
        return 1;
    }
    clearInputBuffer(); // IMPORTANT: Clear buffer after scanf

    if (pass == 5455778081)
    {
        while (running)
        {
            system("clear || cls");
            printf("\n\n\n------------------------ What Do You Want ------------------------\n\n");
            printf("1. Admission Phase Info\n");
            printf("2. Academic Schedule Management\n");
            printf("3. Result Publication Management\n");
            printf("4. Exit\n");
            printf("\nEnter Choice: ");

            if (scanf("%d", &select) != 1)
            {
                printf("Invalid input. Please enter a number (1-4).\n");
                clearInputBuffer(); // Clear the invalid input
                pressEnterToContinue();
                continue;
            }
            clearInputBuffer(); // IMPORTANT: Clear buffer after scanf

            switch (select)
            {
            case 1:
                handleAdmissionPhase(&P); // Pass address of the pointer
                break;
            case 2:
                // Ensure scheduleFile pointer is valid before passing
                if (scheduleFile == NULL)
                {
                    scheduleFile = fopen("schedules.dat", "rb+"); // Try reopening
                    if (scheduleFile == NULL)
                        scheduleFile = fopen("schedules.dat", "wb+");
                    if (scheduleFile == NULL)
                    {
                        perror("ERROR: Cannot access schedule file in main loop");
                        pressEnterToContinue();
                        continue; // Skip this menu option if file is bad
                    }
                }
                manageAcademicSchedules(&scheduleFile); // Pass address
                break;
            case 3:
                // Ensure resultFile pointer is valid before passing
                if (resultFile == NULL)
                {
                    resultFile = fopen("results.dat", "rb+"); // Try reopening
                    if (resultFile == NULL)
                        resultFile = fopen("results.dat", "wb+");
                    if (resultFile == NULL)
                    {
                        perror("ERROR: Cannot access result file in main loop");
                        pressEnterToContinue();
                        continue; // Skip this menu option if file is bad
                    }
                }
                manageResults(&resultFile); // Pass address
                break;
            case 4:
                printf("Exiting program.\n");
                running = 0;
                break;
            default:
                printf("Invalid choice (%d). Please enter a number between 1 and 4.\n", select);
                pressEnterToContinue();
                break;
            }
        } // end while(running)
    }
    else
    {
        printf("Wrong Password\n");
        pressEnterToContinue();
    }

    // --- Close all files before exiting ---
    printf("Closing files...\n");
    if (P != NULL)
    {
        if (fclose(P) != 0)
            perror("Error closing student file");
        else
            printf("Student file closed.\n");
    }
    if (scheduleFile != NULL)
    {
        if (fclose(scheduleFile) != 0)
            perror("Error closing schedule file");
        else
            printf("Schedule file closed.\n");
    }
    if (resultFile != NULL)
    {
        if (fclose(resultFile) != 0)
            perror("Error closing result file");
        else
            printf("Result file closed.\n");
    }

    printf("\nProgram terminated.\n");
    return 0;
}

// --- Helper Functions ---
void clearInputBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void trimWhitespace(char *str)
{
    if (str == NULL || *str == '\0')
        return; // Handle NULL or empty string

    // Remove trailing newline or space
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1]))
    {
        str[--len] = '\0';
    }

    // If string becomes empty after trailing trim
    if (len == 0)
        return;

    // Remove leading space
    char *start = str;
    while (*start && isspace((unsigned char)*start))
    {
        start++;
    }

    // Shift string if leading space was removed
    // Check if start points past the original end (can happen if all were spaces)
    if (start >= str + len)
    {
        *str = '\0'; // String contained only whitespace
        return;
    }

    if (start != str)
    {
        memmove(str, start, strlen(start) + 1);
    }
}

void pressEnterToContinue()
{
    printf("\nPress Enter to continue...");
    // Clear buffer *before* waiting for Enter, in case previous input left junk
    clearInputBuffer();
    getchar(); // Consume the Enter press itself
}

void calculateGrade(float gpa, char *grade)
{
    if (gpa > 4.0)
        gpa = 4.0; // Cap GPA at 4.0
    if (gpa < 0.0)
        gpa = 0.0; // Floor GPA at 0.0

    if (gpa >= 4.0)
        strcpy(grade, "A+");
    else if (gpa >= 3.75)
        strcpy(grade, "A");
    else if (gpa >= 3.50)
        strcpy(grade, "A-");
    else if (gpa >= 3.25)
        strcpy(grade, "B+");
    else if (gpa >= 3.00)
        strcpy(grade, "B");
    else if (gpa >= 2.75)
        strcpy(grade, "B-");
    else if (gpa >= 2.50)
        strcpy(grade, "C+");
    else if (gpa >= 2.25)
        strcpy(grade, "C");
    else if (gpa >= 2.00)
        strcpy(grade, "D");
    else
        strcpy(grade, "F");
}

// --- Admission Phase Handling ---
void handleAdmissionPhase(FILE **P_ptr)
{
    char user[100];
    system("clear || cls");
    printf("Enter your username: ");
    // Use fgets safely
    if (fgets(user, sizeof(user), stdin) == NULL)
    {
        printf("Error reading username.\n");
        return;
    }
    trimWhitespace(user);

    if (strcmp(user, "A") == 0)
    {
        system("clear || cls");
        // Print department info (condensed for brevity)
        printf("========================================================================================\n");
        printf("      =============== Bangladesh University of Business and Technology =============\n");
        printf("========================================================================================\n\n");
        printf(":::::::::::::::::::::::: Information of Departments ::::::::::::::::::::::::\n");
        // ... (rest of the department info print statements) ...
        printf("   1. CSE (GPA: 9.50, Fees: 5,80,000/=)\n");
        printf("   2. BBA (GPA: 9.25, Fees: 5,70,000/=)\n");
        printf("   3. EEE (GPA: 9.00, Fees: 5,40,000/=)\n");
        printf("   4. ENGLISH (GPA: 8.50, Fees: 5,50,000/=)\n\n");

        printf("                  Which Department do you want to manage?                 \n\n");
        printf("1.CSE   2.EEE   3.BBA   4.ENGLISH\n\n");
        printf("Enter choice (1-4): ");

        int deptChoice;
        if (scanf("%d", &deptChoice) != 1)
        {
            printf("Invalid input. Please enter a number.\n");
            clearInputBuffer();
            pressEnterToContinue();
            return;
        }
        clearInputBuffer(); // Clear buffer after scanf

        if (deptChoice >= 1 && deptChoice <= 4)
        {
            int departmentRunning = 1;
            char departmentName[20];
            switch (deptChoice)
            {
            case 1:
                strcpy(departmentName, "CSE");
                break;
            case 2:
                strcpy(departmentName, "EEE");
                break;
            case 3:
                strcpy(departmentName, "BBA");
                break;
            case 4:
                strcpy(departmentName, "ENGLISH");
                break;
            default:
                printf("Internal error: Invalid department choice.\n");
                return; // Should not happen
            }

            while (departmentRunning)
            {
                system("clear || cls");
                printf("\n--- Department: %s ---\n", departmentName);
                printf("1. Add Student\n");
                printf("2. View All Students\n");
                printf("3. Search Student by ID\n");
                printf("4. Search Student by Name\n");
                printf("5. Delete Student by ID\n");
                printf("6. Update Student Info by ID\n");
                printf("7. View Students by Intake & Section\n");
                printf("8. Back to Main Menu\n");
                printf("\nEnter choice (1-8): ");

                int subSelect;
                if (scanf("%d", &subSelect) != 1)
                {
                    printf("Invalid input. Please enter a number (1-8).\n");
                    clearInputBuffer();
                    pressEnterToContinue();
                    continue;
                }
                clearInputBuffer(); // Clear buffer after scanf

                // Ensure the student file pointer is valid before calling sub-functions
                if (*P_ptr == NULL)
                {
                    fprintf(stderr, "ERROR: Student file pointer is NULL in handleAdmissionPhase.\n");
                    // Attempt recovery or exit? For now, let's try to reopen.
                    *P_ptr = fopen("test.txt", "a+");
                    if (*P_ptr == NULL)
                    {
                        perror("FATAL: Could not reopen student file");
                        exit(EXIT_FAILURE);
                    }
                    printf("INFO: Student file pointer was reopened.\n");
                }

                switch (subSelect)
                {
                case 1:
                    addStudent(P_ptr, departmentName);
                    break;
                case 2:
                    viewAllStudents(P_ptr, departmentName);
                    break;
                case 3:
                    searchStudentById(P_ptr, departmentName);
                    break;
                case 4:
                    searchStudentByName(P_ptr, departmentName);
                    break;
                case 5:
                    deleteStudentById(P_ptr, departmentName);
                    break;
                case 6:
                    updateStudentById(P_ptr, departmentName);
                    break;
                case 7:
                    viewStudentsByIntakeSection(P_ptr, departmentName);
                    break;
                case 8:
                    departmentRunning = 0;
                    break;
                default:
                    printf("Invalid choice (%d). Please enter 1-8.\n", subSelect);
                    pressEnterToContinue();
                    break;
                }
                // Pause is handled within sub-functions or implicitly by loop
            }
        }
        else
        {
            printf("Invalid department choice (%d).\n", deptChoice);
            pressEnterToContinue();
        }
    }
    else
    {
        printf("Wrong Username\n");
        pressEnterToContinue();
    }
}

// --- Student Admission Function Implementations ---
// NOTE: These functions interact with a TEXT file ("test.txt").
// The parsing logic is complex and depends heavily on the exact format.
// Consider migrating to a binary format for student data for robustness.

void addStudent(FILE **P_ptr, const char *departmentName)
{
    // Check if file pointer is valid
    if (*P_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Student file is not open in addStudent.\n");
        pressEnterToContinue();
        return;
    }

    int addStudentLoop = 1;
    while (addStudentLoop)
    {
        system("clear || cls");
        printf("\n--- Add New Student to %s ---\n", departmentName);
        // Use descriptive variable names and clear them
        char firstName[100] = "", lastName[100] = "", fatherName[100] = "", motherName[100] = "";
        char studentID[100] = "", intake[100] = "", section[100] = "";
        char presentAddress[200] = "", permanentAddress[200] = "";                                     // Increased size
        char bloodGroup[20] = "", mobileNumber[20] = "", backupMobileNumber[20] = "", email[100] = ""; // Adjusted sizes

        // --- Get Input using fgets and trim ---
        printf("Student First Name: ");
        if (!fgets(firstName, sizeof(firstName), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(firstName);

        printf("Student Last Name: ");
        if (!fgets(lastName, sizeof(lastName), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(lastName);

        printf("Father's Name: ");
        if (!fgets(fatherName, sizeof(fatherName), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(fatherName);

        printf("Mother's Name: ");
        if (!fgets(motherName, sizeof(motherName), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(motherName);

        printf("Student ID: ");
        if (!fgets(studentID, sizeof(studentID), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(studentID);

        printf("Intake: ");
        if (!fgets(intake, sizeof(intake), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(intake);

        printf("Section: ");
        if (!fgets(section, sizeof(section), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(section);

        printf("Present Address: ");
        if (!fgets(presentAddress, sizeof(presentAddress), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(presentAddress);

        printf("Permanent Address: ");
        if (!fgets(permanentAddress, sizeof(permanentAddress), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(permanentAddress);

        printf("Blood Group: ");
        if (!fgets(bloodGroup, sizeof(bloodGroup), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(bloodGroup);

        printf("Mobile Number: ");
        if (!fgets(mobileNumber, sizeof(mobileNumber), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(mobileNumber);

        printf("Backup Mobile Number: ");
        if (!fgets(backupMobileNumber, sizeof(backupMobileNumber), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(backupMobileNumber);

        printf("Email Address: ");
        if (!fgets(email, sizeof(email), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(email);

        // --- Validate Input ---
        if (strlen(studentID) == 0 || strlen(firstName) == 0 || strlen(lastName) == 0)
        {
            printf("\nError: Student ID and Full Name are required.\n");
        }
        else
        {
            // --- Write to File ---
            // Ensure we are at the end for appending in "a+" mode
            if (fseek(*P_ptr, 0, SEEK_END) != 0)
            {
                perror("Error seeking to end of student file before writing");
            }
            else
            {
                // Write the data with labels
                fprintf(*P_ptr, "Student Name: %s %s\n", firstName, lastName);
                fprintf(*P_ptr, "Father's name: %s\n", fatherName);
                fprintf(*P_ptr, "Mother's name: %s\n", motherName);
                fprintf(*P_ptr, "Student ID: %s\n", studentID);
                fprintf(*P_ptr, "Department: %s\n", departmentName);
                fprintf(*P_ptr, "Intake: %s\n", intake);
                fprintf(*P_ptr, "Section: %s\n", section);
                fprintf(*P_ptr, "Present Address: %s\n", presentAddress);
                fprintf(*P_ptr, "Permanent Address: %s\n", permanentAddress);
                fprintf(*P_ptr, "Blood Group: %s\n", bloodGroup);
                fprintf(*P_ptr, "Mobile number: %s\n", mobileNumber);
                fprintf(*P_ptr, "Backup Mobile Number: %s\n", backupMobileNumber);
                fprintf(*P_ptr, "Email: %s\n\n", email); // Add blank line separator

                fflush(*P_ptr); // Flush the output buffer

                if (ferror(*P_ptr))
                {
                    perror("Error writing student data");
                    clearerr(*P_ptr);
                }
                else
                {
                    printf("\n---------- Congratulations! Student added successfully. ----------\n");
                }
            }
        }

        // --- Add another? ---
        printf("\n---------- Add another student to %s? ----------\n", departmentName);
        printf("1. Yes\n2. No (Back to %s Menu)\n", departmentName);
        printf("Enter Choice: ");
        int addChoice;
        if (scanf("%d", &addChoice) != 1)
        {
            addChoice = 2;      // Default to No on bad input
            clearInputBuffer(); // Clear bad input
        }
        else
        {
            clearInputBuffer(); // Clear trailing newline
        }

        if (addChoice != 1)
            addStudentLoop = 0;
    }
    pressEnterToContinue(); // Pause before returning to department menu
}

void viewAllStudents(FILE **P_ptr, const char *departmentName)
{
    if (*P_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Student file is not open in viewAllStudents.\n");
        pressEnterToContinue();
        return;
    }

    system("clear || cls");
    printf("\n\n------------------- Student List for Department: %s -------------------\n\n", departmentName);

    rewind(*P_ptr); // Go to the beginning of the file
    if (ferror(*P_ptr))
    { // Check for errors after rewind
        perror("Error rewinding student file");
        clearerr(*P_ptr);
        pressEnterToContinue();
        return;
    }

    // Check if file is empty by trying to read a character
    int firstChar = fgetc(*P_ptr);
    if (firstChar == EOF)
    {
        if (ferror(*P_ptr))
        {
            perror("Error checking if student file is empty");
        }
        else
        {
            printf("\nNo students added yet!\n");
        }
        clearerr(*P_ptr);
    }
    else
    {
        ungetc(firstChar, *P_ptr); // Put the character back

        printf("%-25s %-15s %-15s %-10s %-10s\n", "Student Name", "Student ID", "Mobile Number", "Intake", "Section");
        printf("------------------------------------------------------------------------------\n");

        // Variables to hold data for the current record being read
        char name1[100] = "", name2[100] = "", studentID[100] = "", mobileNumber[100] = "";
        char currentDept[100] = "", intake[100] = "", section[100] = "";
        char line[512]; // Increased buffer size for reading lines
        char fullName[201] = "";
        int recordsFound = 0;
        int readingRecord = 0; // Flag: are we currently inside a record block?

        // Initialize potential fields for each record
        fullName[0] = '\0';
        studentID[0] = '\0';
        mobileNumber[0] = '\0';
        currentDept[0] = '\0';
        intake[0] = '\0';
        section[0] = '\0';
        name1[0] = '\0';
        name2[0] = '\0';

        while (fgets(line, sizeof(line), *P_ptr))
        {
            char trimmedLine[512];
            strcpy(trimmedLine, line);
            trimWhitespace(trimmedLine);

            if (strlen(trimmedLine) == 0)
            { // Blank line is the record separator
                if (readingRecord)
                {
                    // Process the completed record
                    if (strcmp(currentDept, departmentName) == 0)
                    {
                        printf("%-25s %-15s %-15s %-10s %-10s\n", fullName, studentID, mobileNumber, intake, section);
                        recordsFound = 1;
                    }
                    // Reset for the next potential record
                    readingRecord = 0;
                    fullName[0] = '\0';
                    studentID[0] = '\0';
                    mobileNumber[0] = '\0';
                    currentDept[0] = '\0';
                    intake[0] = '\0';
                    section[0] = '\0';
                    name1[0] = '\0';
                    name2[0] = '\0';
                }
                continue; // Move to the next line
            }

            // If not a blank line, it's part of a record
            readingRecord = 1; // Mark that we are inside a record

            // Attempt to parse relevant fields from the current line
            // Use temporary buffers for sscanf to avoid partial overwrites if parsing fails
            char temp_s1[100], temp_s2[100];
            if (sscanf(trimmedLine, "Student Name: %99s %99s", temp_s1, temp_s2) == 2)
            {
                strcpy(name1, temp_s1);
                strcpy(name2, temp_s2);
                snprintf(fullName, sizeof(fullName), "%s %s", name1, name2);
            }
            else if (sscanf(trimmedLine, "Student ID: %99s", temp_s1) == 1)
            {
                strcpy(studentID, temp_s1);
            }
            else if (sscanf(trimmedLine, "Department: %99s", temp_s1) == 1)
            {
                strcpy(currentDept, temp_s1);
            }
            else if (sscanf(trimmedLine, "Mobile number: %99s", temp_s1) == 1)
            {
                strcpy(mobileNumber, temp_s1);
            }
            else if (sscanf(trimmedLine, "Intake: %99s", temp_s1) == 1)
            {
                strcpy(intake, temp_s1);
            }
            else if (sscanf(trimmedLine, "Section: %99s", temp_s1) == 1)
            {
                strcpy(section, temp_s1);
            }
            // Ignore other lines like Father's Name, Address etc. for this summary view
        }

        // Process the last record if the file doesn't end with a blank line
        if (readingRecord && strcmp(currentDept, departmentName) == 0)
        {
            printf("%-25s %-15s %-15s %-10s %-10s\n", fullName, studentID, mobileNumber, intake, section);
            recordsFound = 1;
        }

        if (!recordsFound)
        {
            printf("\nNo students found in the %s department.\n", departmentName);
        }
        printf("------------------------------------------------------------------------------\n");
    }

    clearerr(*P_ptr); // Clear any EOF or error flags
    pressEnterToContinue();
}

void searchStudentById(FILE **P_ptr, const char *departmentName)
{
    if (*P_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Student file is not open in searchStudentById.\n");
        pressEnterToContinue();
        return;
    }

    int searchAgain = 1;
    while (searchAgain)
    {
        system("clear || cls");
        printf("\n--- Search Student by ID in %s ---\n", departmentName);
        printf("Enter Student ID to search: ");
        char searchID[100];
        if (!fgets(searchID, sizeof(searchID), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(searchID);

        if (strlen(searchID) == 0)
        {
            printf("Search ID cannot be empty.\n");
            pressEnterToContinue();
            continue;
        }

        rewind(*P_ptr);
        if (ferror(*P_ptr))
        {
            perror("Error rewinding student file for search");
            clearerr(*P_ptr);
            pressEnterToContinue();
            return; // Cannot proceed if rewind failed
        }

        int found = 0;
        char line[512];
        // Buffers for the *found* record's details
        char name1[100] = "", name2[100] = "", father[100] = "", mother[100] = "";
        char currentID[100] = "", currentDept[100] = "", intake[100] = "", section[100] = "";
        char presentAddr[200] = "", permanentAddr[200] = "", blood[20] = "";
        char mobile[20] = "", backupMobile[20] = "", email[100] = "";
        char fullName[201] = "";

        // Temporary buffers for parsing within the loop
        char temp_id[100], temp_dept[100];
        int readingRecord = 0;        // Flag: currently processing a record block
        char recordBuffer[2048] = ""; // Buffer to hold the current record block

        // Reset loop state variables
        recordBuffer[0] = '\0';
        readingRecord = 0;
        temp_id[0] = '\0';
        temp_dept[0] = '\0'; // Clear temps for safety

        while (fgets(line, sizeof(line), *P_ptr))
        {
            char trimmedLine[512];
            strcpy(trimmedLine, line);
            trimWhitespace(trimmedLine);

            if (strlen(trimmedLine) == 0)
            { // End of record block (blank line separator)
                if (readingRecord)
                {
                    // Check if the buffered record was the match
                    if (strcmp(temp_id, searchID) == 0 && strcmp(temp_dept, departmentName) == 0)
                    {
                        found = 1;
                        // Parse the *entire buffer* to get all details for the found record
                        // This parsing needs to be robust
                        char *linePtr = recordBuffer;
                        char blockLine[512];
                        // Clear fields before parsing buffer
                        name1[0] = '\0';
                        name2[0] = '\0';
                        father[0] = '\0';
                        mother[0] = '\0';
                        strcpy(currentID, temp_id);
                        strcpy(currentDept, temp_dept); // Keep matched ID/Dept
                        intake[0] = '\0';
                        section[0] = '\0';
                        presentAddr[0] = '\0';
                        permanentAddr[0] = '\0';
                        blood[0] = '\0';
                        mobile[0] = '\0';
                        backupMobile[0] = '\0';
                        email[0] = '\0';

                        while (sscanf(linePtr, "%511[^\n]", blockLine) == 1)
                        { // Read line from buffer
                            char *nextNewline = strchr(linePtr, '\n');

                            // Attempt to parse each field type from blockLine
                            char temp_s1[200], temp_s2[100]; // Larger buffers for address
                            if (sscanf(blockLine, "Student Name: %99s %99s", temp_s1, temp_s2) == 2)
                            {
                                strcpy(name1, temp_s1);
                                strcpy(name2, temp_s2);
                            }
                            else if (sscanf(blockLine, "Father's name: %99[^\n]", temp_s1) == 1)
                            {
                                strcpy(father, temp_s1);
                            }
                            else if (sscanf(blockLine, "Mother's name: %99[^\n]", temp_s1) == 1)
                            {
                                strcpy(mother, temp_s1);
                            }
                            else if (sscanf(blockLine, "Intake: %99s", temp_s1) == 1)
                            {
                                strcpy(intake, temp_s1);
                            }
                            else if (sscanf(blockLine, "Section: %99s", temp_s1) == 1)
                            {
                                strcpy(section, temp_s1);
                            }
                            else if (sscanf(blockLine, "Present Address: %199[^\n]", temp_s1) == 1)
                            {
                                strcpy(presentAddr, temp_s1);
                            }
                            else if (sscanf(blockLine, "Permanent Address: %199[^\n]", temp_s1) == 1)
                            {
                                strcpy(permanentAddr, temp_s1);
                            }
                            else if (sscanf(blockLine, "Blood Group: %19[^\n]", temp_s1) == 1)
                            {
                                strcpy(blood, temp_s1);
                            }
                            else if (sscanf(blockLine, "Mobile number: %19s", temp_s1) == 1)
                            {
                                strcpy(mobile, temp_s1);
                            }
                            else if (sscanf(blockLine, "Backup Mobile Number: %19s", temp_s1) == 1)
                            {
                                strcpy(backupMobile, temp_s1);
                            }
                            else if (sscanf(blockLine, "Email: %99[^\n]", temp_s1) == 1)
                            {
                                strcpy(email, temp_s1);
                            }
                            // Don't need to parse ID/Dept again

                            if (!nextNewline)
                                break; // End of buffer
                            linePtr = nextNewline + 1;
                            if (*linePtr == '\0')
                                break; // Check for empty string after newline
                        }
                        break; // Exit the outer while loop (fgets) as we found the record
                    }
                    // Reset for the next record block
                    readingRecord = 0;
                    recordBuffer[0] = '\0';
                    temp_id[0] = '\0';
                    temp_dept[0] = '\0';
                }
                continue; // Move to next line from file
            }

            // If not a blank line, it's part of a record
            readingRecord = 1;
            // Append line to buffer (check size)
            if (strlen(recordBuffer) + strlen(line) < sizeof(recordBuffer))
            {
                strcat(recordBuffer, line);
            }
            else
            {
                fprintf(stderr, "Warning: Record buffer overflow near potential ID %s\n", temp_id);
            }

            // Attempt to parse ID and Dept from the current line
            char temp_s1[100];
            if (sscanf(trimmedLine, "Student ID: %99s", temp_s1) == 1)
            {
                strcpy(temp_id, temp_s1); // Store the ID found *in this block*
            }
            if (sscanf(trimmedLine, "Department: %99s", temp_s1) == 1)
            {
                strcpy(temp_dept, temp_s1); // Store the Dept found *in this block*
            }
            // Check for match only after parsing the whole block (when blank line is hit)
        }

        // Handle case where the last record matches and file doesn't end with blank line
        if (readingRecord && !found)
        {
            if (strcmp(temp_id, searchID) == 0 && strcmp(temp_dept, departmentName) == 0)
            {
                found = 1;

                char *linePtr = recordBuffer;
                char blockLine[512];
                name1[0] = '\0';
                name2[0] = '\0';
                father[0] = '\0';
                mother[0] = '\0';
                strcpy(currentID, temp_id);
                strcpy(currentDept, temp_dept);
                intake[0] = '\0';
                section[0] = '\0';
                presentAddr[0] = '\0';
                permanentAddr[0] = '\0';
                blood[0] = '\0';
                mobile[0] = '\0';
                backupMobile[0] = '\0';
                email[0] = '\0';
                while (sscanf(linePtr, "%511[^\n]", blockLine) == 1)
                {
                    char *nextNewline = strchr(linePtr, '\n');
                    char temp_s1[200], temp_s2[100];
                    if (sscanf(blockLine, "Student Name: %99s %99s", temp_s1, temp_s2) == 2)
                    {
                        strcpy(name1, temp_s1);
                        strcpy(name2, temp_s2);
                    }
                    else if (sscanf(blockLine, "Father's name: %99[^\n]", temp_s1) == 1)
                    {
                        strcpy(father, temp_s1);
                    }
                    else if (sscanf(blockLine, "Mother's name: %99[^\n]", temp_s1) == 1)
                    {
                        strcpy(mother, temp_s1);
                    }
                    else if (sscanf(blockLine, "Intake: %99s", temp_s1) == 1)
                    {
                        strcpy(intake, temp_s1);
                    }
                    else if (sscanf(blockLine, "Section: %99s", temp_s1) == 1)
                    {
                        strcpy(section, temp_s1);
                    }
                    else if (sscanf(blockLine, "Present Address: %199[^\n]", temp_s1) == 1)
                    {
                        strcpy(presentAddr, temp_s1);
                    }
                    else if (sscanf(blockLine, "Permanent Address: %199[^\n]", temp_s1) == 1)
                    {
                        strcpy(permanentAddr, temp_s1);
                    }
                    else if (sscanf(blockLine, "Blood Group: %19[^\n]", temp_s1) == 1)
                    {
                        strcpy(blood, temp_s1);
                    }
                    else if (sscanf(blockLine, "Mobile number: %19s", temp_s1) == 1)
                    {
                        strcpy(mobile, temp_s1);
                    }
                    else if (sscanf(blockLine, "Backup Mobile Number: %19s", temp_s1) == 1)
                    {
                        strcpy(backupMobile, temp_s1);
                    }
                    else if (sscanf(blockLine, "Email: %99[^\n]", temp_s1) == 1)
                    {
                        strcpy(email, temp_s1);
                    }
                    if (!nextNewline)
                        break;
                    linePtr = nextNewline + 1;
                    if (*linePtr == '\0')
                        break;
                }
            }
        }

        if (found)
        {
            system("clear || cls");
            snprintf(fullName, sizeof(fullName), "%s %s", name1, name2); 
            printf("\n------------------- Student Details Found -------------------\n");
            printf("Name: %s\n", fullName);
            printf("Father's Name: %s\n", father);
            printf("Mother's Name: %s\n", mother);
            printf("Student ID: %s\n", currentID);
            printf("Department: %s\n", currentDept);
            printf("Intake: %s\n", intake);
            printf("Section: %s\n", section);
            printf("Present Address: %s\n", presentAddr);
            printf("Permanent Address: %s\n", permanentAddr);
            printf("Blood Group: %s\n", blood);
            printf("Mobile Number: %s\n", mobile);
            printf("Backup Mobile: %s\n", backupMobile);
            printf("Email: %s\n", email);
            printf("-------------------------------------------------------------\n");
        }
        else
        {
            system("clear || cls");
            printf("\nStudent with ID '%s' not found in the %s department.\n", searchID, departmentName);
        }

        clearerr(*P_ptr);
        printf("\nOptions:\n1. Search Another Student by ID\n2. Back to %s Menu\n", departmentName);
        printf("Enter your choice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            choice = 2;
            clearInputBuffer();
        }
        else
        {
            clearInputBuffer();
        }
        if (choice != 1)
            searchAgain = 0;
    }
}

void searchStudentByName(FILE **P_ptr, const char *departmentName)
{
    if (*P_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Student file is not open in searchStudentByName.\n");
        pressEnterToContinue();
        return;
    }

    int searchAgain = 1;
    while (searchAgain)
    {
        system("clear || cls");
        printf("\n--- Search Student by Name in %s ---\n", departmentName);
        printf("Enter Student Name (or part of it): ");
        char searchName[201];
        if (!fgets(searchName, sizeof(searchName), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(searchName);

        if (strlen(searchName) == 0)
        {
            printf("Search name cannot be empty.\n");
            pressEnterToContinue();
            continue;
        }

        rewind(*P_ptr);
        if (ferror(*P_ptr))
        {
            perror("Error rewinding student file for search");
            clearerr(*P_ptr);
            pressEnterToContinue();
            return;
        }

        int foundCount = 0; 
        char line[512];
        char name1[100], name2[100], father[100], mother[100];
        char currentID[100], currentDept[100], intake[100], section[100];
        char presentAddr[200], permanentAddr[200], blood[20];
        char mobile[20], backupMobile[20], email[100];
        char fullName[201];

        int readingRecord = 0;      
        char recordBuffer[2048] = ""; 

        printf("\n--- Search Results for '%s' in %s ---\n", searchName, departmentName);
        recordBuffer[0] = '\0';
        readingRecord = 0;

        while (fgets(line, sizeof(line), *P_ptr))
        {
            char trimmedLine[512];
            strcpy(trimmedLine, line);
            trimWhitespace(trimmedLine);

            if (strlen(trimmedLine) == 0)
            { 
                if (readingRecord)
                {
                    char *linePtr = recordBuffer;
                    char blockLine[512];
                    name1[0] = '\0';
                    name2[0] = '\0';
                    father[0] = '\0';
                    mother[0] = '\0';
                    currentID[0] = '\0';
                    currentDept[0] = '\0';
                    intake[0] = '\0';
                    section[0] = '\0';
                    presentAddr[0] = '\0';
                    permanentAddr[0] = '\0';
                    blood[0] = '\0';
                    mobile[0] = '\0';
                    backupMobile[0] = '\0';
                    email[0] = '\0';
                    fullName[0] = '\0';

                    while (sscanf(linePtr, "%511[^\n]", blockLine) == 1)
                    {
                        char *nextNewline = strchr(linePtr, '\n');
                        char temp_s1[200], temp_s2[100];
                        if (sscanf(blockLine, "Student Name: %99s %99s", temp_s1, temp_s2) == 2)
                        {
                            strcpy(name1, temp_s1);
                            strcpy(name2, temp_s2);
                            snprintf(fullName, sizeof(fullName), "%s %s", name1, name2);
                        }
                        else if (sscanf(blockLine, "Father's name: %99[^\n]", temp_s1) == 1)
                        {
                            strcpy(father, temp_s1);
                        }
                        else if (sscanf(blockLine, "Mother's name: %99[^\n]", temp_s1) == 1)
                        {
                            strcpy(mother, temp_s1);
                        }
                        else if (sscanf(blockLine, "Student ID: %99s", temp_s1) == 1)
                        {
                            strcpy(currentID, temp_s1);
                        }
                        else if (sscanf(blockLine, "Department: %99s", temp_s1) == 1)
                        {
                            strcpy(currentDept, temp_s1);
                        }
                        else if (sscanf(blockLine, "Intake: %99s", temp_s1) == 1)
                        {
                            strcpy(intake, temp_s1);
                        }
                        else if (sscanf(blockLine, "Section: %99s", temp_s1) == 1)
                        {
                            strcpy(section, temp_s1);
                        }
                        else if (sscanf(blockLine, "Present Address: %199[^\n]", temp_s1) == 1)
                        {
                            strcpy(presentAddr, temp_s1);
                        }
                        else if (sscanf(blockLine, "Permanent Address: %199[^\n]", temp_s1) == 1)
                        {
                            strcpy(permanentAddr, temp_s1);
                        }
                        else if (sscanf(blockLine, "Blood Group: %19[^\n]", temp_s1) == 1)
                        {
                            strcpy(blood, temp_s1);
                        }
                        else if (sscanf(blockLine, "Mobile number: %19s", temp_s1) == 1)
                        {
                            strcpy(mobile, temp_s1);
                        }
                        else if (sscanf(blockLine, "Backup Mobile Number: %19s", temp_s1) == 1)
                        {
                            strcpy(backupMobile, temp_s1);
                        }
                        else if (sscanf(blockLine, "Email: %99[^\n]", temp_s1) == 1)
                        {
                            strcpy(email, temp_s1);
                        }
                        if (!nextNewline)
                            break;
                        linePtr = nextNewline + 1;
                        if (*linePtr == '\0')
                            break;
                    }
                    if (strlen(fullName) > 0 && strstr(fullName, searchName) != NULL && strcmp(currentDept, departmentName) == 0)
                    {
                        if (foundCount == 0)
                        { 
                            printf("\n------------------- Matching Student(s) Found -------------------\n");
                        }
                        foundCount++;
                        printf("--- Match %d ---\n", foundCount);
                        printf("Name: %s\n", fullName);
                        printf("Father's Name: %s\n", father);
                        printf("Mother's Name: %s\n", mother);
                        printf("Student ID: %s\n", currentID);
                        printf("Department: %s\n", currentDept);
                        printf("Intake: %s\n", intake);
                        printf("Section: %s\n", section);
                        printf("Present Address: %s\n", presentAddr);
                        printf("Permanent Address: %s\n", permanentAddr);
                        printf("Blood Group: %s\n", blood);
                        printf("Mobile Number: %s\n", mobile);
                        printf("Backup Mobile: %s\n", backupMobile);
                        printf("Email: %s\n", email);
                        printf("-------------------------------------------------------------\n");
                    }
                    readingRecord = 0;
                    recordBuffer[0] = '\0';
                }
                continue;
            }
            readingRecord = 1;
            if (strlen(recordBuffer) + strlen(line) < sizeof(recordBuffer))
            {
                strcat(recordBuffer, line);
            }
            else
            {
                fprintf(stderr, "Warning: Record buffer overflow during name search\n");
            }
        }
        if (readingRecord)
        {
            char *linePtr = recordBuffer;
            char blockLine[512];
            name1[0] = '\0';
            name2[0] = '\0';
            father[0] = '\0';
            mother[0] = '\0';
            currentID[0] = '\0';
            currentDept[0] = '\0';
            intake[0] = '\0';
            section[0] = '\0';
            presentAddr[0] = '\0';
            permanentAddr[0] = '\0';
            blood[0] = '\0';
            mobile[0] = '\0';
            backupMobile[0] = '\0';
            email[0] = '\0';
            fullName[0] = '\0';
            while (sscanf(linePtr, "%511[^\n]", blockLine) == 1)
            {
                char *nextNewline = strchr(linePtr, '\n');
                char temp_s1[200], temp_s2[100];
                if (sscanf(blockLine, "Student Name: %99s %99s", temp_s1, temp_s2) == 2)
                {
                    strcpy(name1, temp_s1);
                    strcpy(name2, temp_s2);
                    snprintf(fullName, sizeof(fullName), "%s %s", name1, name2);
                }
                else if (sscanf(blockLine, "Father's name: %99[^\n]", temp_s1) == 1)
                {
                    strcpy(father, temp_s1);
                }
                else if (sscanf(blockLine, "Mother's name: %99[^\n]", temp_s1) == 1)
                {
                    strcpy(mother, temp_s1);
                }
                else if (sscanf(blockLine, "Student ID: %99s", temp_s1) == 1)
                {
                    strcpy(currentID, temp_s1);
                }
                else if (sscanf(blockLine, "Department: %99s", temp_s1) == 1)
                {
                    strcpy(currentDept, temp_s1);
                }
                else if (sscanf(blockLine, "Intake: %99s", temp_s1) == 1)
                {
                    strcpy(intake, temp_s1);
                }
                else if (sscanf(blockLine, "Section: %99s", temp_s1) == 1)
                {
                    strcpy(section, temp_s1);
                }
                else if (sscanf(blockLine, "Present Address: %199[^\n]", temp_s1) == 1)
                {
                    strcpy(presentAddr, temp_s1);
                }
                else if (sscanf(blockLine, "Permanent Address: %199[^\n]", temp_s1) == 1)
                {
                    strcpy(permanentAddr, temp_s1);
                }
                else if (sscanf(blockLine, "Blood Group: %19[^\n]", temp_s1) == 1)
                {
                    strcpy(blood, temp_s1);
                }
                else if (sscanf(blockLine, "Mobile number: %19s", temp_s1) == 1)
                {
                    strcpy(mobile, temp_s1);
                }
                else if (sscanf(blockLine, "Backup Mobile Number: %19s", temp_s1) == 1)
                {
                    strcpy(backupMobile, temp_s1);
                }
                else if (sscanf(blockLine, "Email: %99[^\n]", temp_s1) == 1)
                {
                    strcpy(email, temp_s1);
                }
                if (!nextNewline)
                    break;
                linePtr = nextNewline + 1;
                if (*linePtr == '\0')
                    break;
            }
            if (strlen(fullName) > 0 && strstr(fullName, searchName) != NULL && strcmp(currentDept, departmentName) == 0)
            {
                if (foundCount == 0)
                {
                    printf("\n------------------- Matching Student(s) Found -------------------\n");
                }
                foundCount++;
                printf("--- Match %d ---\n", foundCount);
                printf("Name: %s\n", fullName);
                printf("Father's Name: %s\n", father);
                printf("Mother's Name: %s\n", mother);
                printf("Student ID: %s\n", currentID);
                printf("Department: %s\n", currentDept);
                printf("Intake: %s\n", intake);
                printf("Section: %s\n", section);
                printf("Present Address: %s\n", presentAddr);
                printf("Permanent Address: %s\n", permanentAddr);
                printf("Blood Group: %s\n", blood);
                printf("Mobile Number: %s\n", mobile);
                printf("Backup Mobile: %s\n", backupMobile);
                printf("Email: %s\n", email);
                printf("-------------------------------------------------------------\n");
            }
        }
        if (foundCount == 0)
        {
            printf("\nNo students found matching the name '%s' in the %s department.\n", searchName, departmentName);
        }
        clearerr(*P_ptr);
        printf("\nOptions:\n1. Search Again by Name\n2. Back to %s Menu\n", departmentName);
        printf("Enter your choice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            choice = 2;
            clearInputBuffer();
        }
        else
        {
            clearInputBuffer();
        }
        if (choice != 1)
            searchAgain = 0;
    }
}

void viewStudentsByIntakeSection(FILE **P_ptr, const char *departmentName)
{
    if (*P_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Student file is not open in viewStudentsByIntakeSection.\n");
        pressEnterToContinue();
        return;
    }

    int viewAgain = 1;
    while (viewAgain)
    {
        system("clear || cls");
        printf("\n--- View Students by Intake & Section in %s ---\n", departmentName);
        char searchIntake[100], searchSection[100];

        printf("Enter Intake to filter by: ");
        if (!fgets(searchIntake, sizeof(searchIntake), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(searchIntake);

        printf("Enter Section to filter by: ");
        if (!fgets(searchSection, sizeof(searchSection), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(searchSection);

        if (strlen(searchIntake) == 0 || strlen(searchSection) == 0)
        {
            printf("Intake and Section cannot be empty.\n");
            pressEnterToContinue();
            continue;
        }

        rewind(*P_ptr);
        if (ferror(*P_ptr))
        {
            perror("Error rewinding student file");
            clearerr(*P_ptr);
            pressEnterToContinue();
            return;
        }

        printf("\n--- Student List for %s - Intake: %s, Section: %s ---\n\n", departmentName, searchIntake, searchSection);
        printf("%-25s %-15s %-15s\n", "Student Name", "Student ID", "Mobile Number");
        printf("----------------------------------------------------------\n");

        int found = 0;
        char line[512];
        char name1[100], name2[100], studentID[100], mobileNumber[100];
        char currentDept[100], currentIntake[100], currentSection[100];
        char fullName[201];
        int readingRecord = 0;
        fullName[0] = '\0';
        name1[0] = '\0';
        name2[0] = '\0';
        studentID[0] = '\0';
        mobileNumber[0] = '\0';
        currentDept[0] = '\0';
        currentIntake[0] = '\0';
        currentSection[0] = '\0';

        while (fgets(line, sizeof(line), *P_ptr))
        {
            char trimmedLine[512];
            strcpy(trimmedLine, line);
            trimWhitespace(trimmedLine);

            if (strlen(trimmedLine) == 0)
            { 
                if (readingRecord)
                {
                    if (strcmp(currentDept, departmentName) == 0 &&
                        strcmp(currentIntake, searchIntake) == 0 &&
                        strcmp(currentSection, searchSection) == 0)
                    {
                        found = 1;
                        snprintf(fullName, sizeof(fullName), "%s %s", name1, name2);
                        printf("%-25s %-15s %-15s\n", fullName, studentID, mobileNumber);
                    }
                    readingRecord = 0;
                    fullName[0] = '\0';
                    name1[0] = '\0';
                    name2[0] = '\0';
                    studentID[0] = '\0';
                    mobileNumber[0] = '\0';
                    currentDept[0] = '\0';
                    currentIntake[0] = '\0';
                    currentSection[0] = '\0';
                }
                continue;
            }
            readingRecord = 1;
            char temp_s1[100], temp_s2[100];
            if (sscanf(trimmedLine, "Student Name: %99s %99s", temp_s1, temp_s2) == 2)
            {
                strcpy(name1, temp_s1);
                strcpy(name2, temp_s2);
            }
            else if (sscanf(trimmedLine, "Student ID: %99s", temp_s1) == 1)
            {
                strcpy(studentID, temp_s1);
            }
            else if (sscanf(trimmedLine, "Department: %99s", temp_s1) == 1)
            {
                strcpy(currentDept, temp_s1);
            }
            else if (sscanf(trimmedLine, "Mobile number: %99s", temp_s1) == 1)
            {
                strcpy(mobileNumber, temp_s1);
            }
            else if (sscanf(trimmedLine, "Intake: %99s", temp_s1) == 1)
            {
                strcpy(currentIntake, temp_s1);
            }
            else if (sscanf(trimmedLine, "Section: %99s", temp_s1) == 1)
            {
                strcpy(currentSection, temp_s1);
            }
        }
        if (readingRecord)
        {
            if (strcmp(currentDept, departmentName) == 0 &&
                strcmp(currentIntake, searchIntake) == 0 &&
                strcmp(currentSection, searchSection) == 0)
            {
                found = 1;
                snprintf(fullName, sizeof(fullName), "%s %s", name1, name2);
                printf("%-25s %-15s %-15s\n", fullName, studentID, mobileNumber);
            }
        }

        printf("----------------------------------------------------------\n");
        if (!found)
        {
            printf("\nNo students found matching criteria (Dept: %s, Intake: %s, Section: %s).\n", departmentName, searchIntake, searchSection);
        }

        clearerr(*P_ptr);
        printf("\nOptions:\n1. View Another Intake/Section\n2. Back to %s Menu\n", departmentName);
        printf("Enter your choice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            choice = 2;
            clearInputBuffer();
        }
        else
        {
            clearInputBuffer();
        }
        if (choice != 1)
            viewAgain = 0;
    }
}

void deleteStudentById(FILE **P_ptr, const char *departmentName)
{
    int deleteMore = 1;
    while (deleteMore)
    {
        system("clear || cls");
        printf("\n--- Delete Student by ID from %s ---\n", departmentName);
        printf("Enter Student ID to delete: ");
        char deleteID[100];
        if (!fgets(deleteID, sizeof(deleteID), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(deleteID);

        if (strlen(deleteID) == 0)
        {
            printf("Delete ID cannot be empty.\n");
            pressEnterToContinue();
            continue;
        }

        const char *originalFilename = "test.txt";
        const char *tempFilename = "temp_student.tmp";

        FILE *tempFile = fopen(tempFilename, "w");
        if (tempFile == NULL)
        {
            perror("Error creating temporary file");
            pressEnterToContinue();
            return; 
        }
        if (*P_ptr != NULL)
        {
            fclose(*P_ptr);
            *P_ptr = NULL; 
        }
        else
        {
            fprintf(stderr, "Warning: Student file pointer was already NULL before delete operation.\n");
        }

        FILE *P_read = fopen(originalFilename, "r");
        if (P_read == NULL)
        {
            perror("Error opening student file for reading (delete)");
            fclose(tempFile);     
            remove(tempFilename); 
            *P_ptr = fopen(originalFilename, "a+");
            if (*P_ptr == NULL)
            {
                perror("FATAL: Could not reopen student file after delete read error");
                exit(EXIT_FAILURE);
            }
            pressEnterToContinue();
            return; 
        }
        int found = 0;
        char line[512];
        char currentID[100];
        char currentDept[100];
        int skipRecord = 0;         
        char recordBuffer[2048] = ""; 
        recordBuffer[0] = '\0';
        skipRecord = 0;
        currentID[0] = '\0';   
        currentDept[0] = '\0'; 

        while (fgets(line, sizeof(line), P_read))
        {
            char trimmedLine[512];
            strcpy(trimmedLine, line);
            trimWhitespace(trimmedLine);

            if (strlen(trimmedLine) == 0)
            { 
                if (strlen(recordBuffer) > 0 && !skipRecord)
                {
                    if (fputs(recordBuffer, tempFile) == EOF)
                    {
                        perror("Error writing record to temp file");
                    }
                }
                recordBuffer[0] = '\0';
                skipRecord = 0;
                currentID[0] = '\0';
                currentDept[0] = '\0';
                continue; 
            }
            if (strlen(recordBuffer) + strlen(line) < sizeof(recordBuffer))
            {
                strcat(recordBuffer, line);
            }
            else
            {
                fprintf(stderr, "Warning: Record buffer overflow near ID %s during delete.\n", currentID);
            }
            char temp_s1[100];
            if (!skipRecord)
            { 
                if (sscanf(trimmedLine, "Student ID: %99s", temp_s1) == 1)
                {
                    strcpy(currentID, temp_s1);
                }
                if (sscanf(trimmedLine, "Department: %99s", temp_s1) == 1)
                {
                    strcpy(currentDept, temp_s1);
                }
                if (strlen(currentID) > 0 && strlen(currentDept) > 0)
                {
                    if (strcmp(currentID, deleteID) == 0 && strcmp(currentDept, departmentName) == 0)
                    {
                        found = 1;
                        skipRecord = 1; 
                    }
                }
            }
        }
        if (strlen(recordBuffer) > 0 && !skipRecord)
        {
            if (fputs(recordBuffer, tempFile) == EOF)
            {
                perror("Error writing last record to temp file");
            }
        }
        fclose(P_read);
        fclose(tempFile);
        if (found)
        {
            if (remove(originalFilename) != 0)
            {
                perror("Error deleting original student file");
                fprintf(stderr, "Error: Original file could not be deleted. Temp data in '%s'\n", tempFilename);
            }
            else if (rename(tempFilename, originalFilename) != 0)
            {
                perror("Error renaming temp file to original");
                fprintf(stderr, "CRITICAL Error: Original deleted, temp rename failed. Data potentially lost. Temp file is '%s'\n", tempFilename);

            }
            else
            {
                printf("\nStudent deleted successfully.\n");
            }
        }
        else
        {
            printf("\nStudent with ID '%s' not found in %s department. No changes made.\n", deleteID, departmentName);
            remove(tempFilename); 
        }
        *P_ptr = fopen(originalFilename, "a+");
        if (*P_ptr == NULL)
        {
            perror("FATAL: Could not reopen student file after delete operation");
            exit(EXIT_FAILURE);
        }
        printf("\nOptions:\n1. Delete Another Student\n2. Back to %s Menu\n", departmentName);
        printf("Enter your choice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            choice = 2;
            clearInputBuffer();
        }
        else
        {
            clearInputBuffer();
        }
        if (choice != 1)
            deleteMore = 0;
    }
}

void updateStudentById(FILE **P_ptr, const char *departmentName)
{
    int updateMore = 1;
    while (updateMore)
    {
        system("clear || cls");
        printf("\n--- Update Student Information in %s ---\n", departmentName);
        printf("Enter Student ID to update: ");
        char updateID[100];
        if (!fgets(updateID, sizeof(updateID), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(updateID);

        if (strlen(updateID) == 0)
        {
            printf("Update ID cannot be empty.\n");
            pressEnterToContinue();
            continue;
        }

        const char *originalFilename = "test.txt";
        const char *tempFilename = "temp_student.tmp";

        FILE *tempFile = fopen(tempFilename, "w");
        if (tempFile == NULL)
        {
            perror("Error creating temporary file for update");
            pressEnterToContinue();
            return; 
        }
        if (*P_ptr != NULL)
        {
            fclose(*P_ptr);
            *P_ptr = NULL;
        }
        FILE *P_read = fopen(originalFilename, "r");
        if (P_read == NULL)
        {
            perror("Error opening student file for reading (update)");
            fclose(tempFile);
            remove(tempFilename);
            *P_ptr = fopen(originalFilename, "a+"); 
            if (*P_ptr == NULL)
                exit(EXIT_FAILURE);
            pressEnterToContinue();
            return;
        }

        int found = 0;
        char line[512];
        char recordBuffer[2048] = "";
        int recordMatches = 0; 
        char name1[100], name2[100], father[100], mother[100];
        char currentID[100], currentDept[100], intake[100], section[100];
        char presentAddr[200], permanentAddr[200], blood[20];
        char mobile[20], backupMobile[20], email[100];
        recordBuffer[0] = '\0';
        recordMatches = 0;
        currentID[0] = '\0';
        currentDept[0] = '\0'; 
        while (fgets(line, sizeof(line), P_read))
        {
            char trimmedLine[512];
            strcpy(trimmedLine, line);
            trimWhitespace(trimmedLine);

            if (strlen(trimmedLine) == 0)
            { 
                if (strlen(recordBuffer) > 0)
                { 
                    if (recordMatches)
                    {
                        found = 1;
                        char *linePtr = recordBuffer;
                        char blockLine[512];
                        name1[0] = '\0';
                        name2[0] = '\0';
                        father[0] = '\0';
                        mother[0] = '\0';
                        intake[0] = '\0';
                        section[0] = '\0';
                        presentAddr[0] = '\0';
                        permanentAddr[0] = '\0';
                        blood[0] = '\0';
                        mobile[0] = '\0';
                        backupMobile[0] = '\0';
                        email[0] = '\0';
                        while (sscanf(linePtr, "%511[^\n]", blockLine) == 1)
                        {
                            char *nextNewline = strchr(linePtr, '\n');
                            char temp_s1[200], temp_s2[100];
                            if (sscanf(blockLine, "Student Name: %99s %99s", temp_s1, temp_s2) == 2)
                            {
                                strcpy(name1, temp_s1);
                                strcpy(name2, temp_s2);
                            }
                            else if (sscanf(blockLine, "Father's name: %99[^\n]", temp_s1) == 1)
                            {
                                strcpy(father, temp_s1);
                            }
                            else if (sscanf(blockLine, "Mother's name: %99[^\n]", temp_s1) == 1)
                            {
                                strcpy(mother, temp_s1);
                            }
                            else if (sscanf(blockLine, "Intake: %99s", temp_s1) == 1)
                            {
                                strcpy(intake, temp_s1);
                            }
                            else if (sscanf(blockLine, "Section: %99s", temp_s1) == 1)
                            {
                                strcpy(section, temp_s1);
                            }
                            else if (sscanf(blockLine, "Present Address: %199[^\n]", temp_s1) == 1)
                            {
                                strcpy(presentAddr, temp_s1);
                            }
                            else if (sscanf(blockLine, "Permanent Address: %199[^\n]", temp_s1) == 1)
                            {
                                strcpy(permanentAddr, temp_s1);
                            }
                            else if (sscanf(blockLine, "Blood Group: %19[^\n]", temp_s1) == 1)
                            {
                                strcpy(blood, temp_s1);
                            }
                            else if (sscanf(blockLine, "Mobile number: %19s", temp_s1) == 1)
                            {
                                strcpy(mobile, temp_s1);
                            }
                            else if (sscanf(blockLine, "Backup Mobile Number: %19s", temp_s1) == 1)
                            {
                                strcpy(backupMobile, temp_s1);
                            }
                            else if (sscanf(blockLine, "Email: %99[^\n]", temp_s1) == 1)
                            {
                                strcpy(email, temp_s1);
                            }
                            if (!nextNewline)
                                break;
                            linePtr = nextNewline + 1;
                            if (*linePtr == '\0')
                                break;
                        }
                        system("clear || cls"); 
                        printf("\n--- Updating Student ID: %s (Dept: %s) ---\n", currentID, currentDept);
                        printf("--- Enter new info or press Enter to keep current ---\n\n");
                        char newName1[100], newName2[100], newFather[100], newMother[100];
                        char newIntake[100], newSection[100], newPresentAddr[200], newPermanentAddr[200];
                        char newBlood[20], newMobile[20], newBackupMobile[20], newEmail[100];
                        printf("First Name (%s): ", name1);
                        if (!fgets(newName1, sizeof(newName1), stdin))
                            break;
                        trimWhitespace(newName1);
                        printf("Last Name (%s): ", name2);
                        if (!fgets(newName2, sizeof(newName2), stdin))
                            break;
                        trimWhitespace(newName2);
                        printf("Father's Name (%s): ", father);
                        if (!fgets(newFather, sizeof(newFather), stdin))
                            break;
                        trimWhitespace(newFather);
                        printf("Mother's Name (%s): ", mother);
                        if (!fgets(newMother, sizeof(newMother), stdin))
                            break;
                        trimWhitespace(newMother);
                        printf("Intake (%s): ", intake);
                        if (!fgets(newIntake, sizeof(newIntake), stdin))
                            break;
                        trimWhitespace(newIntake);
                        printf("Section (%s): ", section);
                        if (!fgets(newSection, sizeof(newSection), stdin))
                            break;
                        trimWhitespace(newSection);
                        printf("Present Address (%s): ", presentAddr);
                        if (!fgets(newPresentAddr, sizeof(newPresentAddr), stdin))
                            break;
                        trimWhitespace(newPresentAddr);
                        printf("Permanent Address (%s): ", permanentAddr);
                        if (!fgets(newPermanentAddr, sizeof(newPermanentAddr), stdin))
                            break;
                        trimWhitespace(newPermanentAddr);
                        printf("Blood Group (%s): ", blood);
                        if (!fgets(newBlood, sizeof(newBlood), stdin))
                            break;
                        trimWhitespace(newBlood);
                        printf("Mobile (%s): ", mobile);
                        if (!fgets(newMobile, sizeof(newMobile), stdin))
                            break;
                        trimWhitespace(newMobile);
                        printf("Backup Mobile (%s): ", backupMobile);
                        if (!fgets(newBackupMobile, sizeof(newBackupMobile), stdin))
                            break;
                        trimWhitespace(newBackupMobile);
                        printf("Email (%s): ", email);
                        if (!fgets(newEmail, sizeof(newEmail), stdin))
                            break;
                        trimWhitespace(newEmail);
                        fprintf(tempFile, "Student Name: %s %s\n", (strlen(newName1) > 0) ? newName1 : name1, (strlen(newName2) > 0) ? newName2 : name2);
                        fprintf(tempFile, "Father's name: %s\n", (strlen(newFather) > 0) ? newFather : father);
                        fprintf(tempFile, "Mother's name: %s\n", (strlen(newMother) > 0) ? newMother : mother);
                        fprintf(tempFile, "Student ID: %s\n", currentID);  
                        fprintf(tempFile, "Department: %s\n", currentDept); 
                        fprintf(tempFile, "Intake: %s\n", (strlen(newIntake) > 0) ? newIntake : intake);
                        fprintf(tempFile, "Section: %s\n", (strlen(newSection) > 0) ? newSection : section);
                        fprintf(tempFile, "Present Address: %s\n", (strlen(newPresentAddr) > 0) ? newPresentAddr : presentAddr);
                        fprintf(tempFile, "Permanent Address: %s\n", (strlen(newPermanentAddr) > 0) ? newPermanentAddr : permanentAddr);
                        fprintf(tempFile, "Blood Group: %s\n", (strlen(newBlood) > 0) ? newBlood : blood);
                        fprintf(tempFile, "Mobile number: %s\n", (strlen(newMobile) > 0) ? newMobile : mobile);
                        fprintf(tempFile, "Backup Mobile Number: %s\n", (strlen(newBackupMobile) > 0) ? newBackupMobile : backupMobile);
                        fprintf(tempFile, "Email: %s\n\n", (strlen(newEmail) > 0) ? newEmail : email); 
                    }
                    else
                    {
                        if (fputs(recordBuffer, tempFile) == EOF)
                        {
                            perror("Error writing unchanged record to temp file");
                        }
                    }
                }
                recordBuffer[0] = '\0';
                recordMatches = 0;
                currentID[0] = '\0';
                currentDept[0] = '\0';
                continue;
            }
            if (strlen(recordBuffer) + strlen(line) < sizeof(recordBuffer))
            {
                strcat(recordBuffer, line);
            }
            else
            {
                fprintf(stderr, "Warning: Record buffer overflow near ID %s during update.\n", currentID);
            }
            char temp_s1[100];
            if (!recordMatches)
            { 
                if (sscanf(trimmedLine, "Student ID: %99s", temp_s1) == 1)
                {
                    strcpy(currentID, temp_s1);
                }
                if (sscanf(trimmedLine, "Department: %99s", temp_s1) == 1)
                {
                    strcpy(currentDept, temp_s1);
                }
                if (strlen(currentID) > 0 && strlen(currentDept) > 0)
                {
                    if (strcmp(currentID, updateID) == 0 && strcmp(currentDept, departmentName) == 0)
                    {
                        recordMatches = 1; 
                    }
                }
            }
        }
        if (strlen(recordBuffer) > 0)
        {
            if (recordMatches)
            {
                found = 1;
                char *linePtr = recordBuffer;
                char blockLine[512];
                name1[0] = '\0';
                name2[0] = '\0';
                father[0] = '\0';
                mother[0] = '\0';
                intake[0] = '\0';
                section[0] = '\0';
                presentAddr[0] = '\0';
                permanentAddr[0] = '\0';
                blood[0] = '\0';
                mobile[0] = '\0';
                backupMobile[0] = '\0';
                email[0] = '\0';
                while (sscanf(linePtr, "%511[^\n]", blockLine) == 1)
                {
                    char *nextNewline = strchr(linePtr, '\n');
                    char temp_s1[200], temp_s2[100];
                    if (sscanf(blockLine, "Student Name: %99s %99s", temp_s1, temp_s2) == 2)
                    {
                        strcpy(name1, temp_s1);
                        strcpy(name2, temp_s2);
                    }
                    else if (sscanf(blockLine, "Father's name: %99[^\n]", temp_s1) == 1)
                    {
                        strcpy(father, temp_s1);
                    }
                    else if (sscanf(blockLine, "Mother's name: %99[^\n]", temp_s1) == 1)
                    {
                        strcpy(mother, temp_s1);
                    }
                    else if (sscanf(blockLine, "Intake: %99s", temp_s1) == 1)
                    {
                        strcpy(intake, temp_s1);
                    }
                    else if (sscanf(blockLine, "Section: %99s", temp_s1) == 1)
                    {
                        strcpy(section, temp_s1);
                    }
                    else if (sscanf(blockLine, "Present Address: %199[^\n]", temp_s1) == 1)
                    {
                        strcpy(presentAddr, temp_s1);
                    }
                    else if (sscanf(blockLine, "Permanent Address: %199[^\n]", temp_s1) == 1)
                    {
                        strcpy(permanentAddr, temp_s1);
                    }
                    else if (sscanf(blockLine, "Blood Group: %19[^\n]", temp_s1) == 1)
                    {
                        strcpy(blood, temp_s1);
                    }
                    else if (sscanf(blockLine, "Mobile number: %19s", temp_s1) == 1)
                    {
                        strcpy(mobile, temp_s1);
                    }
                    else if (sscanf(blockLine, "Backup Mobile Number: %19s", temp_s1) == 1)
                    {
                        strcpy(backupMobile, temp_s1);
                    }
                    else if (sscanf(blockLine, "Email: %99[^\n]", temp_s1) == 1)
                    {
                        strcpy(email, temp_s1);
                    }
                    if (!nextNewline)
                        break;
                    linePtr = nextNewline + 1;
                    if (*linePtr == '\0')
                        break;
                }
                system("clear || cls");
                printf("\n--- Updating Student ID: %s (Dept: %s) ---\n", currentID, currentDept);
                printf("--- Enter new info or press Enter to keep current ---\n\n");
                char newName1[100], newName2[100], newFather[100], newMother[100], newIntake[100], newSection[100], newPresentAddr[200], newPermanentAddr[200], newBlood[20], newMobile[20], newBackupMobile[20], newEmail[100];
                printf("First Name (%s): ", name1);
                if (!fgets(newName1, sizeof(newName1), stdin))
                    break;
                trimWhitespace(newName1);
                printf("Last Name (%s): ", name2);
                if (!fgets(newName2, sizeof(newName2), stdin))
                    break;
                trimWhitespace(newName2);
                printf("Father's Name (%s): ", father);
                if (!fgets(newFather, sizeof(newFather), stdin))
                    break;
                trimWhitespace(newFather);
                printf("Mother's Name (%s): ", mother);
                if (!fgets(newMother, sizeof(newMother), stdin))
                    break;
                trimWhitespace(newMother);
                printf("Intake (%s): ", intake);
                if (!fgets(newIntake, sizeof(newIntake), stdin))
                    break;
                trimWhitespace(newIntake);
                printf("Section (%s): ", section);
                if (!fgets(newSection, sizeof(newSection), stdin))
                    break;
                trimWhitespace(newSection);
                printf("Present Address (%s): ", presentAddr);
                if (!fgets(newPresentAddr, sizeof(newPresentAddr), stdin))
                    break;
                trimWhitespace(newPresentAddr);
                printf("Permanent Address (%s): ", permanentAddr);
                if (!fgets(newPermanentAddr, sizeof(newPermanentAddr), stdin))
                    break;
                trimWhitespace(newPermanentAddr);
                printf("Blood Group (%s): ", blood);
                if (!fgets(newBlood, sizeof(newBlood), stdin))
                    break;
                trimWhitespace(newBlood);
                printf("Mobile (%s): ", mobile);
                if (!fgets(newMobile, sizeof(newMobile), stdin))
                    break;
                trimWhitespace(newMobile);
                printf("Backup Mobile (%s): ", backupMobile);
                if (!fgets(newBackupMobile, sizeof(newBackupMobile), stdin))
                    break;
                trimWhitespace(newBackupMobile);
                printf("Email (%s): ", email);
                if (!fgets(newEmail, sizeof(newEmail), stdin))
                    break;
                trimWhitespace(newEmail);
                fprintf(tempFile, "Student Name: %s %s\n", (strlen(newName1) > 0) ? newName1 : name1, (strlen(newName2) > 0) ? newName2 : name2);
                fprintf(tempFile, "Father's name: %s\n", (strlen(newFather) > 0) ? newFather : father);
                fprintf(tempFile, "Mother's name: %s\n", (strlen(newMother) > 0) ? newMother : mother);
                fprintf(tempFile, "Student ID: %s\n", currentID);
                fprintf(tempFile, "Department: %s\n", currentDept);
                fprintf(tempFile, "Intake: %s\n", (strlen(newIntake) > 0) ? newIntake : intake);
                fprintf(tempFile, "Section: %s\n", (strlen(newSection) > 0) ? newSection : section);
                fprintf(tempFile, "Present Address: %s\n", (strlen(newPresentAddr) > 0) ? newPresentAddr : presentAddr);
                fprintf(tempFile, "Permanent Address: %s\n", (strlen(newPermanentAddr) > 0) ? newPermanentAddr : permanentAddr);
                fprintf(tempFile, "Blood Group: %s\n", (strlen(newBlood) > 0) ? newBlood : blood);
                fprintf(tempFile, "Mobile number: %s\n", (strlen(newMobile) > 0) ? newMobile : mobile);
                fprintf(tempFile, "Backup Mobile Number: %s\n", (strlen(newBackupMobile) > 0) ? newBackupMobile : backupMobile);
                fprintf(tempFile, "Email: %s\n\n", (strlen(newEmail) > 0) ? newEmail : email);
            }
            else
            {
                if (fputs(recordBuffer, tempFile) == EOF)
                {
                    perror("Error writing last unchanged record to temp file");
                }
            }
        }

        fclose(P_read);
        fclose(tempFile);
        if (found)
        {
            if (remove(originalFilename) != 0)
            {
                perror("Error deleting original file during update");
                fprintf(stderr, "Temp data in '%s'\n", tempFilename);
            }
            else if (rename(tempFilename, originalFilename) != 0)
            {
                perror("Error renaming temp file during update");
                fprintf(stderr, "CRITICAL Error: Original deleted, rename failed. Data lost. Temp file '%s'\n", tempFilename);
            }
            else
            {
                printf("\nStudent updated successfully!\n");
            }
        }
        else
        {
            printf("\nStudent with ID '%s' not found in %s department. No changes made.\n", updateID, departmentName);
            remove(tempFilename); 
        }
        *P_ptr = fopen(originalFilename, "a+");
        if (*P_ptr == NULL)
        {
            perror("FATAL: Could not reopen student file after update");
            exit(EXIT_FAILURE);
        }
        printf("\nOptions:\n1. Update Another Student\n2. Back to %s Menu\n", departmentName);
        printf("Enter your choice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            choice = 2;
            clearInputBuffer();
        }
        else
        {
            clearInputBuffer();
        }
        if (choice != 1)
            updateMore = 0;
    }
}
int isRoutineType(const char *scheduleType)
{
    return (strcmp(scheduleType, "Class Routine") == 0 ||
            strcmp(scheduleType, "Mid Exam Routine") == 0 ||
            strcmp(scheduleType, "Final Exam Routine") == 0);
}

void manageAcademicSchedules(FILE **scheduleFile_ptr)
{
    int scheduleRunning = 1;
    int select;
    while (scheduleRunning)
    {
        system("clear || cls");
        printf("\n\n--- Academic Schedule Management ---\n");
        printf("1. Add New Schedule Entry\n");
        printf("2. View All Schedules for Intake & Section\n");
        printf("3. Update Specific Schedule Entry\n");
        printf("4. Delete Specific Schedule Entry\n");
        printf("5. Back to Main Menu\n");
        printf("\nEnter Choice: ");

        if (scanf("%d", &select) != 1)
        {
            printf("Invalid input. Please enter a number (1-5).\n");
            clearInputBuffer();
            pressEnterToContinue();
            continue;
        }
        clearInputBuffer(); 
        if (*scheduleFile_ptr == NULL)
        {
            fprintf(stderr, "ERROR: Schedule file pointer is NULL in manageAcademicSchedules.\n");
            *scheduleFile_ptr = fopen("schedules.dat", "rb+");
            if (*scheduleFile_ptr == NULL)
                *scheduleFile_ptr = fopen("schedules.dat", "wb+");
            if (*scheduleFile_ptr == NULL)
            {
                perror("FATAL: Schedule file is inaccessible");
                exit(EXIT_FAILURE);
            }
            printf("INFO: Schedule file pointer was reopened.\n");
        }

        switch (select)
        {
        case 1:
            addSchedule(scheduleFile_ptr);
            break;
        case 2:
            viewSchedule(scheduleFile_ptr);
            break;
        case 3:
            updateSchedule(scheduleFile_ptr);
            break;
        case 4:
            deleteSchedule(scheduleFile_ptr);
            break;
        case 5:
            scheduleRunning = 0;
            break;
        default:
            printf("Invalid choice (%d). Please enter 1-5.\n", select);
            break;
        }
        if (scheduleRunning && select >= 1 && select <= 4)
        {
            pressEnterToContinue();
        }
    }
}

void addSchedule(FILE **scheduleFile_ptr)
{
    if (*scheduleFile_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Schedule file is not open in addSchedule.\n");
        pressEnterToContinue();
        return;
    }
    struct AcademicSchedule schedule;
    int addMore = 1;
    while (addMore)
    {
        system("clear || cls");
        printf("\n--- Add New Academic Schedule Entry ---\n");
        memset(&schedule, 0, sizeof(struct AcademicSchedule));

        printf("Enter Intake: ");
        if (!fgets(schedule.intake, sizeof(schedule.intake), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(schedule.intake);

        printf("Enter Section: ");
        if (!fgets(schedule.section, sizeof(schedule.section), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(schedule.section);
        printf("Select Schedule Type:\n");
        printf("  1. Class Routine\n");
        printf("  2. Mid Exam Routine\n");
        printf("  3. Final Exam Routine\n");
        printf("  4. Result Publication Date\n");
        printf("  5. Other Event\n");
        printf("Enter choice (1-5): ");
        int typeChoice;
        if (scanf("%d", &typeChoice) != 1 || typeChoice < 1 || typeChoice > 5)
        {
            printf("Invalid type choice. Defaulting to 'Other Event'.\n");
            strcpy(schedule.scheduleType, "Other Event");
            clearInputBuffer();
        }
        else
        {
            clearInputBuffer(); 
            switch (typeChoice)
            {
            case 1:
                strcpy(schedule.scheduleType, "Class Routine");
                break;
            case 2:
                strcpy(schedule.scheduleType, "Mid Exam Routine");
                break;
            case 3:
                strcpy(schedule.scheduleType, "Final Exam Routine");
                break;
            case 4:
                strcpy(schedule.scheduleType, "Result Publication Date");
                break;
            case 5:
                strcpy(schedule.scheduleType, "Other Event");
                break;
            }
        }
        if (isRoutineType(schedule.scheduleType))
        {
            printf("\n--- Enter %s Details ---\n", schedule.scheduleType);
            printf("Day(s) (e.g., Sunday or Mon-Wed): ");
            if (!fgets(schedule.routineData.day, sizeof(schedule.routineData.day), stdin))
            {
                printf("Input error!\n");
                continue;
            }
            trimWhitespace(schedule.routineData.day);

            printf("Date(s) (e.g., 25/12/2023 or TBA): ");
            if (!fgets(schedule.routineData.date, sizeof(schedule.routineData.date), stdin))
            {
                printf("Input error!\n");
                continue;
            }
            trimWhitespace(schedule.routineData.date);

            printf("Time (e.g., 10:00 AM - 11:30 AM): ");
            if (!fgets(schedule.routineData.time, sizeof(schedule.routineData.time), stdin))
            {
                printf("Input error!\n");
                continue;
            }
            trimWhitespace(schedule.routineData.time);

            printf("Room/Platform (e.g., 501 or Online): ");
            if (!fgets(schedule.routineData.room, sizeof(schedule.routineData.room), stdin))
            {
                printf("Input error!\n");
                continue;
            }
            trimWhitespace(schedule.routineData.room);

            printf("Faculty/Instructor (e.g., Dr. Smith or TBA): ");
            if (!fgets(schedule.routineData.faculty, sizeof(schedule.routineData.faculty), stdin))
            {
                printf("Input error!\n");
                continue;
            }
            trimWhitespace(schedule.routineData.faculty);

            schedule.otherDetails[0] = '\0'; 
        }
        else
        { 
            printf("Enter Details for '%s':\n", schedule.scheduleType);
            if (!fgets(schedule.otherDetails, sizeof(schedule.otherDetails), stdin))
            {
                printf("Input error!\n");
                continue;
            }
            trimWhitespace(schedule.otherDetails);
            schedule.routineData.day[0] = '\0';
            schedule.routineData.date[0] = '\0';
            schedule.routineData.time[0] = '\0';
            schedule.routineData.room[0] = '\0';
            schedule.routineData.faculty[0] = '\0';
        }
        int isValid = 1;
        if (strlen(schedule.intake) == 0)
        {
            printf("Error: Intake is required.\n");
            isValid = 0;
        }
        if (strlen(schedule.section) == 0)
        {
            printf("Error: Section is required.\n");
            isValid = 0;
        }
        if (strlen(schedule.scheduleType) == 0)
        {
            printf("Error: Schedule Type is required.\n");
            isValid = 0;
        }
        if (isRoutineType(schedule.scheduleType))
        {
            if (strlen(schedule.routineData.day) == 0 || strlen(schedule.routineData.time) == 0 || strlen(schedule.routineData.room) == 0)
            {
                printf("Error: For routine types, Day, Time, and Room are required.\n");
                isValid = 0;
            }
        }
        else
        {
            if (strlen(schedule.otherDetails) == 0)
            {
                printf("Error: Details are required for this schedule type.\n");
                isValid = 0;
            }
        }

        if (isValid)
        {
            if (fseek(*scheduleFile_ptr, 0, SEEK_END) != 0)
            {
                perror("Error seeking to end of schedule file");
            }
            else
            {
                size_t written = fwrite(&schedule, sizeof(struct AcademicSchedule), 1, *scheduleFile_ptr);
                fflush(*scheduleFile_ptr); 

                if (written == 1)
                {
                    printf("\nSchedule entry added successfully.\n");
                }
                else
                {
                    perror("\nError writing schedule entry");
                    clearerr(*scheduleFile_ptr);
                }
            }
        }
        else
        {
            printf("Schedule not added due to missing required fields.\n");
        }

        printf("\nOptions:\n1. Add Another Entry\n2. Back to Schedule Menu\nChoice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            choice = 2; 
            clearInputBuffer();
        }
        else
        {
            clearInputBuffer(); 
        }
        if (choice != 1)
            addMore = 0;
    }
}

void viewSchedule(FILE **scheduleFile_ptr)
{
    if (*scheduleFile_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Schedule file is not open in viewSchedule.\n");
        pressEnterToContinue();
        return;
    }
    struct AcademicSchedule schedule;
    struct AcademicSchedule foundSchedules[200]; 
    int count = 0;
    char searchIntake[20], searchSection[20];
    int printedFlags[200] = {0}; 
    const char *displayOrder[] = {
        "Class Routine", "Mid Exam Routine", "Final Exam Routine",
        "Result Publication Date", "Other Event", NULL};

    system("clear || cls");
    printf("\n--- View All Schedules for Intake & Section ---\n");
    printf("Enter Intake: ");
    if (!fgets(searchIntake, sizeof(searchIntake), stdin))
    {
        printf("Input error!\n");
        return;
    }
    trimWhitespace(searchIntake);
    printf("Enter Section: ");
    if (!fgets(searchSection, sizeof(searchSection), stdin))
    {
        printf("Input error!\n");
        return;
    }
    trimWhitespace(searchSection);

    if (strlen(searchIntake) == 0 || strlen(searchSection) == 0)
    {
        printf("Intake and Section required.\n");
        return;
    }

    rewind(*scheduleFile_ptr);
    clearerr(*scheduleFile_ptr); 

    count = 0; 
    while (count < 200 && fread(&schedule, sizeof(struct AcademicSchedule), 1, *scheduleFile_ptr) == 1)
    {
        if (strcmp(schedule.intake, searchIntake) == 0 && strcmp(schedule.section, searchSection) == 0)
        {
            if (strlen(schedule.scheduleType) < sizeof(schedule.scheduleType) - 1)
            {
                foundSchedules[count++] = schedule;
            }
            else
            {
                fprintf(stderr, "Warning: Skipping potentially corrupt schedule record (long type field) at offset %ld.\n", ftell(*scheduleFile_ptr) - sizeof(struct AcademicSchedule));
            }
        }
    }
    if (ferror(*scheduleFile_ptr))
    {
        perror("Error reading schedule file");
        clearerr(*scheduleFile_ptr);
    }

    if (count > 0)
    {
        printf("\n\n***********************************************************************\n");
        printf("         ACADEMIC SCHEDULE - Intake: %s, Section: %s\n", searchIntake, searchSection);
        printf("***********************************************************************\n");
        for (int orderIdx = 0; displayOrder[orderIdx] != NULL; ++orderIdx)
        {
            const char *currentType = displayOrder[orderIdx];
            int typeFoundInSection = 0;

            for (int i = 0; i < count; ++i)
            {
                if (!printedFlags[i] && strcmp(foundSchedules[i].scheduleType, currentType) == 0)
                {
                    if (!typeFoundInSection)
                    { 
                        printf("\n+---------------------------------------------------------------------+\n");
                        printf("| %-67s |\n", currentType);
                        printf("+---------------------------------------------------------------------+\n");
                        typeFoundInSection = 1;
                    }
                    if (isRoutineType(currentType))
                    {
                        printf("| Day: %-61s |\n", foundSchedules[i].routineData.day);
                        printf("| Date: %-60s |\n", foundSchedules[i].routineData.date);
                        printf("| Time: %-60s |\n", foundSchedules[i].routineData.time);
                        printf("| Room: %-60s |\n", foundSchedules[i].routineData.room);
                        printf("| Faculty: %-57s |\n", foundSchedules[i].routineData.faculty);
                    }
                    else
                    {
                        printf("| Details: %-58.58s |\n", foundSchedules[i].otherDetails);
                        if (strlen(foundSchedules[i].otherDetails) > 58)
                        {
                            printf("| %-67s |\n", "... (details may be longer)");
                        }
                    }
                    printf("|---------------------------------------------------------------------|\n"); // Separator
                    printedFlags[i] = 1;
                }
            }
            if (typeFoundInSection)
            { 

            }
        }
        int othersFoundHeader = 0;
        for (int i = 0; i < count; ++i)
        {
            if (!printedFlags[i])
            {
                if (!othersFoundHeader)
                {
                    printf("\n+---------------------------------------------------------------------+\n");
                    printf("| %-67s |\n", "Other Schedule Entries");
                    printf("+---------------------------------------------------------------------+\n");
                    othersFoundHeader = 1;
                }
                printf("| Type: %-60s |\n", foundSchedules[i].scheduleType);
                if (isRoutineType(foundSchedules[i].scheduleType))
                {
                    printf("| Day: %-61s |\n", foundSchedules[i].routineData.day);
                    printf("| Date: %-60s |\n", foundSchedules[i].routineData.date);
                    printf("| Time: %-60s |\n", foundSchedules[i].routineData.time);
                    printf("| Room: %-60s |\n", foundSchedules[i].routineData.room);
                    printf("| Faculty: %-57s |\n", foundSchedules[i].routineData.faculty);
                }
                else
                {
                    printf("| Details: %-58.58s |\n", foundSchedules[i].otherDetails);
                    if (strlen(foundSchedules[i].otherDetails) > 58)
                    {
                        printf("| %-67s |\n", "... (details may be longer)");
                    }
                }
                printf("|---------------------------------------------------------------------|\n");
                printedFlags[i] = 1;
            }
        }


        printf("\n***********************************************************************\n");
    }
    else
    {
        printf("\nNo schedules found matching Intake '%s' and Section '%s'.\n", searchIntake, searchSection);
    }
}

void updateSchedule(FILE **scheduleFile_ptr)
{
    if (*scheduleFile_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Schedule file is not open in updateSchedule.\n");
        pressEnterToContinue();
        return;
    }

    struct AcademicSchedule schedule; 
    char searchIntake[20], searchSection[20], searchType[50];
    int found = 0;
    long recordPos = -1; 
    int updateAnother = 1;

    while (updateAnother)
    {
        system("clear || cls");
        printf("\n--- Update Specific Schedule Entry ---\n");
        printf("Enter Intake of the schedule to update: ");
        if (!fgets(searchIntake, sizeof(searchIntake), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(searchIntake);

        printf("Enter Section: ");
        if (!fgets(searchSection, sizeof(searchSection), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(searchSection);

        printf("Enter Schedule Type to update: ");
        if (!fgets(searchType, sizeof(searchType), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(searchType);

        if (strlen(searchIntake) == 0 || strlen(searchSection) == 0 || strlen(searchType) == 0)
        {
            printf("All search fields (Intake, Section, Type) are required.\n");
            pressEnterToContinue();
            continue;
        }

        rewind(*scheduleFile_ptr);
        clearerr(*scheduleFile_ptr);

        found = 0;
        recordPos = -1;
        while ((recordPos = ftell(*scheduleFile_ptr)) != -1 &&
               fread(&schedule, sizeof(struct AcademicSchedule), 1, *scheduleFile_ptr) == 1)
        {
            if (strcmp(schedule.intake, searchIntake) == 0 &&
                strcmp(schedule.section, searchSection) == 0 &&
                strcmp(schedule.scheduleType, searchType) == 0)
            {
                found = 1;
                break; 
            }
        }
        if (!found && ferror(*scheduleFile_ptr))
        {
            perror("Error reading schedule file during update search");
            clearerr(*scheduleFile_ptr);
            pressEnterToContinue();
            continue;
        }

        if (found)
        {
            printf("\n--- Found Schedule Entry ---");
            printf("\nIntake: %s, Section: %s, Type: %s", schedule.intake, schedule.section, schedule.scheduleType);

            if (isRoutineType(schedule.scheduleType))
            {
                printf("\nCurrent Day: %s", schedule.routineData.day);
                printf("\nCurrent Date: %s", schedule.routineData.date);
                printf("\nCurrent Time: %s", schedule.routineData.time);
                printf("\nCurrent Room: %s", schedule.routineData.room);
                printf("\nCurrent Faculty: %s", schedule.routineData.faculty);
            }
            else
            {
                printf("\nCurrent Details:\n%s", schedule.otherDetails);
            }

            printf("\n\n--- Enter New Details (Press Enter to keep current) ---\n");
            struct RoutineInfo newRoutineData; 
            char newOtherDetails[500];
            char inputBuffer[512]; 
            int changed = 0;
            if (isRoutineType(schedule.scheduleType))
            {
                printf("New Day (%s): ", schedule.routineData.day);
                if (!fgets(inputBuffer, sizeof(inputBuffer), stdin))
                    break;
                trimWhitespace(inputBuffer);
                if (strlen(inputBuffer) > 0)
                {
                    strncpy(schedule.routineData.day, inputBuffer, sizeof(schedule.routineData.day) - 1);
                    schedule.routineData.day[sizeof(schedule.routineData.day) - 1] = '\0';
                    changed = 1;
                }

                printf("New Date (%s): ", schedule.routineData.date);
                if (!fgets(inputBuffer, sizeof(inputBuffer), stdin))
                    break;
                trimWhitespace(inputBuffer);
                if (strlen(inputBuffer) > 0)
                {
                    strncpy(schedule.routineData.date, inputBuffer, sizeof(schedule.routineData.date) - 1);
                    schedule.routineData.date[sizeof(schedule.routineData.date) - 1] = '\0';
                    changed = 1;
                }

                printf("New Time (%s): ", schedule.routineData.time);
                if (!fgets(inputBuffer, sizeof(inputBuffer), stdin))
                    break;
                trimWhitespace(inputBuffer);
                if (strlen(inputBuffer) > 0)
                {
                    strncpy(schedule.routineData.time, inputBuffer, sizeof(schedule.routineData.time) - 1);
                    schedule.routineData.time[sizeof(schedule.routineData.time) - 1] = '\0';
                    changed = 1;
                }

                printf("New Room (%s): ", schedule.routineData.room);
                if (!fgets(inputBuffer, sizeof(inputBuffer), stdin))
                    break;
                trimWhitespace(inputBuffer);
                if (strlen(inputBuffer) > 0)
                {
                    strncpy(schedule.routineData.room, inputBuffer, sizeof(schedule.routineData.room) - 1);
                    schedule.routineData.room[sizeof(schedule.routineData.room) - 1] = '\0';
                    changed = 1;
                }

                printf("New Faculty (%s): ", schedule.routineData.faculty);
                if (!fgets(inputBuffer, sizeof(inputBuffer), stdin))
                    break;
                trimWhitespace(inputBuffer);
                if (strlen(inputBuffer) > 0)
                {
                    strncpy(schedule.routineData.faculty, inputBuffer, sizeof(schedule.routineData.faculty) - 1);
                    schedule.routineData.faculty[sizeof(schedule.routineData.faculty) - 1] = '\0';
                    changed = 1;
                }
            }
            else
            {
                printf("New Details (Current: %s):\n", schedule.otherDetails);
                if (!fgets(newOtherDetails, sizeof(newOtherDetails), stdin))
                    break;
                trimWhitespace(newOtherDetails);
                if (strlen(newOtherDetails) > 0)
                {
                    strncpy(schedule.otherDetails, newOtherDetails, sizeof(schedule.otherDetails) - 1);
                    schedule.otherDetails[sizeof(schedule.otherDetails) - 1] = '\0';
                    changed = 1;
                }
            }
            if (changed)
            {
                if (fseek(*scheduleFile_ptr, recordPos, SEEK_SET) != 0)
                {
                    perror("Error seeking back to record position for update");
                }
                else
                {
                    size_t written = fwrite(&schedule, sizeof(struct AcademicSchedule), 1, *scheduleFile_ptr);
                    fflush(*scheduleFile_ptr); 

                    if (written == 1)
                    {
                        printf("\nSchedule entry updated successfully.\n");
                    }
                    else
                    {
                        perror("\nError writing updated schedule entry");
                        clearerr(*scheduleFile_ptr);
                    }
                }
            }
            else
            {
                printf("\nNo changes entered. Schedule not updated.\n");
            }
        }
        else
        {
            printf("\nSchedule entry not found for Intake '%s', Section '%s', Type '%s'.\n", searchIntake, searchSection, searchType);
        }
        printf("\nOptions:\n1. Update Another Entry\n2. Back to Schedule Menu\nChoice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            choice = 2;
            clearInputBuffer();
        }
        else
        {
            clearInputBuffer(); 
        }
        if (choice != 1)
            updateAnother = 0;
    }
}

void deleteSchedule(FILE **scheduleFile_ptr)
{
    if (*scheduleFile_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Schedule file is not open in deleteSchedule.\n");
        pressEnterToContinue();
        return;
    }

    struct AcademicSchedule schedule; 
    char deleteIntake[20], deleteSection[20], deleteType[50];
    int found = 0;
    int deleteAnother = 1;

    const char *originalFilename = "schedules.dat";
    const char *tempFilename = "temp_schedules.tmp";

    while (deleteAnother)
    {
        system("clear || cls");
        printf("\n--- Delete Specific Schedule Entry ---\n");
        printf("Enter Intake of schedule to delete: ");
        if (!fgets(deleteIntake, sizeof(deleteIntake), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(deleteIntake);
        printf("Enter Section: ");
        if (!fgets(deleteSection, sizeof(deleteSection), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(deleteSection);
        printf("Enter Schedule Type to delete: ");
        if (!fgets(deleteType, sizeof(deleteType), stdin))
        {
            printf("Input error!\n");
            continue;
        }
        trimWhitespace(deleteType);

        if (strlen(deleteIntake) == 0 || strlen(deleteSection) == 0 || strlen(deleteType) == 0)
        {
            printf("All fields (Intake, Section, Type) required.\n");
            pressEnterToContinue();
            continue;
        }
        if (fclose(*scheduleFile_ptr) != 0)
        {
            perror("Warning: Error closing main schedule file before delete");
        }
        *scheduleFile_ptr = NULL; 
        FILE *scheduleFile_r = fopen(originalFilename, "rb");
        FILE *tempScheduleFile_w = fopen(tempFilename, "wb");

        if (scheduleFile_r == NULL || tempScheduleFile_w == NULL)
        {
            if (scheduleFile_r == NULL)
                perror("Error opening original schedule file for reading (delete)");
            if (tempScheduleFile_w == NULL)
                perror("Error creating temporary schedule file (delete)");
            if (scheduleFile_r)
                fclose(scheduleFile_r);
            if (tempScheduleFile_w)
                fclose(tempScheduleFile_w);
            remove(tempFilename);
            *scheduleFile_ptr = fopen(originalFilename, "rb+");
            if (*scheduleFile_ptr == NULL)
                *scheduleFile_ptr = fopen(originalFilename, "wb+");
            if (*scheduleFile_ptr == NULL)
            {
                perror("FATAL: Could not reopen schedule file");
                exit(EXIT_FAILURE);
            }
            pressEnterToContinue();
            return; 
        }
        found = 0; 
        while (fread(&schedule, sizeof(struct AcademicSchedule), 1, scheduleFile_r) == 1)
        {
            if (strcmp(schedule.intake, deleteIntake) == 0 &&
                strcmp(schedule.section, deleteSection) == 0 &&
                strcmp(schedule.scheduleType, deleteType) == 0)
            {
                found = 1; 
            }
            else
            {
                if (fwrite(&schedule, sizeof(struct AcademicSchedule), 1, tempScheduleFile_w) != 1)
                {
                    perror("Error writing record to temp schedule file");
                    break;
                }
            }
        }
        if (ferror(scheduleFile_r))
        {
            perror("Error reading from original schedule file during delete");
        }
        fclose(scheduleFile_r);
        fclose(tempScheduleFile_w);
        if (found)
        {
            if (remove(originalFilename) != 0)
            {
                perror("Error deleting original schedule file");
                fprintf(stderr, "Temp data in '%s'\n", tempFilename);
            }
            else if (rename(tempFilename, originalFilename) != 0)
            {
                perror("Error renaming temp schedule file");
                fprintf(stderr, "CRITICAL Error: Original deleted, rename failed. Data lost. Temp file '%s'\n", tempFilename);
            }
            else
            {
                printf("\nSchedule entry deleted successfully.\n");
            }
        }
        else
        {
            printf("\nSchedule entry not found. No changes made.\n");
            remove(tempFilename); 
        }
        *scheduleFile_ptr = fopen(originalFilename, "rb+"); 
        if (*scheduleFile_ptr == NULL)
            *scheduleFile_ptr = fopen(originalFilename, "wb+");
        if (*scheduleFile_ptr == NULL)
        {
            perror("FATAL: Could not reopen schedule file after delete");
            exit(EXIT_FAILURE);
        }
        printf("\nOptions:\n1. Delete Another Entry\n2. Back to Schedule Menu\nChoice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            choice = 2;
            clearInputBuffer();
        }
        else
        {
            clearInputBuffer();
        }
        if (choice != 1)
            deleteAnother = 0;
    }
}


void manageResults(FILE **resultFile_ptr)
{
    int resultRunning = 1;
    int select;
    while (resultRunning)
    {
        system("clear || cls");
        printf("\n\n--- Result Publication Management ---\n");
        printf("1. Add Student Result\n");
        printf("2. View Individual Result\n");
        printf("3. View Result List\n");
        printf("4. Update Student Result\n");
        printf("5. Delete Student Result\n");
        printf("6. Back to Main Menu\n");
        printf("\nEnter Choice (1-6): ");

        if (scanf("%d", &select) != 1)
        {
            printf("Invalid input.\n");
            clearInputBuffer();
            pressEnterToContinue();
            continue;
        }
        clearInputBuffer();
        if (*resultFile_ptr == NULL)
        {
            fprintf(stderr, "ERROR: Result file pointer is NULL in manageResults.\n");
            *resultFile_ptr = fopen("results.dat", "rb+");
            if (*resultFile_ptr == NULL)
                *resultFile_ptr = fopen("results.dat", "wb+");
            if (*resultFile_ptr == NULL)
            {
                perror("FATAL: Result file inaccessible");
                exit(EXIT_FAILURE);
            }
            printf("INFO: Result file pointer was reopened.\n");
        }

        switch (select)
        {
        case 1:
            addResult(resultFile_ptr);
            break;
        case 2:
            viewIndividualResult(resultFile_ptr);
            break;
        case 3:
            viewResultList(resultFile_ptr);
            break;
        case 4:
            updateResult(resultFile_ptr);
            break;
        case 5:
            deleteResult(resultFile_ptr);
            break;
        case 6:
            resultRunning = 0;
            break;
        default:
            printf("Invalid choice (%d). Please enter 1-6.\n", select);
            break;
        }
        if (resultRunning && select >= 1 && select <= 5)
        {
            pressEnterToContinue();
        }
    }
}

void addResult(FILE **resultFile_ptr)
{
    if (*resultFile_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Result file is not open in addResult.\n");
        pressEnterToContinue();
        return;
    }
    struct StudentResult result;
    int addMore = 1;
    while (addMore)
    {
        system("clear || cls");
        printf("\n--- Add New Student Result ---\n");
        memset(&result, 0, sizeof(struct StudentResult));

        printf("Enter Student ID: ");
        if (!fgets(result.studentID, sizeof(result.studentID), stdin))
            continue;
        trimWhitespace(result.studentID);
        printf("Enter Student Name: ");
        if (!fgets(result.name, sizeof(result.name), stdin))
            continue;
        trimWhitespace(result.name);
        printf("Enter Intake: ");
        if (!fgets(result.intake, sizeof(result.intake), stdin))
            continue;
        trimWhitespace(result.intake);
        printf("Enter Section: ");
        if (!fgets(result.section, sizeof(result.section), stdin))
            continue;
        trimWhitespace(result.section);

        printf("Enter GPA (0.0-4.0): ");
        while (scanf("%f", &result.gpa) != 1 || result.gpa < 0.0 || result.gpa > 4.0)
        {
            printf("Invalid GPA. Enter 0.0-4.0: ");
            clearInputBuffer(); 
        }
        clearInputBuffer(); 

        calculateGrade(result.gpa, result.grade);

        if (strlen(result.studentID) == 0 || strlen(result.name) == 0 || strlen(result.intake) == 0 || strlen(result.section) == 0)
        {
            printf("\nError: All fields (ID, Name, Intake, Section) are required.\n");
        }
        else
        {
            if (fseek(*resultFile_ptr, 0, SEEK_END) != 0)
            {
                perror("Error seeking result file for add");
            }
            else
            {
                size_t written = fwrite(&result, sizeof(struct StudentResult), 1, *resultFile_ptr);
                fflush(*resultFile_ptr); 
                if (written == 1)
                {
                    printf("\nResult added successfully. Grade: %s\n", result.grade);
                }
                else
                {
                    perror("\nError writing result");
                    clearerr(*resultFile_ptr);
                }
            }
        }
        printf("\nOptions:\n1. Add Another Result\n2. Back to Result Menu\nChoice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            choice = 2;
            clearInputBuffer();
        }
        else
        {
            clearInputBuffer();
        }
        if (choice != 1)
            addMore = 0;
    }
}

void viewIndividualResult(FILE **resultFile_ptr)
{
    if (*resultFile_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Result file is not open in viewIndividualResult.\n");
        pressEnterToContinue();
        return;
    }
    struct StudentResult result;
    char searchID[20], searchIntake[20], searchSection[20];
    int found = 0;

    system("clear || cls");
    printf("\n--- View Individual Student Result ---\n");
    printf("Enter Student ID: ");
    if (!fgets(searchID, sizeof(searchID), stdin))
        return;
    trimWhitespace(searchID);
    printf("Enter Intake: ");
    if (!fgets(searchIntake, sizeof(searchIntake), stdin))
        return;
    trimWhitespace(searchIntake);
    printf("Enter Section: ");
    if (!fgets(searchSection, sizeof(searchSection), stdin))
        return;
    trimWhitespace(searchSection);

    if (strlen(searchID) == 0 || strlen(searchIntake) == 0 || strlen(searchSection) == 0)
    {
        printf("All search fields required.\n");
        return;
    }

    rewind(*resultFile_ptr);
    clearerr(*resultFile_ptr);
    printf("\n--- Result Details ---\n");
    while (fread(&result, sizeof(struct StudentResult), 1, *resultFile_ptr) == 1)
    {
        if (strcmp(result.studentID, searchID) == 0 &&
            strcmp(result.intake, searchIntake) == 0 &&
            strcmp(result.section, searchSection) == 0)
        {
            found = 1;
            printf("\nStudent ID: %s\nName: %s\nIntake: %s\nSection: %s\nGPA: %.2f\nGrade: %s\n",
                   result.studentID, result.name, result.intake, result.section, result.gpa, result.grade);
            printf("-----------------------------------\n");
            break;
        }
    }
    if (ferror(*resultFile_ptr))
    {
        perror("Error reading result file");
        clearerr(*resultFile_ptr);
    }
    if (!found)
        printf("\nNo result found matching criteria.\n");
}

void viewResultList(FILE **resultFile_ptr)
{
    if (*resultFile_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Result file is not open in viewResultList.\n");
        pressEnterToContinue();
        return;
    }
    struct StudentResult result;
    char searchIntake[20], searchSection[20];
    int found = 0;

    system("clear || cls");
    printf("\n--- View Result List by Intake & Section ---\n");
    printf("Enter Intake: ");
    if (!fgets(searchIntake, sizeof(searchIntake), stdin))
        return;
    trimWhitespace(searchIntake);
    printf("Enter Section: ");
    if (!fgets(searchSection, sizeof(searchSection), stdin))
        return;
    trimWhitespace(searchSection);

    if (strlen(searchIntake) == 0 || strlen(searchSection) == 0)
    {
        printf("Intake and Section required.\n");
        return;
    }

    rewind(*resultFile_ptr);
    clearerr(*resultFile_ptr);
    printf("\n--- Result List for Intake: %s, Section: %s ---\n\n", searchIntake, searchSection);
    printf("%-15s %-25s %-8s %-8s\n", "Student ID", "Name", "GPA", "Grade");
    printf("-------------------------------------------------------------\n");
    while (fread(&result, sizeof(struct StudentResult), 1, *resultFile_ptr) == 1)
    {
        if (strcmp(result.intake, searchIntake) == 0 && strcmp(result.section, searchSection) == 0)
        {
            found = 1;
            printf("%-15s %-25s %-8.2f %-8s\n", result.studentID, result.name, result.gpa, result.grade);
        }
    }
    if (ferror(*resultFile_ptr))
    {
        perror("Error reading result file");
        clearerr(*resultFile_ptr);
    }
    printf("-------------------------------------------------------------\n");
    if (!found)
        printf("\nNo results found matching criteria.\n");
}

void updateResult(FILE **resultFile_ptr)
{
    if (*resultFile_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Result file is not open in updateResult.\n");
        pressEnterToContinue();
        return;
    }
    struct StudentResult result; 
    char searchID[20], searchIntake[20], searchSection[20];
    int found = 0;
    long recordPos = -1;
    float newGPA;
    int updateAnother = 1;

    while (updateAnother)
    {
        system("clear || cls");
        printf("\n--- Update Student Result ---\n");
        printf("Enter Student ID: ");
        if (!fgets(searchID, sizeof(searchID), stdin))
            continue;
        trimWhitespace(searchID);
        printf("Enter Intake: ");
        if (!fgets(searchIntake, sizeof(searchIntake), stdin))
            continue;
        trimWhitespace(searchIntake);
        printf("Enter Section: ");
        if (!fgets(searchSection, sizeof(searchSection), stdin))
            continue;
        trimWhitespace(searchSection);

        if (strlen(searchID) == 0 || strlen(searchIntake) == 0 || strlen(searchSection) == 0)
        {
            printf("All search fields required.\n");
            pressEnterToContinue();
            continue;
        }

        rewind(*resultFile_ptr);
        clearerr(*resultFile_ptr);

        found = 0;
        recordPos = -1;
        while ((recordPos = ftell(*resultFile_ptr)) != -1 &&
               fread(&result, sizeof(struct StudentResult), 1, *resultFile_ptr) == 1)
        {
            if (strcmp(result.studentID, searchID) == 0 &&
                strcmp(result.intake, searchIntake) == 0 &&
                strcmp(result.section, searchSection) == 0)
            {
                found = 1;
                printf("\n--- Found Result ---\n");
                printf("Student: %s (%s)\n", result.name, result.studentID);
                printf("Current GPA: %.2f, Grade: %s\n", result.gpa, result.grade);
                break; 
            }
        }
        if (!found && ferror(*resultFile_ptr))
        {
            perror("Error reading result file during update search");
            clearerr(*resultFile_ptr);
            pressEnterToContinue();
            continue;
        }

        if (found)
        {
            printf("\nEnter New GPA (0.0 - 4.0): ");
            while (scanf("%f", &newGPA) != 1 || newGPA < 0.0 || newGPA > 4.0)
            {
                printf("Invalid GPA (0.0-4.0): ");
                clearInputBuffer();
            }
            clearInputBuffer(); 

            result.gpa = newGPA;
            calculateGrade(result.gpa, result.grade);
            if (fseek(*resultFile_ptr, recordPos, SEEK_SET) != 0)
            {
                perror("Error seeking for result update");
            }
            else
            {
                size_t written = fwrite(&result, sizeof(struct StudentResult), 1, *resultFile_ptr);
                fflush(*resultFile_ptr); 
                if (written == 1)
                {
                    printf("\nResult updated. New GPA: %.2f, Grade: %s\n", result.gpa, result.grade);
                }
                else
                {
                    perror("\nError writing result update");
                    clearerr(*resultFile_ptr);
                }
            }
        }
        else
        {
            printf("\nResult not found for ID %s, Intake %s, Section %s.\n", searchID, searchIntake, searchSection);
        }
        printf("\nOptions:\n1. Update Another Result\n2. Back to Result Menu\nChoice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            choice = 2;
            clearInputBuffer();
        }
        else
        {
            clearInputBuffer();
        }
        if (choice != 1)
            updateAnother = 0;
    }
}

void deleteResult(FILE **resultFile_ptr)
{
    if (*resultFile_ptr == NULL)
    {
        fprintf(stderr, "ERROR: Result file is not open in deleteResult.\n");
        pressEnterToContinue();
        return;
    }
    struct StudentResult result;
    char deleteID[20], deleteIntake[20], deleteSection[20];
    int found = 0;
    int deleteAnother = 1;

    const char *originalFilename = "results.dat";
    const char *tempFilename = "temp_results.tmp";

    while (deleteAnother)
    {
        system("clear || cls");
        printf("\n--- Delete Student Result ---\n");
        printf("Enter Student ID: ");
        if (!fgets(deleteID, sizeof(deleteID), stdin))
            continue;
        trimWhitespace(deleteID);
        printf("Enter Intake: ");
        if (!fgets(deleteIntake, sizeof(deleteIntake), stdin))
            continue;
        trimWhitespace(deleteIntake);
        printf("Enter Section: ");
        if (!fgets(deleteSection, sizeof(deleteSection), stdin))
            continue;
        trimWhitespace(deleteSection);

        if (strlen(deleteID) == 0 || strlen(deleteIntake) == 0 || strlen(deleteSection) == 0)
        {
            printf("All fields required.\n");
            pressEnterToContinue();
            continue;
        }
        if (fclose(*resultFile_ptr) != 0)
        { 
        }
        *resultFile_ptr = NULL;

        FILE *resultFile_r = fopen(originalFilename, "rb");
        FILE *tempFile_w = fopen(tempFilename, "wb");

        if (resultFile_r == NULL || tempFile_w == NULL)
        {
            if (resultFile_r == NULL)
                perror("Error opening original results file (delete)");
            if (tempFile_w == NULL)
                perror("Error creating temp results file (delete)");
            if (resultFile_r)
                fclose(resultFile_r);
            if (tempFile_w)
                fclose(tempFile_w);
            remove(tempFilename);

            *resultFile_ptr = fopen(originalFilename, "rb+");
            if (*resultFile_ptr == NULL)
                *resultFile_ptr = fopen(originalFilename, "wb+");
            if (*resultFile_ptr == NULL)
                exit(EXIT_FAILURE);
            pressEnterToContinue();
            return;
        }

        found = 0;
        while (fread(&result, sizeof(struct StudentResult), 1, resultFile_r) == 1)
        {
            if (strcmp(result.studentID, deleteID) == 0 &&
                strcmp(result.intake, deleteIntake) == 0 &&
                strcmp(result.section, deleteSection) == 0)
            {
                found = 1; 
            }
            else
            {
                if (fwrite(&result, sizeof(struct StudentResult), 1, tempFile_w) != 1)
                {
                    perror("Error writing to temp results file");
                    break;
                }
            }
        }
        if (ferror(resultFile_r))
            perror("Error reading original results file (delete)");

        fclose(resultFile_r);
        fclose(tempFile_w);

        if (found)
        {
            if (remove(originalFilename) != 0)
            {
                perror("Error deleting original results file");
                fprintf(stderr, "Temp data in '%s'\n", tempFilename);
            }
            else if (rename(tempFilename, originalFilename) != 0)
            {
                perror("Error renaming temp results file");
                fprintf(stderr, "CRITICAL Error: Original deleted, rename failed. Data lost. Temp file '%s'\n", tempFilename);
            }
            else
            {
                printf("\nResult deleted successfully.\n");
            }
        }
        else
        {
            printf("\nResult not found. No changes made.\n");
            remove(tempFilename);
        }


        *resultFile_ptr = fopen(originalFilename, "rb+");
        if (*resultFile_ptr == NULL)
            *resultFile_ptr = fopen(originalFilename, "wb+");
        if (*resultFile_ptr == NULL)
        {
            perror("FATAL: Could not reopen result file after delete");
            exit(EXIT_FAILURE);
        }


        printf("\nOptions:\n1. Delete Another Result\n2. Back to Result Menu\nChoice: ");
        int choice;
        if (scanf("%d", &choice) != 1)
        {
            choice = 2;
            clearInputBuffer();
        }
        else
        {
            clearInputBuffer();
        }
        if (choice != 1)
            deleteAnother = 0;
    }
}

//description: An advanced C++17 CGPA calculator with robust error handling, RAII, and modern coding practices.

#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>           // For numeric_limits to handle input errors
#include <stdexcept>      // For custom exceptions in advanced error handling
#include <memory>       // For smart pointers if needed, though not heavily used here
#include <algorithm>  // For std::for_each or other algorithms11

// Using namespace std is avoided in advanced code to prevent name collisions.
// Instead, we'll use std:: prefixes or selective using declarations.
// Custom exception for invalid grades or credits, promoting robust error handling.
class InvalidGradeException : public std::runtime_error {
public:
    InvalidGradeException(const std::string& msg) : std::runtime_error(msg) {}
};
class InvalidCreditException : public std::runtime_error {
public:
    InvalidCreditException(const std::string& msg) : std::runtime_error(msg) {}
};
/*
 * Alright, so this is an upgraded CGPA Calculator. I took the original and made it way more solid using modern C++ tricks.
 * Think RAII for handling resources automatically, exceptions to catch screw-ups without crashing, and keeping things
 * const-correct so nothing gets accidentally changed. File stuff now cleans up after itself, and input checks are
 * beefed up with custom errors. The comments here explain not just what’s happening, but why I did it this way – like,
 * why vectors are great for this, or how exceptions make the code tougher. It’s all about making it easier to maintain,
 * faster, and less buggy. If you’re into coding, this should feel like a step up from basic stuff.
 * Here’s the breakdown of what’s new and improved:
 * Key points covered (as per your reminder):
 * - Take input for the number of courses taken by the student (done in main, case 1, with validation).
 * - For each course, input the grade and the credit hours (handled in the loop with getValidatedInput).
 * - Calculate the total credits and total grade points (grade × credit hours) (this happens in calculateGPA and calculateCGPA).
 * - Compute the GPA for the semester and then the overall CGPA (Semester::calculateGPA for per-semester, Student::calculateCGPA for overall).
 * - Display individual course grades and the final CGPA to the user (via displayCourses and displayAll).
 */
/*
 * Class: Course
 * Basically, this just bundles a course's grade and credits together.
 * I made it like a simple struct with public stuff so it's super lightweight and easy to copy.
 * No need for getters/setters here – keeps it straightforward. And hey, no dynamic memory, so it's safe and quick.
 * Ties into the points: This struct holds the grade and credit for each course, which are used in calculations.
 */
class Course {
public:
    double grade;  // The numeric grade, like 7.5, stored as double for precise math.
    double credit; // How many credits this course is worth – used for weighting.
    // Constructor that sets things up right away – efficient and clean.
    Course(double g, double c) : grade(g), credit(c) {
        // Could add checks here, but I left it flexible for now.
    }
};
/*
 * Class: Semester
 * This handles all the courses for one semester, figures out the GPA, and shows the details.
 * I separated this out so the Student class doesn't have to worry about course-level junk.
 * Vectors are perfect here for storing courses – they grow as needed and manage memory automatically.
 * Made methods const where possible to avoid sneaky changes, and used range-based loops to keep things readable.
 * It's all exception-safe too, meaning it won't throw surprises unless something really goes wrong.
 * Key points: Calculates total credits and grade points for GPA (grade × credit), and displays individual courses.
 */
class Semester {
private:
    std::vector<Course> courses;  // A list of courses; vector's great for fast access and no manual cleanup.
public:
    // Adds a course by building it right in the vector – saves on copying.
    // Not a huge deal here, but it's a nice efficiency boost.
    void addCourse(double grade, double credit) {
        courses.emplace_back(grade, credit);
    }
    // Calculates the GPA: basically, total points (grade times credits) divided by total credits.
    // If no credits, it just returns 0 to avoid dividing by zero – safe bet.
    // Marked const so you can call it on a semester that won't change, and noexcept because it won't throw.
    // Key point: Computes GPA using total credits and grade points (grade × credit).
    double calculateGPA() const noexcept {
        double totalCredits = 0.0, totalPoints = 0.0;
        for (const auto& c : courses) {  // Easy loop that won't mess up iterators.
            totalCredits += c.credit;
            totalPoints += c.grade * c.credit;
        }
        return totalCredits == 0.0 ? 0.0 : totalPoints / totalCredits;
    }
    // Prints out the courses nicely.
    // Uses cout for output – could swap it out if you want to print to a file instead.
    // Const so it works on semesters you don't want to edit.
    // Key point: Displays individual course grades.
    void displayCourses() const {
        for (size_t i = 0; i < courses.size(); ++i) {
            std::cout << "Course " << i + 1
                    << " | Grade: " << courses[i].grade
                    << " | Credit: " << courses[i].credit << std::endl;
        }
    }
    // Gives you the list of courses without copying the whole thing – way faster for big lists.
    // It's const so you can't mess with the original, keeping things encapsulated.
    // If you really need to change it, we could add a non-const version later.
    const std::vector<Course>& getCourses() const noexcept {
        return courses;
    }
};
/*
 * Class: Student
 * This ties everything together – holds all the semesters, computes the big CGPA, and deals with saving/loading.
 * Keeps the multi-semester stuff separate from the semester details, which makes the code cleaner.
 * File operations are handled with RAII so they close automatically, and I added exceptions for when things go south.
 * The save/load uses a plain text format that's easy to read, but you could fancy it up with JSON if you want.
 * Input checks prevent loading junk data.
 * Key points: Computes overall CGPA by aggregating across semesters, and displays final CGPA.
 */
class Student {
private:
    std::vector<Semester> semesters;  // Just a list of semesters – grows as you add them.
public:
    // Adds a semester using move to avoid copying the whole thing.
    // Efficient for when semesters get big.
    void addSemester(Semester sem) {
        semesters.push_back(std::move(sem));
    }
    // Figures out the overall CGPA by crunching all courses across semesters.
    // Same math as the semester GPA, just bigger scale.
    // Const and noexcept for safety – no changes, no surprises.
    // Key point: Computes overall CGPA using total credits and grade points across all semesters.
    double calculateCGPA() const noexcept {
        double totalCredits = 0.0, totalPoints = 0.0;
        for (const auto& sem : semesters) {
            for (const auto& c : sem.getCourses()) {
                totalCredits += c.credit;
                totalPoints += c.grade * c.credit;
            }
        }
        return totalCredits == 0.0 ? 0.0 : totalPoints / totalCredits;
    }
    // Saves everything to a file in a simple format: number of courses, then grade and credit for each.
    // Text is human-readable and works everywhere. RAII means the file closes even if something breaks.
    // If it can't open, it throws an error so you know what happened.
    void saveToFile() {
        try {
            std::ofstream file("cgpa_data.txt");
            if (!file) {
                throw std::runtime_error("Failed to open file for saving.");
            }
            for (const auto& sem : semesters) {
                file << sem.getCourses().size() << std::endl;
                for (const auto& c : sem.getCourses()) {
                    file << c.grade << " " << c.credit << std::endl;
                }
            }
            std::cout << "Data saved successfully." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error saving data: " << e.what() << std::endl;
        }
    }
    // Loads data from the file, wiping out what's there first.
    // If no file, it just tells you and moves on. Exceptions catch bad data or I/O issues.
    // Clears everything on error to avoid half-loaded messes.
    void loadFromFile() {
        try {
            std::ifstream file("cgpa_data.txt");
            if (!file) {
                std::cout << "No saved data found." << std::endl;
                return;
            }
            semesters.clear();
            int courseCount;
            while (file >> courseCount) {
                Semester sem;
                for (int i = 0; i < courseCount; ++i) {
                    double g, c;
                    if (!(file >> g >> c)) {
                        throw std::runtime_error("Corrupt data in file.");
                    }
                    sem.addCourse(g, c);
                }
                semesters.push_back(std::move(sem));
            }
            std::cout << "Data loaded successfully." << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error loading data: " << e.what() << std::endl;
            semesters.clear();  // Reset on error to avoid partial loads.
        }
    }
    // Shows all semesters with their GPAs, plus the final CGPA, formatted nicely.
    // Fixed decimals for consistency. Const so it works on read-only students.
    // Key points: Displays individual course grades (via displayCourses) and the final CGPA.
    void displayAll() const {
        std::cout << std::fixed << std::setprecision(2);
        for (size_t i = 0; i < semesters.size(); ++i) {
            std::cout << "\nSemester " << i + 1 << ":" << std::endl;
            semesters[i].displayCourses();
            std::cout << "GPA: " << semesters[i].calculateGPA() << std::endl;
        }
        std::cout << "\nFinal CGPA: " << calculateCGPA() << std::endl;//C:/MinGW/bin/g++.exe
    }
};
/*
 * Function: getValidatedInput
 * A handy template to grab and check user input for numbers.
 * Cuts down on repeating code. Throws or loops on bad input, and fixes the input stream so it doesn't get stuck.
 * Reusable for different types like int or double.
 * Ties into points: Used for inputting number of courses, grades, and credits with validation.
 */
template <typename T>
T getValidatedInput(const std::string& prompt, T minVal, T maxVal) {
    T value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value && value >= minVal && value <= maxVal) {
            return value;
        } else {
            std::cout << "Invalid input. Please try again." << std::endl;
            std::cin.clear();  // Clear error flags.
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Ignore bad input.
        }
    }
}
/*
 * Function: main
 * The starting point – runs a menu loop for adding semesters, showing results, saving, loading, or quitting.
 * Keeps the user stuff separate from the math, so it's easier to test or change.
 * Used a lambda for the menu to avoid repeating the print code. Input validation keeps things from breaking on dumb entries.
 * Key points: Takes input for number of courses, grade and credit for each, computes GPA/CGPA, and displays results.
 */
int main() {
    Student student;
    int choice;
    // Lambda for showing the menu – keeps it tidy if the menu gets longer.
    auto displayMenu = []() {
        std::cout << "\n--- CGPA CALCULATOR MENU ---" << std::endl;
        std::cout << "1. Add Semester" << std::endl;
        std::cout << "2. Display Result" << std::endl;
        std::cout << "3. Save to File" << std::endl;
        std::cout << "4. Load from File" << std::endl;
        std::cout << "5. Exit" << std::endl;
        std::cout << "Enter choice: ";
    };
    do {
        displayMenu();
        choice = getValidatedInput<int>("", 1, 5);  // Makes sure choice is between 1 and 5.
        switch (choice) {
        case 1: {
            Semester sem;
            int n = getValidatedInput<int>("Enter number of courses: ", 1, 100);  // Capped at 100 to be reasonable.
            for (int i = 0; i < n; ++i) {
                double grade = getValidatedInput<double>("Enter numeric grade (0–10): ", 0.0, 10.0);
                double credit = getValidatedInput<double>("Enter credit hours (>0): ", 0.01, 100.0);  // Gotta be positive.
                sem.addCourse(grade, credit);
            }
            student.addSemester(std::move(sem));
            break;
        }
        case 2:
            student.displayAll();
            break;
        case 3:
            student.saveToFile();
            break;
        case 4:
            student.loadFromFile();
            break;
        case 5:
            std::cout << "Exiting program." << std::endl;
            break;
        }
    } while (choice != 5);
return 0;
}
// End of code
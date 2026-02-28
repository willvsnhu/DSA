// ProjectTwo.cpp - ABCU Advising Assistance Program
// William Vanderlinden
// Data structure choice: Hash table (std::unordered_map) for fast lookups.
//
// Requirements implemented:
//  - Prompt for filename
//  - Menu options: 1 Load, 2 Print sorted list, 3 Print course info, 9 Exit
//  - Two-pass load with prerequisite validation (skip bad lines; do not exit on line errors)
//  - Print sorted courses (alphanumeric by course number)
//  - Print course title + prerequisite course numbers and titles
//
// Notes:
//  - Single CPP file, no CSVParser headers used
//  - Uses basic CSV split on commas (no quoted-field handling)

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Course {
    std::string courseNumber;
    std::string title;
    std::vector<std::string> prerequisites;
};

// ---------- Utility helpers ----------

static inline std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) start++;
    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) end--;
    return s.substr(start, end - start);
}

// Uppercases a course number for consistent matching (e.g., "cs200" -> "CS200").
static inline std::string normalizeCourseNumber(const std::string& s) {
    std::string t = trim(s);
    for (char& ch : t) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    return t;
}

// Simple CSV split by comma (no quotes support).
static std::vector<std::string> splitByComma(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(line);
    while (std::getline(ss, token, ',')) {
        tokens.push_back(trim(token));
    }
    // Handle trailing comma -> add empty token if line ends with comma
    if (!line.empty() && line.back() == ',') {
        tokens.push_back("");
    }
    return tokens;
}

// ---------- Core program functions ----------

using CourseTable = std::unordered_map<std::string, Course>;

static CourseTable loadCoursesFromFile(const std::string& fileName) {
    CourseTable coursesTable;
    std::unordered_set<std::string> allCourseNumbers;

    // ----- Pass 1: collect valid course IDs -----
    {
        std::ifstream in(fileName);
        if (!in.is_open()) {
            std::cerr << "ERROR: Could not open file: " << fileName << "\n";
            return coursesTable;
        }

        std::string line;
        int lineNumber = 0;
        while (std::getline(in, line)) {
            lineNumber++;

            if (trim(line).empty()) {
                continue; // skip empty lines
            }

            auto tokens = splitByComma(line);

            // Must have at least courseNumber + title
            if (tokens.size() < 2) {
                std::cerr << "ERROR: Bad format on line " << lineNumber << " (skipping line)\n";
                continue;
            }

            std::string courseNum = normalizeCourseNumber(tokens[0]);
            std::string title = trim(tokens[1]);

            if (courseNum.empty() || title.empty()) {
                std::cerr << "ERROR: Missing courseNumber/title on line " << lineNumber
                    << " (skipping line)\n";
                continue;
            }

            if (allCourseNumbers.find(courseNum) != allCourseNumbers.end()) {
                std::cerr << "ERROR: Duplicate courseNumber '" << courseNum << "' on line "
                    << lineNumber << " (skipping line)\n";
                continue;
            }

            allCourseNumbers.insert(courseNum);
        }
    }

    // ----- Pass 2: validate prereqs and insert valid courses -----
    {
        std::ifstream in(fileName);
        if (!in.is_open()) {
            std::cerr << "ERROR: Could not open file: " << fileName << "\n";
            return coursesTable;
        }

        std::string line;
        int lineNumber = 0;
        while (std::getline(in, line)) {
            lineNumber++;

            if (trim(line).empty()) {
                continue;
            }

            auto tokens = splitByComma(line);

            if (tokens.size() < 2) {
                std::cerr << "ERROR: Bad format on line " << lineNumber << " (skipping line)\n";
                continue;
            }

            std::string courseNum = normalizeCourseNumber(tokens[0]);
            std::string title = trim(tokens[1]);

            if (courseNum.empty() || title.empty()) {
                std::cerr << "ERROR: Missing courseNumber/title on line " << lineNumber
                    << " (skipping line)\n";
                continue;
            }

            // Validate prerequisites (courses may have NONE)
            bool prereqIsValid = true;
            for (size_t i = 2; i < tokens.size(); i++) {
                std::string prereq = normalizeCourseNumber(tokens[i]);

                // Ignore blank prereq tokens (treat as no prerequisite)
                if (prereq.empty()) {
                    continue;
                }

                if (allCourseNumbers.find(prereq) == allCourseNumbers.end()) {
                    std::cerr << "ERROR: Line " << lineNumber << " invalid prerequisite '" << prereq
                        << "' for course '" << courseNum << "' (skipping course)\n";
                    prereqIsValid = false;
                    break;
                }
            }

            if (!prereqIsValid) {
                continue; // IMPORTANT: do not insert this course
            }

            // Build course object
            Course c;
            c.courseNumber = courseNum;
            c.title = title;
            c.prerequisites.clear();

            for (size_t i = 2; i < tokens.size(); i++) {
                std::string prereq = normalizeCourseNumber(tokens[i]);
                if (!prereq.empty()) {
                    c.prerequisites.push_back(prereq);
                }
            }

            // Insert into hash table (duplicate keys should not occur due to pass 1,
            // but we guard anyway by skipping if already present)
            if (coursesTable.find(c.courseNumber) == coursesTable.end()) {
                coursesTable[c.courseNumber] = c;
            }
        }
    }

    return coursesTable;
}

static void printCourseListSorted(const CourseTable& coursesTable) {
    if (coursesTable.empty()) {
        std::cout << "No course data loaded.\n";
        return;
    }

    std::vector<std::string> courseNums;
    courseNums.reserve(coursesTable.size());
    for (const auto& kv : coursesTable) {
        courseNums.push_back(kv.first);
    }

    std::sort(courseNums.begin(), courseNums.end()); // alphanumeric by course number

    for (const auto& num : courseNums) {
        const Course& c = coursesTable.at(num);
        std::cout << c.courseNumber << ", " << c.title << "\n";
    }
}

static void printCourseInfo(const CourseTable& coursesTable, const std::string& inputCourseNumber) {
    if (coursesTable.empty()) {
        std::cout << "No course data loaded.\n";
        return;
    }

    std::string courseNumber = normalizeCourseNumber(inputCourseNumber);
    auto it = coursesTable.find(courseNumber);
    if (it == coursesTable.end()) {
        std::cout << "Course not found: " << courseNumber << "\n";
        return;
    }

    const Course& course = it->second;
    std::cout << course.courseNumber << ", " << course.title << "\n";

    if (course.prerequisites.empty()) {
        std::cout << "Prerequisites: None\n";
        return;
    }

    std::cout << "Prerequisites:\n";
    for (const auto& prereqNum : course.prerequisites) {
        auto pit = coursesTable.find(prereqNum);
        if (pit != coursesTable.end()) {
            std::cout << "  " << pit->second.courseNumber << ", " << pit->second.title << "\n";
        }
        else {
            // Should not happen due to validation, but keep safe fallback
            std::cout << "  " << prereqNum << " (missing info)\n";
        }
    }
}

static void printMenu() {
    std::cout << "\nMenu:\n";
    std::cout << "  1. Load Data Structure\n";
    std::cout << "  2. Print Course List\n";
    std::cout << "  3. Print Course\n";
    std::cout << "  9. Exit\n";
    std::cout << "Enter your choice: ";
}

static bool readIntChoice(int& outChoice) {
    std::string line;
    if (!std::getline(std::cin, line)) return false;
    line = trim(line);
    if (line.empty()) return false;

    // Basic numeric parse
    try {
        size_t idx = 0;
        int val = std::stoi(line, &idx);
        if (idx != line.size()) return false;
        outChoice = val;
        return true;
    }
    catch (...) {
        return false;
    }
}

int main() {
    std::cout << "Welcome to ABCU Advising Program\n";

    CourseTable coursesTable;
    bool dataLoaded = false;

    // Prompt user for the filename up front (required by the prompt).
    std::string fileName;
    std::cout << "Enter the course data file name: ";
    std::getline(std::cin, fileName);
    fileName = trim(fileName);

    int choice = 0;
    while (true) {
        printMenu();
        if (!readIntChoice(choice)) {
            std::cout << "Invalid input. Please enter 1, 2, 3, or 9.\n";
            continue;
        }

        if (choice == 1) {
            if (fileName.empty()) {
                std::cout << "Enter the course data file name: ";
                std::getline(std::cin, fileName);
                fileName = trim(fileName);
            }

            coursesTable = loadCoursesFromFile(fileName);
            if (!coursesTable.empty()) {
                dataLoaded = true;
                std::cout << "Data loaded successfully (" << coursesTable.size() << " courses).\n";
            }
            else {
                dataLoaded = false;
                std::cout << "No courses loaded. Check errors above and try again.\n";
            }

        }
        else if (choice == 2) {
            if (!dataLoaded) {
                std::cout << "Please load data first (Option 1).\n";
                continue;
            }
            printCourseListSorted(coursesTable);

        }
        else if (choice == 3) {
            if (!dataLoaded) {
                std::cout << "Please load data first (Option 1).\n";
                continue;
            }
            std::string courseNumber;
            std::cout << "Enter a course number (e.g., CS200): ";
            std::getline(std::cin, courseNumber);
            printCourseInfo(coursesTable, courseNumber);

        }
        else if (choice == 9) {
            std::cout << "Goodbye.\n";
            break;

        }
        else {
            std::cout << "Invalid option. Please enter 1, 2, 3, or 9.\n";
        }
    }

    return 0;
}
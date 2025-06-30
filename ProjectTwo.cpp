//============================================================================
// Name        : ProjectTwo.cpp
// Author      : Keir Charlton-Molloy
// Version     : 1.0  
// Description : Project Two - Course Planner (Binary Search Tree)
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <limits>
#include <cctype>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// Forward declarations of helper functions and major methods
string toUpper(string str);
bool validateCourseFile(const string& fileName);
void loadCourses(const string& fileName, class BinarySearchTree* bst);

// Structure to hold a single course and its prerequisites
struct Course {
    string courseNumber;          // Unique identifier (uppercase)
    string courseTitle;           // Full course title
    vector<string> prereqNumbers; // List of prerequisite course numbers (uppercase)

    // Default constructor
    Course() {}
};

// Internal node structure for the Binary Search Tree
struct Node {
    Course course; // Course data for this node
    Node* left;    // Pointer to left child
    Node* right;   // Pointer to right child

    // Default constructor
    Node() {
        left = nullptr;
        right = nullptr;
    }

    // Initialize node with a course
    Node(Course aCourse) : Node() {
        course = aCourse;
    }
};

//============================================================================
// Binary Search Tree class definition
//============================================================================
class BinarySearchTree {

private:
    Node* root; // Root node of the Binary Search Tree

    // Recursively add a course to the tree
    void addNode(Node* node, Course course);

    // Recursively print all courses in order
    void inOrder(Node* node);

    // Helper to delete all nodes and free memory
    void destroyTree(Node* node);

public:
    BinarySearchTree();                        // Default constructor
    virtual ~BinarySearchTree();               // Destructor
    void Insert(Course course);                // Insert a course into the Binary Search Tree
    Course Search(const string& courseNumber); // Find a course by its number
    void InOrder();                            // Print all courses in order
    void Clear();                              // Remove all courses from the tree
};

/**
 * Default constructor
 * Initializes root to nullptr
 */
BinarySearchTree::BinarySearchTree() {
    root = nullptr;
}

/**
 * Destructor
 * Destroys the entire tree to free memory
 */
BinarySearchTree::~BinarySearchTree() {
    destroyTree(root);
}

/**
 * Recursively deletes all nodes in the tree
 *
 * @param node The current node to delete
 */
void BinarySearchTree::destroyTree(Node* node) {
    if (node != nullptr) {
        // Destroy left subtree
        destroyTree(node->left);

        // Destroy right subtree
        destroyTree(node->right);

        // Delete current node
        delete node;
    }
}

/**
 * Insert a course into the tree
 */
void BinarySearchTree::Insert(Course course) {
    // If root is empty the new node becomes the root
    if (root == nullptr) {
        root = new Node(course);
    }
    else {
        // Otherwise call recursive helper
        addNode(root, course);
    }
}

/**
 * Recursively adds a new course node to the Binary Search Tree
 *
 * @param node   The current node in the tree
 * @param course The course to insert
 */
void BinarySearchTree::addNode(Node* node, Course course) {
    // If the course number is less than the current node's, go left
    if (course.courseNumber < node->course.courseNumber) {
        // If the left child is empty, insert here
        if (node->left == nullptr) {
            node->left = new Node(course);
        }
        else {
            // Otherwise, recurse down the left subtree
            addNode(node->left, course);
        }
    }
    else {
        // If the right child is empty, insert here
        if (node->right == nullptr) {
            node->right = new Node(course);
        }
        else {
            // Otherwise, recurse down the right subtree
            addNode(node->right, course);
        }
    }
}

/**
 * Searches for a course by its number (case-insensitive)
 *
 * @param courseNumber The course number to search for
 * @return The matching Course if found, or an empty Course if not found
 */
Course BinarySearchTree::Search(const string& courseNumber) {
    // Normalize search key to uppercase
    string key = toUpper(courseNumber);

    // Start at the root node
    Node* currentNode = root;

    // Traverse the tree to search for the course
    while (currentNode != nullptr) {
        // If a match is found, return the current course
        if (currentNode->course.courseNumber == key) {
            return currentNode->course;
        }
        // If search key is less, go left
        else if (key < currentNode->course.courseNumber) {
            currentNode = currentNode->left;
        }
        // Otherwise, go right
        else {
            currentNode = currentNode->right;
        }
    }

    // If not found, return an empty course object
    return Course();
}

/**
 * Traverses the Binary Search Tree and prints all courses in alphanumeric order
 */
void BinarySearchTree::InOrder() {
    // Start in-order traversal from the root node
    inOrder(root);
}

/**
 * Recursively traverses the tree in order (left, root, right)
 *
 * @param node The current node in the traversal
 */
void BinarySearchTree::inOrder(Node* node) {
    if (node != nullptr) {
        // Traverse left subtree
        inOrder(node->left);

        // Output the course number and course title
        cout << node->course.courseNumber << ", " << node->course.courseTitle << endl;

        // Traverse right subtree
        inOrder(node->right);
    }
}

/**
 * Removes all courses from the tree and frees memory
 */
void BinarySearchTree::Clear() {
    // Delete all nodes starting from the root
    destroyTree(root);
    // Reset the root pointer to indicate the tree is empty
    root = nullptr;
}

//============================================================================
// Utility functions
//============================================================================

/**
 * Converts an entire string to uppercase
 *
 * @param str The string to convert
 * @return The uppercase version of the input string
 */
string toUpper(string str) {
    // Convert each character to uppercase
    for (size_t i = 0; i < str.size(); i++) {
        str[i] = toupper(str[i]);
    }
    // Return the modified string
    return str;
}

/**
 * Splits a CSV line by commas into a vector of strings
 *
 * @param line The input line to split
 * @return Vector containing each CSV token
 */
static vector<string> splitCSV(const string& line) {
    // Create vector to hold tokens
    vector<string> tokens;
    string token;
    stringstream ss(line);

    // Extract tokens separated by commas
    while (getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    // Return vector of tokens
    return tokens;
}

/**
 * Validate course CSV file contents using two passes
 * Checks for correct file format, missing data, and invalid prerequisites
 *
 * @param fileName Path to CSV file
 * @return true if file contents are valid, false otherwise
 */
bool validateCourseFile(const string& fileName) {
    // Attempt to open the file for reading
    ifstream inFile(fileName);

    // If the file cannot be opened, display an error message and return false
    if (!inFile) {
        cout << "Unable to open file." << endl;
        return false;
    }

    // Create a set to store all course numbers found in the file
    unordered_set<string> allCourseNumbers;
    string line;
    int lineNum = 0; // Tracks the line number for error reporting

    // First pass: Read each line, collect course numbers, and check for basic format errors
    while (getline(inFile, line)) {
        lineNum++;
        // Use a helper function to split the line into CSV tokens
        vector<string> tokens = splitCSV(line);

        // Check if the line contains at least a course number and course title
        if (tokens.size() < 2) {
            cout << "Error: Insufficient data on line " << lineNum << endl;
            return false;
        }

        // Add the course number (converted to uppercase) to the set
        allCourseNumbers.insert(toUpper(tokens[0]));
    }

    // Reset the file pointer to the beginning for the second pass
    inFile.clear();
    inFile.seekg(0);
    lineNum = 0;

    // Second pass: Validate that every prerequisite listed exists as a course number
    while (getline(inFile, line)) {
        lineNum++;
        vector<string> tokens = splitCSV(line);

        // Start checking prerequisites from the third token onward (index 2)
        for (size_t i = 2; i < tokens.size(); i++) {
            string prereq = toUpper(tokens[i]);

            // Only validate non-empty prerequisites
            if (!prereq.empty() && !allCourseNumbers.count(prereq)) {
                cout << "Error: Prerequisite " << tokens[i]
                    << " not found for course " << tokens[0]
                    << " on line " << lineNum << endl;
                return false;
            }
        }
    }

    // If all checks passed, file is valid
    return true;
}

/**
 * Loads courses from a CSV file and inserts them into the given Binary Search Tree
 *
 * @param fileName Name of the CSV file containing course data
 * @param bst Pointer to the BinarySearchTree where courses will be loaded
 */
void loadCourses(const string& fileName, BinarySearchTree* bst) {
    // Attempt to open the file
    ifstream input(fileName);
    if (!input) {
        cout << "Unable to open file." << endl;
        return;
    }

    // Clear any existing data from the Binary Search Tree to start fresh
    bst->Clear();

    string rawLine;

    // Read each line from the file
    while (getline(input, rawLine)) {
        // Skip empty lines
        if (rawLine.empty()) continue;

        // Split the line into CSV columns
        vector<string> columns = splitCSV(rawLine);

        // Create a new Course object
        Course newCourse;
        newCourse.courseNumber = toUpper(columns[0]); // Store course number in uppercase
        newCourse.courseTitle = columns[1];           // Store course title

        // Read and store all prerequisites (if any)
        // (Columns 0 and 1 are course number and title; prerequisites start at column 2)
        for (size_t col = 2; col < columns.size(); col++) {
            string prereq = toUpper(columns[col]);
            if (!prereq.empty()) {
                newCourse.prereqNumbers.push_back(prereq);
            }
        }

        // Insert the course into the Binary Search Tree
        bst->Insert(newCourse);
    }
}

//============================================================================
// Main program: Course Planner
//============================================================================
int main() {

    // Create a BinarySearchTree to store all courses
    BinarySearchTree* courseTree = new BinarySearchTree();
    bool dataLoaded = false;         // Tracks if data has been loaded
    int userChoice = 0;              // User's menu selection
    string fileName;                 // Name of the CSV file to load

    cout << "Welcome to the course planner." << endl;

    // Main program loop
    while (userChoice != 9) {
        // Display menu options to the user
        cout << "\n    1. Load Data Structure." << endl;
        cout << "    2. Print Course List." << endl;
        cout << "    3. Print Course." << endl;
        cout << "    9. Exit\n" << endl;
        cout << "What would you like to do? ";

        // Prevent invalid integer input
        if (!(cin >> userChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Input is not a valid option." << endl;
            userChoice = 0;   // Reset choice to continue loop
            continue;
        }
        cin.ignore(); // Remove trailing newline from input buffer

        // Handle the user's menu selection
        switch (userChoice) {

        case 1:
            // Load courses from CSV file
            // Prompt user for the CSV file name
            cout << "Enter the name of the data file to load: ";
            getline(cin, fileName);

            // Validate the file before loading
            if (validateCourseFile(fileName)) {
                // If valid, load the courses into the Binary Search Tree
                loadCourses(fileName, courseTree);
                dataLoaded = true;
                cout << "Courses loaded successfully." << endl;
            }
            else {
                // Inform user if file validation fails
                cout << "File validation failed." << endl;
            }
            break;

        case 2:
            // Print the entire course list in alphanumeric order
            // Check if course data has been loaded
            if (!dataLoaded) {
                cout << "Courses not loaded yet." << endl;
            }
            else {
                // Print all courses in alphanumeric order
                cout << "Here is a sample schedule:\n" << endl;
                courseTree->InOrder();
            }
            break;

        case 3:
            // Look up and print details for a single course
            // Check if course data has been loaded
            if (!dataLoaded) {
                cout << "Courses not loaded yet." << endl;
            }
            else {
                // Prompt user for the course number to look up
                cout << "What course do you want to know about? ";

                string lookupCode;
                getline(cin, lookupCode);

                // Search for the course in the Binary Search Tree
                Course lookedUp = courseTree->Search(lookupCode);

                // If course is not found, inform the user
                if (lookedUp.courseNumber.empty()) {
                    cout << "Course " << lookupCode << " not found." << endl;
                }
                else {
                    // Print the course number and title
                    cout << lookedUp.courseNumber << ", "
                        << lookedUp.courseTitle << endl;

                    // Print prerequisites, if any
                    if (lookedUp.prereqNumbers.empty()) {
                        cout << "Prerequisites: None" << endl;
                    }
                    else {
                        cout << "Prerequisites: ";
                        // Print each prerequisite, separated by commas
                        for (size_t idx = 0; idx < lookedUp.prereqNumbers.size(); idx++) {
                            cout << lookedUp.prereqNumbers[idx];

                            if (idx < lookedUp.prereqNumbers.size() - 1) {
                                cout << ", ";
                            }
                        }
                        cout << endl;
                    }
                }
            }
            break;

        case 9:
            // Print exit message
            cout << "Thank you for using the course planner!" << endl;
            break;

        default:
            // Inform user of invalid menu selection
            cout << userChoice << " is not a valid option." << endl;
        }
    }

    // Clean up heap memory before exiting the program
    delete courseTree;
    return 0;
}
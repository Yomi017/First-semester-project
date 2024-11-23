CREATE DATABASE db_university;
USE DATABASE db_university;
CREATE TABLE student (
    ID INTEGER,
    Name TEXT,
    GPA FLOAT
);
INSERT INTO student VALUES (2, 'Ahmed', 3.0);
INSERT INTO student VALUES (3, 'Sara', 3.8);
INSERT INTO student VALUES (4, 'Nora', 3.2);
UPDATE student SET GPA = 1, Name = 'Ali' WHERE ID = 2;
CREATE TABLE course (
    CourseID INTEGER,
    CourseName TEXT,
    Credits INTEGER
);
INSERT INTO course VALUES (101, 'Mathematics', 3);
INSERT INTO course VALUES (102, 'Physics', 4);
INSERT INTO course VALUES (103, 'Chemistry', 3);
CREATE TABLE enrollment (
    StudentID INTEGER,
    CourseID INTEGER,
    Semester TEXT
);
INSERT INTO enrollment VALUES (2, 101, 'Fall 2023');
INSERT INTO enrollment VALUES (3, 102, 'Fall 2023');
INSERT INTO enrollment VALUES (4, 103, 'Fall 2023');

SELECT ID FROM student WHERE GPA > 3.0 AND ID < 4;
SELECT course.CourseID, enrollment.Semester FROM course INNER JOIN enrollment ON course.CourseID = enrollment.CourseID;
UPDATE enrollment SET Semester = 'Spring 2024', CourseID = 2 * (CourseID + 1) WHERE StudentID < 4;
UPDATE enrollment SET Semester = 'Spring 2023', CourseID = 2 * (CourseID + 1) WHERE StudentID = 3;
DELETE FROM enrollment WHERE StudentID = 4;
SELECT * FROM enrollment;

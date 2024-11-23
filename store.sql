CREATE DATABASE db_university;
USE DATABASE db_university;
CREATE TABLE enrollment (
  StudentID INTEGER,
  CourseID INTEGER,
  Semester TEXT
);
INSERT INTO enrollment VALUES (2,204,'Spring 2024');
INSERT INTO enrollment VALUES (3,414,'Spring 2023');
CREATE TABLE course (
  CourseID INTEGER,
  CourseName TEXT,
  Credits INTEGER
);
INSERT INTO course VALUES (101,'Mathematics',3);
INSERT INTO course VALUES (102,'Physics',4);
INSERT INTO course VALUES (103,'Chemistry',3);
CREATE TABLE student (
  ID INTEGER,
  Name TEXT,
  GPA FLOAT
);
INSERT INTO student VALUES (2,'Ali',1);
INSERT INTO student VALUES (3,'Sara',3.8);
INSERT INTO student VALUES (4,'Nora',3.2);

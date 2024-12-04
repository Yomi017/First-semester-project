CREATE DATABASE test_university;
USE DATABASE test_university;
CREATE TABLE student (
  ID INTEGER,
  Name TEXT,
  GPA FLOAT,
  Major TEXT
);
INSERT INTO student VALUES (1,'Alice Johnson',4.0,'Computer Science');
INSERT INTO student VALUES (2,'Bob Smith',4.0,'Electrical Engineering');
INSERT INTO student VALUES (3,'Catherine Lee',4.0,'Mathematics');
INSERT INTO student VALUES (5,'Eva White',4.0,'Chemistry');
CREATE TABLE course (
  CourseID INTEGER,
  CourseName TEXT,
  Department TEXT
);
INSERT INTO course VALUES (101,'Introduction to Computer Science','Computer Science');
INSERT INTO course VALUES (102,'Circuit Analysis','Electrical Engineering');
INSERT INTO course VALUES (103,'Linear Algebra','Mathematics');
INSERT INTO course VALUES (105,'Organic Chemistry','Chemistry');
CREATE TABLE enrollment (
  StudentID INTEGER,
  CourseID INTEGER
);
INSERT INTO enrollment VALUES (1,101);
INSERT INTO enrollment VALUES (1,103);
INSERT INTO enrollment VALUES (2,102);
INSERT INTO enrollment VALUES (3,103);
INSERT INTO enrollment VALUES (4,104);
INSERT INTO enrollment VALUES (5,105);

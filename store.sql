CREATE DATABASE test_university;
USE DATABASE test_university;
CREATE TABLE achievement (
  GPA FLOAT,
  Achievement TEXT
);
INSERT INTO achievement VALUES (5.0,'Dean List');
INSERT INTO achievement VALUES (11.0,'Dean List');
INSERT INTO achievement VALUES (15.0,'Dean List');
CREATE TABLE student (
  ID INTEGER,
  Name TEXT,
  GPA FLOAT,
  Major TEXT
);
INSERT INTO student VALUES (1,'Alice Johnson',5.0,'Computer Science');
INSERT INTO student VALUES (2,'Bob Smith',11.0,'Electrical Engineering');
INSERT INTO student VALUES (3,'Catherine Lee',11.0,'Mathematics');
INSERT INTO student VALUES (6,'Eva White',15.0,'Chemistry');
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

CREATE DATABASE university_db;
USE DATABASE university_db;
CREATE TABLE course_enrollment (
  StudentID INTEGER,
  Course TEXT
);
INSERT INTO course_enrollment VALUES (1001,'Data Science');
INSERT INTO course_enrollment VALUES (1001,'Machine Learning');
INSERT INTO course_enrollment VALUES (1002,'Financial Technology');
INSERT INTO course_enrollment VALUES (1002,'Advanced Algorithms');

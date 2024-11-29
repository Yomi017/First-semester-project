CREATE DATABASE db_university;
USE DATABASE db_university;
CREATE TABLE student (
  ID INTEGER,
  Name TEXT,
  GPA FLOAT
);
INSERT INTO student VALUES (2,'Ali',1);
INSERT INTO student VALUES (3,'Sara',3.8);
INSERT INTO student VALUES (4,'Nora',3.2);
SELECT ID FROM student WHERE GPA != 1;
DELETE FROM student WHERE GPA != 1;
SELECT * FROM student;
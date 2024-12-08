SELECT student.Name, achievement.Achievement, course.CourseName, student.GPA
FROM student
INNER JOIN enrollment
ON student.ID = enrollment.StudentID
INNER JOIN course
ON enrollment.CourseID = course.CourseID
INNER JOIN achievement
ON student.GPA = achievement.GPA;
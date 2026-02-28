# DSA

**What was the problem you were solving in the projects for this course?**

The main problem I was solving was how to design and implement an advising assistance program that could efficiently store, organize, and retrieve course information for academic advisors. The system needed to read course data from a file, validate prerequisites, and allow users to print a sorted list of courses or look up detailed information for a specific course. The core challenge was choosing and implementing the appropriate data structure to support these requirements effectively.

**How did you approach the problem? Consider why data structures are important to understand.**

I approached the problem by first designing pseudocode for multiple data structures (vector, hash table, and binary search tree) and analyzing their runtime and memory tradeoffs. Understanding data structures was critical because they directly affect performance, especially for searching and sorting operations. Instead of just focusing on making the program work, I evaluated how each structure would scale and how it would impact user operations like course lookups and sorted printing.

**How did you overcome any roadblocks you encountered while going through the activities or project?**

One of the biggest roadblocks was correctly handling prerequisite validation and ensuring invalid data did not corrupt the structure. I overcame this by breaking the problem into smaller steps, using a two-pass file-reading strategy, and carefully tracing through edge cases like duplicates or missing prerequisites. Reviewing feedback and iterating on pseudocode before coding also helped reduce errors later.

**How has your work on this project expanded your approach to designing software and developing programs?**

This project expanded my approach by reinforcing the importance of planning before coding. Writing detailed pseudocode and performing Big O analysis forced me to think about structure, efficiency, and long-term maintainability rather than jumping straight into implementation.

**How has your work on this project evolved the way you write programs that are maintainable, readable, and adaptable?**

It has made me more intentional about modular design, clear function separation, consistent naming conventions, and input validation. I now focus more on writing code that is easy to test, extend, and debug, which makes programs more adaptable to future changes or requirements.

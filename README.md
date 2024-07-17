# pipesExample
Code to demonstrate the use of fork and pipes.
My code was created using a linux VM and is coded in C. I manually created directories d0, d1, and d2.
With parent copies of files f1, f2, f3, f4 residing in directory d0 also being manually added and present for the code to operate correctly. This code forks to create 2 child processes and uses pipes to share data between the processes. The parent process sends the name of the files and data to the children, and the children create their own copies of the data within their own folder. Child 1 uses directory d1, and child 2 uses directory d2. Child 1 receives and copies files f1.txt and f2.txt. Child process 2 receives and copies data from f3.txt and f4.txt. The parent waits for the children to terminate then the parent terminates.

run command: gcc -o code2 code2.cpp
Run this command in the parent directory with d0 d1 and d2 present in that same directory.
This will compile the code 

run command: code2 
This will run the code source file.

README

This zip file contains the files and programs for executing a process schedulers given a list of processes in csv format as a text file:
question1.cc
question1
question2.cc
question2
jobs.txt

The programs are split into 2 c++ files:

question1.cc - program for question 1, Multiqueue Adaptive Scheduling (MAS)
question2.cc - program for question 2, Shortest-Remaining-Time-First (SRTF) is question2.cc

both question1.cc (MAS) and question2.cc (SRTF) print the following:
1.) Termination and waiting times of all processes
2.) Average termination time and average waiting time
3.) Order in which processes were allowed to execute on the CPU

question1 - executable compiled from question1.cc
question2 - executable compiled from question2.cc

To compile question1.cc and create executable question1:
g++ -std=c++11-Wall -Werror -o question1 question1.cc

To compile question2.cc and create executable question2:
g++ -std=c++11-Wall -Werror -o question2 question2.cc

To run the executable, specify a single file path as an argument. File must be .txt and must be csv values with 3 columns: process id, burst time, and arrival time - in that order. File must also contain at least a single entry.

If invalid file is given, an error is printed, along with usage details and an example command of the executable

For example, jobs.txt is in the same directory as question1, so use the following command to see MAS scheduler:
./question1 jobs.txt

For example, jobs.txt is in the same directory as question2, so use the following command to see SRTF scheduler:
./question1 jobs.txt

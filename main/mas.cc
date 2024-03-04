#include <iostream>
#include <fstream>
#include <queue>
#include <cstdlib>
#include <sstream>
#include <list>
#include <map>

using namespace std;

// creates a Process, with the relevant information about the process
struct Process
{
  int processID;
  int burstTime;
  int burstTimeLeft;
  int arrivalTime;
  int terminationTime;
  int turnaroundTime;
  int waitingTime;
  int queueIn;
};

// compares 2 processes based on arrival time first, then it's burst time, then ID
class cmpProcessArrivalTime
{
public:
  int operator()(const Process &p1, const Process &p2)
  {
    if (p1.arrivalTime == p2.arrivalTime)
    {
      return p1.processID > p2.processID;
    }
    else
    {
      return p1.arrivalTime > p2.arrivalTime;
    }
  }
};

// compares 2 processes based on id
class cmpProcessID
{
public:
  int operator()(const Process &p1, const Process &p2)
  {
    return p1.processID > p2.processID;
  }
};

// prints an error message to console
void errorMessage()
{
  cout << "USAGE: Executable file must take in 1 valid file path" << endl;
  cout << "EXAMPLE: ./question1 <file.txt>" << endl;
}

// prints the order of which processes were being executed on, along with its termination time
void printOrder(queue<int> &orderOfProcesses, map<int, int> IDFinishTime)
{
  cout << "---ORDER OF PROCESS---" << endl;
  cout << orderOfProcesses.front() << "(" << IDFinishTime[orderOfProcesses.front()] << ")";
  orderOfProcesses.pop();
  while (!orderOfProcesses.empty())
  {
    cout << "," << orderOfProcesses.front() << "(" << IDFinishTime[orderOfProcesses.front()] << ")";
    orderOfProcesses.pop();
  }
  cout << endl
       << endl;
}

// prints the process id's turnaround and waiting times, in order of id
void updateTimes(vector<Process> &allProcessID, map<int, int> IDFinishTime)
{
  cout << endl;
  cout << setw(4) << "ID" << setw(15) << "Arrival Time" << setw(13) << "Burst Time" << setw(15) << "Finished Time" << setw(18) << "Turnaround Time" << setw(15) << "Waiting Time" << endl;
  int i;
  for (i = 0; i < allProcessID.size(); i++)
  {
    allProcessID[i].terminationTime = IDFinishTime[allProcessID[i].processID];
    allProcessID[i].turnaroundTime = allProcessID[i].terminationTime - allProcessID[i].arrivalTime;
    allProcessID[i].waitingTime = allProcessID[i].turnaroundTime - allProcessID[i].burstTime;
    cout << setw(4) << allProcessID[i].processID << setw(11) << allProcessID[i].arrivalTime << setw(12) << allProcessID[i].burstTime << setw(13) << allProcessID[i].terminationTime << setw(16) << allProcessID[i].turnaroundTime << setw(17) << allProcessID[i].waitingTime << endl;
  }
  cout << endl;
}

// prints the average turnaround and waiting time of all processes
void printAverageTimes(vector<Process> allProcessID)
{
  int i;
  int totalWaitingTime = 0;
  int totalTurnaroundTime = 0;
  for (i = 0; i < allProcessID.size(); i++)
  {
    totalWaitingTime += allProcessID[i].waitingTime;
    totalTurnaroundTime += allProcessID[i].turnaroundTime;
  }
  float avgWaitingTime = (float)totalWaitingTime / (float)i;
  float avgTurnaroundTime = (float)totalTurnaroundTime / (float)i;

  cout << "Average turnaround time for all processes: " << avgTurnaroundTime << endl;
  cout << "Average waiting time for all processes:    " << avgWaitingTime << endl;
  cout << endl;
}

int main(int argc, char **argv)
{
  // throws error if not given exactly 1 argument
  if (argc != 2)
  {
    cout << "ERROR: Invalid number of arguments given" << endl;
    errorMessage();
    exit(EXIT_FAILURE);
  }

  priority_queue<Process, vector<Process>, cmpProcessArrivalTime> arrivalQueue; // order of arrival time, breaking ties with ID
  priority_queue<Process, vector<Process>, cmpProcessID> processIDqueue;        // order of ID

  // checks if given argument is a file
  ifstream schedulerFile;
  schedulerFile.open(argv[1]);

  // throws error if argument given is not a valid file
  if (!schedulerFile.is_open())
  {
    cout << "ERROR: Invalid file argument given" << endl;
    errorMessage();
    exit(EXIT_FAILURE);
  }

  // holds current line of given txt file
  string line = "";

  // reads line of text file, creates a Process from the information in the line, and added to 2 priority queues
  // 1 queue holds process in order of ID, other queue holds process in order of arrival time
  while (getline(schedulerFile, line))
  {
    int processID;
    int burstTime;
    int arrivalTime;
    string placeholder;

    stringstream currentLine(line);

    getline(currentLine, placeholder, ',');
    processID = atoi(placeholder.c_str());

    getline(currentLine, placeholder, ',');
    burstTime = atoi(placeholder.c_str());

    getline(currentLine, placeholder, ',');
    arrivalTime = atoi(placeholder.c_str());

    Process p;
    p.processID = processID;
    p.arrivalTime = arrivalTime;
    p.burstTime = burstTime;
    p.burstTimeLeft = burstTime;
    arrivalQueue.push(p);
    processIDqueue.push(p);
  }

  schedulerFile.close(); // finished reading file, close file

  vector<Process> allProcessArrivalTime; // order of arrival time, for scheduling purposes
  vector<Process> allProcessID;          // order of ID, to iterate for termination, turnaround and waiting time

  // transfers the priority queues to a list, for iterative purposes when scheduling processes
  // maintains order held in priority queues
  while (!arrivalQueue.empty())
  {
    allProcessArrivalTime.push_back(arrivalQueue.top()); // add to all processes
    arrivalQueue.pop();                                  // remove process 1 by 1 from priority queue
  }

  while (!processIDqueue.empty())
  {
    allProcessID.push_back(processIDqueue.top());
    processIDqueue.pop();
  }

  // multiadaptive queues
  queue<Process> q0; // highest priority for processing, will execute process until complete or for 6 seconds
  queue<Process> q1; // 2nd highest priority for processing, will execute until process complete, preempted or for 12 seconds
  queue<Process> q2; // lowest priority for processing, will execute until process is complete or preempted

  queue<int> orderOfProcesses; // order of when process allowed to be executed
  map<int, int> IDFinishTime;  // maps id to termination time

  Process *current = NULL; // pointer to current process executed
  Process *prev = NULL;    // pointer to previous process

  int counter = -1; // counter to simulate time in scheduling process
  int timer = -1;   // timer to keep track of how long a process is allowed to execute

  // starts scheduling and processing each process, using counter as a timer
  while (!allProcessArrivalTime.empty() || !q0.empty() || !q1.empty() || !q2.empty())
  {
    counter += 1;
    timer -= 1;

    // get arriving process to q0
    while (!allProcessArrivalTime.empty() && allProcessArrivalTime.front().arrivalTime == counter)
    {
      q0.push(allProcessArrivalTime.front());
      allProcessArrivalTime.erase(allProcessArrivalTime.begin());
      if (current == NULL)
      {
        current = &q0.front();
      }
    }
    if (current == NULL) // current is null, means nothing in queues to process
    {
      continue;
    }

    counter == current->arrivalTime ? current->burstTimeLeft -= 0 : current->burstTimeLeft -= 1; // execute current process

    // check if current process is complete
    if (current->burstTimeLeft == 0)
    {
      if (current->queueIn == 0)
      {
        q0.pop();
      }
      else if (current->queueIn == 1)
      {
        q1.pop();
      }
      else
      {
        q2.pop();
      }
      current->terminationTime = counter;
      IDFinishTime[current->processID] = counter;
      current = NULL;
    }
    else if (timer == 0) // else check if process timer window is finished
    {
      if (current->queueIn == 0) // move process from q0 to q1
      {
        q0.pop();
        q1.push(*current);
        current->queueIn = 1;
      }
      else // move process from q1 to q2
      {
        q1.pop();
        q2.push(*current);
        current->queueIn = 2;
      }
    }

    if (!q0.empty()) // get current process from front of q0
    {
      current = &q0.front();
      current->queueIn = 0;
    }
    else if (!q1.empty()) // if q0 empty, get current process from front of q1
    {
      current = &q1.front();
      current->queueIn = 1;
    }
    else if (!q2.empty()) // if q0 and q1 empty, get current process from front of q2
    {
      current = &q2.front();
      current->queueIn = 2;
    }

    if (current == NULL) // check if there is a current process executing. If not, go to next iteration
    {
      continue;
    }

    if (current != prev) // check if not-null current process is different previous process
    {
      // add current process to order of execution if its not the same as previous process
      if (orderOfProcesses.empty() || (!orderOfProcesses.empty() && orderOfProcesses.back() != current->processID))
      {
        orderOfProcesses.push(current->processID);
      }

      // checks if previous process was preempted                                                                                             // current process is different, place in order of execution
      if (prev != NULL && prev->processID != current->processID && prev->burstTimeLeft > 0 && timer != 0) // previous process was preempted
      {
        if (prev->queueIn == 0)
        {
          q0.pop();
          q0.push(*prev); // remove and push preempted process back to its queue
        }
        else if (prev->queueIn == 1)
        {
          q1.pop();
          q1.push(*prev); // push preempted process back to its queue
        }
        else
        {
          q2.pop();
          q2.push(*prev); // push preempted process back to its queue
        }
      }
    }

    if ((current != prev || timer == 0) && current->queueIn == 0) // reset timer if process changed, or timer is 0
    {
      timer = 6;
    }
    else if ((current != prev || timer == 0) && current->queueIn == 1) // reset timer if process changed, or timer is 0
    {
      timer = 12;
    }
    else if (((current != prev || timer == 0) && current->queueIn == 2))
    {
      timer = -1; // reset timer, -1 indicates timer not being tracked for this process
    }
    prev = current;
  }

  // print out all processes executed and relevant information about the scheduling performed
  updateTimes(allProcessID, IDFinishTime);
  printAverageTimes(allProcessID);
  printOrder(orderOfProcesses, IDFinishTime);
  allProcessID.clear();
  IDFinishTime.clear();

  return 0;
}

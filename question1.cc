#include <iostream>
#include <fstream>
#include <queue>
#include <cstdlib>
#include <sstream>
#include <list>
#include <map>

using namespace std;

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

class cmpProcessID
{
public:
  int operator()(const Process &p1, const Process &p2)
  {
    return p1.processID > p2.processID;
  }
};

void errorMessage()
{
  cout << "USAGE: Executable file must take in 1 valid file path" << endl;
  cout << "EXAMPLE: ./question1.out <file.txt>" << endl;
}

void printOrder(queue<int> orderOfProcesses, map<int, int> IDFinishTime)
{
  cout << "---ORDER OF PROCESS---" << endl;
  cout << orderOfProcesses.front() << "(" << IDFinishTime[orderOfProcesses.front()] << ")";
  orderOfProcesses.pop();
  while (!orderOfProcesses.empty())
  {
    cout << "," << orderOfProcesses.front() << "(" << IDFinishTime[orderOfProcesses.front()] << ")";
    orderOfProcesses.pop();
  }
  cout << endl;
}

void updateTimes(vector<Process> &allProcessID, map<int, int> IDFinishTime)
{
  cout << endl;
  cout << setw(4) << "ID" << setw(18) << "Turnaround time" << setw(15) << "Waiting time" << endl;
  int i;
  for (i = 0; i < allProcessID.size(); i++)
  {
    allProcessID[i].terminationTime = IDFinishTime[allProcessID[i].processID];
    allProcessID[i].turnaroundTime = allProcessID[i].terminationTime - allProcessID[i].arrivalTime;
    allProcessID[i].waitingTime = allProcessID[i].turnaroundTime - allProcessID[i].burstTime;
    cout << setw(4) << allProcessID[i].processID << setw(12) << allProcessID[i].terminationTime << setw(18) << allProcessID[i].waitingTime << endl;
  }
  cout << endl;
}

int main(int argc, char **argv)
{
  // Throws error if not given exactly 1 argument
  if (argc != 2)
  {
    cout << "ERROR: Invalid number of arguments given" << endl;
    errorMessage();
    exit(EXIT_FAILURE);
  }

  // Priority queue to have processes in order of arrival time
  priority_queue<Process, vector<Process>, cmpProcessArrivalTime> arrivalQueue;
  priority_queue<Process, vector<Process>, cmpProcessID> processIDqueue;

  ifstream schedulerFile;
  schedulerFile.open(argv[1]);

  // Throws error if argument given is not a valid file
  if (!schedulerFile.is_open())
  {
    cout << "ERROR: Invalid file argument given" << endl;
    errorMessage();
    exit(EXIT_FAILURE);
  }

  string line = "";

  // Reads line of text file, creates a new Process object and added to priority queue
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

  schedulerFile.close(); // Finished reading file, close file

  vector<Process> allProcessArrivalTime; // List to hold all processes for scheduling to take place and iterating
  vector<Process> allProcessID;          // List to hold all processes in id order, to iterate for termination, turnaround and waiting time

  // Populates allProcessArrivalTime and processList with all processes
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
  queue<Process> q0;
  queue<Process> q1;
  queue<Process> q2;

  queue<int> orderOfProcesses; // order of when process is executed
  map<int, int> IDFinishTime;  // maps id to termination time

  Process *current = NULL;
  Process *prev = NULL;

  int counter = -1;
  int timer = -1;

  // starts scheduling and processing each process, using counter as a timer
  while (!allProcessArrivalTime.empty() || !q0.empty() || !q1.empty() || !q2.empty())
  {
    counter += 1;
    timer -= 1;

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

    counter == current->arrivalTime ? current->burstTimeLeft -= 0 : current->burstTimeLeft -= 1; // decrement process counter

    if (current->burstTimeLeft == 0) // Set current process to NULL if finished pocessing
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
    else if (timer == 0) // else move process to another queue if timer for process expired
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
      timer = -1; //  ignoring timer when current process executing is in q2
    }

    if (current == NULL) // check if there is a current process executing. If not, go to next iteration
    {
      continue;
    }

    if (current != prev) // check if not-null current process is different previous process
    {
      orderOfProcesses.push(current->processID);                 // current process is different, place in order of execution
      if (prev != NULL && prev->burstTimeLeft > 0 && timer != 0) // previous process was preempted
      {
        if (prev->queueIn == 0)
        {
          q0.pop();
          q0.push(*prev); // remove and push preempted process to back of q0
        }
        else if (prev->queueIn == 1)
        {
          q1.pop();
          q1.push(*prev); // push preempted process to back of q1
        }
        else
        {
          q2.pop();
          q2.push(*prev); // push preempted process to back of q2
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
    prev = current; // set prev as current after changes in scheduling either from timer or preempt process
  }

  updateTimes(allProcessID, IDFinishTime);
  printOrder(orderOfProcesses, IDFinishTime);

  return 0;
}

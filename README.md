# Searching With Pipes

## Purpose
##### This project was done as part of a operating systems class
#### General:

##### My over all approach to was to keep this process as simple as possible if the by defining  all the possible parent command messages  REQUEST, READY, PIVOT,ect. Next made only three global variables for the arrays that would be used as child to parent and parent to child pipes. I also defined to variables one for READ and WRITE  that will be used in the pipes for communication, and the initial Id setting pipe used by all. I created to processes one for the child to use and the other for the parent to use, I will describe each one after I go through how I set up my main function.  With being said my main function was very simple I first used pipe() to create all the needed pipes for the parent and children to communicate properly. Then forks where used  in nested if – else statements to five deep. Thus this spawned five children all with same parent. Each child process called the child process function respectively. The final else statement called the parent process function. Then once each process was completed the Program would terminate.

#### Child Process Function:
##### This function gets an id to use from the id pipe  from the parents. The process then used that id to create a specific pipe id to correctly to figure out which pipe to use and what text it should call. The it reads the corresponding text file and saves it to a local array. Once this is done it send the READY message to the parent. Then process enters in a while loop waiting and ready to receive commands from the parent. When a certain message is received it performs a certain task and repeats until all the tasks until median is found. The quits the function.

#### Parent Process Function:
##### This function initially sends each child there own id to open all the files there corresponding text files with. Waits for all the files to send pack the READY message then the parent is ready. Then it enters a while loop of sending commands to the child process to find the median, it sends out the pivot value to all children and adjust the k values according to the total number of response of values larger then the pivot from the children. Once the median it sends a terminate value to all the child processes, then is finished.

## Author 
##### -Tye Borden
## Acknowledgements 
###### -Saint Mary's Unviversity Operating Systems Class 2018

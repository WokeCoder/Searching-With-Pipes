/*
 ============================================================================
 Name        : CSCI3431-Assignment1-A00310101.c
 Author      : Tye Borden A00310101
 Version     :1
 ============================================================================
 This project is to demonstrate the how one parent process spawns five
 child processes. These five processes help a parent to mind the median
 of values between 1-25 placed in groups of 5, in five separate text files.
 */

#include <stdio.h>
#include <stdlib.h>

//Below are are the defined constants for this project
#define REQUEST  100
#define PIVOT  200
#define LARGE  300
#define SMALL  400
#define READY 500

#define TERMINATE -2

#define READ 0
#define WRITE 1



int chtp[5][2]; //These are the arrays for the pipes for child to parent
int ptch[5][2]; //These are the arrays for the pipes for parent to child
int isRe[2]; //Pipe set ids for children
void childProcess(); //This the child process constructor
void parentProcess(); //This is the parent process constructor

int main(void) {
	//ID pipe
	pipe(isRe);
	/* Pipes for parent to children */
	pipe(ptch[0]);
	pipe(ptch[1]);
	pipe(ptch[2]);
	pipe(ptch[3]);
	pipe(ptch[4]);

	/* Pipes form  child to parent */
	pipe(chtp[0]);
	pipe(chtp[1]);
	pipe(chtp[2]);
	pipe(chtp[3]);
	pipe(chtp[4]);

	/*
	 * This where the parent spawns five child processes
	 */
	if (fork() == 0) {

		childProcess();
	} else {
		if (fork() == 0) {

			childProcess();
	} else {
			if (fork() == 0) {

				childProcess();
			} else {
				if (fork() == 0) {

					childProcess();
				} else {
					if (fork() == 0) {

						childProcess();
					} else {
						parentProcess();
					}
				}
			}
		}
	}

	return 0;
}
/*
 * The process is the process that is only used by the
 * child processes. This is where the child will take in
 * different commands from the parent process. Theses task
 * help the parent process find which values is the median
 * value.
 *
 * @param id this is given to each process.
 */
void childProcess() {



	/* Used to initialize the values for  the buffer,
	 * the array to store the values from the text
	 * files and line count for the files and the pivot value.
	 * */
	int buf, values[5], lineCount,pivot,message;

	FILE *f; //Where the file will be stored
	char line[128], fileName[16];//String for File Name and lines
	lineCount = 0;

	int valuesLeft = 5;//Used to identify how many values left in values array

//Setting the length of the array
	int length = sizeof(values) / sizeof(values[0]);

	/*Reading first the id given from the parent process*/
	sleep(1);
	close(isRe[WRITE]);
	read(isRe[READ], &buf, sizeof(buf));

		int pipeid = buf - 1;
		int id = buf;
	/*Id given for what text file to open*/
	sprintf(fileName, "input_%d.txt", pipeid);
	f = fopen(fileName, "r");
	if (f != NULL) {
		while (fgets(line, sizeof(line), f)) {
			char *num = line;
			int var = atoi(num);
			values[lineCount++] = var;
		}
	} else {
		perror(fileName);
	}


	/*Message saying that  the child is ready*/
	printf("Child %d sends READY\n", id);
	int value = 500;
	close(chtp[pipeid][READ]);
	write(chtp[pipeid][WRITE], &value, sizeof(int));


	/* This is where the child process enters the requests loop until the median
	is found */
	while (1) {

		close(ptch[pipeid][WRITE]);
		read(ptch[pipeid][READ], &buf,sizeof(int));



		/* This where child process deals with REQUEST message */
		if (buf == REQUEST) {

			/*Checks to see if there are any values left in its array
			 * if sends a random one back if not it returns -1 signaling none left
			 * */
			if (valuesLeft == 0) {
				message = -1;
				close(chtp[pipeid][READ]);
				write(chtp[pipeid][WRITE], &message, sizeof(int));
			} else {

				//Random value creator
				srand(time(NULL));
				int ran = rand() % 5;
				while(values[ran] == -3){
					srand(time(NULL));
					ran = rand() % 5;
				}
				message = values[ran]; //Sets the message be sent as the random variable

				printf("Child %d sends %d to parent\n", id, message);
				close(chtp[pipeid][READ]);
				write(chtp[pipeid][WRITE], &message, sizeof(int)); //Sends value for pivot
			}
			sleep(1);
			/*When the parent broadcasts the pivot value*/
		} else if (buf == PIVOT) {

			/*Reads in and sets pivot equal to message */
			close(ptch[pipeid][WRITE]);
			read(ptch[pipeid][READ], &message, sizeof(int));
			pivot = message;

			int count = 0; // Initialize the count for values greater then the pivot
			/*Checks if there are any values left in the array if not returns zero
			else gives the count*/
			if (valuesLeft == 0){
				message = 0;
			} else {
				for (int i = 0; i < length; i++) {
					if (pivot < values[i] && values[i] != -3) {
						count++;
					}
				}
				message = count;
			}
			/*Sends the number of values larger then pivot  to the parent process*/
			printf("Child %d receives pivot and replies %d\n", id,
					message);
			close(chtp[pipeid][READ]);
			write(chtp[pipeid][WRITE], &message, sizeof(int));


		/*Removes all values in the array larger then the pivot*/
		} else if (buf == LARGE) {
			 int larger = 0;
			for (int i = 0; i < length; i++) {
				if (values[i] > pivot) {
					values[i] = -3;
					larger++;

				}
			}

						valuesLeft = valuesLeft - larger; //Updates values left in the array
			sleep(1);
			/*Removes all values in the array smaller then pivot */
		} else if (buf == SMALL) {
			int smaller = 0;
			for (int i = 0; i < length; i++) {
				if (values[i] < pivot && values[i] != -3) {
					values[i] = -3;
					smaller++;
				}
			}

			valuesLeft = valuesLeft -smaller;//Updates values left in the array
			sleep(1);

			/*Terminates the child process once the median is found*/
		} else if (buf == TERMINATE) {

			printf("Child %d terminates\n", id);
			exit(0);

		} else {
			printf("Error");
		}

	}

}
/*
*This function runs the parent process. It sends directions to the child
*to help figure out what the median value is between all of the values in the
*given text files.
*/
void parentProcess() {


	int pivot;
	int n = 25;
	int k = n / 2;
	int messages[5];
	int isReady =0;




	/*Sends each child their own id*/
	for (int i = 1; i <= 5; i++) {
				close(isRe[READ]);
				write(isRe[WRITE], &i, sizeof(int));
		}


	int bufc;
	sleep(1);//This sleep allows time for each child to respond
	/*Reads that each child is now READY*/
	for (int i = 0; i < 5; i++) {
		close(chtp[i][WRITE]);
		read(chtp[i][READ], &bufc, sizeof(int));

	}

	printf("Parent is READY\n");//Parents states they too are ready

	/*
	This is the conversation loop of instruction and replies from parent to
	and vice versa
	*/
	while (1) {
		/*Choose random child to send a Request too*/
		srand(time(NULL));
		int ran = rand() % 5;
		int value = 100;
		printf("Parent sends REQUEST to Child %d\n", ran + 1);
		close(ptch[ran][READ]);
		write(ptch[ran][WRITE], &value, sizeof(int));
		sleep(1);

		/*
		Reads the childs pivot response if the childs array has  no values left
		it returns and picks a new child else the values is set to the pivot
		 */
		close(chtp[ran][WRITE]);
		read(chtp[ran][READ], &bufc, sizeof(int));
		if (bufc == -1) {
			continue;
		} else {
			pivot = bufc;
		}

		/*
		Sends the PIVOT message to all children
		*/
		for (int i = 0; i < 5; i++) {
			int ptc = i;
			messages[i] = 200;
			close(ptch[ptc][READ]);
			write(ptch[ptc][WRITE], &messages[i], sizeof(int));
		}
		printf("Parent broadcasts pivot %d to all children\n", pivot);
		/*
		Sends pivot value to all children
		*/
		for (int i = 0; i < 5; i++) {
			int ptc = i;
			messages[i] = pivot;
			close(ptch[ptc][READ]);
			write(ptch[ptc][WRITE], &messages[i], sizeof(int));
		}
		sleep(1);



		int m = 0; // Sends Sum values of counts of values larger then pivot to zero
		for (int i = 0; i < 5; i++) {
			int ptc = i;
			close(chtp[ptc][WRITE]);
			read(chtp[ptc][READ], &messages[i], sizeof(int));
			m += messages[i]; //Sums count values from each child
		}
		printf("Parent: m =%d+%d+%d+%d+%d= %d", messages[0], messages[1],
				messages[2], messages[3], messages[4], m);

				/*
				Checks to see if the sum of values that are larger then the pivot is
				less then k if so sends LARGE message
				 */
		if (m < k) {
			printf(" < k = %d Median not Found!\n",k);

			k = k - m; //Resets k to previous minus new sum m

			for (int i = 0; i < 5; i++) {//Sends LARGE message to all children
				int ptc = i;
				messages[i] = 300;
				close(ptch[ptc][READ]);
				write(ptch[ptc][WRITE], &messages[i], sizeof(int));
			}

			/*
			Checks to see if the sum of values that are larger then the pivot is
			more then k if so sends SMALL message
			 */
		} else if (m > k) {//Sends SMALL to all children
			printf(" > k=%d Median not Found!\n", k);

			for (int i = 0; i < 5; i++) {
				int ptc = i;
				messages[i] = 400;
				close(ptch[ptc][READ]);
				write(ptch[ptc][WRITE], &messages[i], sizeof(int));
			}
			/*
			When k is equal to m the median is found and while instruction loop
			breaks
			*/
		} else {
			printf(" = k = %d Median Found!\n",k);
			break;
		}
	}

	sleep(1);
	/*
	Parent sends TERMINATE signal to all children to end their respective
	processes
	*/
	printf("Parent sends kill signal to all children\n");
	for (int i = 0; i < 5; i++) {
		int ptc = i;
		messages[i] = -2;
		close(ptch[ptc][READ]);
		write(ptch[ptc][WRITE], &messages[i], sizeof(int));
	}

	exit(0);
}

/*
  Alex Eseyin
  1001598309
*/

// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments

#define MAX_HIST_COUNT 15       // Max number of history to be saved

/* pidNum is an array that saves that last 15 Process IDs and also
keeps track of the just in cases the user wants to print them */
int pidNum[15];
int pidcounter=0; // this keeps track of the number of PIDS so the PID history can be updated
char *cmdHistory[MAX_HIST_COUNT]; //Array of characters that holds up to 15 commands enetred by the user
int u=0; //Updates the count of the number of commands the user runs , this is for the History.


/* storePid function is responsible for storing all 
the process Ids that was created everytime a 
command led to a fork, This process Ids are stored 
in the pidNum array */
void storePid(int pidValue) 
{
  int i = 0;
  if(pidcounter<15)
  {
    for (i=0;i<sizeof(pidNum);i++)
    {
      pidNum[pidcounter]=pidValue;
    }
   
  }
   else
    {
      pidNum[0]=pidValue; // This allows us to replace the oldest PID in the the Array with the Most recent
      i++;
    }
}
/* printPid function does exaclty what is says , 
it prints the 15 most recent process ID that 
were stored in the storePid function */ 
void printPid()
{ 
  int i=0;
  if(pidcounter==0) // Prints a message if there is no Pid tp print
  {
    printf("No pids to dispay just yet \n");
  }
  else if(pidcounter>0||pidcounter<15) // <-- Only Prints the PIDs that has been stored if they are less that the 15 Pid Max
  {
    for(i = 0;i<pidcounter;i++)
  {
    printf("%d: %d \n",i+1,pidNum[i]);
  }
  }
  else
  {
    for(i = 0;i<15;i++)
  {
    printf("%d: %d \n",i+1,pidNum[i]);
  }

  }
}
/* storeHistory copys the command string 
and saves it in the cmdHistory array*/
void storeHistory(char *cmd_str)
{

  
  if(u > 14)
  {
    u =0;
  }
  strcpy(cmdHistory[u], cmd_str);
  u=u+1;

  //  else if(u<15)
  //  {
  //   strcpy(cmdHistory[u], cmd_str);
  //  }
  //  else
  //  {
  //   u=0;
  //   strcpy(cmdHistory[u], cmd_str);
  //  }
  
}
/* retrieveHistory prints out the history from the 
cmdHistory array*/
void retrieveHistory()
{
  int i;
  if(u<14)
  {
    for (i=0;i<u;i++)
  {
    printf("%d:%s\n", i+1,cmdHistory[i]);
  }
  }
  else
  {
     for (i=0;i<15;i++)
  {
    printf("%d:%s\n", i+1,cmdHistory[i]);
  }

  }

}

//re_run()

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
  int i;

  //allocates memory for the cmhHistory array
  for(i=0;i<MAX_HIST_COUNT;i++)
  {
    cmdHistory[i] = (char*) malloc(MAX_COMMAND_SIZE);
  }

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    /*Copy the entire command string and store it in the command history array */
    storeHistory(cmd_str);

    //Print statement just to see what is in *cmd_str  for debugging purpose
    // for(z=0;z<sizeof(cmd_str);z++)
    // {
    //   printf("what is in workingstr: %c \n", cmd_str);
    // }

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }
    
    if(token[0]==NULL)
    {
        /* This portion is left empty on purpouse, 
        If the user inputs a space or hits enter , 
        the program keeps running and does nothing 
        unlit an actual command is typed int */

    }
    /* strcmp compares the token inputed by the user, 
    if user enters the words quit or exit the program will exit
    */
    else if(strcmp(token[0],"quit")==0 || strcmp(token[0],"exit") ==0)
    {
      printf("Exit..... \n"); //Debugging print statment
      return EXIT_SUCCESS;
    }

    /*
     if the user enters the command 'cd' , the program shall chnage 
     the directory to what the user specifies
    */
    else if(strcmp(token[0],"cd")==0)
    {
      chdir(token[1]); // moves to the next token.
    }

    /*
     if the user enters the command 'showpids' , 
     the program shall list the 15 most recent PIDS by calling
      the printPid() function
    */
    else if(strcmp(token[0],"showpids")==0)
    {
      printPid();
    }

    /*
     if the user enters the command 'history' , 
     the program shall list the 15 most recent commands entered by the user
    */
     else if(strcmp(token[0],"history")==0)
    {
      retrieveHistory();
    }

     /*
     TO DO 
     */
    else if (strcmp(token[0],"!")==0)
    {
      printf("! functionality was not implemented\n");
      //re_run(token[0]);
    }

    /*
      fork system call
    */
    else
    {
      pid_t pid =fork();

      if( pid == 0 )
    {
      int var = execvp( token[0],token);
    
      if( var== -1 )
      {
        //perror("execl failed: ");
        printf("Command not found \n");
      }
    }
    else 
    {
      int status;
      //int value=pid;
      waitpid( pid,& status,0 );
      storePid(pid); //Stores the value of the current process Id  in the 'storePid' function.
      pidcounter++; //Updates Pidcounter everything a fork is successful.

    }
      free( working_root );

  }
  }
  return 0;
}

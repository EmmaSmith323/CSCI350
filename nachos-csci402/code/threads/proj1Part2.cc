/* 
//Group 25 
//Part2.cc 
*/

#ifndef PROJ1_PART2_CC
#define PROJ1_PART2_CC

#include "synch.h"
#include "list.h"
#include "vector"
#include "utility.h"
#include <stdlib.h> 
#include <iostream>

/* //Settings Variables */


int CLERKCOUNT;		/* //The number of clerks */
int CUSTOMERCOUNT; 	/* //Number of customers */
int SENATORCOUNT;  /* //Number of senators */


bool valid;
bool THEEND = false;

/* //Globals or constants */
/* enum CLERKSTATE {AVAILABLE, SIGNALEDCUSTOMER, BUSY, ONBREAK}; */			/* //enum for the CLERKSTATE */
#define AVAILABLE 0
#define SIGNALEDCUSTOMER 1
#define BUSY 2
#define ONBREAK 3

#define MAXCLERKS 5
#define MAXCUSTOMERS 50
#define MAXSENATORS 10


char CUSTOMERTEXT[] = "Customer";
char SENATORTEXT[] = "Senator";

/* /////////////////////////////////
// Locks, CVS, and Monitors?
//Locks */
Lock *applicationClerkLineLock; /* // = new Lock("applicationClerkLineLock");	//The applicationClerkLineLock */
Lock *pictureClerkLineLock; /* // = new Lock("pictureClerkLineLock");	//The applicationClerkLineLock */
Lock *passportClerkLineLock; /* // = new Lock("passportClerkLineLock"); */
Lock *cashierLineLock; /*// = new Lock("cashierLineLock"); */
Lock *managerLock; /* // = new Lock("managerLock");	//Lock for the manager */

/* std::vector<Lock*> applicationClerkLock;
std::vector<Lock*> pictureClerkLock;
std::vector<Lock*> passportClerkLock;
std::vector<Lock*> cashierLock; */

Lock* applicationClerkLock[MAXCLERKS];
Lock* pictureClerkLock[MAXCLERKS];
Lock* passportClerkLock[MAXCLERKS];
Lock* cashierClerkLock[MAXCLERKS];

/* //CVS */
Condition *passportOfficeOutsideLineCV; /* //This can be the outside line for when senators are present */
Condition *senatorLineCV;

/* std::vector<Condition*> applicationClerkLineCV;
std::vector<Condition*> applicationClerkBribeLineCV;	//applicationClerk CVs
std::vector<Condition*> applicationClerkCV; */

Condition* applicationClerkLineCV[MAXCLERKS];
Condition* applicationClerkBribeLineCV[MAXCLERKS];	/* //applicationClerk CVs */
Condition* applicationClerkCV[MAXCLERKS];

Condition *applicationClerkBreakCV; /* // = new Condition("applicationClerkBreakCV");//To keep track of clerks on break */

/* std::vector<Condition*> pictureClerkLineCV;
std::vector<Condition*> pictureClerkBribeLineCV;	//pictureClerk CVs
std::vector<Condition*> pictureClerkCV; */

Condition* pictureClerkLineCV[MAXCLERKS];
Condition* pictureClerkBribeLineCV[MAXCLERKS];	//pictureClerk CVs
Condition* pictureClerkCV[MAXCLERKS];

Condition *pictureClerkBreakCV; /* // = new Condition("pictureClerkBreakCV");//To keep track of clerks on break */

/* std::vector<Condition*> passportClerkLineCV;
std::vector<Condition*> passportClerkBribeLineCV;	//passportClerk CVs
std::vector<Condition*> passportClerkCV; */

Condition* passportClerkLineCV[MAXCLERKS];
Condition* passportClerkBribeLineCV[MAXCLERKS];	//passportClerk CVs
Condition* passportClerkCV[MAXCLERKS];

Condition *passportClerkBreakCV; /* // = new Condition("passportClerkBreakCV");//To keep track of clerks on break */

/* std::vector<Condition*> cashierLineCV;
std::vector<Condition*> cashierBribeLineCV;	//passportClerk CVs
std::vector<Condition*> cashierCV; */

Condition* cashierLineCV[MAXCLERKS];
Condition* cashierBribeLineCV[MAXCLERKS];	/* //passportClerk CVs */
Condition* cashierCV[MAXCLERKS];

Condition *cashierBreakCV; /* // = new Condition("cashierBreakCV");//To keep track of clerks on break */


/* //States */
/* std::vector<CLERKSTATE> applicationClerkState(CLERKCOUNT, BUSY);	//applicationClerkState
std::vector<CLERKSTATE> pictureClerkState(CLERKCOUNT, BUSY);	//applicationClerkState
std::vector<CLERKSTATE> passportClerkState(CLERKCOUNT, BUSY);	//applicationClerkState
std::vector<CLERKSTATE> cashierState(CLERKCOUNT, BUSY);	//applicationClerkState */

int applicationClerkState[MAXCLERKS];	/* //applicationClerkState */
int pictureClerkState[MAXCLERKS];  /* //applicationClerkState */
int passportClerkState[MAXCLERKS];	/* //applicationClerkState */
int cashierState[MAXCLERKS];	/* //applicationClerkState */

int setup = 0;
for (int setup = 0; setup < MAXCLERKS; setup++)
{
	applicationClerkState[setup] = 2;
	pictureClerkState[setup] = 2;
	passportClerkState[setup] = 2;
	cashierState[setup] = 2;
}

 
/* //LineCounts */
/*
std::vector<int> applicationClerkLineCount(CLERKCOUNT, 0);			//applicationClerkLineCount
std::vector<int> applicationClerkBribeLineCount(CLERKCOUNT, 0);		//applicationClerkBribeLineCount
std::vector<int> pictureClerkLineCount(CLERKCOUNT, 0);			//pictureClerkLineCount
std::vector<int> pictureClerkBribeLineCount(CLERKCOUNT, 0);		//pictureClerkBribeLineCount
std::vector<int> passportClerkLineCount(CLERKCOUNT, 0);			//passportClerkLineCount
std::vector<int> passportClerkBribeLineCount(CLERKCOUNT, 0);		//passportClerkBribeLineCount
std::vector<int> cashierLineCount(CLERKCOUNT, 0);			//cashierLineCount
std::vector<int> cashierBribeLineCount(CLERKCOUNT, 0);		//cashierBribeLineCount 
*/

int applicationClerkLineCount[MAXCLERKS];			/* //applicationClerkLineCount */
int applicationClerkBribeLineCount[MAXCLERKS];		/* //applicationClerkBribeLineCount */
int pictureClerkLineCount[MAXCLERKS];			/* //pictureClerkLineCount */
int pictureClerkBribeLineCount[MAXCLERKS];		/* //pictureClerkBribeLineCount */
int passportClerkLineCount[MAXCLERKS];			/* //passportClerkLineCount */
int passportClerkBribeLineCount[MAXCLERKS];		/* //passportClerkBribeLineCount */
int cashierLineCount[MAXCLERKS];			/* //cashierLineCount */
int cashierBribeLineCount[MAXCLERKS];		/* //cashierBribeLineCount */

int lineSetup = 0;
for (int lineSetup = 0; lineSetup < MAXCLERKS; lineSetup++)
{
	applicationClerkLineCount[lineSetup] = 0;			
	applicationClerkBribeLineCount[lineSetup] = 0;		
	pictureClerkLineCount[lineSetup] = 0;			
    pictureClerkBribeLineCount[lineSetup] = 0;		
	passportClerkLineCount[lineSetup] = 0;			
	passportClerkBribeLineCount[lineSetup] = 0;		
	cashierLineCount[lineSetup] = 0;			
	cashierBribeLineCount[lineSetup] = 0;
}

/* //Shared Data //Should only be accessed with the corresponding lock / cv */
/*
std::vector<int> applicationClerkSharedData(CLERKCOUNT, 0);	//This can be used by the customer to pass SSN
std::vector<int> pictureClerkSharedDataSSN(CLERKCOUNT, 0); //This can be used by the customer to pass SSN
std::vector<int> pictureClerkSharedDataPicture(CLERKCOUNT, 0); // This can be used by the customer to pass acceptance of the picture
std::vector<int> passportClerkSharedDataSSN(CLERKCOUNT, 0); //This can be used by the customer to pass SSN
std::vector<bool> applicationCompletion(CUSTOMERCOUNT, 0); //Used by passportCerkto verify that application has been completed
std::vector<bool> pictureCompletion(CUSTOMERCOUNT, 0); //Used by passportClerk to verify that picture has beeen completed 
std::vector<bool> passportCompletion(CUSTOMERCOUNT, 0); // Used by cashier to verify that the passport is complete
std::vector<int> passportPunishment(CUSTOMERCOUNT, 0); //Used by passportClerk to punish bad people.
std::vector<int> cashierSharedDataSSN(CLERKCOUNT, 0); //This can be used by the customer to pass SSN
std::vector<int> cashierRejection(CUSTOMERCOUNT, 0); //Used by the cashier to reject customers.
std::vector<int> doneCompletely(CUSTOMERCOUNT, 0); //Used by customer to tell when done.
*/

int applicationClerkSharedData[MAXCLERKS];	/* //This can be used by the customer to pass SSN */
int pictureClerkSharedDataSSN[MAXCLERKS]; /* //This can be used by the customer to pass SSN */
int pictureClerkSharedDataPicture[MAXCLERKS]; /* // This can be used by the customer to pass acceptance of the picture */
int passportClerkSharedDataSSN[MAXCLERKS]; /* //This can be used by the customer to pass SSN */

int sharedSetup = 0;
for (int sharedSetup = 0; sharedSetup < MAXCLERKS; sharedSetup++)
{
	applicationClerkSharedData[sharedSetup] = 0;			
	pictureClerkSharedDataSSN[sharedSetup] = 0;		
	pictureClerkSharedDataPicture[sharedSetup] = 0;			
    passportClerkSharedDataSSN[sharedSetup] = 0;		
}

bool applicationCompletion[MAXCUSTOMERS]; /* //Used by passportCerkto verify that application has been completed */
bool pictureCompletion[MAXCUSTOMERS]; /* //Used by passportClerk to verify that picture has beeen completed */
bool passportCompletion[MAXCUSTOMERS]; /* // Used by cashier to verify that the passport is complete */
int passportPunishment[MAXCUSTOMERS]; /* //Used by passportClerk to punish bad people. */
int cashierSharedDataSSN[MAXCUSTOMERS]; /* //This can be used by the customer to pass SSN */
int cashierRejection[MAXCUSTOMERS]; /* //Used by the cashier to reject customers. */
int doneCompletely[MAXCUSTOMERS]; /* //Used by customer to tell when done. */

int otherSetup = 0;
for (int otherSetup = 0; otherSetup < MAXCUSTOMERS; otherSetup++)
{
	applicationCompletion[otherSetup] = 0;			
	pictureCompletion[otherSetup] = 0;		
	passportCompletion[otherSetup] = 0;			
    passportPunishment[otherSetup] = 0;
    cashierSharedDataSSN[otherSetup] = 0;		
	cashierRejection[otherSetup] = 0;			
    doneCompletely[otherSetup] = 0;	
    	
}

int customersPresentCount = 0; /* //For telling the manager we're in the office */
int senatorPresentCount = 0;
int checkedOutCount = 0;	/* //For the manager to put everyone to sleep when the customers have all finished */
int senatorLineCount = 0;	/* //For counting the sentors.//They wait in a private line for the manager while waiting for customers to leave. */
int passportOfficeOutsideLineCount = 0;
bool senatorSafeToEnter = false; /* //To tell senators when it is safe to enter */
bool senatorPresentWaitOutSide = false; /* //Set by the manager to tell customers when a senator is present... */
/* 
//
//End variables
///////////////////////////////// 
*/



/* //Utility Functions */

/* // Utility function to pick the shortest line
   //Customer has already chosen type of line to get in just needs to pick which line
//Assumptions: The caller has a lock for the given MVs
//Parameters: 
	//lineCount: a vector of the lineCount
	//clerkState: a vector of the clerkState
*/
/* int pickShortestLine(std::vector<int>& pickShortestlineCount, std::vector<CLERKSTATE>& pickShortestclerkState){ */
int pickShortestLine(int& pickShortestlineCount[], int& pickShortestclerkState[]){
	int i;
	int myLine = -1;
	int lineSize = 1000;
	for(i=0; i < CLERKCOUNT; i++){
		/* //If lineCount < lineSize and clerk is not on break */
		if(pickShortestlineCount[i] < lineSize && pickShortestclerkState[i] != ONBREAK ){
			myLine = i;
			lineSize = pickShortestlineCount[i];
		}
	}
	return myLine;	/* //This is the shortest line */
}/* //End pickShortestLine */

/* //Used by customerInteractions to return customer/senator text... */
char* MYTYPE(int VIP){
	if(VIP == 0){
		return CUSTOMERTEXT;
	}else if(VIP == 1){
		return SENATORTEXT;
	}else{
		return NULL;
	}
}

/* //End Utility Functions */




/* //Customer */
/* // Customers enter the Passport Office with a completed application. 
// Their job is the following:
//    Get their application accepted by the ApplicationClerk
//    Get their official picture taken by the PictureClerk. Customers have a random percentage to not like their picture.
//    Get their "filed" application and picture recognized by the PassportClerk
//    Pay the Cashier once their documentation has been approved by the PassportClerk. 
		//Once a Customer has paid for their passport by the Cashier, they are done. 

// Customers can either get their application accepted first, or get their picture taken first. 
	//They make this decision randomly. 
	//Once they decide which clerk to go to first, they get in the shortest line for that clerk.

// Customers also have up to $1600 to help reduce the time required to get their passport completed.
	// Each clerk is willing to accept $500 to allow a Customer to move up in line. 
	// Customer who can pay move in front of any Customer that has not paid. 
	// HOWEVER, they do not move in front of any Customer that has also paid. 
	// Customer money is to be deterined randomly, in increments of $100, $600, $1100, and $1600.
*/
bool customerApplicationClerkInteraction(int SSN, int &money, int VIP = 0); /* //forward declaration//prolly not cleaner like this just thought it would be nice to implement after the main Customer thread. */
bool customerPictureClerkInteraction(int SSN, int &money, int VIP = 0);
bool customerPassportClerkInteraction(int SSN, int &money, int VIP = 0);
bool customerCashierInteraction(int SSN, int &money, int VIP = 0);
void customerCheckIn();
bool customerCheckSenator(int SSN);
void customerCheckIn(int SSN);
void customerCheckOut(int SSN);

void Customer(int id){
	int i;
	int j;
	bool appClerkDone = false; /* //State vars */
	bool pictureClerkDone = false;
	bool passportClerkDone = false;
	bool cashierDone = false; 
	int SSN = id;
	int myLine = -1;
	int money = (rand()%4)*500 + 100;
	bool appClerkFirst = rand() % 2;

	customerCheckIn(SSN);

	while(true){

		/* //Check if a senator is present and wait outside if there is. */
		customerCheckSenator(SSN);

		if( !(appClerkDone) && (appClerkFirst || pictureClerkDone) ){ /* //Go to applicationClerk */
			appClerkDone = customerApplicationClerkInteraction(SSN, money);
		}
		else if( !pictureClerkDone ){
			/* //Go to the picture clerk */
			pictureClerkDone = customerPictureClerkInteraction(SSN, money);
		}
		else if(!passportClerkDone){
			passportClerkDone = customerPassportClerkInteraction(SSN, money);
			if (!passportClerkDone) { for (i = 0; i < rand() % 901 + 100; i++) { currentThread->Yield(); } }
		}
		else if(!cashierDone){
			cashierDone = customerCashierInteraction(SSN, money);
			if (!cashierDone) {	for (j = 0; j < rand() % 901 + 100; j++) { currentThread->Yield(); } }
		}
		else{
			/* //This terminates the customer should go at end. */
			customerCheckOut(SSN);
		}
	}

	/* //Here are the output Guidelines for the Customer */
	if(false){
	printf("Customer %i has gotten in regular line for PictureClerk %i.\n", SSN, myLine);
	printf("Customer %i has gotten in regular line for PassportClerk %i.\n", SSN, myLine);
	printf("Customer %i has gotten in regular line for Cashier %i.\n", SSN, myLine);
	printf("Customer %i has gotten in bribe line for PictureClerk %i.\n", SSN, myLine);
	printf("Customer %i has gotten in bribe line for PassportClerk %i.\n", SSN, myLine);
	printf("Customer %i has gotten in bribe line for Cashier %i.\n", SSN, myLine);
	printf("Customer %i has given SSN %i to PictureClerk %i.\n", SSN, SSN, myLine);
	printf("Customer %i has given SSN %i to PassportClerk %i.\n", SSN, SSN, myLine);
	printf("Customer %i has given SSN %i to Cashier %i.\n", SSN, SSN, myLine);
	printf("Customer %i does not like their picture from PictureClerk %i.\brn", SSN, myLine);
	printf("Customer %i does like their picture from PictureClerk %i.\n", SSN, myLine);
	printf("Customer %i has gone to PassportClerk %i too soon. They are going to the back of the line.\n", SSN, myLine);
	printf("Customer %i has gone to Cashier %i too soon. They are going to the back of the line.\n", SSN, myLine);
	printf("Customer %i has given Cashier %i $100.", SSN, myLine);
	}

}/* //End Customer */


/* //Wait outside or something there's a Senator present */
void customerSenatorPresentWaitOutside(int SSN){
	printf("Customer %i is going outside the PassportOffice because there is a Senator present.\n", SSN);

	/* //Go outside. */
	customersPresentCount--;
	passportOfficeOutsideLineCount++;
	passportOfficeOutsideLineCV->Wait(managerLock);
	/* /Can go back inside now. */
	passportOfficeOutsideLineCount--;
	customersPresentCount++;
}

/* // Checks if a senator is present. Then goes outside if there is. */
bool customerCheckSenator(int SSN){
	/* //DEBUG('s', "DEBUG: Customer %i about to CHECK for senator...\n", SSN); */
	managerLock->Acquire();
	bool present = senatorPresentWaitOutSide;

	if(present)
		customerSenatorPresentWaitOutside(SSN);

	managerLock->Release();
	return present;
}

void customerCheckIn(int SSN){
	managerLock->Acquire();
	customersPresentCount++;
	managerLock->Release();
}

/* //To tell the manager they did a great job and let him know we're done. */
void customerCheckOut(int SSN){
	managerLock->Acquire();
	customersPresentCount--;
	checkedOutCount++;
	managerLock->Release();
	printf("Customer %i is leaving the Passport Office.\n", SSN);
	currentThread->Finish();
}






/*
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
//
//	customer/senator - Clerk Interactions
//
/////////////////////////////////////////////////////////////////
*/









/*
//The Customer's Interaction with the applicationClerk
//    Get their application accepted by the ApplicationClerk
*/
bool customerApplicationClerkInteraction(int SSN, int &money, int VIP){
	int myLine = -1;
	char* myType = MYTYPE(VIP);
	bool bribe = (money > 500) && (rand()%2) && !VIP; /* //VIPS dont bribe... */
	/* //I have decided to go to the applicationClerk */

	/* //I should acquire the line lock */
	applicationClerkLineLock->Acquire();
	/* //lock acquired */

	/*
	//Can I go to counter, or have to wait? Should i bribe?
	//Pick shortest line with clerk not on break
	//Should i get in the regular line else i should bribe?
	*/
	if(!bribe){ /* //Get in regular line */
		myLine = pickShortestLine(applicationClerkLineCount, applicationClerkState);
	}else{ /* //get in bribe line */
		myLine = pickShortestLine(applicationClerkBribeLineCount, applicationClerkState);
	}
	
	/* //I must wait in line */
	if(applicationClerkState[myLine] != AVAILABLE){
		if(!bribe){
			applicationClerkLineCount[myLine]++;
			printf("%s %i has gotten in regular line for ApplicationClerk %i.\n", myType, SSN, myLine);
			applicationClerkLineCV[myLine]->Wait(applicationClerkLineLock);
			applicationClerkLineCount[myLine]--;
			/* //See if the clerk for my line signalled me, otherwise check if a senator is here and go outside. */
			if(applicationClerkState[myLine] != SIGNALEDCUSTOMER){
				applicationClerkLineLock->Release();
				if(customerCheckSenator(SSN))
					return false;
			}
		}else{
			applicationClerkBribeLineCount[myLine]++;
			printf("%s %i has gotten in bribe line for ApplicationClerk %i.\n", myType, SSN, myLine);
			applicationClerkBribeLineCV[myLine]->Wait(applicationClerkLineLock);
			applicationClerkBribeLineCount[myLine]--;
			/* //See if the clerk for my line signalled me, otherwise check if a senator is here and go outside. */
			if(applicationClerkState[myLine] != SIGNALEDCUSTOMER){
				applicationClerkLineLock->Release();
				if(customerCheckSenator(SSN))
					return false;
			}
			money -= 500;
		}
	}
	/* //Clerk is AVAILABLE */
	applicationClerkState[myLine] = BUSY;
	applicationClerkLineLock->Release();
	/* //Lets talk to clerk */
	applicationClerkLock[myLine]->Acquire();
	/* //Give my data to my clerk */
	/* //We already have a lock so put my SSN in applicationClerkSharedData */
	applicationClerkSharedData[myLine] = SSN;
	printf("%s %i has given SSN %i to ApplicationClerk %i.\n", myType, SSN, SSN, myLine);
	applicationClerkCV[myLine]->Signal(applicationClerkLock[myLine]);
	/* //Wait for clerk to do their job */
	applicationClerkCV[myLine]->Wait(applicationClerkLock[myLine]);
	
	/* //Done */
	applicationClerkLock[myLine]->Release();
	/* //Done Return */
	return true;
}/* //End customerApplicationClerkInteraction */


/* //The Customer's Interaction with the pictureClerk */
/* //Get their picture accepted by the pictureClerk */
bool customerPictureClerkInteraction(int SSN, int &money, int VIP){
	int myLine = -1;
	char* myType = MYTYPE(VIP);
	bool bribe = (money > 500) && (rand()%2) && !VIP;

	/* //I should acquire the line lock */
	pictureClerkLineLock->Acquire();
	/* //lock acquired */

	/*
	//Can I go to counter, or have to wait? Should i bribe?
	//Pick shortest line with clerk not on break
	//Should i get in the regular line else i should bribe?
	*/
	if(!bribe){ /* //Get in regular line */
		myLine = pickShortestLine(pictureClerkLineCount, pictureClerkState);
	}else{ /* //get in bribe line */
		myLine = pickShortestLine(pictureClerkBribeLineCount, pictureClerkState);
	}
	
	/* //I must wait in line */
	if(pictureClerkState[myLine] != AVAILABLE){
		if(!bribe){
			pictureClerkLineCount[myLine]++;
			printf("%s %i has gotten in regular line for PictureClerk %i.\n", myType, SSN, myLine);
			pictureClerkLineCV[myLine]->Wait(pictureClerkLineLock);
			pictureClerkLineCount[myLine]--;
			if(pictureClerkState[myLine] != SIGNALEDCUSTOMER){
				pictureClerkLineLock->Release();
				if(customerCheckSenator(SSN))
					return false;
			}
		}else{
			pictureClerkBribeLineCount[myLine]++;
			printf("%s %i has gotten in bribe line for PictureClerk %i.\n", myType, SSN, myLine);
			pictureClerkBribeLineCV[myLine]->Wait(pictureClerkLineLock);
			pictureClerkBribeLineCount[myLine]--;
			if(pictureClerkState[myLine] != SIGNALEDCUSTOMER){
				pictureClerkLineLock->Release();
				if(customerCheckSenator(SSN))
					return false;
			}
			money -= 500;
		}
	}
	/* //Clerk is AVAILABLE */
	pictureClerkState[myLine] = BUSY;
	pictureClerkLineLock->Release();
	/* //Lets talk to clerk */
	pictureClerkLock[myLine]->Acquire();
	/* 
	//Give my data to my clerk
	//We already have a lock so put my SSN in pictureClerkSharedData
	*/
	pictureClerkSharedDataSSN[myLine] = SSN;
	printf("%s %i has given SSN %i to PictureClerk %i.\n", myType, SSN, SSN, myLine);


	pictureClerkCV[myLine]->Signal(pictureClerkLock[myLine]);
	pictureClerkCV[myLine]->Wait(pictureClerkLock[myLine]);
	/* //Wait for clerk to take the picture */
	while(pictureClerkSharedDataPicture[myLine] == 0) {
		if(rand()%10 > 7) {
			printf("%s %i does not like their picture from PictureClerk %i.\n", myType, SSN, myLine);
			pictureClerkSharedDataPicture[myLine] = 0;
			pictureClerkCV[myLine]->Signal(pictureClerkLock[myLine]);
			/* //Wait for clerk to take the picture */
			pictureClerkCV[myLine]->Wait(pictureClerkLock[myLine]);
		}
		else {
			printf("%s %i does like their picture from PictureClerk %i.\n", myType, SSN, myLine);
			pictureClerkSharedDataPicture[myLine] = 1;
			pictureClerkCV[myLine]->Signal(pictureClerkLock[myLine]);
			/* //Wait for clerk to take the picture */
			pictureClerkCV[myLine]->Wait(pictureClerkLock[myLine]);
		}
	}
	pictureClerkLock[myLine]->Release();
	/* //Done Return */
	return true;
}/* //End customerPictureClerkInteraction */

bool customerPassportClerkInteraction(int SSN, int &money, int VIP){
	int myLine = -1;
	char* myType = MYTYPE(VIP);
	bool bribe = (money > 500) && (rand()%2) && !VIP;
	/* //I should acquire the line lock */
	passportClerkLineLock->Acquire();
	/*
	//lock acquired
	//Can I go to counter, or have to wait? Should i bribe?
	//Pick shortest line with clerk not on break
	//Should i get in the regular line else i should bribe?
	*/
	if(!bribe){ /* //Get in regular line */
		myLine = pickShortestLine(passportClerkLineCount, passportClerkState);
	}else{ /* //get in bribe line */
		myLine = pickShortestLine(passportClerkBribeLineCount, passportClerkState);
	}
	/* //I must wait in line */
	if(passportClerkState[myLine] != AVAILABLE){
		if(!bribe){
			passportClerkLineCount[myLine]++;
			printf("%s %i has gotten in regular line for PassportClerk %i.\n", myType, SSN, myLine);
			passportClerkLineCV[myLine]->Wait(passportClerkLineLock);
			passportClerkLineCount[myLine]--;
			if(passportClerkState[myLine] != SIGNALEDCUSTOMER){
				passportClerkLineLock->Release();
				if(customerCheckSenator(SSN))
					return false;
			}
		}else{
			passportClerkBribeLineCount[myLine]++;
			printf("%s %i has gotten in bribe line for PassportClerk %i.\n", myType, SSN, myLine);
			passportClerkBribeLineCV[myLine]->Wait(passportClerkLineLock);
			passportClerkBribeLineCount[myLine]--;
			if(passportClerkState[myLine] != SIGNALEDCUSTOMER){
				passportClerkLineLock->Release();
				if(customerCheckSenator(SSN))
					return false;
			}
			money -= 500;
		}
	}
	/* //Clerk is AVAILABLE */
	passportClerkState[myLine] = BUSY;
	passportClerkLineLock->Release();

	/* //Lets talk to clerk */
	passportClerkLock[myLine]->Acquire();
	/*
	//Give my data to my clerk
	//We already have a lock so put my SSN in passportClerkSharedData
	*/
	passportClerkSharedDataSSN[myLine] = SSN;
	printf("%s %i has given SSN %i to PassportClerk %i.\n", myType, SSN, SSN, myLine);
	passportClerkCV[myLine]->Signal(passportClerkLock[myLine]);
	/* //Wait for clerk to do their job */
	passportClerkCV[myLine]->Wait(passportClerkLock[myLine]);
	if(passportPunishment[SSN] == 1) {
		printf("%s %i has gone to PassportClerk %i too soon. They are going to the back of the line.\n", myType, SSN, myLine);
		passportClerkLock[myLine]->Release();
		return false;
	}
	/* //Done */
	passportClerkLock[myLine]->Release();
	/* //Done Return */
	return true;

}/* //End of customerPassportClerkInteraction */


bool customerCashierInteraction(int SSN, int &money, int VIP){
	int myLine = -1;
	char* myType = MYTYPE(VIP);
	bool bribe = (money > 500) && (rand()%2) && !VIP;

	/* //I should acquire the line lock */
	cashierLineLock->Acquire();
	/* //lock acquired */
	/*
	//Can I go to counter, or have to wait? Should i bribe?
	//Pick shortest line with clerk not on break
	//Should i get in the regular line else i should bribe?
	*/
	if(!bribe){ /* //Get in regular line */
		myLine = pickShortestLine(cashierLineCount, cashierState);
	}else{ /* //get in bribe line */
		myLine = pickShortestLine(cashierBribeLineCount, cashierState);
	}
	
	/* //I must wait in line */
	if(cashierState[myLine] != AVAILABLE){
		if(!bribe){
			cashierLineCount[myLine]++;
			printf("%s %i has gotten in regular line for Cashier %i.\n", myType, SSN, myLine);
			cashierLineCV[myLine]->Wait(cashierLineLock);
			cashierLineCount[myLine]--;
			if(cashierState[myLine] != SIGNALEDCUSTOMER){
				cashierLineLock->Release();
				if(customerCheckSenator(SSN))
					return false;
			}
		}else{
			cashierBribeLineCount[myLine]++;
			printf("%s %i has gotten in bribe line for Cashier %i.\n", myType, SSN, myLine);
			cashierBribeLineCV[myLine]->Wait(cashierLineLock);
			cashierBribeLineCount[myLine]--;
			if(cashierState[myLine] != SIGNALEDCUSTOMER){
				cashierLineLock->Release();
				if(customerCheckSenator(SSN))
					return false;
			}
			money -= 500;
			
		}
	}
	/* //Clerk is AVAILABLE */
	cashierState[myLine] = BUSY;
	cashierLineLock->Release();

	/* //Lets talk to clerk */
	cashierLock[myLine]->Acquire();
	/* 
	//Give my data to my clerk
	//We already have a lock so put my SSN in cashierSharedData
	*/
	cashierSharedDataSSN[myLine] = SSN;
	printf("%s %i has given SSN %i to Cashier %i.\n", myType, SSN, SSN, myLine);
	cashierCV[myLine]->Signal(cashierLock[myLine]);
	/* //Wait for clerk check completion */
	cashierCV[myLine]->Wait(cashierLock[myLine]);

	if (cashierRejection[SSN] == 1) {
		printf("%s %i has gone to Cashier %i too soon. They are going to the back of the line.\n", myType, SSN, myLine);
		cashierLock[myLine]->Release();
		return false;
	}
	else {
		money -= 100;
		printf("%s %i has given Cashier %i $100.\n", myType, SSN, myLine);
		cashierCV[myLine]->Signal(cashierLock[myLine]);
		/* //Wait for clerk to give passport */
		cashierCV[myLine]->Wait(cashierLock[myLine]);
	}
	/* //Done */
	cashierLock[myLine]->Release();
	/* //Done Return */
	return true;



}/* //End of customerCashierInteraction */








/*
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
*/
/* /Senator: */
/* 
// Senators, being special people who need their privacy and security, get to use the Passport Office all by themselves. 
	// When a Senator shows up, they wait until all Customers currently being serviced are handled. 
	// This means only the Customers currently with a Clerk are to be handled. 
	// All other Customers must "leave" the Passport Office. 
	// Customers must remember the type of line they are in, but they have to get back in line once the Senator leaves.
	// Once the current Customers have been serviced, the Senator gets to go to each Clerk type and get their passport. 
	// Any Cusotmer showing up when a Senator is present must wait "outside" - a different line altogether.
// Once a Senator is finished, all waiting Customers get to proceed as normal.
*/
void senatorArriveAtPassportOffice();
void senatorLeavePassportOffice(int SSN);
void Senator(int id){
	int SSN = id;
	int myLine = -1;
	int money = (rand()%4)*500 + 100;
	bool appClerkDone = false; //State vars
	bool pictureClerkDone = false;
	bool passportClerkDone = false;
	bool cashierDone = false; 
	bool appClerkFirst = rand() % 2;
	int i;
	int j;


	/* //Check in */
	senatorArriveAtPassportOffice();

	/* //Safe to do 'normal' interactions now.... */

	while(!cashierDone){

		if( !(appClerkDone) && (appClerkFirst || pictureClerkDone) ){ /* //Go to applicationClerk */
			appClerkDone = customerApplicationClerkInteraction(SSN, money, 1);
		}
		else if( !pictureClerkDone ){
			/* //Go to the picture clerk */
			pictureClerkDone = customerPictureClerkInteraction(SSN, money, 1);
		}
		else if(!passportClerkDone){
			passportClerkDone = customerPassportClerkInteraction(SSN, money, 1);
			if (!passportClerkDone) { for (i = 0; i < rand() % 901 + 100; i++) { currentThread->Yield(); } }
		}
		else if(!cashierDone){
			cashierDone = customerCashierInteraction(SSN, money, 1);
			if (!cashierDone) {	for (j = 0; j < rand() % 901 + 100; j++) { currentThread->Yield(); } }
		}
	}

	/* //Done lets leave */
	senatorLeavePassportOffice(SSN);

/* //Here are the output Guidelines for the Senator */
	if(false){
	printf("Senator %i has gotten in regular line for ApplicationClerk %i.\n", SSN, myLine);
	printf("Senator %i has gotten in regular line for PictureClerk %i.\n", SSN, myLine);
	printf("Senator %i has gotten in regular line for PassportClerk %i.\n", SSN, myLine);
	printf("Senator %i has gotten in regular line for Cashier %i.\n", SSN, myLine);
	printf("Senator %i has given SSN %i to ApplicationClerk %i.\n", SSN, SSN, myLine);
	printf("Senator %i has given SSN %i to PictureClerk %i.\n", SSN, SSN, myLine);
	printf("Senator %i has given SSN %i to PassportClerk %i.\n", SSN, SSN, myLine);
	printf("Senator %i has given SSN %i to Cashier %i.\n", SSN, SSN, myLine);
	printf("Senator %i does not like their picture from PictureClerk %i.\n", SSN, myLine);
	printf("Senator %i does like their picture from PictureClerk %i.\n", SSN, myLine);
	printf("Senator %i has gone to PassportClerk %i too soon. They are going to the back of the line.\n", SSN, myLine);
	printf("Senator %i has gone to Cashier %i too soon. They are going to the back of the line.\n", SSN, myLine);
	printf("Senator %i has given Cashier %i $100.", SSN, myLine);
	}

} /* //End Senator */

/*
//Called when the senator gets to the passport office...
//	tells the manager they are here
//	waits if there are customers
*/
void senatorArriveAtPassportOffice(){
	/*
	//Talk to manager tell them I've arrived.
	//see if senators are here.
	*/
	managerLock->Acquire();
	while(!senatorSafeToEnter){/* /Wait in the senator line for customers to leave */
		senatorLineCount++;
		senatorLineCV->Wait(managerLock);
		senatorLineCount--;
	}
	senatorPresentCount++;
	managerLock->Release();
}

void senatorLeavePassportOffice(int SSN){
	/* //Done lets leave */
	managerLock->Acquire();
	senatorPresentCount--;
	checkedOutCount++;
	managerLock->Release();
	printf("Senator %i is leaving the Passport Office.\n", SSN);
}
















/*
//////////////////////////////////////////////
///// CLERKS
//////////////////////////////////////////////
*/






/*
//This may be necessary to check for race conditions while a senator is waiting outside
// Before the customer leaves their line the clerk might think they are able to call them
// This may not be necessary idk
*/
bool clerkCheckForSenator(){
	/* //DEBUG('s', "DEBUG: Clerk bout to check for senator.\n"); */
	int i;
	managerLock->Acquire();
	if(senatorPresentWaitOutSide && !senatorSafeToEnter){
		managerLock->Release();
		/*
		//Lets just wait a bit...
		//printf("DEBUG: Clerk yielding for senator.\n");
		*/
		for(i = 0; i < rand()%780 + 20; i++) { currentThread->Yield(); }
		return true;
	}
	managerLock->Release();
	return false;
}





/* //ApplicationClerk - an application clerk accepts a completed Application. */
/*
// A completed Application requires an "completed" application and a Customer "social security number".
// You can assume that the Customer enters the passport office with a completed passport application. 
// The "social security number" can be a random number, or a sequentially increasing number. 
// In any event, it must be a unique number for each Customer.
//
// PassportClerks "record" that a Customer has a completed application. 
// The Customer must pass the application to the PassportClerk. 
// This consists of giving the ApplicationClerk their Customer "social security number" - their personal number.
// The application is assumed to be passed, it is not explicitly provided in the shared data between the 2 threads.
// The ApplicationClerk then "records" that a Customer, with the provided social security number, has a completed application. 
	//This information is used by the PassportClerk. Customers are told when their application has been "filed".
// Any money received from Customers wanting to move up in line must be added to the ApplicationClerk received money amount.
// ApplicationClerks go on break if they have no Customers in their line.
// There is always a delay in an accepted application being "filed".
// This is determined by a random number of 'currentThread->Yield() calls - the number is to vary from 20 to 100.
*/
void applicationClerkcheckAndGoOnBreak(int myLine); /* //Too many of these forward declarations */
void ApplicationClerk(int id){
	int i;
	int myLine = id;
	int money = 0;
	int customerFromLine;/* //0 no line, 1 bribe line, 2 regular line */
	/* //Keep running */
	while(true){

		if(clerkCheckForSenator()) continue; /* //Waiting for senators to enter just continue. */

		applicationClerkLineLock->Acquire();

		/* //If there is someone in my bribe line */
		if(applicationClerkBribeLineCount[myLine] > 0){
			customerFromLine = 1;
			applicationClerkBribeLineCV[myLine]->Signal(applicationClerkLineLock);
			applicationClerkState[myLine] = SIGNALEDCUSTOMER;
		}else if(applicationClerkLineCount[myLine] > 0){/* //if there is someone in my regular line */
			customerFromLine = 2;
			applicationClerkLineCV[myLine]->Signal(applicationClerkLineLock);
			applicationClerkState[myLine] = SIGNALEDCUSTOMER;
		}else{
			/*
			//No Customers
			//Go on break if there is another clerk
			*/
			customerFromLine = 0;
			applicationClerkcheckAndGoOnBreak(myLine);
			applicationClerkLineLock->Release();
		}

		/* //Should only do this when we have a customer... */
		if(customerFromLine != 0){
			printf("ApplicationClerk %i has signalled a Customer to come to their counter.\n", myLine);
			applicationClerkLock[myLine]->Acquire();
			applicationClerkLineLock->Release();
			/* //wait for customer data */
			applicationClerkCV[myLine]->Wait(applicationClerkLock[myLine]);
			/* 
			//Customer Has given me their SSN?
			//And I have a lock
			*/
			int customerSSN = applicationClerkSharedData[myLine];
			
			/* //Customer from bribe line? //maybe should be separate signalwait  ehh? */
			if(customerFromLine == 1){
				money += 500;
				printf("ApplicationClerk %i has received $500 from Customer %i.\n", myLine, customerSSN);
				currentThread->Yield();/* //Just to change things up a bit. */
			}
			

			printf("ApplicationClerk %i has received SSN %i from Customer %i.\n", myLine, customerSSN, customerSSN);
			
			/* //Signal Customer that I'm Done. */
			applicationClerkCV[myLine]->Signal(applicationClerkLock[myLine]);
			applicationClerkLock[myLine]->Release();
			
			/* //yield for filing time */
			for(i = 0; i < rand()%81 + 20; i++) { currentThread->Yield(); }
			
			/* //TODO: NEED TO ACQUIRE A LOCK FOR THIS!! */
			applicationCompletion[customerSSN] = 1;
			printf("ApplicationClerk %i has recorded a completed application for Customer %i.\n", myLine, customerSSN);
		}/* //end if have customer */

	}

}/* //End ApplicationClerk */

/*
//Utility for applicationClerk to gon on brak
// Assumptions: called with clerkLineLock
*/
void applicationClerkcheckAndGoOnBreak(int myLine){
	/* //Only go on break if there is at least one other clerk */
	int i;
	bool freeOrAvailable = false;
	for(i = 0; i < CLERKCOUNT; i++){
		if(i != myLine && ( applicationClerkState[i] == AVAILABLE || applicationClerkState[i] == BUSY ) ){
			freeOrAvailable = true;
			break;
		}
	}
	/* //There is at least one clerk...go on a break. */
	if(freeOrAvailable){
		applicationClerkState[myLine] = ONBREAK;
		printf("ApplicationClerk %i is going on break.\n", myLine);
		applicationClerkBreakCV->Wait(applicationClerkLineLock);
		applicationClerkState[myLine] = BUSY;
		printf("ApplicationClerk %i is coming off break.\n", myLine);
	}else{
		/*
		//If everyone is on break...
		//applicationClerkState[myLine] = AVAILABLE;
		*/
		applicationClerkLineLock->Release();
		/* //Should we go to sleep? */
		managerLock->Acquire();/* //Do we really need to acquire a lock for this? */
		if(checkedOutCount == (CUSTOMERCOUNT + SENATORCOUNT)){managerLock->Release(); currentThread->Finish();}
		managerLock->Release();/* //Guess not */
		currentThread->Yield();
		applicationClerkLineLock->Acquire();
	}
	/* //applicationClerkState[myLine] = AVAILABLE; */
}





/* //PictureClerk - they take pictures of Customers. */
/*
	//Customers can turn in their application or get their picture taken in any order. 
	//This is to be randomly determined by each customer when they are executing. 
//
// PictureClerks "take" the picture of a Customer. Customers are "shown" their picture. 
	// They have some percentage chance of not liking their picture.
	// You get to decide what that percentage is. It cannot be zero. 
	// Once a Customer accepts their picture, this "official" picture is filed by the PictureClerk.
	// Customers are told when this has been completed.
// Any money received from Customers wanting to move up in line must be added to the PictureClerk received money amount.
// PictureClerks go on break if they have no Customers in their line.
// There is always a delay in an accepted picture being "filed". 
// This is determined by a random number of 'currentThread->Yield() calls - the number is to vary from 20 to 100.
*/
void pictureClerkcheckAndGoOnBreak(int myLine);
void PictureClerk(int id){
		int i;
		int myLine = id;
		int money = 0;
		int identifier = -1; /* //TODO: REMOVE THIS!!! */
		int customerFromLine;/* //0 no line, 1 bribe line, 2 regular line */
		/* //Keep running */
		while(true){
	
			if(clerkCheckForSenator()) continue; /* //Waiting for senators to enter just continue. */

			pictureClerkLineLock->Acquire();
			DEBUG('p', "DEBUG: PictureClerk %i pICTURECLERKLINELOCK acquired top of while\n", myLine);

			/* //If there is someone in my bribe line */
			if(pictureClerkBribeLineCount[myLine] > 0){
				customerFromLine = 1;
				pictureClerkBribeLineCV[myLine]->Signal(pictureClerkLineLock);
				pictureClerkState[myLine] = SIGNALEDCUSTOMER;
			}else if(pictureClerkLineCount[myLine] > 0){/* //if there is someone in my regular line */
				customerFromLine = 2;
				pictureClerkLineCV[myLine]->Signal(pictureClerkLineLock);
				pictureClerkState[myLine] = SIGNALEDCUSTOMER;
			}else{
				/* //Go on a break! */
				customerFromLine = 0;
				pictureClerkcheckAndGoOnBreak(myLine);
				pictureClerkLineLock->Release();
				DEBUG('p', "DEBUG: PictureClerk %i pICTURECLERKLINELOCK released after checkbreak.\n", myLine);
			}

			/* //Should only do this when we are BUSY? We have a customer... */
			if(customerFromLine != 0){
				printf("PictureClerk %i has signalled a Customer to come to their counter.\n", myLine);
				pictureClerkLock[myLine]->Acquire();
			 	/* //pictureClerkSharedDataPicture[myLine] = 0; */
				pictureClerkLineLock->Release();
				DEBUG('p', "DEBUG: PictureClerk %i pICTURECLERKLINELOCK released after signalling customer.\n", myLine);
				/* //wait for customer data */
				pictureClerkCV[myLine]->Wait(pictureClerkLock[myLine]);
				/*
				//Customer Has given me their SSN?
				//And I have a lock
				*/
				int customerSSN = pictureClerkSharedDataSSN[myLine];
				/* //Customer from bribe line? //maybe should be separate signalwait  ehh? */
			if(customerFromLine == 1){
				money += 500;
				printf("PictureClerk %i has received $500 from Customer %i.\n", myLine, customerSSN);
				currentThread->Yield();/* //Just to change things up a bit. */
			}
			
				printf("PictureClerk %i has received SSN %i from Customer %i.\n", myLine, customerSSN, customerSSN);
				pictureClerkSharedDataPicture[myLine] = 0;
				while(pictureClerkSharedDataPicture[myLine] == 0) {
					/* //Taking picture */
					printf("PictureClerk %i has taken a picture of Customer %i.\n", myLine, customerSSN);
					/* //Signal Customer that I'm Done and show them the picture. Then wait for response. */
					pictureClerkCV[myLine]->Signal(pictureClerkLock[myLine]);
					pictureClerkCV[myLine]->Wait(pictureClerkLock[myLine]);
					if(pictureClerkSharedDataPicture[myLine] == 0)  {
						printf("PictureClerk %i has has been told that Customer %i does not like their picture.\n", myLine, customerSSN);
					}

				}
				printf("PictureClerk %i has has been told that Customer %i does like their picture.\n", myLine, customerSSN);
				/*
				//Yield before submitting.
				//Signal Customer that I'm Done.
				*/
				pictureClerkCV[myLine]->Signal(pictureClerkLock[myLine]);
				for(i = 0; i < rand()%81 + 20; i++) { currentThread->Yield(); }
				printf("PictureClerk %i has recorded a completed picture for Customer %i.\n", myLine, customerSSN);
				pictureCompletion[customerSSN] = 1;


				pictureClerkLock[myLine]->Release();
			}
	
		}

}/* //End PictureClerk */

/*
//Utility for applicationClerk to gon on brak
// Assumptions: called with clerkLineLock
*/
void pictureClerkcheckAndGoOnBreak(int myLine){
	/* //Only go on break if there is at least one other clerk */
	int i;
	bool freeOrAvailable = false;
	for(i = 0; i < CLERKCOUNT; i++){
		if(i != myLine && ( pictureClerkState[i] == AVAILABLE || pictureClerkState[i] == BUSY ) ){
			freeOrAvailable = true;
			break;
		}
	}
	/* //There is at least one clerk...go on a break. */
	if(freeOrAvailable){
		pictureClerkState[myLine] = ONBREAK;
		printf("PictureClerk %i is going on break.\n", myLine);
		pictureClerkBreakCV->Wait(pictureClerkLineLock);
		pictureClerkState[myLine] = BUSY;
		printf("PictureClerk %i is coming off break.\n", myLine);
	}else{
		/* //If everyone is on break... */
		/* //applicationClerkState[myLine] = AVAILABLE; */
		pictureClerkLineLock->Release();
		DEBUG('p', "DEBUG: PictureClerk %i pICTURECLERKLINELOCK released middle of checkbreak\n", myLine);
		/* //Should we GO TO SLEEP? */
		managerLock->Acquire();/* //Do we really need to acquire a lock for this? */
		if(checkedOutCount == (CUSTOMERCOUNT + SENATORCOUNT)){managerLock->Release(); currentThread->Finish();}
		managerLock->Release();/* //Guess not */
		currentThread->Yield();
		pictureClerkLineLock->Acquire();
		DEBUG('p', "DEBUG: PictureClerk %i pICTURECLERKLINELOCK acquired end of checkbreak\n", myLine);

	}
	/* //applicationClerkState[myLine] = AVAILABLE; */
}








/* // PassportClerk - they check that a Customer has filed their completed application and has an official picture taken. */
/*
//
// PassportClerks will "certify" a Customer ONLY if the Customer has a filed application and picture. 
	// If a Customer shows up to the PassportClerk BEFORE both documenets are filed,
	// they they (the Customer) are punished by being forec to wait for some arbitrary amount of time.
	// This is to be from 100 to 1000 currentThread->Yield() calls.
	// After these calls are completed, ,the Customer goes to the back of the PassportClerk line.
	// NOTE It takes time for a PassportClerk to "record" a Customer's completed documents. 
*/
void passportClerkcheckAndGoOnBreak(int myLine);
void PassportClerk(int id){
	int i;
	int myLine = id;
	int money = 0;
	int identifier = -1; /* //TODO: REMOVE THIS SORRY FOR ADDING THESE */
	int customerFromLine;/* //0 no line, 1 bribe line, 2 regular line */
		
	/* //Keep running */
	while(true){

		if(clerkCheckForSenator()) continue; /* //Waiting for senators to enter just continue. */

		passportClerkLineLock->Acquire();
		/* //If there is someone in my bribe line */
		if(passportClerkBribeLineCount[myLine] > 0){
			customerFromLine = 1;
			passportClerkBribeLineCV[myLine]->Signal(passportClerkLineLock);
			passportClerkState[myLine] = SIGNALEDCUSTOMER;
		}else if(passportClerkLineCount[myLine] > 0){/* //if there is someone in my regular line */
			customerFromLine = 2;
			passportClerkLineCV[myLine]->Signal(passportClerkLineLock);
			passportClerkState[myLine] = SIGNALEDCUSTOMER;
		}else{
			customerFromLine = 0;
			passportClerkcheckAndGoOnBreak(myLine);
			passportClerkLineLock->Release();
		}

		/* //Should only do this when we are BUSY? We have a customer... */
		if(customerFromLine != 0){
			printf("PassportClerk %i has signalled a Customer to come to their counter.\n", myLine);
			passportClerkLock[myLine]->Acquire();
			passportClerkLineLock->Release();
			/* //wait for customer data */
			passportClerkCV[myLine]->Wait(passportClerkLock[myLine]);
			/*
			//Customer Has given me their SSN?
			//And I have a lock
			*/
			int customerSSN = passportClerkSharedDataSSN[myLine];
			/* //Customer from bribe line? //maybe should be separate signalwait  ehh? */
			if(customerFromLine == 1){
				money += 500;
				printf("PassportClerk %i has received $500 from Customer %i.\n", myLine, customerSSN);
				currentThread->Yield();/* //Just to change things up a bit. */
			}
			
			printf("PassportClerk %i has received SSN %i from Customer %i.\n", myLine, customerSSN, customerSSN);
			
			/* //Do my job - customer waiting */
			if(!(applicationCompletion[customerSSN] == 1 && pictureCompletion[customerSSN] == 1)) {
				passportPunishment[customerSSN] = 1;
				printf("PassportClerk %i has determined that Customer %i does not have both their application and picture completed.\n", myLine, customerSSN);
				passportClerkCV[myLine]->Signal(passportClerkLock[myLine]);
			}
			else {
				passportPunishment[customerSSN] = 0;
				printf("PassportClerk %i has determined that Customer %i has both their application and picture completed.\n", myLine, customerSSN);
				passportCompletion[customerSSN] = true;
				/* //Signal Customer that I'm Done. */
				passportClerkCV[myLine]->Signal(passportClerkLock[myLine]);
				for(i = 0; i < rand()%81 + 20; i++) { currentThread->Yield(); }
				printf("PassportClerk %i has recorded Customer %i passport documentation.\n", myLine, customerSSN);
			}

			passportClerkLock[myLine]->Release();
		}

	}
	

	/* //Here are the output Guidelines for the PassportClerk */
	if(false){
	printf("PassportClerk %i is going on break.\n", myLine);
	printf("PassportClerk %i is coming off break.\n", myLine);
	}

}/* //End PassportClerk */


/*
//Utility for passportClerk to gon on brak
// Assumptions: called with clerkLineLock
*/
void passportClerkcheckAndGoOnBreak(int myLine){
	/* //Only go on break if there is at least one other clerk */
	int i;
	bool freeOrAvailable = false;
	for(i = 0; i < CLERKCOUNT; i++){
		if(i != myLine && ( passportClerkState[i] == AVAILABLE || passportClerkState[i] == BUSY ) ){
			freeOrAvailable = true;
			break;
		}
	}
	/* //There is at least one clerk...go on a break. */
	if(freeOrAvailable){
		passportClerkState[myLine] = ONBREAK;
		printf("PassportClerk %i is going on break.\n", myLine);
		passportClerkBreakCV->Wait(passportClerkLineLock);
		passportClerkState[myLine] = BUSY;
		printf("PassportClerk %i is coming off break.\n", myLine);
	}else{
		/*
		//If everyone is on break...
		//passportClerkState[myLine] = AVAILABLE;
		*/
		passportClerkLineLock->Release();
		/* /Should we go to sleep? */
		managerLock->Acquire();/* //Do we really need to acquire a lock for this? */
		if(checkedOutCount == (CUSTOMERCOUNT + SENATORCOUNT)){managerLock->Release(); currentThread->Finish();}
		managerLock->Release();/* //Guess not */
		currentThread->Yield();
		passportClerkLineLock->Acquire();
	}
	/* //passportClerkState[myLine] = AVAILABLE; */
}






/* 
// Cashier - Once a PassportClerk has "certified" that a proper application has been completed, 
	the Cashier will accept the $100 necessary to pay for the new passport.
*/
/*
// Cashiers take the $100 passport fee from Customer 
	// - ONLY after they see a "certification" by the PassportClerk.
	// If a Customer tries to pay for their passport BEFORE they are certified by the PassportClerk,
	//  then they are punished by being forced to wait for some arbitrary amount of time.
	// This is to be from 100 to 1000 curerntThread->Yield() calls. 
	// After these calls are completed, the Customer goes to the back of a Cashier line.
// Once a Cashier accepts a Customers money, they "provide" the completed passport.
	// The Cashier records that a passport has been given to the Customer, 
	// so that multiple passports cannot be handed out to the same Customer.
//Any money received from Customers. 
	// Whether the standard application fee,
	// or from those Customers wanting to move up in line must be added to the Cashier received money amount.
// Cashiers go on break if they have no Customers in their line
*/
void cashiercheckAndGoOnBreak(int myLine);
void Cashier(int id){

	int myLine = id;
	int money = 0;
	int identifier = -1; /* //TODO: REMOVE THIS SORRY FOR ADDING THESE */
	int customerFromLine;/* //0 no line, 1 bribe line, 2 regular line */
		
	/* //Keep running */
	while (true){

		if(clerkCheckForSenator()) continue; /* //Waiting for senators to enter just continue. */

		cashierLineLock->Acquire();

		/* //If there is someone in my bribe line */
		if (cashierBribeLineCount[myLine] > 0){
			customerFromLine = 1;
			cashierBribeLineCV[myLine]->Signal(cashierLineLock);
			cashierState[myLine] = SIGNALEDCUSTOMER;
		}
		else if (cashierLineCount[myLine] > 0){/* //if there is someone in my regular line */
			customerFromLine = 2;
			cashierLineCV[myLine]->Signal(cashierLineLock);
			cashierState[myLine] = SIGNALEDCUSTOMER;
		}
		else{
			customerFromLine = 0;
			cashiercheckAndGoOnBreak(myLine);
			cashierLineLock->Release();
		}

		/* //Should only do this when we are BUSY? We have a customer... */
		if (customerFromLine != 0){
			printf("Cashier %i has signalled a Customer to come to their counter.\n", myLine);
			cashierLock[myLine]->Acquire();
			cashierLineLock->Release();
			/* //wait for customer data */
			cashierCV[myLine]->Wait(cashierLock[myLine]);
			/*
			//Customer Has given me their SSN?
			//And I have a lock
			*/
			int customerSSN = cashierSharedDataSSN[myLine];
			/* //Customer from bribe line? //maybe should be separate signalwait  ehh? */
			if(customerFromLine == 1){
				money += 500;
				printf("Cashier %i has received $500 from Customer %i.\n", myLine, customerSSN);
				currentThread->Yield();/* //Just to change things up a bit. */
			}
			
			printf("Cashier %i has received SSN %i from Customer %i.\n", myLine, customerSSN, customerSSN);

			/* //Do my job - customer waiting */
			if (passportCompletion[customerSSN] == 0) {
				printf("Cashier %i has received the $100 from Customer%i before certification. They are to go to the back of my line.\n", myLine, customerSSN);
				cashierRejection[customerSSN] = 1;
			}
			else {
				cashierRejection[customerSSN] = 0;
				printf("Cashier %i has verified that Customer %i has been certified by a PassportClerk.\n", myLine, customerSSN);
				/* //tell customer and wait to be paid */
				cashierCV[myLine]->Signal(cashierLock[myLine]);
				cashierCV[myLine]->Wait(cashierLock[myLine]);
				printf("Cashier %i has received the $100 from Customer%i after certification.\n", myLine, customerSSN);
				/* //Signal Customer that I'm Done. */
				doneCompletely[customerSSN] = 1;
				printf("Cashier %i has provided Customer %i their completed passport.\n", myLine, customerSSN);
				cashierCV[myLine]->Signal(cashierLock[myLine]);
		
			}
			cashierLock[myLine]->Release();
		}

	}


	/* //Here are the output Guidelines for the Cashier */
	if(false){
	printf("Cashier %i has signalled a Customer to come to their counter.\n", myLine);
	printf("Cashier %i has received SSN %i from Customer %i.\n", myLine, identifier, identifier);
	printf("Cashier %i has verified that Customer %i has been certified by a PassportClerk.\n", myLine, identifier);
	printf("Cashier %i has received the $100 from Customer %i after certification.\n", myLine, identifier);
	printf("Cashier %i has recorded the $100 from Customer %i before certification. They are to go to the back of my line.\n", myLine, identifier);
	printf("Cashier %i has provided Customer %i their completed passport.\n", myLine, identifier);
    printf("Cashier %i has recorded that Customer %i has been given their completed passport.\n", myLine, identifier);
	printf("Cashier %i is going on break.\n", myLine);
	printf("Cashier %i is coming off break.\n", myLine);
	}



}/* //End Cashier */



/*
//Utility for cashier to gon on brak
// Assumptions: called with clerkLineLock
*/
void cashiercheckAndGoOnBreak(int myLine){
	/* //Only go on break if there is at least one other clerk */
	int i;
	bool freeOrAvailable = false;
	for(i = 0; i < CLERKCOUNT; i++){
		if(i != myLine && ( cashierState[i] == AVAILABLE || cashierState[i] == BUSY ) ){
			freeOrAvailable = true;
			break;
		}
	}
	/* //There is at least one clerk...go on a break. */
	if(freeOrAvailable){
		cashierState[myLine] = ONBREAK;
		printf("Cashier %i is going on break.\n", myLine);
		cashierBreakCV->Wait(cashierLineLock);
		cashierState[myLine] = BUSY;
		printf("Cashier %i is coming off break.\n", myLine);
	}else{
		/*
		//If everyone is on break...
		//cashierState[myLine] = AVAILABLE;
		*/
		cashierLineLock->Release();
		/* //Should we go to sleep? Check end of day */
		managerLock->Acquire();/* //Do we really need to acquire a lock for this? */
		if(checkedOutCount == (CUSTOMERCOUNT + SENATORCOUNT)){managerLock->Release(); currentThread->Finish();}
		managerLock->Release();/* //Guess not */
		currentThread->Yield();
		cashierLineLock->Acquire();
	}
	/* //cashierState[myLine] = AVAILABLE; */
}


/*
////////////////////////////////////////////////
////////////////////////////////////////////////
// Manager
////////////////////////////////////////////////
*/
/*
// Managers tell the various Clerks when to start working, when lines get too long. 
//
// Clerks go on break when they have no one in their line.
	// Managers are responsible for waking up Clerks when more than 3 Customers get in any particular line.
// Managers are also responsible for keeping track of how much money the office has made.
	// You are to print the total received from each clerk type, and a total from all clerks.
	// This is to be printed on a fairly regular basis.
*/
void managerCheckandWakupClerks();/* //Forward declaration..? */
void checkEndOfDay();
void managerCountMoney();
void managerSenatorCheck();
void Manager(int id){
	
	int i;
	/* //Until End of Simulation */
	while(true){
		for(i = 0; i < 1000; i++) { 
		

			/* //SENATORS */
			managerSenatorCheck();

			/*
			//Check Lines Wake up Clerk if More than 3 in a line.
			//Check ApplicationClerk
			*/
			managerCheckandWakupClerks();

			/* //Check if all the customers are gone and let all the clerks go home */
			checkEndOfDay();

			currentThread->Yield(); 
			currentThread->Yield(); 
			currentThread->Yield(); 
		}/* //Let someone else get the CPU */
		
		/* //Count and print money */
		managerCountMoney();
	}

}/* //End Manager */




/* //Wake up customers in all lines */
/* void managerBroacastLine(std::vector<Condition*> &line, std::vector<Condition*> &bribeLine, Lock* lock, int count){ */
void managerBroacastLine(Condition* &line[], Condition* &bribeLine[], Lock* lock, int count){
	int i;
	DEBUG('s', "DEBUG: MANAGER: BROADCAST acquiring lock %s.\n", lock->getName());
	lock->Acquire();
	DEBUG('s', "DEBUG: MANAGER: BROADCAST acquired lock %s.\n", lock->getName());
	for(i = 0; i < count; i++){
		line[i]->Broadcast(lock);
		bribeLine[i]->Broadcast(lock);
	}
	lock->Release();
	DEBUG('s', "DEBUG: MANAGER: BROADCAST released lock %s.\n", lock->getName());
}

void managerBroadcastCustomerLines(){
	/* //Wake up all customers in line. So they can go outside */

	/* //App clerks */
	managerBroacastLine(applicationClerkLineCV, applicationClerkBribeLineCV, applicationClerkLineLock, CLERKCOUNT);
	DEBUG('s', "DEBUG: MANAGER: FINISHED BROADCAST to applicaiton lines.\n");
	/* //Picture clerks */
	managerBroacastLine(pictureClerkLineCV, pictureClerkBribeLineCV, pictureClerkLineLock, CLERKCOUNT);
	DEBUG('s', "DEBUG: MANAGER: FINISHED BROADCAST to picture lines.\n");
	/* //Passport Clerks */
	managerBroacastLine(passportClerkLineCV, passportClerkBribeLineCV, passportClerkLineLock, CLERKCOUNT);
	DEBUG('s', "DEBUG: MANAGER: FINISHED BROADCAST to passport lines.\n");
	/* //Cashiers */
	managerBroacastLine(cashierLineCV, cashierBribeLineCV, cashierLineLock, CLERKCOUNT);
	DEBUG('s', "DEBUG: MANAGER: FINISHED BROADCAST to cashier lines.\n");
}

/* //Checks if a sentor is present...does something */
void managerSenatorCheck(){
	/* //customersPresentCount */
	bool senatorWaiting;
	bool senatorsInside;
	bool customersInside;
	bool customersOutside;

	
	managerLock->Acquire();

	senatorWaiting = (senatorLineCount > 0);
	senatorsInside = (senatorPresentCount > 0);
	customersInside = (customersPresentCount > 0);
	customersOutside = (passportOfficeOutsideLineCount > 0);

	/* //See if a senator is waiting in line... */
	if(senatorWaiting){
		if(!senatorPresentWaitOutSide){ DEBUG('s', "DEBUG: MANAGER NOTICED A SENATOR!.\n"); }
		senatorPresentWaitOutSide = true;

		/* //Wake up customers in line so they go outside. */
		if(customersInside){
			DEBUG('s', "DEBUG: MANAGER CUSTOMER PRESENT COUNT: %i.\n", customersPresentCount);
			managerLock->Release();
			managerBroadcastCustomerLines();
			DEBUG('s', "DEBUG: MANAGER: FINISHED BROADCAST to customers.\n");
			return;
		}
	}
	
	if(senatorWaiting && !customersInside){
		if(true || !senatorSafeToEnter) DEBUG('s', "DEBUG: MANAGER: SENATORS SAFE TO ENTER.\n");
		senatorSafeToEnter = true;
		senatorLineCV->Broadcast(managerLock);
		DEBUG('s', "DEBUG: MANAGER: FINISHED BROADCAST to senators.\n");
	}

	if(senatorSafeToEnter && !senatorWaiting && !senatorsInside){
		if(senatorSafeToEnter){DEBUG('s', "DEBUG: SENATORS GONE CUSTOMERS COME BACK IN!.\n");}
		senatorSafeToEnter = false;
		senatorPresentWaitOutSide = false;
		passportOfficeOutsideLineCV->Broadcast(managerLock);
	}


	managerLock->Release();
}/* //End managerSenatorCheck */


/* //This will put the clerks and the manager to sleep so everyone can go to sleep and nachos can clean up */
void checkEndOfDay(){
	managerLock->Acquire();

	if (checkedOutCount == (CUSTOMERCOUNT + SENATORCOUNT)){
		/*
		//DEBUG('s', "DEBUG: MANAGER: END OF DAY!\n");
		//All the customers are gone
		//Lets all go to sleep
		*/
		THEEND = true;
		managerLock->Release();
		/* //finish all clerk threads */



		currentThread->Finish();
	}
	managerLock->Release();
}

/* //Utilities for Manager */
/*
// managerCheckandWakeupCLERK
// checks if a line has more than 3 customers... 
// if so, signals a clerk on break
// Returns true if there was asleeping clerk and needed to wake one up
*/

/* bool managerCheckandWakeupCLERK(Lock* managerCWCLineLock, std::vector<int>& managerCWClineCount, std::vector<CLERKSTATE>& managerCWCState, Condition* managerCWCBreakCV, int managerCWCount){ */
bool managerCheckandWakeupCLERK(Lock* managerCWCLineLock, int& managerCWClineCount[], int& managerCWCState[], Condition* managerCWCBreakCV, int managerCWCount){
	int i;
	bool wakeUp = false;/* //should we wake up a clerk? */
	bool asleep = false;/* //is any clerk asleep? */
	managerCWCLineLock->Acquire();
	for(i = 0; i < managerCWCount; i++){
		if(managerCWCState[i] == ONBREAK)
			asleep = true;
		if(managerCWClineCount[i] > 3)
			wakeUp = true;
	}
	if(wakeUp && asleep){managerCWCBreakCV->Signal(managerCWCLineLock);}
	managerCWCLineLock->Release();
	return asleep && wakeUp;
}


/* //managerCheckandWakupClerks() */
/* //Checks all types of clerks for lines longer than 3 and wakes up a sleaping clerk if there is one */
void managerCheckandWakupClerks(){
	/* //Check Application Clerks */
	if(managerCheckandWakeupCLERK(applicationClerkLineLock, applicationClerkLineCount, applicationClerkState, applicationClerkBreakCV, CLERKCOUNT)){
		printf("Manager has woken up an ApplicationClerk.\n");
	}

	/* //Check Picture Clerks */
	if(managerCheckandWakeupCLERK(pictureClerkLineLock, pictureClerkLineCount, pictureClerkState, pictureClerkBreakCV, CLERKCOUNT)){
		printf("Manager has woken up a PictureClerk.\n");
	}
	
	/* //Check Passport Clerks */
	if(managerCheckandWakeupCLERK(passportClerkLineLock, passportClerkLineCount, passportClerkState, passportClerkBreakCV, CLERKCOUNT)){
		printf("Manager has woken up a PassportClerk.\n");
	}

	/* //Check Cashiers */
	if(managerCheckandWakeupCLERK(cashierLineLock, cashierLineCount, cashierState, cashierBreakCV, CLERKCOUNT)){
		printf("Manager has woken up a Cashier.\n");
	}

}

void managerCountMoney(){
	int total = 0;
	
	
/*	
//	printf("Manager has counted a total of $%i for ApplicationClerks.\n", -1);
//	printf("Manager has counted a total of $%i for PictureClerks.\n", -1);
//	printf("Manager has counted a total of $%i for PassportClerks.\n", -1);
//	printf("Manager has counted a total of $%i for Cashiers.\n", -1);
*/
	printf("Manager has counted a total of $%i for the passport office.\n", total);
}




/*
//End Manager functions
///////////////////////////////
*/







/* //This runs the simulation */
void Part2TestSuit(){

	int n;
	int m;
	int i;
	int j;
	int k;
	char p;
	
	while (true) {
		THEEND = false;
		checkedOutCount = 0;
		valid = false;
		while (valid == false) {
			printf("How many customer threads?\n");
			scanf("\n%i", &CUSTOMERCOUNT);
			if (CUSTOMERCOUNT <= 0 || CUSTOMERCOUNT > 50) {
				printf("That is not a valid value, must be between 1 and 50 \n");
			}
			else {
				valid = true;
			}
		}
		valid = false;
		while (valid == false) {		
			printf("How many clerk threads?\n");
			scanf(" %i", &CLERKCOUNT);
			if (CLERKCOUNT <= 0 || CLERKCOUNT > 5) {
				printf("That is not a valid value, must be between 1 and 5 \n");
			}
			else {
				valid = true;
			}
		}
		valid = false;
		while (valid == false) {
			printf("How many Senator threads?\n");
			scanf("%i", &SENATORCOUNT);
			if (SENATORCOUNT <= 0 || SENATORCOUNT > 10) {
				printf("That is not a valid value, must be between 1 and 10\n");
			}
			else {
				valid = true;
			}
		}

	/* //Initialize Variables */
	senatorPresentWaitOutSide = false;
	senatorSafeToEnter = false;

	applicationClerkLineLock = new Lock("applicationClerkLineLock");	/* //The applicationClerkLineLock */
	pictureClerkLineLock = new Lock("pictureClerkLineLock");	/* //The applicationClerkLineLock */
	passportClerkLineLock = new Lock("passportClerkLineLock");
	cashierLineLock = new Lock("cashierLineLock");

	managerLock = new Lock("managerLock");	/* //Lock for the manager */

	applicationClerkBreakCV = new Condition("applicationClerkBreakCV");/* //To keep track of clerks on break */
	pictureClerkBreakCV = new Condition("pictureClerkBreakCV");/* //To keep track of clerks on break */
	passportClerkBreakCV = new Condition("passportClerkBreakCV");/* //To keep track of clerks on break */
	cashierBreakCV = new Condition("cashierBreakCV");/* //To keep track of clerks on break */
	passportOfficeOutsideLineCV = new Condition("outsideLineCV");/* //To keep track of clerks on break */
	senatorLineCV = new Condition("senatorLineCV");



		printf("Number of Customers = %i \n", CUSTOMERCOUNT);
		printf("Number of ApplicationClerks = %i \n", CLERKCOUNT);
		printf("Number of PictureClerks = %i \n", CLERKCOUNT);
		printf("Number of PassportClerks = %i \n", CLERKCOUNT);
		printf("Number of Cashiers = %i \n", CLERKCOUNT);
		printf("Number of Senators = %i \n", SENATORCOUNT);

		
		//States
		applicationClerkState.clear();
		applicationClerkState.resize(CLERKCOUNT, BUSY);
		pictureClerkState.clear();
		pictureClerkState.resize(CLERKCOUNT, BUSY);
		passportClerkState.clear();
		passportClerkState.resize(CLERKCOUNT, BUSY);
		cashierState.clear();
		cashierState.resize(CLERKCOUNT, BUSY);
		//LineCounts
		applicationClerkLineCount.clear();
		applicationClerkLineCount.resize(CLERKCOUNT, 0);
		applicationClerkBribeLineCount.clear();
		applicationClerkBribeLineCount.resize(CLERKCOUNT, 0);
		pictureClerkLineCount.clear();
		pictureClerkLineCount.resize(CLERKCOUNT, 0);
		pictureClerkBribeLineCount.clear();
		pictureClerkBribeLineCount.resize(CLERKCOUNT, 0);
		passportClerkLineCount.clear();
		passportClerkLineCount.resize(CLERKCOUNT, 0);
		passportClerkBribeLineCount.clear();
		passportClerkBribeLineCount.resize(CLERKCOUNT, 0);
		cashierLineCount.clear();
		cashierLineCount.resize(CLERKCOUNT, 0);
		cashierBribeLineCount.clear();
		cashierBribeLineCount.resize(CLERKCOUNT, 0);
		//Shared Data - Should only be accessed with the corresponding lock or CV
		applicationClerkSharedData.clear();
		applicationClerkSharedData.resize(CLERKCOUNT, 0);
		pictureClerkSharedDataSSN.clear();
		pictureClerkSharedDataSSN.resize(CLERKCOUNT, 0);
		pictureClerkSharedDataPicture.clear();
		pictureClerkSharedDataPicture.resize(CLERKCOUNT, 0);
		passportClerkSharedDataSSN.clear();
		passportClerkSharedDataSSN.resize(CLERKCOUNT, 0);
		applicationCompletion.clear();
		applicationCompletion.resize(CUSTOMERCOUNT + SENATORCOUNT, 0);
		pictureCompletion.clear();
		pictureCompletion.resize(CUSTOMERCOUNT + SENATORCOUNT, 0);
		passportCompletion.clear();
		passportCompletion.resize(CUSTOMERCOUNT + SENATORCOUNT, 0);
		passportPunishment.clear();
		passportPunishment.resize(CUSTOMERCOUNT + SENATORCOUNT, 0);
		cashierSharedDataSSN.clear();
		cashierSharedDataSSN.resize(CUSTOMERCOUNT + SENATORCOUNT, 0);
		cashierRejection.clear();
		cashierRejection.resize(CUSTOMERCOUNT + SENATORCOUNT, 0);
		doneCompletely.clear();
		doneCompletely.resize(CUSTOMERCOUNT + SENATORCOUNT, 0);


		/* //Initialize dynamic variables */
		/*
		for (int i = 0; i < CLERKCOUNT; i++){
			applicationClerkLock.push_back(new Lock("applicationClerkLock" + i));
			applicationClerkLineCV.push_back(new Condition("applicationClerkLineCV" + i));
			applicationClerkBribeLineCV.push_back(new Condition("applicationClerkBribeLineCV" + i));
			applicationClerkCV.push_back(new Condition("applicationClerkCV" + i));

			pictureClerkLock.push_back(new Lock("pictureClerkLock" + i));
			pictureClerkLineCV.push_back(new Condition("picutreClerkLineCV" + i));
			pictureClerkBribeLineCV.push_back(new Condition("pictureClerkBribeLineCV" + i));
			pictureClerkCV.push_back(new Condition("pictureClerkCV" + i));

			passportClerkLock.push_back(new Lock("passportClerkLock" + i));
			passportClerkLineCV.push_back(new Condition("picutreClerkLineCV" + i));
			passportClerkBribeLineCV.push_back(new Condition("passportClerkBribeLineCV" + i));
			passportClerkCV.push_back(new Condition("passportClerkCV" + i));

			cashierLock.push_back(new Lock("cashierLock" + i));
			cashierLineCV.push_back(new Condition("cashierLineCV" + i));
			cashierBribeLineCV.push_back(new Condition("cashierBribeLineCV" + i));
			cashierCV.push_back(new Condition("cashierCV" + i));
		}
		*/
		
		for (n = 0; n < CLERKCOUNT; n++){			
			applicationClerkLock[n] = new Lock("applicationClerkLock" + n));
			applicationClerkLineCV[n] = new Lock("applicationClerkLineCV" + n));
			applicationClerkBribeLineCV[n] = new Lock("applicationClerkBribeLineCV" + n));
			applicationClerkCV[n] = new Lock("applicationClerkCV" + n));
			
			pictureClerkLock[n] = new Lock("pictureClerkLock" + n));
			pictureClerkLineCV[n] = new Lock("pictureClerkLineCV" + n));
			pictureClerkBribeLineCV[n] = new Lock("pictureClerkBribeLineCV" + n));
			pictureClerkCV[n] = new Lock("pictureClerkCV" + n));
			
			passportClerkLock[n] = new Lock("passportClerkLock" + n));
			passportClerkLineCV[n] = new Lock("passportClerkLineCV" + n));
			passportClerkBribeLineCV[n] = new Lock("passportClerkBribeLineCV" + n));
			passportClerkCV[n] = new Lock("passportClerkCV" + n));
			
			cashierLock[n] = new Lock("cashierLock" + n));
			cashierLineCV[n] = new Lock("cashierLineCV" + n));
			cashierBribeLineCV[n] = new Lock("cashierBribeLineCV" + n));
			cashierCV[n] = new Lock("cashierCV" + n));
		}

		Thread *t;

		for(i = 0; i < CUSTOMERCOUNT; i++){
			t = new Thread("Customer " + i);
			t->Fork(Customer, i);
		}
		
		for(j = 0; j < SENATORCOUNT; j++){
			t = new Thread("Senator " + j);
			t->Fork(Senator, CUSTOMERCOUNT + j);
		}



		for (k = 0; k < CLERKCOUNT; k++){
			t = new Thread("ApplicationClerk " + k);
			t->Fork(ApplicationClerk, k);

			t = new Thread("PictureClerk " + k);
			t->Fork(PictureClerk, k);

			t = new Thread("PassportClerk " + k);
			t->Fork(PassportClerk, k);

			t = new Thread("Cashier " + k);
			t->Fork(Cashier, k);
		}

		t = new Thread("Manager");
		t->Fork(Manager, 0);


		while (!THEEND) {
			currentThread->Yield();
		}

		printf("Type q to quit, type c to continue to another simulation.\n");
		scanf(" %c", &p);
		if (p == 'q') {
			printf("Have a nice day! \n");
			return;
		}
		else if (p == 'c') {
			printf("Starting a new simulation. \n");
		}

		/*
		for (int i = 0; i < CLERKCOUNT; i++){
			delete applicationClerkLock.back();
			applicationClerkLock.pop_back();

			delete applicationClerkLineCV.back();
			applicationClerkLineCV.pop_back();
			delete applicationClerkBribeLineCV.back();
			applicationClerkBribeLineCV.pop_back();
			delete applicationClerkCV.back();
			applicationClerkCV.pop_back();


			delete pictureClerkLock.back();
			pictureClerkLock.pop_back();
			delete  pictureClerkLineCV.back();
			pictureClerkLineCV.pop_back();
			delete  pictureClerkBribeLineCV.back();
			pictureClerkBribeLineCV.pop_back();
			delete  pictureClerkCV.back();
			pictureClerkCV.pop_back();
			
			delete passportClerkLock.back();
			passportClerkLock.pop_back();
			delete  passportClerkLineCV.back();
			passportClerkLineCV.pop_back();
			delete  passportClerkBribeLineCV.back();
			passportClerkBribeLineCV.pop_back();
			delete  passportClerkCV.back();
			passportClerkCV.pop_back();
			
			delete cashierLock.back();
			cashierLock.pop_back();
			delete cashierLineCV.back();
			cashierLineCV.pop_back();
			delete cashierBribeLineCV.back();
			cashierBribeLineCV.pop_back();
			delete  cashierCV.back();
			cashierCV.pop_back();
		}
		*/
		
		for (m = 0; m < CLERKCOUNT; m++){			
			applicationClerkLock[m] = NULL;
			applicationClerkLineCV[m] = NULL;
			applicationClerkBribeLineCV[m] = NULL;
			applicationClerkCV[m] = NULL;

			pictureClerkLock[m] = NULL;
			pictureClerkLineCV[m] = NULL;
			pictureClerkBribeLineCV[m] = NULL;
			pictureClerkCV[m] = NULL;
			
			passportClerkLock[m] = NULL;
			passportClerkLineCV[m] = NULL;
			passportClerkBribeLineCV[m] = NULL;
			passportClerkCV[m] = NULL;
			
			cashierLock[m] = NULL;
			cashierLineCV[m] = NULL;
			cashierBribeLineCV[m] = NULL;
			cashierCV[m] = NULL;
		}



	}/* //end of true */
}










#endif//PROJ1_PART2_CC

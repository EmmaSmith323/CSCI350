// nettest.cc 
//	Test out message delivery between two "Nachos" machines,
//	using the Post Office to coordinate delivery.
//
//	Two caveats:
//	  1. Two copies of Nachos must be running, with machine ID's 0 and 1:
//		./nachos -m 0 -o 1 &
//		./nachos -m 1 -o 0 &
//
//	  2. You need an implementation of condition variables,
//	     which is *not* provided as part of the baseline threads 
//	     implementation.  The Post Office won't work without
//	     a correct implementation of condition variables.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"

// Test out message delivery, by doing the following:
//	1. send a message to the machine with ID "farAddr", at mail box #0
//	2. wait for the other machine's message to arrive (in our mailbox #0)
//	3. send an acknowledgment for the other machine's message
//	4. wait for an acknowledgement from the other machine to our 
//	    original message

void
MailTest(int farAddr)
{
    PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
    char *data = "Hello there!";
    char *ack = "Got it!";
    char buffer[MaxMailSize];

    // construct packet, mail header for original message
    // To: destination machine, mailbox 0
    // From: our machine, reply to: mailbox 1
    outPktHdr.to = farAddr;		
    outMailHdr.to = 0;
    outMailHdr.from = 1;
    outMailHdr.length = strlen(data) + 1;

    // Send the first message
    bool success = postOffice->Send(outPktHdr, outMailHdr, data); 

    if ( !success ) {
      printf("The postOffice Send failed. You must not have the other Nachos running. Terminating Nachos.\n");
      interrupt->Halt();
    }

    // Wait for the first message from the other machine
    postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    fflush(stdout);

    // Send acknowledgement to the other machine (using "reply to" mailbox
    // in the message that just arrived
    outPktHdr.to = inPktHdr.from;
    outMailHdr.to = inMailHdr.from;
    outMailHdr.length = strlen(ack) + 1;
    success = postOffice->Send(outPktHdr, outMailHdr, ack); 

    if ( !success ) {
      printf("The postOffice Send failed. You must not have the other Nachos running. Terminating Nachos.\n");
      interrupt->Halt();
    }

    // Wait for the ack from the other machine to the first message we sent.
    postOffice->Receive(1, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    fflush(stdout);

    // Then we're done!
    interrupt->Halt();
}

//server functions
//make sure to add support for servers sending information to other servers, for project 4
//for now, they will be called by their respective syscall name
void networkTest()
{
	//get the network to work with the clients
	PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
    char *data = "Hello there!";
    char *ack = "Got it!";
    char buffer[MaxMailSize];
    
    //the 'data' will end up being the name of the Lock/CV/MV, and/or the syscall name
    int syscall_index = -1;
    int rv = -1;
    
    //set syscall_index to whatever is passed to it by the syscalls in exception.cc
    //syscall_index = part of data that contains the 'syscall index', or 'sysIndex' in exception.cc
    switch (syscall_index)
    {
    	case SC_CreateLock:
			DEBUG('a', "CreateLock syscall.\n");
			rv = CreateLock();
			break;

		case SC_Acquire:
			DEBUG('a', "Acquire syscall.\n");
			rv = Acquire(machine->ReadRegister(4));
			break;

		case SC_Release:
			DEBUG('a', "Release syscall.\n");
			rv = Release(machine->ReadRegister(4));
			break;

		case SC_DestroyLock:
			DEBUG('a', "DestroyLock syscall.\n");
			rv = DestroyLock(machine->ReadRegister(4));
			break;
	}
	
	//write rv back to data and send it back out to the right client

}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//
//	Lock Syscalls
///////////////////////////////////////////////////////////////////////////////////////////
class LockTableEntry{
public:
	Lock* lock;
	AddrSpace* space;
	bool isToBeDeleted;
};
#define lockTableSize 200
BitMap lockTableBitMap(lockTableSize);
LockTableEntry* lockTable[lockTableSize];


bool Lock_Syscall_InputValidation(int lock){
	if(lock < 0 || lock >= lockTableSize){
		printf("Invalid Lock Identifier: %i ", lock);
		return FALSE;
	}

	LockTableEntry* lockEntry = lockTable[lock];

	if(lockEntry == NULL){
		printf("Lock %i does not exist. ", lock);
		return FALSE;
	}
	if(lockEntry->space != currentThread->space){
		printf("Lock %i does not belong to this process. ", lock);
		return FALSE;
	}
	return TRUE;
}


///////////////////////////////
// Creates the Lock
///////////////////////////////
int CreateLock(){
	DEBUG('L', "In CreateLock_Syscall\n");

	int lockTableIndex = lockTableBitMap.Find();
	if(lockTableIndex == -1){
		printf("Max Number of Locks created. Unable to CreateLock\n");
		return -1;
	}

	LockTableEntry* te = new LockTableEntry();
	te->lock = new Lock("Lock " + lockTableIndex);
	te->space = currentThread->space;
	te->isToBeDeleted = FALSE;

	lockTable[lockTableIndex] = te;

	return lockTableIndex;
}

//changed Acquire_Syscall to return the lock number
/***********************
*	Acquire the lock
*/
int Acquire(int lock){
	DEBUG('L', "In Acquire_Syscall\n");

	if(!Lock_Syscall_InputValidation(lock)){
	 printf("Unable to Acquire.\n");
	 return -1;
	}

	DEBUG('L', "Acquiring lock.\n");
	lockTable[lock]->lock->Acquire();
	return lock;
}

//changed Release_Syscall to return the lock number
/*****************
* 	Release the lock
*/
int Release(int lock){
	DEBUG('L', "In Release_Syscall\n");

	if(!Lock_Syscall_InputValidation(lock)){
	 printf("Unable to Release.\n");
	 return -1;
	}
	
	LockTableEntry* le = lockTable[lock];

	DEBUG('L', "Releasing lock.\n");
	le->lock->Release();
	
	if(le->isToBeDeleted && !(le->lock->isBusy()) ){
		DEBUG('L', "Lock %i no longer busy. Deleting.\n", lock);
		delete le->lock;
		le->lock = NULL;
		delete le;
		lockTable[lock] = NULL;
		lockTableBitMap.Clear(lock);
	}
	return lock;
}

//changed Destroy_Syscall to return the lock number
int DestroyLock_Syscall(int lock){
	DEBUG('L', "In DestroyLock_Syscall\n");

	if(!Lock_Syscall_InputValidation(lock)){
	 printf("Unable to DestroyLock.\n");
	 return -1;
	}

	LockTableEntry* le = lockTable[lock];

	if((le->lock->isBusy()) ){
		le->isToBeDeleted = TRUE;
		DEBUG('L', "Lock %i BUSY marking for deletion.\n", lock);
	}else{
		delete le->lock;
		le->lock = NULL;
		delete le;
		lockTable[lock] = NULL;
		lockTableBitMap.Clear(lock);
		DEBUG('L', "Lock %i deleted.\n", lock);
	}
	return lock;
}





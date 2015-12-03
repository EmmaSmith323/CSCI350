// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include "synch.h" 
#include "bitmap.h"

#include "network.h"
#include "post.h"
#include "interrupt.h"

using namespace std;

int copyin(unsigned int vaddr, int len, char *buf) {
		// Copy len bytes from the current thread's virtual address vaddr.
		// Return the number of bytes so read, or -1 if an error occors.
		// Errors can generally mean a bad virtual address was passed in.
		bool result;
		int n=0;			// The number of bytes copied in
		int *paddr = new int;

		while ( n >= 0 && n < len) {
			result = machine->ReadMem( vaddr, 1, paddr );
			while(!result) // FALL 09 CHANGES
		{
				result = machine->ReadMem( vaddr, 1, paddr ); // FALL 09 CHANGES: TO HANDLE PAGE FAULT IN THE ReadMem SYS CALL
		}	
			
			buf[n++] = *paddr;
		 
			if ( !result ) {
	//translation failed
	return -1;
			}

			vaddr++;
		}

		delete paddr;
		return len;
}

int copyout(unsigned int vaddr, int len, char *buf) {
		// Copy len bytes to the current thread's virtual address vaddr.
		// Return the number of bytes so written, or -1 if an error
		// occors.  Errors can generally mean a bad virtual address was
		// passed in.
		bool result;
		int n=0;			// The number of bytes copied in

		while ( n >= 0 && n < len) {
			// Note that we check every byte's address
			result = machine->WriteMem( vaddr, 1, (int)(buf[n++]) );

			if ( !result ) {
	//translation failed
	return -1;
			}

			vaddr++;
		}

		return n;
}

void Create_Syscall(unsigned int vaddr, int len) {
		// Create the file with the name in the user buffer pointed to by
		// vaddr.  The file name is at most MAXFILENAME chars long.  No
		// way to return errors, though...
		char *buf = new char[len+1];	// Kernel buffer to put the name in

		if (!buf) return;

		if( copyin(vaddr,len,buf) == -1 ) {
	printf("%s","Bad pointer passed to Create\n");
	delete buf;
	return;
		}

		buf[len]='\0';

		fileSystem->Create(buf,0);
		delete[] buf;
		return;
}

int Open_Syscall(unsigned int vaddr, int len) {
		// Open the file with the name in the user buffer pointed to by
		// vaddr.  The file name is at most MAXFILENAME chars long.  If
		// the file is opened successfully, it is put in the address
		// space's file table and an id returned that can find the file
		// later.  If there are any errors, -1 is returned.
		char *buf = new char[len+1];	// Kernel buffer to put the name in
		OpenFile *f;			// The new open file
		int id;				// The openfile id

		if (!buf) {
	printf("%s","Can't allocate kernel buffer in Open\n");
	return -1;
		}

		if( copyin(vaddr,len,buf) == -1 ) {
	printf("%s","Bad pointer passed to Open\n");
	delete[] buf;
	return -1;
		}

		buf[len]='\0';

		f = fileSystem->Open(buf);
		delete[] buf;

		if ( f ) {
	if ((id = currentThread->space->fileTable.Put(f)) == -1 )
			delete f;
	return id;
		}
		else
	return -1;
}

void Write_Syscall(unsigned int vaddr, int len, int id) {
		// Write the buffer to the given disk file.  If ConsoleOutput is
		// the fileID, data goes to the synchronized console instead.  If
		// a Write arrives for the synchronized Console, and no such
		// console exists, create one. For disk files, the file is looked
		// up in the current address space's open file table and used as
		// the target of the write.
		
		char *buf;		// Kernel buffer for output
		OpenFile *f;	// Open file for output

		if ( id == ConsoleInput) return;
		
		if ( !(buf = new char[len]) ) {
	printf("%s","Error allocating kernel buffer for write!\n");
	return;
		} else {
				if ( copyin(vaddr,len,buf) == -1 ) {
			printf("%s","Bad pointer passed to to write: data not written\n");
			delete[] buf;
			return;
	}
		}

		if ( id == ConsoleOutput) {
			for (int ii=0; ii<len; ii++) {
	printf("%c",buf[ii]);
			}

		} else {
	if ( (f = (OpenFile *) currentThread->space->fileTable.Get(id)) ) {
			f->Write(buf, len);
	} else {
			printf("%s","Bad OpenFileId passed to Write\n");
			len = -1;
	}
		}

		delete[] buf;
}

int Read_Syscall(unsigned int vaddr, int len, int id) {
		// Write the buffer to the given disk file.  If ConsoleOutput is
		// the fileID, data goes to the synchronized console instead.  If
		// a Write arrives for the synchronized Console, and no such
		// console exists, create one.    We reuse len as the number of bytes
		// read, which is an unnessecary savings of space.
		char *buf;		// Kernel buffer for input
		OpenFile *f;	// Open file for output

		if ( id == ConsoleOutput) return -1;
		
		if ( !(buf = new char[len]) ) {
	printf("%s","Error allocating kernel buffer in Read\n");
	return -1;
		}

		if ( id == ConsoleInput) {
			//Reading from the keyboard
			scanf("%s", buf);

			if ( copyout(vaddr, len, buf) == -1 ) {
	printf("%s","Bad pointer passed to Read: data not copied\n");
			}
		} else {
	if ( (f = (OpenFile *) currentThread->space->fileTable.Get(id)) ) {
			len = f->Read(buf, len);
			if ( len > 0 ) {
					//Read something from the file. Put into user's address space
						if ( copyout(vaddr, len, buf) == -1 ) {
				printf("%s","Bad pointer passed to Read: data not copied\n");
		}
			}
	} else {
			printf("%s","Bad OpenFileId passed to Read\n");
			len = -1;
	}
		}

		delete[] buf;
		return len;
}

void Close_Syscall(int fd) {
		// Close the file associated with id fd.  No error reporting.
		OpenFile *f = (OpenFile *) currentThread->space->fileTable.Remove(fd);

		if ( f ) {
			delete f;
		} else {
			printf("%s","Tried to close an unopen file\n");
		}
}

////////////////////////////////////////////////////////
/*******************************************************
	Our implementations for proj2 above code was provided
	*****************************************************/

Lock kernel_threadLock("Kernel Thread Lock");
//int forkCalled = 0;

void kernel_thread(int vaddr){
	DEBUG('f', "IN kernel_thread.\n");
 	
	currentThread->space->Fork(vaddr);//add stack space to pagetable and init registers...

	//(ProcessTable->getProcessEntry(currentThread->space))->addThread();
	//kernel_threadLock.Acquire();
	//forkCalled--;
  //kernel_threadLock.Release();
	DEBUG('f', "End kernel_thread.\n");
	machine->Run();
	ASSERT(FALSE);
}

/* Fork a thread to run a procedure ("func") in the *same* address space 
 * as the current thread.
 */
void Fork_Syscall(int funct){
	kernel_threadLock.Acquire();
	(ProcessTable->getProcessEntry(currentThread->space))->addThread();
  kernel_threadLock.Release();
	Thread* t;
	DEBUG('f', "In fork syscall. funct = %i\n", funct);
	t = new Thread("Forked thread.");
	t->space = currentThread->space;
 // DEBUG('f', "CurrentSpace: %i  TSpace: %i\n", currentThread->space, t->space);
	t->Fork((VoidFunctionPtr)kernel_thread, funct); //kernel_thread??
	currentThread->Yield();//It should not be necessary to yield here
	DEBUG('f', "End of Fork Syscall.\n");
}//end Fork_Syscall


Lock execLock("ExecLock");
int execCalled = 0;

/************************************************************************
* Run the executable, stored in the Nachos file "name", and return the  *
* address space identifier                                              *
***********************************************************************/
//Lock* kernel_exec_lock = new Lock("Kernel Exec lock for filename...");
//char *kernel_execBUF = null;
void kernel_exec(int intName){

	char* name = (char*)intName;

	DEBUG('e', "Kernel_exec system call: FileName: %s \n\n", name);



	OpenFile *executable = fileSystem->Open(name);

	AddrSpace *space;



	if (executable == NULL) {

		printf("Unable to open file %s\n", name);

		return;

	}



	space = new AddrSpace(executable);



	currentThread->space = space;

	//processTable.insert(space, (new ProcessTableEntry(space)));

	ProcessTable->addProcess(space);

	delete executable;      // close file



	space->InitRegisters();   // set the initial register values

	space->RestoreState();    // load page table register

	delete[] name;

	execLock.Acquire();

	execCalled--;

	execLock.Release();

	machine->Run();     // jump to the user progam

	ASSERT(FALSE);      // machine->Run never returns;

	// the address space exits

	// by doing the syscall "exit"*/

}


SpaceId Exec_Syscall(unsigned int vaddr, int len){
		execLock.Acquire();
		execCalled++;
		execLock.Release();
		DEBUG('e', "In exec syscall. vaddr: %i, len: %i\n", vaddr, len);

		char *buf;   // Kernel buffer
		
		if ( !(buf = new char[len]) ) {
			printf("%s","Error allocating kernel buffer for write!\n");
			return -1;
		} else {
				if ( copyin(vaddr,len,buf) == -1 ) {
					printf("%s","Bad pointer passed to to Exec: Exec aborted.\n");
					delete[] buf;
					return -1;
				}
		}

		//string name(buf);

		DEBUG('e' ,"The filename: %s\n", buf);
		//DEBUG('e' ,"Or as a string: %s\n", name.c_str());

		Thread* t;
		t = new Thread("Execed Thread.");
		t->Fork((VoidFunctionPtr)kernel_exec, (int)buf);

	return -1;
}



/*************************************************************************
* Exit()
*************************************************************************/
void Exit_Syscall(int status){
	ProcessTableEntry* p = ProcessTable->getProcessEntry(currentThread->space);
	execLock.Acquire();
	kernel_threadLock.Acquire();//TODO: RACE CONDITION THIS WONT WORK!!!?

	cout << "exit readReg 4:    " << machine->ReadRegister(2) << endl;
	cout << "exit status:  " << status << endl;
	//Case 1
		//Not last thread in process
		//reclaim 8 pages of stack
		//vpn,ppn,valid
		//memoryBitMap->Clear(ppn)
		//valid = false
	if(p->getNumThreads() > 1){
		p->removeThread();
		DEBUG('E', "Not the last thread. Left: %i \n", p->getNumThreads());
		currentThread->space->Exit();
	}
	//Case 2
		//Last executing thread in last process
		//interupt->Halt();//shut downs nachos
	else if(p->getNumThreads() == 1 && ProcessTable->getNumProcesses() == 1 && !execCalled){
		DEBUG('E', "LAST THREAD LAST PROCESS\n");
		interrupt->Halt();
	}
	//Case 3
		//Last executing thread in a process - not last process
		//reclaim all unreclaimed memory
		//for(pageTable)
			//if valid clear
		//locks/cvs match addspace* w/ process table

	//Minumum this must have
	else{
		DEBUG('E', "Last thread in process.\n");
		ProcessTable->deleteProcess(currentThread->space);
		delete currentThread->space;
	}
	execLock.Release();
	kernel_threadLock.Release();

	currentThread->Finish();
}


/*************************************************************
//Prints and int
**************************************************************/
void PrintInt_Syscall(int wat){
	printf("%i", wat);
}

/*************************************************************
//Prints a String
**************************************************************/
void PrintString_Syscall(unsigned int vaddr, int len){
	char *buf;		// Kernel buffer for output
		
		
	if ( !(buf = new char[len]) ) {
		printf("%s","Error allocating kernel buffer for write!\n");
		return;
	} else {
		if ( copyin(vaddr,len,buf) == -1 ) {
			printf("%s","Bad pointer passed to to PrintString: data not pinted.\n");
			delete[] buf;
			return;
		}
	}

	for (int ii=0; ii<len; ii++) {
		printf("%c",buf[ii]);
	}

	delete[] buf;
}//End PrintStringSyscall




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
int CreateLock_Syscall(){
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


/***********************
*	Acquire the lock
*/
void Acquire_Syscall(int lock){
	DEBUG('L', "In Acquire_Syscall\n");

	if(!Lock_Syscall_InputValidation(lock)){
	 printf("Unable to Acquire.\n");
	 return;
	}

	DEBUG('L', "Acquiring lock.\n");
	lockTable[lock]->lock->Acquire();

}

/*****************
* 	Release the lock
*/
void Release_Syscall(int lock){
	DEBUG('L', "In Release_Syscall\n");

	if(!Lock_Syscall_InputValidation(lock)){
	 printf("Unable to Release.\n");
	 return;
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

}

void DestroyLock_Syscall(int lock){
	DEBUG('L', "In DestroyLock_Syscall\n");

	if(!Lock_Syscall_InputValidation(lock)){
	 printf("Unable to DestroyLock.\n");
	 return;
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


}



//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//
//	Condition Syscalls
///////////////////////////////////////////////////////////////////////////////////////////

class ConditionTableEntry{
public:
	Condition* condition;
	AddrSpace* space;
	bool isToBeDeleted;
};

#define ConditionTableSize 200
BitMap ConditionTableBitMap(ConditionTableSize);
ConditionTableEntry* ConditionTable[ConditionTableSize];


bool Condition_Syscall_InputValidation(int cond, int lock){

	if(!Lock_Syscall_InputValidation(lock)){
	 return FALSE;
	}

	if(cond < 0 || cond >= ConditionTableSize){
		printf("Invalid Condition Identifier: %i ", cond);
		return FALSE;
	}

	ConditionTableEntry* condEntry = ConditionTable[cond];
	if(condEntry == NULL){
		printf("Condition %i does not exist. ", cond);
		return FALSE;
	}
	if(condEntry->space != currentThread->space){
		printf("Lock %i does not belong to this process. ", cond);
		return FALSE;
	}
	return TRUE;
}



int CreateCondition_Syscall(){
	DEBUG('C', "In CreateCondition_Syscall\n");
	
	int conID = ConditionTableBitMap.Find();
	if(conID == -1){
		printf("Max Number of Conditions created. Unable to CreateCondition\n");
		return -1;
	}

	ConditionTableEntry* ce = new ConditionTableEntry();

	ce->condition = new Condition("Condition " + conID);
	ce->space = currentThread->space;
	ce->isToBeDeleted = FALSE;

	ConditionTable[conID]	= ce;

	return conID;
}

void Wait_Syscall(int condition, int lock){
	DEBUG('C', "In Wait_Syscall\n");

	if(!Condition_Syscall_InputValidation(condition, lock)){
		printf("Unable to Wait.\n");
		return;
	}

	ConditionTableEntry* ce = ConditionTable[condition];
	LockTableEntry* le = lockTable[lock];

	ce->condition->Wait(le->lock);
}

void Signal_Syscall(int condition, int lock){
	DEBUG('C', "In Signal_Syscall\n");

	if(!Condition_Syscall_InputValidation(condition, lock)){
		printf("Unable to Signal.\n");
		return;
	}

	ConditionTableEntry* ce = ConditionTable[condition];
	LockTableEntry* le = lockTable[lock];

	ce->condition->Signal(le->lock);

	if(ce->isToBeDeleted && !ce->condition->isBusy()){
		DEBUG('C', "Condition %i no longer BUSY. Deleting.", condition);
		ConditionTable[condition] = NULL;
		delete ce->condition;
		ce->condition = NULL;
		delete ce;
		ConditionTableBitMap.Clear(condition);
	}

}

void Broadcast_Syscall(int condition, int lock){
	DEBUG('C', "In Broadcast_Syscall\n");

	if(!Condition_Syscall_InputValidation(condition, lock)){
		printf("Unable to Broadcast.\n");
		return;
	}

	ConditionTableEntry* ce = ConditionTable[condition];
	LockTableEntry* le = lockTable[lock];

	ce->condition->Broadcast(le->lock);


	if(ce->isToBeDeleted && !ce->condition->isBusy()){
		DEBUG('C', "Condition %i no longer BUSY. Deleting.", condition);
		ConditionTable[condition] = NULL;
		delete ce->condition;
		ce->condition = NULL;
		delete ce;
		ConditionTableBitMap.Clear(condition);
	}
}

void DestroyCondition_Syscall(int condition){
	DEBUG('C', "In DestroyCondition_Syscall\n");

	ConditionTableEntry* ce = ConditionTable[condition];

	if((ce->condition->isBusy()) ){
		ce->isToBeDeleted = TRUE;
		DEBUG('C', "Condition %i BUSY marking for deletion.\n", condition);
	}else{
		ConditionTable[condition] = NULL;
		delete ce->condition;
		ce->condition = NULL;
		delete ce;
		ConditionTableBitMap.Clear(condition);
		DEBUG('C', "Condition %i deleted.\n", condition);
	}

}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//
//	Rand Syscall
///////////////////////////////////////////////////////////////////////////////////////////

int Rand_Syscall(){
	return rand();
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//
//	Monitor Syscalls
///////////////////////////////////////////////////////////////////////////////////////////
class MonitorTableEntry{
public:
	int* monitorArray;
	int arrayLength;
	char* nameBuf;
	AddrSpace* space;
};

#define MonitorTableSize 200
BitMap MonitorTableBitMap(MonitorTableSize);
MonitorTableEntry* MonitorTable[MonitorTableSize];

bool Monitor_Syscall_InputValidation(int monitor)
{
	if(monitor < 0 || monitor >= MonitorTableSize){
		printf("Invalid Monitor Identifier: %i ", monitor);
		return FALSE;
	}

	MonitorTableEntry* moniEntry = MonitorTable[monitor];
	if(moniEntry == NULL){
		printf("Monitor %i does not exist. ", monitor);
		return FALSE;
	}
	
	return TRUE;
}

int CreateMonitor_Syscall(unsigned int name, int sSize, int aLength)
{
	DEBUG('C', "In CreateMonitor_Syscall\n");
	int monitorID = MonitorTableBitMap.Find();
	if(monitorID == -1){
		printf("Max Number of Monitor Variables created. Unable to CreateMonitor\n");
		return -1;
	}

	MonitorTableEntry* me = new MonitorTableEntry();
	if (copyin(name, sSize, me->nameBuf) == -1)
	{
		printf("%s","Bad pointer passed to Create\n");
		delete me->nameBuf;
		return -1;
	}
	me->monitorArray = new int[aLength];
	me->arrayLength = aLength;
	me->space = currentThread->space;
	MonitorTable[monitorID] = me;
	return monitorID;
}

void SetMonitor_Syscall(int id, int index, int value)
{
	DEBUG('M', "In SetMonitor_Syscall\n");
	
	if(!Monitor_Syscall_InputValidation(id)){
		printf("Unable to SetMonitor.\n");
		return;
	}
	
	MonitorTableEntry* me = MonitorTable[id];
	if (me->arrayLength <= index)
	{
		printf("Index is greater or equal to size of Monitor Variable array. Unable to SetMonitor\n");
		return;
	}
	else
	{
		me->monitorArray[index] = value;
	}
}

int GetMonitor_Syscall(int id, int index)
{
	DEBUG('M', "In GetMonitor_Syscall\n");
	if(!Monitor_Syscall_InputValidation(id)){
		printf("Unable to GetMonitor.\n");
		return -1;
	}
	MonitorTableEntry* me = MonitorTable[id];
	if (me->arrayLength <= index)
	{
		printf("Index is greater or equal to size of Monitor Variable array. Unable to GetMonitor\n");
		return -1;
	}
	else
	{
		return me->monitorArray[index];
	}
}

void DestroyMonitor_Syscall(int monitor)
{
	DEBUG('M', "In DestroyMonitor_Syscall\n");
	
	MonitorTableEntry* me = MonitorTable[monitor];

	MonitorTable[monitor] = NULL;
	delete me->monitorArray;
	me->monitorArray = NULL;
	delete me->nameBuf;
	me->nameBuf = NULL;
	delete me;
	MonitorTableBitMap.Clear(monitor);
	DEBUG('M', "Monitor %i deleted.\n", monitor);
}


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//
//	Remote Procedure Call Functions
///////////////////////////////////////////////////////////////////////////////////////////
//void ClientDataTransfer(unsigned int name, int sysIndex, int firstArg, int secondArg, int thirdArg)
//{
//		PacketHeader outPktHdr, inPktHdr;
//    	MailHeader outMailHdr, inMailHdr;
//    	char *data;
//    	char *ack = "Syscall info recieved";
//    	char buffer[MaxMailSize];
    	
    	//write data (as in, sysIndex, name, and all the args) to 'data')
    		
    	//outPktHdr.to = farAddr;
    	//outPktHdr.to = 0;		
    	///outMailHdr.to = 0;
    	//outMailHdr.from = 1;
   		//outMailHdr.length = strlen(data) + 1;
   		
   		//bool success = postOffice->Send(outPktHdr, outMailHdr, data); 
   		
   		//if ( !success ) {
      	//	printf("The postOffice Send failed. You must not have the other Nachos running. Terminating Nachos.\n");
      	//	interrupt->Halt();
    	//}
    	
    	//postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
   		//printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
   		//take the buffer, and break it into the necessary data
    	//fflush(stdout);		
//}


//
//int handleMemoryFull() {
//	cout << "handle memory full" << endl;
//	int ppn = -1;
//	//Select page to be eviceted, RAND or FIFO
//	//if (fifo) {
//		//ppn = system->FIFOReplacementQueue->pop();
//		//system->FIFOReplacementQueue->push(ppn);
//	//}
//	//else {
//		//ppn = rand() % NumPhysPages;
//	//}
//	ppn = rand() % NumPhysPages;
//	
//
//	//SWAP FILE STUFF
//
//	cout << "recently evicted and reclaimed ppn = " << ppn << endl;
//	return ppn;
//}

//
//
//int handleIPTMiss(int vpn) {
//	cout << "handle IPT miss. " << endl;
//	int ppn = -1;
//	//Allocate 1 pg memory and read page from exe into this page
//
//	ppn = pageTableBitMap->Find();
//	if (ppn == -1) {
//
//		ppn = handleMemoryFull();
//
//	}
//	
//	cout << "Free page to be written to. ppn = " << ppn << endl;
//	
//	//Read page from executable. Update IPT, and PageTable
//
//	//  "into" --the buffer to contain the data to be read from disk
//
//	//	"from" -- the buffer containing the data to be written to disk 
//
//	//	"numBytes" -- the number of bytes to transfer
//
//	//	"position" -- the offset within the file of the first byte to be read/written
//	//executable->ReadAt( &(machine->mainMemory[PageSize * ppn]), PageSize, noffH.code.inFileAddr + (i * PageSize) );
//	//(char *from, int numBytes, int position)???
//	//currentThread->space->executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]), PageSize, currentThread->space->pageTable[vpn].byteOffset);
//	//WriteAt(&(machine->mainMemory[PageSize*ppn]), PageSize, currentThread->space->pageTable[vpn].byteOffset);
//
//
//	ipt->entries[ppn].virtualPage = vpn;
//	ipt->entries[ppn].physicalPage = ppn;
//	ipt->entries[ppn].valid = TRUE;
//	ipt->entries[ppn].readOnly = FALSE;
//	ipt->entries[ppn].use = FALSE;
//	ipt->entries[ppn].dirty = FALSE;
//
//	currentThread->space->pageTable[ppn].virtualPage = vpn;
//	currentThread->space->pageTable[ppn].physicalPage = ppn;
//	currentThread->space->pageTable[ppn].valid = TRUE;
//	currentThread->space->pageTable[ppn].readOnly = FALSE;
//	currentThread->space->pageTable[ppn].use = FALSE;
//	currentThread->space->pageTable[ppn].dirty = FALSE;
//
//	//Are these right?
//	currentThread->space->pageTable[ppn].byteOffset = 0;
//
//	return ppn;
//}



void handlePageFault(int vpn) {

	//cout << "handle page fault. vpn is "<< vpn << endl;

	//Searching the IPT

	int ppn = -1;
	//cout << "from ipt:  vnp = " << ipt->entries[vpn].virtualPage << "  ppn = " << ipt->entries[vpn].physicalPage << "    valid " << ipt->entries[vpn].valid << "  dirty  " << ipt->entries[vpn].dirty << endl;


	for (int i = 0; i < NumPhysPages; i++) {
		//cout << "ipt valid? " << ipt->entries[vpn].valid << "  ipt vpn is " << ipt->entries[vpn].virtualPage << " looking for vpn " << vpn << endl;
		//cout << "this owner is " << ipt->entries[vpn].owner << "  current space is " << currentThread->space << endl;
		if (ipt->entries[vpn].valid && ipt->entries[vpn].virtualPage == vpn &&  ipt->entries[vpn].owner == currentThread->space) {
			ppn = ipt->entries[vpn].physicalPage;
			//cout << "Got it in ipt.  ppn = " << i << endl;
			break;
		}
	}
	if (ppn == -1) {
		//Handle IPT miss
		//ppn = handleIPTMiss(vpn);
	}

	//Update TLB, even if IPT miss it will be in IPT by now.

	machine->tlb[machine->currentTLB].virtualPage = ipt->entries[ppn].virtualPage;
	machine->tlb[machine->currentTLB].physicalPage = ipt->entries[ppn].physicalPage;
	machine->tlb[machine->currentTLB].valid = ipt->entries[ppn].valid;
	machine->tlb[machine->currentTLB].readOnly = ipt->entries[ppn].readOnly;
	machine->tlb[machine->currentTLB].use = ipt->entries[ppn].use;
	machine->tlb[machine->currentTLB].dirty = ipt->entries[ppn].dirty;

	machine->currentTLB = (machine->currentTLB + 1) % TLBSize;

	return;


}


void ExceptionHandler(ExceptionType which) {
		int type = machine->ReadRegister(2); // Which syscall?
		int rv=0; 	// the return value from a syscall


		if (which == SyscallException) {
			switch (type) {
			default:
				DEBUG('a', "Unknown syscall - shutting down.\n");
			case SC_Halt:
				DEBUG('a', "Shutdown, initiated by user program.\n");
				interrupt->Halt();
				break;
			case SC_Create:
				DEBUG('a', "Create syscall.\n");
				Create_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
				break;
			case SC_Open:
				DEBUG('a', "Open syscall.\n");
				rv = Open_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
				break;
			case SC_Write:
				DEBUG('a', "Write syscall.\n");
				Write_Syscall(machine->ReadRegister(4),
					machine->ReadRegister(5),
					machine->ReadRegister(6));
				break;
			case SC_Read:
				DEBUG('a', "Read syscall.\n");
				rv = Read_Syscall(machine->ReadRegister(4),
					machine->ReadRegister(5),
					machine->ReadRegister(6));
				break;
			case SC_Close:
				DEBUG('a', "Close syscall.\n");
				Close_Syscall(machine->ReadRegister(4));
				break;

			case SC_Fork:
				DEBUG('a', "Fork syscall.\n");
				Fork_Syscall(machine->ReadRegister(4));
				break;

			case SC_Yield:
				DEBUG('a', "Yield syscall.\n");
				currentThread->Yield();
				break;

			case SC_PrintInt:
				DEBUG('a', "PrintInt syscall.\n");
				PrintInt_Syscall(machine->ReadRegister(4));
				break;

			case SC_PrintString:
				DEBUG('a', "PrintString syscall.\n");
				PrintString_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
				break;

			case SC_Exit:
				DEBUG('a', "Exit syscall.\n");
				Exit_Syscall(machine->ReadRegister(4));
				break;

			case SC_Exec:
				DEBUG('a', "Exec syscall.\n");
				rv = Exec_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
				break;

			case SC_CreateLock:
				DEBUG('a', "CreateLock syscall.\n");
				rv = CreateLock_Syscall();
				break;

			case SC_Acquire:
				DEBUG('a', "Acquire syscall.\n");
				Acquire_Syscall(machine->ReadRegister(4));
				break;

			case SC_Release:
				DEBUG('a', "Release syscall.\n");
				Release_Syscall(machine->ReadRegister(4));
				break;

			case SC_DestroyLock:
				DEBUG('a', "DestroyLock syscall.\n");
				DestroyLock_Syscall(machine->ReadRegister(4));
				break;

			case SC_CreateCondition:
				DEBUG('a', "CreateCondition syscall.\n");
				rv = CreateCondition_Syscall();
				break;

			case SC_Wait:
				DEBUG('a', "Wait syscall.\n");
				Wait_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
				break;

			case SC_Signal:
				DEBUG('a', "Signal syscall.\n");
				Signal_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
				break;

			case SC_Broadcast:
				DEBUG('a', "Broadcast syscall.\n");
				Broadcast_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
				break;

			case SC_DestroyCondition:
				DEBUG('a', "DestroyCondition syscall.\n");
				DestroyCondition_Syscall(machine->ReadRegister(4));
				break;

			case SC_Rand:
				DEBUG('a', "Rand syscall.\n");
				rv = Rand_Syscall();
				break;
				
			case SC_CreateMonitor:
				DEBUG('a', "CreateMonitor syscall.\n");
				rv = CreateMonitor_Syscall(machine->ReadRegister(4), machine->ReadRegister(5), machine->ReadRegister(6));
				break;

			case SC_SetMonitor:
				DEBUG('a', "SetMonitor syscall.\n");
				SetMonitor_Syscall(machine->ReadRegister(4), machine->ReadRegister(5), machine->ReadRegister(6));
				break;

			case SC_GetMonitor:
				DEBUG('a', "GetMonitor syscall.\n");
				rv = GetMonitor_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
				break;

			case SC_DestroyMonitor:
				DEBUG('a', "DestroyMonitor syscall.\n");
				DestroyMonitor_Syscall(machine->ReadRegister(4));				
				break;


			}

			// Put in the return value and increment the PC
			machine->WriteRegister(2, rv);
			machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
			machine->WriteRegister(PCReg, machine->ReadRegister(NextPCReg));
			machine->WriteRegister(NextPCReg, machine->ReadRegister(PCReg) + 4);
			return;

			} else if (which == PageFaultException) {

			//Do TLB population here
			int vpn = (int)(machine->registers[BadVAddrReg]/PageSize);
			//cout << "pageFaultException. vpn = " << vpn << endl;



			handlePageFault(vpn);

			//cout << "from pagetable:  vnp = " << currentThread->space->pageTable[vpn].virtualPage << "  ppn = " << currentThread->space->pageTable[vpn].physicalPage << endl;
			//cout << "    valid " << currentThread->space->pageTable[vpn].valid << "  dirty  " << currentThread->space->pageTable[vpn].dirty << endl;



			//machine->tlb[machine->currentTLB].virtualPage = currentThread->space->pageTable[vpn].virtualPage;
			//machine->tlb[machine->currentTLB].physicalPage = currentThread->space->pageTable[vpn].physicalPage;
			//machine->tlb[machine->currentTLB].valid = currentThread->space->pageTable[vpn].valid;
			//machine->tlb[machine->currentTLB].readOnly = currentThread->space->pageTable[vpn].readOnly;
			//machine->tlb[machine->currentTLB].use = currentThread->space->pageTable[vpn].use;
			//machine->tlb[machine->currentTLB].dirty = currentThread->space->pageTable[vpn].dirty;
			//
			//
			//machine->currentTLB = (machine->currentTLB + 1) % TLBSize;





		} else {
			cout<<"Unexpected user mode exception - which:"<<which<<"  type:"<< type<<endl;
			interrupt->Halt();
		}

	

}


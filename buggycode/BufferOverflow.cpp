#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "memory.h"
#include <iostream>
#include <iomanip>
using namespace std;
// Note: GCC and MSVC uses different memory alignment
// Try "12345678DevilEvecosia" as a password for gcc build
// Try "1234567812345678Devil I am. Ha Ha" as a password for MSVC debug x86 build. 
// If doesn't work, investigate why :)

void demoBufferOverflowData() {
	int             unused_variable = 30;
#define NORMAL_USER            'n'
#define ADMIN_USER             'a'
	int             userRights = NORMAL_USER;
#define USER_INPUT_MAX_LENGTH  8
	char    userName[USER_INPUT_MAX_LENGTH];
	char    passwd[USER_INPUT_MAX_LENGTH];

	// print some info about variables
	printf("%-20s: %p\n", "userName", userName);
	printf("%-20s: %p\n", "passwd", passwd);
	printf("%-20s: %p\n", "unused_variable", &unused_variable);
	printf("%-20s: %p\n", "userRights", &userRights);
	printf("%-20s: %p\n", "demoBufferOverflowData", demoBufferOverflowData);
	// problem
	printf("\n");

	// Get user name
	memset(userName, 1, USER_INPUT_MAX_LENGTH);
	memset(passwd, 2, USER_INPUT_MAX_LENGTH);
	printf("login as: ");
	fflush(stdout);
	//gets(userName); // use scanf("%s", userName); if gets fails with identifier not found
	scanf("%s", userName);

	// Get password
	printf("%s@vulnerable.machine.com: ", userName);
	fflush(stdout);
	//gets(passwd);  
	scanf("%s", passwd); // use scanf("%s", passwd); if gets fails with identifier not found

	// Check user rights (set to NORMAL_USER and not changed in code)
	if (userRights == NORMAL_USER) {
		printf("\nWelcome, normal user '%s', your rights are limited.\n\n", userName);
		fflush(stdout);
	}
	if (userRights == ADMIN_USER) {
		printf("\nWelcome, all mighty admin user '%s'!\n", userName);
		fflush(stdout);
	}

	// How to FIX:
	//memset(userName, 0, USER_INPUT_MAX_LENGTH);
	//fgets(userName, USER_INPUT_MAX_LENGTH - 1, stdin);
	//gets_s(userName, USER_INPUT_MAX_LENGTH - 1);
	//memset(passwd, 0, USER_INPUT_MAX_LENGTH);
	//fgets(passwd, USER_INPUT_MAX_LENGTH - 1, stdin);
}


void demoAdjacentMemoryOverflow(char* userName, char* password) {
	// See more at http://www.awarenetwork.org/etc/alpha/?x=5
	// Once string is not null terminated, a lot of functions will behave wrongly:
	// sprintf, fprintf, snprintf, strcpy, strcat, strlen, strstr, strchr, read...
	// memcpy, memmove - if length to copy is computed via strlen(string)

	char message[100];
	char realPassword[] = "very secret password nbusr123";
	char buf[8];

	// print some info about variables
	printf("%-20s: %p\n", "message", message);
	printf("%-20s: %p\n", "userName", userName);
	printf("%-20s: %p\n", "password", password);
	printf("%-20s: %p\n", "realPassword", &realPassword);
	printf("%-20s: %p\n", "buf", &buf);
	printf("\n");

	memset(buf, 0, sizeof(buf));
	memset(message, 1, sizeof(message));
	strncpy(buf, userName, sizeof(buf));              // We will copy only characters which fits into buf

													  // Now print username to standard output - nothing sensitive, right?
	sprintf(message, "Checking '%s' password\n", buf);
	printf("%s", message);
	if (strcmp(password, realPassword) == 0) {
		printf("Correct password.\n");
	}
	else {
		printf("Wrong password.\n");
	}

	// FIX: Do not allow to have non-terminated string 
	// Clear buffer for text with zeroes (terminating zero will be there)
	// strncpy(buf,arg1,sizeof(buf) - 1);
}

typedef struct _some_structure {
	float   someData[1000];
} some_structure;

void demoDataTypeOverflow(int totalItemsCount, some_structure* pItem, int itemPosition) {
	// See http://blogs.msdn.com/oldnewthing/archive/2004/01/29/64389.aspx
	some_structure* data_copy = NULL;
	int bytesToAllocation = totalItemsCount * sizeof(some_structure);
	printf("Bytes to allocation: %d\n", bytesToAllocation);
	data_copy = (some_structure*)malloc(bytesToAllocation);
	if (itemPosition >= 0 && itemPosition < totalItemsCount) {
		memcpy(&(data_copy[itemPosition]), pItem, sizeof(some_structure));
	}
	else {
		printf("Out of bound assignment");
		return;
	}
	free(data_copy);
}


int main() {
	printf("%-20s: %p\n", "main", main);
	printf("\n");

	//
	// Let's play with buffer overflow - change internal data
	// VS, Debug mode
	//

	printf("\n\n#### demoBufferOverflowData ####\n");
	demoBufferOverflowData();
	demoBufferOverflowData();
	/**/
	//
	// More on buffer overflow - reveal sensitive data.
	// Try to login as admin. Password is hardcoded in the application.
	//
	// VS, Debug mode
	/*
	printf("\n\n#### demoAdjacentMemoryOverflow ####\n");
	demoAdjacentMemoryOverflow("admin", "I don't know the password");
	demoAdjacentMemoryOverflow("adminxxxx", "I still don't know the password");
	demoAdjacentMemoryOverflow("admin", "very secret password nbu123");
	/**/
	//
	// And more on buffer overflow - run different function or user code!
	// Eclipse, Debug - work
	// VS, Release - stack protection nonce example - does not work due to canary word
	//	demoBufferOverunFunction();

	//
	// Not only arrays may overflow - integers as well
	//  
	//
	/*
	// printf("\n\n#### demoDataTypeOverflow ####\n");
	some_structure data;
	memset(&data, 1, sizeof(some_structure));
	demoDataTypeOverflow(10, &data, 5);			// Correct operation
	demoDataTypeOverflow(1073742, &data, 5);	// 1073742 * 4000 => only 704 B allocated
	/**/

	return 0;
}




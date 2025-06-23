#ifndef __ILRD_UID_H__
#define __ILRD_UID_H__

#include <time.h>           /* time_t */
#include <sys/types.h>      /* pid_t , size_t */

typedef struct{
    time_t time;
    size_t counter;
    pid_t pid;
    unsigned char ip[14];
} ilrd_uid_t;

extern const ilrd_uid_t bad_uid;

/* 
*   @desc:          Create Unique UID.
*   @params: 		None.
*   @return value:  Unique ID by value.
*   @error: 		Returns bad_uid if failed to create the UID.
*   @time complex: 	O(n)
*   @space complex: O(n)
*/
ilrd_uid_t UIDCreate(void);


/* 
*   @desc:          Checks if the uids are the same
*   @params: 		@uid1: Unique ID
*                   @uid2: Unique ID
*   @return value:  1 if they are the same otherwise 0
*   @error: 		None
*   @time complex: 	O(1)
*   @space complex: O(1)
*/
int UIDIsSame(ilrd_uid_t uid1, ilrd_uid_t uid2);

#endif

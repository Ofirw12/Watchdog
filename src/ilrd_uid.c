
#include <ifaddrs.h> /*getifaddrs*/
#include <stdatomic.h> /*atomic_uint, atomic_fetch_add*/
#include <string.h> /*memcpy, memcmp*/
#include <unistd.h> /*getpid*/

#include "ilrd_uid.h"

const ilrd_uid_t bad_uid = {0};

ilrd_uid_t UIDCreate(void)
{
	static atomic_uint counter = 0;
	ilrd_uid_t uid = {0};
	struct ifaddrs* ifap = NULL;	
	if (-1 == getifaddrs(&ifap))
	{
		return bad_uid;
	}
	
	uid.counter = atomic_fetch_add(&counter, 1);
	
	uid.time = time(NULL);
	uid.pid = getpid();
	memcpy(uid.ip, ifap->ifa_addr->sa_data, sizeof(uid.ip));
	
	freeifaddrs(ifap);
	
	return uid;
}

int UIDIsSame(ilrd_uid_t uid1, ilrd_uid_t uid2)
{
	return ((uid1.time == uid2.time) && 
			(uid1.counter == uid2.counter) &&
			(uid1.pid == uid2.pid) &&
			(0 == memcmp(uid1.ip, uid2.ip, sizeof(uid1.ip))));
}	

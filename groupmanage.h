#ifndef MY_GROUP_MANGE 
#define MY_GROUP_MANGE
#include<iostream> 
#include<string> 
#include<vector> 
#include<algorithm> 
#include<assert.h> 

#include<string.h> 

using namespace std ; 

// two ways to creat , have or not have usernumbers 
// gname: the name of group ,
// return the id of the group 
int creat_group( string gname, string ownernumber,
	   	vector<string> usernumbers ) ;
int creat_group( string gname, string ownernumber ) ;

// buf : "gname ownernumber maxnumbers(int)" 
int CreatGroup(string buf ) ; 

int Search( int gid) ;		// search gid  
bool   search( int gid) ;

//buf : userNumber groupNumber 
//return -1 if error else return the new member's number 
int Invite(string buf) ; // invite friends  

//TODO 
//buf : userNumber groupNumber
int AddRequst(string buf) ;  // request for joining


// buf : the number of user , the group number ;
//
int Dissolve( string buf) ;	// dissolv the group 
								
								
class GroupTalk{
public:
	GroupTalk(  int id, string name, int ownernumbe	) ; 
	GroupTalk(  int id, string name, int ownernumbe, vector<int> usernumbers) ; 

	~GroupTalk() ; 	

	
	// return -1 if not find 
	int find_a_member		(int m_id) ; 
	//return -1 if the member already is in the group
	int add_a_member		(int m_id) ; 
	

	int getgid()			{ return gid_ ; } 
	int getownernumber()	{ return gowner_number_ ; } 
	string getgname()		{ return gname_; } 
	string setgname(string name) { gname_ = name ; return gname_ ; } 

private:
	int gid_ ; 
	string gname_ ; 
	int gowner_number_ ; 
	vector<int> m_numbers_ ; 

} ; 
class GroupMange{
public:
	GroupMange(vector<GroupTalk *> group, int count);
	const int init_number_ = 40000000 ; 
	
	//return nullptr if not find the group
	GroupTalk *	 find_a_group	(int gid) ; 
	int			 remove_a_group	(int gid) ; 	
	void		 add_a_group	(GroupTalk *group)  ;  
	int			 getGroup_count	() {return group_count_ ; } 

private: 
	vector<GroupTalk *> group_talk_vct_ ; 
	int group_count_ ;
} ; 
#endif   // MY_GROUP_MANGE

#include"groupmanage.h"


/*TODO SQL
 * stroe group data int the group_talk from the sql 
 *
 */
vector<GroupTalk*> group_talk ; 

GroupMange GroupHandler(group_talk, 0) ; 


int creat_group( string gname ,int ownernumber)
{
	
	int id = GroupHandler.init_number_ + GroupHandler.getGroup_count() ;  
	GroupTalk *group = new GroupTalk(id , gname , ownernumber ) ;
	GroupHandler.add_a_group(group) ; 
	return id ; 
}
int creat_group( string gname ,int ownernumber,
		vector<int> usernumbers)
{
	int id = GroupHandler.init_number_ + GroupHandler.getGroup_count() ;  
	GroupTalk *group = new( GroupTalk(id, gname, ownernumber usernumbers )) ; 
	GroupHandler.add_a_group(group) ;
	return id ; 
}
int CreatGroup(string buffer)
{
	const char *buf = buffer.c_str() ;  
	char * name , *ownernumber; 
	  ;
	sscanf( buf , "%s %s %d",name , ownernumber, &maxnumbers) ;
	int id = creat_group(name , ownernumber  ) ;  

	return id ; 
}
int Search(int gid) 
{
	if( GroupHandler.find_a_group(gid) )
	{
		return gid ; 
	}
	return -1 ; 
}
/*
bool search(int gid)
{
	if(IsInDatabase(gid))
	{
		return true ; 
	}	
	else 
	{
		return false ; 	
	}
}
*/
int Invite(string buffer)
{
	const char *buf = buffer.c_str() ; 
	int userNumber, groupNumber ; 	
	sscanf(buf, "%d %d", &userNumber, &groupNumber) ; 
	auto ptr_group = GroupHandler.find_a_group(groupNumber) ; 

	if(ptr_group->add_a_member(userNumber) < 0) 
	{
		return -1 ; 
	}
	return userNumber ; 
} 	

int Dissolve(string buffer)
{
	const char *buf = buffer.c_str() ;  
	int userNumber , groupNumber ; 
	sscanf(buf , "%d %d", &userNumber, &groupNumber) ; 

	return GroupHandler.remove_a_group(groupNumber ) ;  
}

GroupTalk::GroupTalk(int id, string name, int ownernumber )
	: gid_(id) , gname_(name), gowner_number_(ownernumber) 
{}

GroupTalk::GroupTalk(int id, string name, int ownernumber,
		 vector<int> usernumbers)
	: gid_(id) , gname_(name), gowner_number_(ownernumber),
	  	m_numbers_(usernumbers)
{} 

int GroupTalk::find_a_member(int m_id)
{
	auto ret = find(m_numbers_.begin(), m_numbers_.end(), m_id) ; 

	if(ret !=m_numbers_.end() )
	{
		return *ret ; 
	} 
	return -1 ; 
}
int GroupTalk::add_a_member(int member_number)
{
	if(find_a_member(member_number)<0 )
	{
		m_numbers_.push_back(member_number) ; 	
		return member_number ; 
	}	
	return -1 ;  	
}




GroupMange::GroupMange(vector<GroupTalk*> group, int count =0)
	: group_talk_vct_(group) , group_count_ (0)
{} 
void GroupMange::add_a_group(GroupTalk * group)
{
	group_count_ ++ ; 
	group_talk_vct_.push_back(group) ; 
} 
GroupTalk* GroupMange::find_a_group(int gid)
{
	for( auto iter = group_talk_vct_.begin();
		   	iter != group_talk_vct_.end() ;	iter ++) 
	{
		if( (*iter!=nullptr) &&( gid == (*iter)->getgid()) ) 
		{
			return (* iter) ; 			
		}	
	}
	return nullptr ;
}
int GroupMange::remove_a_group(int gid)
{
	auto tmp_group = find_a_group(gid) ; 
	if(tmp_group )
	{
		auto iter = find(group_talk_vct_.begin(),group_talk_vct_.end(),tmp_group) ; 
		group_talk_vct_.erase(iter) ; 	
		return gid; 
	}
	return -1 ; 	
	
}

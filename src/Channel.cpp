#include "Channel.hpp"

Channel::~Channel(void)
{

}

const std::string& Channel::getName(void) const
{
	return (this->_name);
}

std::ostream&	operator<<(std::ostream& os, const Channel& channel)
{
	os << "**\tname: " << channel._name << std::endl;
	os << "\tpassword: " << channel._password << std::endl;
	os << "\ttopic: " << channel._topic << std::endl;
	os << "\tmaxUsers: " << channel._maxUsers << std::endl;
	os << "\tisTopicRestricted" << channel._isTopicRestricted << std::endl;
	os << "\tisWhitelisted" << channel._isWhitelisted << std::endl;
	os << "\tUsers:" << std::endl << channel._users << std::endl;
	os << "\twhitelist:" << std::endl << channel._whitelist << std::endl;
	os << "\toperators:" << std::endl << channel._operators << std::endl;
	return (os);
}

std::ostream& operator<<(std::ostream& os, const std::map<std::string, Channel>& channels)
{
	for (std::map<std::string, Channel>::const_iterator it=channels.begin(); it!=channels.end(); it++)
	{
		os << "*";
		os << it->second;
	}
	return (os);
}

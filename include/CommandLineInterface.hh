#ifndef _COMMAND_LINE_INTERFACE_HH_
#define _COMMAND_LINE_INTERFACE_HH_

#include <iomanip>
#include <iostream>
#include <vector>
#include <TString.h>
#include "MessageLogger.hh"

class CommandLineInterface{
public:
	CommandLineInterface();
	~CommandLineInterface();

	void Add( const TString flag, const TString message, TString* value );
	void Add( const TString flag, const TString message, bool* value );

	void CheckFlags(unsigned int argc, char* argv[]);

private:
	std::vector<TString> m_flags;
	std::vector<TString> m_messages;
	std::vector<TString> m_types;
	std::vector<void*> m_values;

	unsigned int m_maximum_flag_size;
	unsigned int m_maximum_message_size;
	unsigned int m_maximum_type_size;

	void GeneralAdd( TString flag, const TString message, const TString type, void* value );
	MessageLogger *log = MessageLogger::GetInstance();

};

#endif
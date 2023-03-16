#include "CommandLineInterface.hh"

///////////////////////////////////////////////////////////////////////////////
CommandLineInterface::CommandLineInterface(){
	m_flags.resize(0);
	m_messages.resize(0);
	m_types.resize(0);
	m_values.resize(0);

	m_maximum_flag_size = 0;
	m_maximum_type_size = 0;
	m_maximum_message_size = 0;
	
	log->Construction("Constructed CommandLineInterface");
}
///////////////////////////////////////////////////////////////////////////////
CommandLineInterface::~CommandLineInterface(){
	m_flags.clear();
	m_messages.clear();
	m_types.clear();
	m_values.clear();
	
	log->Construction("Deleted CommandLineInterface");
}
///////////////////////////////////////////////////////////////////////////////
void CommandLineInterface::GeneralAdd( TString flag, const TString message, const TString type, void* value ){
	// Store values
	m_flags.push_back(flag);
	m_messages.push_back(message);
	m_types.push_back(type);
	m_values.push_back( (void*)value );

	// Reset size markers
	if ( (unsigned int)flag.Length() > m_maximum_flag_size )m_maximum_flag_size = (unsigned int)flag.Length();
	if ( (unsigned int)type.Length() > m_maximum_type_size )m_maximum_type_size = (unsigned int)type.Length();
	if ( (unsigned int)message.Length() > m_maximum_message_size )m_maximum_message_size = (unsigned int)message.Length();

	return;
}
///////////////////////////////////////////////////////////////////////////////
void CommandLineInterface::Add(const TString flag, const TString message, bool* value){
	GeneralAdd( flag, message, "bool", (void*)value );
	return;
}
///////////////////////////////////////////////////////////////////////////////
void CommandLineInterface::Add( const TString flag, const TString message, TString* value ){
	GeneralAdd( flag, message, "string", (void*)value );
	return;
}
///////////////////////////////////////////////////////////////////////////////
void CommandLineInterface::CheckFlags( unsigned int argc, char* argv[] ){
	// Declare loop variables
	unsigned int i;
	unsigned int j;

	if ( argc == 1 ){
		// No flags entered -> print usage
		std::cout << "Use " << argv[0] << " with the following flags:" << std::endl;
		
		for ( i = 0; i < m_flags.size(); ++i ){
			// LHS
			std::cout << std::left << "    [";

			// MIDDLE
			if ( m_types.at(i) == "bool") {
				std::cout << std::setw(m_maximum_flag_size+m_maximum_type_size+3) << m_flags.at(i);
			}
			else{
				 std::cout << 
					std::setw(m_maximum_flag_size) << m_flags.at(i) << " " << 
					std::setw(m_maximum_type_size) << m_types.at(i).Prepend("<").Append(">");
			}

			// END
			std::cout << " : " << std::setw(m_maximum_message_size) << m_messages.at(i) << "]" << std::endl;
		}
		return;
	}

	// Loop over inputs
	for ( i = 1; i < argc; ++i ){

		// Test input flag against the flags that were entered
		for ( j = 0; j < m_flags.size(); ++j ){

			// Check for a match...
			if ( argv[i] == m_flags.at(j) ){

				// Process according to variable type, storing global variables and incrementing i where needed
				// Bool
				if ( m_types.at(j) == "bool"){
					*( (bool*)m_values.at(j) ) = true;
					break;
				}
				// String
				else if ( m_types.at(j) == "string" ){
					*( (TString*)m_values.at(j) ) = (TString)argv[i+1];
					i++;
					break;
				}

			}

		}

		// Flag doesn't match anything known
		if ( j == m_flags.size() ){
			std::cerr << "Flag \"" << argv[i] << "\" is unknown" << std::endl;
		}

	}

	return;
}
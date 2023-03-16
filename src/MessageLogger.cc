#include "MessageLogger.hh"

///////////////////////////////////////////////////////////////////////////////
MessageLogger::MessageLogger(){
	m_time = new TDatime();
	m_print_date = false;
	m_print_timestamp = false;
	m_print_console_level = LevelWarning;
	m_print_file_level = LevelWarning;
	
	this->Construction("MessageLogger::MessageLogger() -- MessageLogger object created");
}
///////////////////////////////////////////////////////////////////////////////
MessageLogger::~MessageLogger(){
	this->Construction("MessageLogger::~MessageLogger() -- MessageLogger object (about to be) destroyed");
	delete m_time;
}
///////////////////////////////////////////////////////////////////////////////
// Pad a string
TString MessageLogger::PadString( TString s, const int length, const char c = ' ') const{
	if ( s.Length() < length ){
		s.Append( c, length - s.Length() );
	}
	return s;
}
///////////////////////////////////////////////////////////////////////////////
// Format the string
TString MessageLogger::MakeFormattedString( TString s, const TerminalFormat f = TerminalFormatDefault, const TerminalForeground fg = TerminalForegroundDefault, const TerminalBackground bg = TerminalBackgroundDefault) const{
	if ( f == TerminalFormat::TerminalFormatDefault ){
		if ( fg == TerminalForegroundDefault && bg == TerminalBackgroundDefault ){
			return s;
		}
		s.Prepend( Form("\e[%d;%dm", (int)fg, (int)bg ) );
	}
	else{
		s.Prepend( Form("\e[%d;%d;%dm", (int)f, (int)fg, (int)bg ) );
	}
	
	// Reset all attributes
	s.Append( "\e[0m" );

	return s;
}
///////////////////////////////////////////////////////////////////////////////
void MessageLogger::GeneralMessage( const TString message, const Level level ) const{
	if ( this->GetPrintConsoleLevel() <= level ){ 
		// Assign general values
		TString s_level = "";

		// Different messages
		std::ostream *o = &std::cout;

		// Different formats
		TString message_type_name = "";
		int message_type_length = 9;
		TerminalFormat tf = TerminalFormatDefault;
		TerminalForeground tfg = TerminalForegroundDefault;
		TerminalBackground tbg = TerminalBackgroundDefault;

		// Specify the formatting
		if ( level == LevelConstruction ){
			tf = TerminalFormatDim;
			tfg = TerminalForegroundLightGrey;
			message_type_name = "CONSTRUCT";
		}
		else if ( level == LevelDebug ){
			tf = TerminalFormatDim;
			message_type_name = "DEBUG";
		}
		else if ( level == LevelWarning ){
			tf = TerminalFormatBold;
			tfg = TerminalForegroundRed;
			message_type_name = "WARNING";
		}
		else if ( level == LevelError ){
			o = &std::cerr;
			tf = TerminalFormatBold;
			tbg = TerminalBackgroundRed;
			message_type_name = "ERROR";
		}

		s_level = PadString(message_type_name, message_type_length );

		// Append or prepend the different formats and make them the same length
		s_level.Prepend('[');
		s_level.Append("]");

		// Supplementary information for lines below
		if ( level == LevelLog ){
			s_level = TString( ' ', message_type_length + 2 + (int)m_print_date ); // 2 brackets + possible space in date
		}

		// Send out the message
		(*o) << MakeFormattedString( this->GetTime(level) + s_level + " | " + message, tf, tfg, tbg ) << std::endl;
	}
	return;
}
///////////////////////////////////////////////////////////////////////////////
// Always prints
void MessageLogger::Log( TString s ) const {
	GeneralMessage(s, LevelLog);
	return;
}
///////////////////////////////////////////////////////////////////////////////
void MessageLogger::Construction( TString s ) const {
	GeneralMessage(s, LevelConstruction);
	return;
}
///////////////////////////////////////////////////////////////////////////////
void MessageLogger::Debug( TString s ) const {
	GeneralMessage(s, LevelDebug);
	return;
}
///////////////////////////////////////////////////////////////////////////////
void MessageLogger::Warning( TString s ) const {
	GeneralMessage(s, LevelWarning);
	return;
}
///////////////////////////////////////////////////////////////////////////////
void MessageLogger::Error( TString s ) const {
	GeneralMessage(s, LevelError);
	GeneralMessage("TERMINATING PROGRAM", LevelError);
	std::exit(1);
	return;
}
///////////////////////////////////////////////////////////////////////////////
TString MessageLogger::GetTime( const int level ) const {
	TString time = "";
	int length = 0;

	if ( m_print_timestamp ){
		m_time->Set();
		length = 8;
		if ( m_print_date ){
			time.Append( Form( "%04d.%02d.%02d ", m_time->GetYear(), m_time->GetMonth(), m_time->GetDay() ) );
			length += 10;
		}
		time.Append( Form( "%02d:%02d:%02d ", m_time->GetHour(), m_time->GetMinute(), m_time->GetSecond() ) );
	}

	if ( level == -1 ){
		time = TString( ' ', length );
	}
	return time;
}








































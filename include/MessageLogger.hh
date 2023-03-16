#ifndef _MESSAGE_LOGGER_HH_
#define _MESSAGE_LOGGER_HH_

#include <fstream>
#include <iostream>
#include <vector>
#include <TDatime.h>
#include <TString.h>

// N.B. this is a singleton class, so that we don't have multiple instances!
class MessageLogger{
public:
	// Print level enum
	enum Level : unsigned char{
		LevelConstruction = 0, LevelDebug, LevelWarning, LevelError, LevelLog
	};

	// Terminal format
	enum TerminalForeground : unsigned char{
		TerminalForegroundDefault = 39,
		TerminalForegroundBlack = 30,
		TerminalForegroundRed = 31,
		TerminalForegroundGreen = 32,
		TerminalForegroundYellow = 33,
		TerminalForegroundBlue = 34,
		TerminalForegroundMagenta = 35,
		TerminalForegroundCyan = 36,
		TerminalForegroundLightGrey = 37,
		TerminalForegroundDarkGrey = 90,
		TerminalForegroundLightRed = 91,
		TerminalForegroundLightGreen = 92,
		TerminalForegroundLightYellow = 93,
		TerminalForegroundLightBlue = 94,
		TerminalForegroundLightMagenta = 95,
		TerminalForegroundLightCyan = 96,
		TerminalForegroundWhite = 97
	};
	enum TerminalBackground : unsigned char{
		TerminalBackgroundDefault = 49,
		TerminalBackgroundBlack = 40,
		TerminalBackgroundRed = 41,
		TerminalBackgroundGreen = 42,
		TerminalBackgroundYellow = 43,
		TerminalBackgroundBlue = 44,
		TerminalBackgroundMagenta = 45,
		TerminalBackgroundCyan = 46,
		TerminalBackgroundLightGrey = 47,
		TerminalBackgroundDarkGrey = 100,
		TerminalBackgroundLightRed = 101,
		TerminalBackgroundLightGreen = 102,
		TerminalBackgroundLightYellow = 103,
		TerminalBackgroundLightBlue = 104,
		TerminalBackgroundLightMagenta = 105,
		TerminalBackgroundLightCyan = 106,
		TerminalBackgroundWhite = 107
	};
	enum TerminalFormat : unsigned char{
		TerminalFormatDefault = 0,
		TerminalFormatBold = 1,
		TerminalFormatDim = 2,
		TerminalFormatUnderlined = 4,
		TerminalFormatBlink = 5,
		TerminalFormatInverted = 7,
		TerminalFormatHidden = 8
	};

	// Constructors and destructors
	MessageLogger();
	~MessageLogger();
	MessageLogger( const MessageLogger& log ) = delete;

	// Logging functions
	void Log( TString s ) const;
	void Construction( TString s ) const;
	void Debug( TString s ) const;
	void Warning( TString s ) const;
	void Error( TString s ) const;
	
	// Setters and Getters
	inline void SetWillPrintTimestamp( const bool b ){ m_print_timestamp = b; }
	inline void SetWillPrintDate( const bool b ){ m_print_date = b; }

	inline void SetPrintConsoleLevel( const Level a ){ m_print_console_level = a; }
	inline void SetPrintFileLevel( const Level a ){ m_print_file_level = a; }

	inline Level GetPrintConsoleLevel() const { return m_print_console_level; }
	inline Level GetPrintFileLevel() const { return m_print_file_level; }


	// Singleton functions must be in class declaration
	static MessageLogger* GetInstance(){
		if ( m_instance_ptr == nullptr ){
			m_instance_ptr = new MessageLogger();
		}
		return m_instance_ptr;
	};

private:
	TDatime *m_time;
	bool m_print_date;
	bool m_print_timestamp;
	static MessageLogger* m_instance_ptr;
	Level m_print_console_level;
	Level m_print_file_level;

	void GeneralMessage( const TString message, const Level level ) const;
	TString GetTime( const int level ) const;
	TString MakeFormattedString( TString s, const TerminalFormat f, const TerminalForeground fg, const TerminalBackground bg) const;
	TString PadString( TString s, const int length, const char c ) const;

};

#endif
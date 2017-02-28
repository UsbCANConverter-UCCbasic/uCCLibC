#ifndef UCCBLIBC_HPP
#define UCCBLIBC_HPP

#include <cstring>
#include <string>
#include <vector>
#include "uCCButil.h"

struct CANFrame
{
    CANFrame();

	enum limitingValues
	{
		STANDARD_ID_LIMIT = 0x7FF,
		EXTENDED_ID_LIMIT = 0x1FFFFFFF,
		MAX_DATA_LENGTH = 8
	};
	
	uint32 GetID();
	void SetID(uint32 id);
	bool IsRemote();
	void SetRemote(bool remote);
	bool IsExtended();
	void SetExtended(bool extended);
	uint8 GetRTRDataLength();
	void SetRTRDataLength(uint8 len);
	
	std::string ToString();

	const char* GetData();
	void SetData(const char* message);

private:
	char _data[MAX_DATA_LENGTH + 1];
	uint32 _id;
	bool _remote;
	bool _extended;
	uint8 _RTRDataLength;
};

struct FilterFlag
{
	uint8 activation	: 1;
	uint8 maskMode		: 1;
	uint8 scale			: 1;
	uint8 FIFONr		: 1;
};

struct IDMaskFlag
{
	uint8 RTR1	: 1;
	uint8 EX1	: 1;
	uint8 RTR2	: 1;
	uint8 EX2	: 1;
};

class uCCBlibC
{
public:
	enum typeOfEOL
	{
		EOL_CR		= 13,
		EOL_LF		= 10,
		EOL_CRLF	= 1310,
		EOL_NONE	= 0
	};

	enum baudRate
	{
		BAUD_10k	= 0,
		BAUD_20k	= 1,
		BAUD_50K	= 2,
		BAUD_100k	= 3,
		BAUD_125k	= 4,
		BAUD_250k	= 5,
		BAUD_500k	= 6,
		BAUD_800k	= 7,
		BAUD_1M		= 8,
	};

	uCCBlibC();
	~uCCBlibC();

	std::string OpenCommand();
	std::string CloseCommand();
	std::string HardwareVersionCommand();
	std::string FirmwareVersionCommand();
	std::string SerialNumberCommand();
	std::string LoopbackModeCommand();
	std::string ListenOnlyModeCommand();
	std::string StatusErrorFlagCommand();
	std::string BaudRateSettingCommand(baudRate);

	static std::vector<CANFrame> PrepareDataFrames(uint32 id, bool extended, std::string message);
	static CANFrame PrepareRemoteFrame(uint32 id, bool extended, uint8 length);

	std::string StringRepresentationOfFrame(CANFrame frame);
	static CANFrame FrameFromString(std::string message);

	std::string AcceptanceFilter(uint32 filterNumber, uint32 bankNumber, FilterFlag filterFlag, 
								 uint32 id, IDMaskFlag idFlag, uint32 mask, IDMaskFlag maskFlag);

	std::string AcceptanceFilter(uint32 filterNumber, uint32 bankNumber, uint32 id);

	std::string ConvertEolToString(typeOfEOL eol);

	void SetEOL(typeOfEOL eol) { lineDelimiter = eol; }
	typeOfEOL GetEOL() { return lineDelimiter; }

private:
	typeOfEOL lineDelimiter;
};

#endif //UCCBLIBC_HPP

#include "uCCBlibC.h"
#include <iostream>

CANFrame::CANFrame()
	: _id(0)
	, _remote(false)
	, _extended(false)
	, _RTRDataLength(0)
{
	for (uint8 i = 0; i < MAX_DATA_LENGTH + 1; i++)
		_data[i] = '\0';
}

uint32 CANFrame::GetID() { return _id; }

void CANFrame::SetID(uint32 id)
{
	if (id > STANDARD_ID_LIMIT)
	{
		_extended = true;

		if (id > EXTENDED_ID_LIMIT)
			id = EXTENDED_ID_LIMIT;
	}

	_id = id;
}

bool CANFrame::IsRemote() { return _remote; }

void CANFrame::SetRemote(bool remote) { _remote = remote; }

bool CANFrame::IsExtended() { return _extended; }

void CANFrame::SetExtended(bool extended)
{
	if (!extended)
		if (_id > STANDARD_ID_LIMIT)
			_id = STANDARD_ID_LIMIT;
	_extended = extended;
}

uint8 CANFrame::GetRTRDataLength() { return _RTRDataLength; }

void CANFrame::SetRTRDataLength(uint8 len)
{
	if (len > 8)
		len = 8;

	_RTRDataLength = len;
}

const char* CANFrame::GetData() { return _data; }

void CANFrame::SetData(const char* message)
{
	uint8 i;
	for (i = 0; (i < MAX_DATA_LENGTH) && (message[i] != '\0'); i++)
	{
		_data[i] = message[i];
	}

	_data[i] = '\0';
}

std::string CANFrame::ToString()
{
	std::string result;
	if (_extended)
	{
		if (_remote)
		{
			result = "R";
			result += NumToHexString(_id, 8);
			result += (char)('0' + _RTRDataLength);
		}
		else
		{
			result = "T";
			result += NumToHexString(_id, 8);
			size_t len = std::strlen(_data);
			result += (char)('0' + len);
			for (uint8 i = 0; i < len; i++)
			{
				result += NumToHexString(_data[i], 2);
			}
		}
	}
	else
	{
		if (_remote)
		{
			result = "r";
			result += NumToHexString(_id, 3);
			result += (char)('0' + _RTRDataLength);
		}
		else
		{
			result = "t";
			result += NumToHexString(_id, 3);
			size_t len = std::strlen(_data);
			if (len > CANFrame::MAX_DATA_LENGTH)
				len = CANFrame::MAX_DATA_LENGTH;
			result += (char)('0' + len);
			for (uint8 i = 0; i < len; i++)
			{
				result += NumToHexString(_data[i], 2);
			}
		}
	}

	return result;
}

uCCBlibC::uCCBlibC()
	: lineDelimiter(EOL_CR)
{ }

uCCBlibC::~uCCBlibC()
{ }

std::string uCCBlibC::AcceptanceFilter(uint32 filterNumber, uint32 bankNumber, FilterFlag filterFlag,
									   uint32 id, IDMaskFlag idFlag, uint32 mask, IDMaskFlag maskFlag)
{
	std::string result = "M";

	if (filterNumber > 0x1B)
		filterNumber = 0x1B;
	if (bankNumber > 0x1B)
		bankNumber = 0x1B;

	result.append(NumToHexString(filterNumber, 2));
	result.append(NumToHexString(bankNumber, 2));
	uint8 flag = (filterFlag.activation) | (filterFlag.maskMode << 1) | (filterFlag.scale << 2) | (filterFlag.FIFONr << 3);
	result.append(NumToHexString(flag, 1));
	result.append(NumToHexString(id, 8));
	flag = (idFlag.EX1) | (idFlag.RTR1 << 1) | (idFlag.EX2 << 2) | (idFlag.RTR2 << 3);
	result.append(NumToHexString(flag, 1));
	result.append(NumToHexString(mask, 8));
	flag = (maskFlag.EX1) | (maskFlag.RTR1 << 1) | (maskFlag.EX2 << 2) | (maskFlag.RTR2 << 3);
	result.append(NumToHexString(flag, 1));
	return result + ConvertEolToString(lineDelimiter);
}

std::string uCCBlibC::AcceptanceFilter(uint32 filterNumber, uint32 bankNumber, uint32 id)
{
	std::string result = "M";

	if (filterNumber > 0x1B)
		filterNumber = 0x1B;
	if (bankNumber > 0x1B)
		bankNumber = 0x1B;

	result.append(NumToHexString(filterNumber, 2));
	result.append(NumToHexString(bankNumber, 2));
	result += '7';
	result.append(NumToHexString(id, 8));
	result += '0';
	result.append(NumToHexString(id, 8));
	result += '0';
	return result + ConvertEolToString(lineDelimiter);
}

std::string uCCBlibC::StringRepresentationOfFrame(CANFrame frame)
{
	return frame.ToString() + ConvertEolToString(lineDelimiter);
}

std::vector<CANFrame> uCCBlibC::PrepareDataFrames(uint32 id, bool extended, std::string message)
{
	std::vector<CANFrame> v;
	size_t len = message.length();

	for (uint32 i = 0; i < len; i += 8)
	{
		CANFrame f;
		f.SetExtended(extended);
		f.SetID(id); // will change STANDARD to EXTENDED if too large ID
		f.SetData(message.substr(i, CANFrame::MAX_DATA_LENGTH).data());
		v.push_back(f);
	}

	return v;
}

CANFrame uCCBlibC::FrameFromString(std::string message)
{
	CANFrame dest;
	uint8 index = 0, length = message.length(), dataLength;
	switch (message[index])
	{
		case 't':
			dest.SetExtended(false);
			dest.SetRemote(false);
			break;
		case 'T':
			dest.SetExtended(true);
			dest.SetRemote(false);
			break;
		case 'r':
			dest.SetExtended(false);
			dest.SetRemote(true);
			break;
		case 'R':
			dest.SetExtended(true);
			dest.SetRemote(true);
			break;
		default:
			dest.SetID(0);
			dest.SetExtended(false);
			dest.SetRemote(false);
			dest.SetRTRDataLength(0);
			dest.SetData("");
			return dest;
	}

	index++;
	if (index > length)
	{
		dest.SetID(0);
		dest.SetRTRDataLength(0);
		dest.SetData("");
		return dest;
	}

	if (dest.IsExtended())
	{
		dest.SetID(HexStringToNum(message.substr(index, 8)));
		index += 8;
		if (index > length)
		{
			dest.SetRTRDataLength(0);
			dest.SetData("");
			return dest;
		}
		dataLength = (message[index] - '0');
		index++;
	}
	else
	{
		dest.SetID(HexStringToNum(message.substr(index, 3)));
		index += 3;
		if (index > length)
		{
			dest.SetRTRDataLength(0);
			dest.SetData("");
			return dest;
		}
		dataLength = (message[index] - '0');
		index++;
	}

	if (dest.IsRemote())
	{
		dest.SetRTRDataLength(dataLength);
		dest.SetData("");
	}
	else
	{
		dest.SetRTRDataLength(0);
		char data[CANFrame::MAX_DATA_LENGTH] = {};
		uint32 i;
		for (i = 0; i < dataLength; i++, index += 2)
		{
			if (index > length)
			{
				dest.SetData("");
				return dest;
			}
			data[i] = (char)(HexStringToNum(message.substr(index, 2)));
		}

		dest.SetData(data);
	}

	return dest;
}

CANFrame uCCBlibC::PrepareRemoteFrame(uint32 id, bool extended, uint8 length)
{
	CANFrame f;

	f.SetRemote(true);
	f.SetExtended(extended);
	f.SetID(id); // will change STANDARD to EXTENDED if too large ID
	f.SetRTRDataLength(length);

	return f;
}

std::string uCCBlibC::ConvertEolToString(typeOfEOL eol)
{
	std::string result = "";
	switch (eol)
	{
		case EOL_CR:
			result = "\r";
			break;
		case EOL_LF:
			result = "\n";
			break;
		case EOL_CRLF:
			result = "\r\n";
			break;
		default:
			result = "";
			break;
	}

	return result;
}

std::string uCCBlibC::OpenCommand()
{
	return "O" + ConvertEolToString(lineDelimiter);
}

std::string uCCBlibC::CloseCommand()
{
	return "C" + ConvertEolToString(lineDelimiter);
}

std::string uCCBlibC::HardwareVersionCommand()
{
	return "V" + ConvertEolToString(lineDelimiter);
}

std::string uCCBlibC::FirmwareVersionCommand()
{
	return "v" + ConvertEolToString(lineDelimiter);
}

std::string uCCBlibC::SerialNumberCommand()
{
	return "N" + ConvertEolToString(lineDelimiter);
}

std::string uCCBlibC::LoopbackModeCommand()
{
	return "l" + ConvertEolToString(lineDelimiter);
}

std::string uCCBlibC::ListenOnlyModeCommand()
{
	return "L" + ConvertEolToString(lineDelimiter);
}

std::string uCCBlibC::StatusErrorFlagCommand()
{
	return "F" + ConvertEolToString(lineDelimiter);
}

std::string uCCBlibC::BaudRateSettingCommand(baudRate baudrate)
{
	std::string result = "S";
	return result + (char)('0' + (baudrate)) + ConvertEolToString(lineDelimiter);
}

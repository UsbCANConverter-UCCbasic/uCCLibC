#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include "catch.hpp"
#include "uCCBlibC.h"

TEST_CASE("Setting filters")
{
	WHEN("Using complex acceptance filter")
	{
		uCCBlibC u;
		FilterFlag f;
		f.FIFONr = 1;
		f.scale = 0;
		f.maskMode = 1;
		f.activation = 1;
		IDMaskFlag m;
		m.RTR2 = 1;
		m.EX2 = 1;
		m.RTR1 = 0;
		m.EX1 = 1;
		
		REQUIRE(u.AcceptanceFilter(0x01, 0x01, f, 0x01, m, 0x01, m) == "M0101B00000001D00000001D\r");

		f.FIFONr = 0;
		f.scale = 1;
		m.EX2 = 0;
		m.RTR1 = 1;
		REQUIRE(u.AcceptanceFilter(0x1F, 0xEF, f, 0xFFFF1, m, 0xABCD1, m) == "M1B1B7000FFFF1B000ABCD1B\r");
	}
	WHEN("Using simple acceptance filter")
	{
		uCCBlibC u;

		REQUIRE(u.AcceptanceFilter(0x01, 0x01, 0x01) == "M01017000000010000000010\r");
		REQUIRE(u.AcceptanceFilter(0x1F, 0xFF, 0x01) == "M1B1B7000000010000000010\r");
	}
}

TEST_CASE("Converting string to CANFrame", "[string2CANFrame conversion]")
{
	CANFrame f;
	WHEN("Message = t001461626364")
	{
		f = uCCBlibC::FrameFromString("t001461626364");
		REQUIRE(f.GetID() == 1);
		REQUIRE(!f.IsRemote());
		REQUIRE(!f.IsExtended());
		REQUIRE(f.GetRTRDataLength() == 0);
		REQUIRE(std::strcmp(f.GetData(), "abcd") == 0);
	}
	WHEN("Message = zzzz")
	{
		f = uCCBlibC::FrameFromString("zzzz");
		REQUIRE(f.GetID() == 0);
		REQUIRE(!f.IsRemote());
		REQUIRE(!f.IsExtended());
		REQUIRE(f.GetRTRDataLength() == 0);
		REQUIRE(std::strcmp(f.GetData(), "") == 0);
	}
	WHEN("Message = R000000EF8")
	{
		f = uCCBlibC::FrameFromString("R000000EF8");
		REQUIRE(f.GetID() == 0x000000EF);
		REQUIRE(f.IsRemote());
		REQUIRE(f.IsExtended());
		REQUIRE(f.GetRTRDataLength() == 8);
		REQUIRE(std::strcmp(f.GetData(), "") == 0);
	}
	WHEN("Message = r7ff4")
	{
		f = uCCBlibC::FrameFromString("r7ff4");
		REQUIRE(f.GetID() == 0x7FF);
		REQUIRE(f.IsRemote());
		REQUIRE(!f.IsExtended());
		REQUIRE(f.GetRTRDataLength() == 4);
		REQUIRE(std::strcmp(f.GetData(), "") == 0);
	}
	WHEN("Message = r")
	{
		f = uCCBlibC::FrameFromString("r");
		REQUIRE(f.GetID() == 0);
		REQUIRE(f.IsRemote());
		REQUIRE(!f.IsExtended());
		REQUIRE(f.GetRTRDataLength() == 0);
		REQUIRE(std::strcmp(f.GetData(), "") == 0);
	}
	WHEN("Message = T012345673303037")
	{
		f = uCCBlibC::FrameFromString("T012345673303037");
		REQUIRE(f.GetID() == 0x01234567);
		REQUIRE(!f.IsRemote());
		REQUIRE(f.IsExtended());
		REQUIRE(f.GetRTRDataLength() == 0);
		REQUIRE(std::strcmp(f.GetData(), "007") == 0);
	}
}

TEST_CASE("Converting hexadecimal strings to numerics", "[hex to num]")
{
	REQUIRE(HexStringToNum("1EFF0A82") == 0x1EFF0A82);
	REQUIRE(HexStringToNum("5AD") == 0x5AD);
	REQUIRE(HexStringToNum("0") == 0x0);
	REQUIRE(HexStringToNum("FFFFFFFF") == 0xFFFFFFFF);
	REQUIRE(HexStringToNum("0001") == 0x1);
	REQUIRE(HexStringToNum("0x0A") == 0xA);
	REQUIRE(HexStringToNum("000000f2") == 0xF2);
	REQUIRE(HexStringToNum("") == 0);
	REQUIRE(HexStringToNum("FFFFFFFFF") == 0);
}

TEST_CASE("Creating data frames", "[data frames]")
{
	uCCBlibC u;
	std::string text = "abcd";
	std::vector<CANFrame> result = uCCBlibC::PrepareDataFrames(0x1, false, text);
	REQUIRE(result.size() == 1);
	REQUIRE(u.StringRepresentationOfFrame(*result.begin()) == "t001461626364\r");

	text = "If any bug exists, catch it!";
	//49 66 20 61 6e 79 20 62 // 75 67 20 65 78 69 73 74 // 73 2c 20 63 61 74 63 68 // 20 69 74 21
	result = uCCBlibC::PrepareDataFrames(0x1, false, text);
	std::vector<CANFrame>::const_iterator it = result.begin();
	REQUIRE(result.size() == 4);
	REQUIRE(u.StringRepresentationOfFrame(*it++) == "t0018496620616E792062\r");
	REQUIRE(u.StringRepresentationOfFrame(*it++) == "t00187567206578697374\r");
	REQUIRE(u.StringRepresentationOfFrame(*it++) == "t0018732C206361746368\r");
	REQUIRE(u.StringRepresentationOfFrame(*it) == "t001420697421\r");

	result = uCCBlibC::PrepareDataFrames(0x1, true, text);
	it = result.begin();
	REQUIRE(result.size() == 4);
	REQUIRE(u.StringRepresentationOfFrame(*it++) == "T000000018496620616E792062\r");
	REQUIRE(u.StringRepresentationOfFrame(*it++) == "T0000000187567206578697374\r");
	REQUIRE(u.StringRepresentationOfFrame(*it++) == "T000000018732C206361746368\r");
	REQUIRE(u.StringRepresentationOfFrame(*it) == "T00000001420697421\r");
}

TEST_CASE("Creating remote frames", "[RTR frames]")
{
	uCCBlibC u;
	REQUIRE(u.StringRepresentationOfFrame(uCCBlibC::PrepareRemoteFrame(0x1F, false, 4)) == "r01F4\r");
	REQUIRE(u.StringRepresentationOfFrame(uCCBlibC::PrepareRemoteFrame(CANFrame::EXTENDED_ID_LIMIT, false, 0)) == "R1FFFFFFF0\r");
	REQUIRE(u.StringRepresentationOfFrame(uCCBlibC::PrepareRemoteFrame(CANFrame::STANDARD_ID_LIMIT, false, 2)) == "r7FF2\r");
	REQUIRE(u.StringRepresentationOfFrame(uCCBlibC::PrepareRemoteFrame(0x4FE78E, false, 8)) == "R004FE78E8\r");
	REQUIRE(u.StringRepresentationOfFrame(uCCBlibC::PrepareRemoteFrame(0, false, 0)) == "r0000\r");
	REQUIRE(u.StringRepresentationOfFrame(uCCBlibC::PrepareRemoteFrame(0, true, 0)) == "R000000000\r");

	WHEN("Message type changes")
	{
		CANFrame f = uCCBlibC::PrepareRemoteFrame(0, false, 0);
		CANFrame f2 = f;
		REQUIRE(u.StringRepresentationOfFrame(f) == "r0000\r");
		f.SetData("\nda,");
		f.SetRemote(false);
		f.SetID(5);
		REQUIRE(u.StringRepresentationOfFrame(f) == "t00540A64612C\r");
		REQUIRE(u.StringRepresentationOfFrame(f2) == "r0000\r");
	}
}

TEST_CASE("Converting CANFrame to string", "[CANFrame2string conversion]")
{
	CANFrame f;
	uCCBlibC u;
	REQUIRE(u.StringRepresentationOfFrame(f) == "t0000\r");

	f.SetID(15);
	REQUIRE(u.StringRepresentationOfFrame(f) == "t00F0\r");

	f.SetData("abcd");
	REQUIRE(u.StringRepresentationOfFrame(f) == "t00F461626364\r");

	f.SetRemote(true);
	REQUIRE(u.StringRepresentationOfFrame(f) == "r00F0\r");

	f.SetExtended(true);
	REQUIRE(u.StringRepresentationOfFrame(f) == "R0000000F0\r");

	f.SetExtended(false);
	f.SetRTRDataLength(8);
	REQUIRE(u.StringRepresentationOfFrame(f) == "r00F8\r");

	f.SetID(CANFrame::EXTENDED_ID_LIMIT);
	REQUIRE(u.StringRepresentationOfFrame(f) == "R1FFFFFFF8\r");

	f.SetExtended(false);
	REQUIRE(u.StringRepresentationOfFrame(f) == "r7FF8\r");

	f.SetID(CANFrame::STANDARD_ID_LIMIT);
	REQUIRE(u.StringRepresentationOfFrame(f) == "r7FF8\r");

	f.SetRemote(false);
	REQUIRE(u.StringRepresentationOfFrame(f) == "t7FF461626364\r");

	f.SetID(CANFrame::EXTENDED_ID_LIMIT);
	REQUIRE(u.StringRepresentationOfFrame(f) == "T1FFFFFFF461626364\r");

	f.SetID(1);
	f.SetData("dd");
	f.SetExtended(false);
	REQUIRE(u.StringRepresentationOfFrame(f) == "t00126464\r");

	f.SetData("");
	REQUIRE(u.StringRepresentationOfFrame(f) == "t0010\r");

	f.SetData("0123456789");
	REQUIRE(u.StringRepresentationOfFrame(f) == "t00183031323334353637\r");
}

TEST_CASE("Converting numerics to hexadecimal strings", "[num to hex]")
{
	REQUIRE(NumToHexString(0x1EFF0A82) == "1EFF0A82");
	REQUIRE(NumToHexString(0x5AD, 0) == "5AD");
	REQUIRE(NumToHexString(0x0, 0) == "0");
	REQUIRE(NumToHexString(0xFFFFFFFF) == "FFFFFFFF");
	REQUIRE(NumToHexString(0x1) == "0001");
	REQUIRE(NumToHexString(10, 2) == "0A");
	REQUIRE(NumToHexString(0xF2, 8) == "000000F2");
}

TEST_CASE("Using CANFrame object", "[CANFrame]")
{
	CANFrame f;

	WHEN("default CANFrame object is used")
	{
		REQUIRE(std::strcmp(f.GetData(), "") == 0);
		REQUIRE(std::strlen(f.GetData()) == 0);
		REQUIRE(f.GetID() == 0);
		REQUIRE(f.GetRTRDataLength() == 0);
		REQUIRE(!f.IsExtended());
		REQUIRE(!f.IsRemote());
	}

	WHEN("CANFrame data is changed")
	{
		f.SetData("\nda,");
		REQUIRE(std::strcmp(f.GetData(), "\nda,") == 0);
		REQUIRE(std::strlen(f.GetData()) == 4);

		f.SetData("%3;");
		REQUIRE(std::strcmp(f.GetData(), "%3;") == 0);
		REQUIRE(std::strlen(f.GetData()) == 3);
	}
}

TEST_CASE("Setting baud rate command", "[set baud rate]")
{
	uCCBlibC u;
	uCCBlibC::typeOfEOL eol = u.GetEOL();
	REQUIRE(u.BaudRateSettingCommand(uCCBlibC::BAUD_100k) == "S3" + u.ConvertEolToString(eol));
	REQUIRE(u.BaudRateSettingCommand(uCCBlibC::BAUD_125k) == "S4" + u.ConvertEolToString(eol));
	REQUIRE(u.BaudRateSettingCommand(uCCBlibC::BAUD_250k) == "S5" + u.ConvertEolToString(eol));
	REQUIRE(u.BaudRateSettingCommand(uCCBlibC::BAUD_500k) == "S6" + u.ConvertEolToString(eol));
	REQUIRE(u.BaudRateSettingCommand(uCCBlibC::BAUD_800k) == "S7" + u.ConvertEolToString(eol));
	REQUIRE(u.BaudRateSettingCommand(uCCBlibC::BAUD_1M) == "S8" + u.ConvertEolToString(eol));
}

TEST_CASE("Getting one-character commands", "[one char commands]")
{
	uCCBlibC u;
	uCCBlibC::typeOfEOL eol = u.GetEOL();
	REQUIRE(u.OpenCommand() == "O" + u.ConvertEolToString(eol));
	REQUIRE(u.CloseCommand() == "C" + u.ConvertEolToString(eol));
	REQUIRE(u.HardwareVersionCommand() == "V" + u.ConvertEolToString(eol));
	REQUIRE(u.FirmwareVersionCommand() == "v" + u.ConvertEolToString(eol));
	REQUIRE(u.SerialNumberCommand() == "N" + u.ConvertEolToString(eol));
	REQUIRE(u.LoopbackModeCommand() == "l" + u.ConvertEolToString(eol));
	REQUIRE(u.ListenOnlyModeCommand() == "L" + u.ConvertEolToString(eol));
	REQUIRE(u.StatusErrorFlagCommand() == "F" + u.ConvertEolToString(eol));
}

TEST_CASE("Converting EOL enum to string", "[EOL conversion]")
{
	uCCBlibC u;
	REQUIRE(u.ConvertEolToString(uCCBlibC::EOL_CR) == "\r");
	REQUIRE(u.ConvertEolToString(uCCBlibC::EOL_LF) == "\n");
	REQUIRE(u.ConvertEolToString(uCCBlibC::EOL_CRLF) == "\r\n");
	REQUIRE(u.ConvertEolToString(uCCBlibC::EOL_NONE) == "");
}

TEST_CASE("Setting/getting EOL characters", "[eol]")
{
	uCCBlibC u;
	WHEN("Using default value")
	{
		REQUIRE(u.GetEOL() == uCCBlibC::EOL_CR);
	}
	WHEN("Using LF")
	{
		u.SetEOL(uCCBlibC::EOL_LF);
		REQUIRE(u.GetEOL() == uCCBlibC::EOL_LF);
	}
	WHEN("Using CRLF")
	{
		u.SetEOL(uCCBlibC::EOL_CRLF);
		REQUIRE(u.GetEOL() == uCCBlibC::EOL_CRLF);
	}
	WHEN("Using CR")
	{
		u.SetEOL(uCCBlibC::EOL_CR);
		REQUIRE(u.GetEOL() == uCCBlibC::EOL_CR);
	}
	WHEN("Disabled")
	{
		u.SetEOL(uCCBlibC::EOL_NONE);
		REQUIRE(u.GetEOL() == uCCBlibC::EOL_NONE);
	}
}

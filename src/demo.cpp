#include "uCCBlibC.h"
#include <iostream>
#include <iomanip>

using std::cout;
using std::cin;

int main(int argc, char * argv[])
{
	cout << "/***				   ***/\n";
	cout << " *             _____     	     * \n";
	cout << " *            / ____|                * \n";
	cout << " *      _   _| |     __ _ _ __       * \n";
	cout << " *     | | | | |    / _` | '_ \\      * \n";
	cout << " *     | |_| | |___| (_| | | | |     * \n";
	cout << " *      \\__,_|\\_____\\__,_|_| |_|     * \n";
	cout << " *                 	DEVICES      * \n";
	cout << " *          			     * \n";
	cout << " *	    C++ lib examples         * \n";
	cout << "/***				   ***/\n\n";

	cout << "\n\n";

	uCCBlibC u;
	u.SetEOL(uCCBlibC::EOL_LF);

	cout << "Open command:\n" << u.OpenCommand() << '\n';
	cout << "Close command:\n" << u.CloseCommand() << '\n';
	cout << "Firmware version command:\n" << u.FirmwareVersionCommand() << '\n';
	cout << "Hardware version command:\n" << u.HardwareVersionCommand() << '\n';
	cout << "Serial number command:\n" << u.SerialNumberCommand() << '\n';
	cout << "Status error flag command:\n" << u.StatusErrorFlagCommand() << '\n';
	cout << "Loopback mode command:\n" << u.LoopbackModeCommand() << '\n';
	cout << "Listen-only mode command:\n" << u.ListenOnlyModeCommand() << '\n';
	cout << "Baud rate 100k command:\n" << u.BaudRateSettingCommand(uCCBlibC::BAUD_100k) << '\n';
	cout << "Send standard remote frame (id = 0x1, data length = 8):\n" << 
		u.PrepareRemoteFrame(0x1, false, 8).ToString() << "\n\n";

	cout << "Send extended remote frame (id = 0x1FFFF, data length = 4):\n" <<
		u.PrepareRemoteFrame(0x1FFFF, true, 4).ToString() << "\n\n";

	cout << "Send standard data frame (id = 0x1, message \"canExampleMessage\"\nhex values: "
			"{63 61 6e 45 78 61 6d 70 6c 65 4d 65 73 73 61 67 65}):\n";
	auto v = u.PrepareDataFrames(0x1, false, "canExampleMessage");
	for (auto i : v)
		cout << i.ToString() << '\n';

	cout << '\n';
	cout << "Send extended data frame (id = 0x1, message \"extendedFrame\"\nhex values: "
			"{65 78 74 65 6e 64 65 64 46 72 61 6d 65}):\n";
	v = u.PrepareDataFrames(0x1, true, "extendedFrame");
	for (auto i : v)
		cout << i.ToString() << '\n';

	cout << '\n';
	cout << "Set filter for id = 0xFF (filter number and bank number equal 0):\n" <<
		u.AcceptanceFilter(0x0, 0x0, 0xFF) << '\n';

	cin.get();
	return 0;
}
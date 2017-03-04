#include "uCCBlibC.h"
#include <iostream>
#include "rs232.h"

using std::cout;
using std::cin;

void RS232_ReadData(int cport_nr, unsigned char* buf)
{
	int n = 0;
	while ((n = RS232_PollComport(cport_nr, buf, 4095)) == 0);

	if (n > 0)
	{
		buf[n] = 0;   /* null termination at the end of a string */

		for (int i = 0; i < n; i++)
		{
			if (buf[i] < 32)  /* replace unreadable control-codes by dots */
			{
				buf[i] = '.';
			}
		}
	}
}

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

	/* Using RS232 library to send commands via serial to the device */

	u.SetEOL(uCCBlibC::EOL_CR);
	int cport_nr = RS232_GetPortnr("COM3"),		/* argument depends on OS and port with UCCB device */
		bdrate = 115200;						/* 115200 baud */

	unsigned char buf[4096];

	char mode[] = { '8', 'N', '1', 0 };			/* 8 data bits, none parity, 1 stop bit, no handshaking */

	if (RS232_OpenComport(cport_nr, bdrate, mode))
	{
		cout << "Cannot open port\n";
		return -1;
	}

	/* close can communication */
	RS232_cputs(cport_nr, u.CloseCommand().data());
	RS232_ReadData(cport_nr, buf);
	cout << "Received: " << buf << '\n';

	/* get serial number */
	RS232_cputs(cport_nr, u.SerialNumberCommand().data());
	RS232_ReadData(cport_nr, buf);
	cout << "Received: " << buf << '\n';

	/* get hardware number */
	RS232_cputs(cport_nr, u.HardwareVersionCommand().data());
	RS232_ReadData(cport_nr, buf);
	cout << "Received: " << buf << '\n';

	/* open can communication */
	RS232_cputs(cport_nr, u.OpenCommand().data());
	RS232_ReadData(cport_nr, buf);
	cout << "Received: " << buf << '\n';

	/* send data frames */
	v = u.PrepareDataFrames(0x1, false, "canExampleMessage");
	for (auto i : v)
	{
		RS232_cputs(cport_nr, u.StringRepresentationOfFrame(i).data());
		RS232_ReadData(cport_nr, buf);
		cout << "Received: " << buf << '\n';
	}

	/* close can communication */
	RS232_cputs(cport_nr, u.CloseCommand().data());
	RS232_ReadData(cport_nr, buf);
	cout << "Received: " << buf << '\n';


	RS232_CloseComport(cport_nr);

	cout << "Press enter to exit...";
	cin.get();
	return 0;
}
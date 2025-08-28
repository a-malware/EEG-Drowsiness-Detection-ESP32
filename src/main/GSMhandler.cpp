#include "GSMHandler.h"

GSMHandler::GSMHandler(SoftwareSerial& serial) : _serial(serial) {}

bool GSMHandler::begin() {
	_serial.begin(9600);
	delay(1000);
	return _initializeGSM();
}

bool GSMHandler::_initializeGSM() {
	_serial.println("AT");
	delay(1000);
	if (_serial.find("OK")) {
		_serial.println("AT+CMGF=1");  // Set text mode
		delay(1000);
		return _serial.find("OK");
	}
	return false;
}

bool GSMHandler::sendSMS(const char* number, const char* message) {
	if (!_initializeGSM()) return false;

	_serial.print("AT+CMGS=\"");
	_serial.print(number);
	_serial.println("\"");
	delay(1000);
	_serial.println(message);
	delay(1000);
	_serial.write(0x1A);  // Ctrl+Z to send
	delay(1000);

	return _serial.find("OK");
}
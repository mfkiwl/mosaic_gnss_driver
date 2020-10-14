#include <iostream>
#include <fstream>
#include <iomanip>
#include <mosaic_gnss_driver/parsers/nmeaparse/NMEAParser.h>
//#include <nmeaparse/NMEACommand.h>
#include <mosaic_gnss_driver/parsers/nmeaparse/GPSService.h>

using namespace std;
using namespace nmea;

  
int main(){

	// Fill with your NMEA bytes... make sure it ends with \n
	//char bytestream[] = {"$GPGGA,123148.00,1118.6640322,N,07548.1240164,E,1,15,0.8,11.3353,M,-91.7608,M,,*73\n" 
	//					"$GPRMC,123148.00,A,1118.6640322,N,07548.1240164,E,0.0,,180920,1.3,E,A*12\n"};

	// Create a GPS service that will keep track of the fix data.
	NMEAParser parser;
	GPSService gps(parser);
	parser.log = false;
	
	/*cout << "Fix  Sats  Sig\t\tSpeed    Dir  Lat         , Lon           Accuracy" << endl;
	// Handle any changes to the GPS Fix... This is called whenever it's updated.
	gps.onUpdate += [&gps](){
		cout << (gps.fix.locked() ? "[*] " : "[ ] ") << setw(2) << setfill(' ') << gps.fix.trackingSatellites << "/" << setw(2) << setfill(' ') << gps.fix.visibleSatellites << " ";
		cout << fixed << setprecision(2) << setw(5) << setfill(' ') << gps.fix.almanac.averageSNR() << " dB   ";
		cout << fixed << setprecision(2) << setw(6) << setfill(' ') << gps.fix.speed << " km/h [" << GPSFix::travelAngleToCompassDirection(gps.fix.travelAngle, true) << "]  ";
		cout << fixed << setprecision(6) << gps.fix.latitude << "\xF8 " "N, " << gps.fix.longitude << "\xF8 " "E" << "  ";
		cout << "+/- " << setprecision(1) << gps.fix.horizontalAccuracy() << "m  ";
		cout << endl;
	};
   */



	// -- STREAM THE DATA  ---

	// From a buffer in memory...
   
	//parser.readBuffer((uint8_t*)bytestream, sizeof(bytestream));
	//cout << gps.fix.toString() << endl;
	// -- OR --
	// From a device byte stream...
	// gps.parser.readByte(byte_from_device);

	// -- OR --
	// From a file
	string line;
	ifstream file("/home/sachi/catkin_ws/src/mosaic_gnss_driver/test/data/nmea/capture_004.nmea");
	while (getline(file, line)){
		try {
			parser.readSentence(line);
			cout << gps.fix.toString() << endl;
		}
		catch (NMEAParseError& e){
			cout << e.message << endl << endl;
			// You can keep feeding data to the gps service...
			// The previous data is ignored and the parser is reset.
		}
	}
	
	// Show the final fix information*/
	//cout << gps.fix.toString() << endl;


	//cin.ignore();


	return 0;
}


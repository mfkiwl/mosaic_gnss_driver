# NMEAParser

*Cross platform C++ 11 NMEA Parser & GPS Framework*

## It's too easy!
This is all you need to use the GPS NMEA sentence data.


    NMEAParser parser;
    GPSService gps(parser);
    // (optional) Called when a sentence is valid syntax
    parser.onSentence += [](const NMEASentence& nmea){
        cout << "Received $" << nmea.name << endl;
    };
    // (optional) Called when data is read/changed
    gps.onUpdate += [](GPSService& gps){
        // There are *tons* of GPSFix properties
        if( gps.fix.locked() ){
            cout << " # Position: " << gps.fix.latitude << ", " << gps.fix.longitude << endl;
        } else {
            cout << " # Searching..." << endl;
        }
    };
    // Send in a log file or a byte stream
    try {
        parser.readLine("FILL WITH A NMEA MESSAGE");
    } catch (NMEAParseError&) {
        // Syntax error, skip

##Features

* **NMEA Parsing** of standard and custom sentences.

  - Standard:
```` 
    GPGGA, GPGSA, GPGSV, GPRMC, GPVTG
````

* **GPS Fix** class to manage and organize all the GPS related data.


* **Flexible**
   - Stream data directly from a hardware byte stream
   - Read log files, even if they are cluttered with other unrelated data.
   - Easily hook into the parser to handle custom sentences and get their data.
   - Simplified GPS data

## Details
NMEA is very popular with GPS. Unfortunately the GPS data is fragmented
between many different NMEA sentences. Sometimes there is even conflicting 
information in them!

The underlying **NMEAParser** can read a byte stream from a hardware device or 
it can read log files with flexibility in the formatting. The accepted strings are...

````
"$[name:alphanum],[param[i]:alphanum],...(*alphanum[2]\r)\n"
````

The params allow '-' and any combination of whitespace.

The handler for "MYNMEA" is called. This is where you can catch the sentence and process it.

    parser.setSentenceHandler("MYNMEA",[](const NMEASentence& nmea){
        if( ! nmea.checksumOK() ){
            // bad checksum, but valid syntax
        }
        if( nmea.parameters.size() < 3 ){
            // missing data, throw something.
            // catch it at the read*() call.
        }
        int mydata = parseInt(nmea.parameters[2]);
    };



There are 2 ways to operate...

* **LAX**  It will eat anything.
    - Useful for reading log files.
    - Call ````readByte(), readBuffer(), readLine()````
* **STRICT**   It will throw errors on anything that's not explicitly NMEA.
    - Call ```` readSentence() ````


## GPS Fix data available
Available data when all 5 GPS sentences are received. If some are missing then some parameters will never change from their default values.
All data is checked for consistency. For example, visible satellites can never be less than the tracking satellites, etc.


**GPSFix**

    GPSAlmanac     almanac;
    GPSTimestamp 	timestamp;

    char 		status;		// Status: A=active, V=void (not locked)
    uint8_t 	type;		// Type: 1=none, 2=2d, 3=3d
    uint8_t 	quality;	// Quality (1-6) 

    double 		dilution;		// Combination of Vertical & Horizontal
    double 		horizontalDilution;	// Horizontal dilution of precision, initialized to 10, best =1, worst = >20
    double 		verticalDilution;	// Vertical is less accurate

    double 		altitude;		// meters
    double 		latitude;		// degrees N
    double 		longitude;		// degrees E
    double 		speed;			// km/h
    double 		travelAngle;		// degrees true north (0-359)
    int32_t 	trackingSatellites;
    int32_t 	visibleSatellites;

    bool 		locked();		// Whether or not the position is locked on, or accurate.
    double 		horizontalAccuracy();	// Gets accuracy of position in meters
    double 		verticalAccuracy();
    bool 		hasEstimate();		// If no fix is available, this says the position data is close to a real fix.
		
    std::chrono::seconds timeSinceLastUpdate();	// Returns time from last timestamp to right now, in seconds.
    

**GPSSatellite**

    double 		snr;		// Signal-to-noise ratio. 0-99 dB
    uint32_t 	prn;		// pseudo-random number.  (basically a satellite id)
    double 		elevation;	// 0-90 deg
    double 		azimuth;	// 0-359 deg


**GPSAlmanac**

    std::vector<GPSSatellite> satellites;	// mapped by prn (id number)
    double 		averageSNR(); 
    double 		minSNR();
    double 		maxSNR();
    double 		percentComplete();	// if all the satellite information is loaded (0-100)

		
**GPSTimestamp**    *(UTC Time)*

    int32_t 	hour;		// Parsed values from GPS
    int32_t 	min;		
    double 		sec;		
    int32_t 	month;		
    int32_t 	day;		
    int32_t 	year;		
    double 		rawTime;	// Values collected directly from the GPS
    int32_t 	rawDate;	
    time_t 		getTime();	// Converts timestamp into Epoch time, seconds since 1/1/1970.

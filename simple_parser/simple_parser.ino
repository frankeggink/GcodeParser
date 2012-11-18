//simple parser
//Frank Eggink 11-11-2012
//
//eerste versie van een parser
//parser is in staat om te bepalen of het om een G,M code gaat
//parser is in staat om te bepalen om welk type het gaat, bijvoorbeeld G1, G2,M300, op basis van het 2e karakter in het commando)
//getValue kan de x en y waarde uit een string halen


#include <SoftwareServo.h>

//servo variables

SoftwareServo servo;
boolean servoEnabled = true;

//parser variables
char *strchr_pointer;
double value;
int codenum;



//comm variables
const int MAX_CMD_SIZE = 256;
char buffer[MAX_CMD_SIZE];
char serial_char;
int serial_count = 0;
boolean comment_mode = false;


void setup() 
{
	Serial.begin(9600);

	servo.attach(2);
	servo.write(50);

	if (servoEnabled)
	{
		for(int i=0; i<100; i++)
		{
			SoftwareServo::refresh();
			delay(4);
		}
	}

}

void loop()
{

	//parse("G21");
	//Serial.println();
	//parse("G90");
	//Serial.println();
	//parse("G92 X0.00 Y0.00 Z0.00");
	//Serial.println();
	//parse("M300 S50.00");
	//Serial.println();
	//parse("G1 X-23.84 Y0.67 F3500.00");
	//Serial.println();
	//parse("G1 X-66.56 Y4.26 F3500.00");
	//Serial.println();
	//parse("G4 P150");
	//Serial.println();


	//parse("Hallo");
	//Serial.println();
	//delay(10000);

	getSerialCommand();
	//delay(100);
	if (servoEnabled)
	{
		SoftwareServo::refresh();
	}

}



void getSerialCommand()
{
	//TODO : remove spaces at start of command 

	if(Serial.available() > 0) {
		serial_char = Serial.read();
		// say what you got:
		//Serial.print("I received: ");
		//Serial.println(serial_char, DEC);
		if (serial_char == '\n' || serial_char == '\r')
		{
			buffer[serial_count] = 0;
			//TODO: process command

			parse(buffer);
			//Serial.println(buffer);
			//Serial.println();


			clear_buffer();
			comment_mode = false;
		}
		else //not end of command
		{
			if(serial_char == ';' || serial_char == '(') 
			{ 
				comment_mode = true;
			}
			if (comment_mode != true)
			{
				buffer[serial_count] = serial_char;
				serial_count++;
				if(serial_count > MAX_CMD_SIZE)
				{

					clear_buffer();
					Serial.flush();

				}
			}
		}     
	}
	//	else if (Serial.available() <=0) {
	//		Serial.println("arduino is not receiving");
	//	}
}


void clear_buffer()
{
	serial_count = 0;
}


//function to parse simple gcode
void parse(char *command) 
{

	double xVal;
	double yVal;
	double pVal;
	double fVal;
	double sVal;

	boolean hasXVal;
	boolean hasYVal;
	boolean hasPVal;
	boolean hasFVal;
	boolean hasSVal;

	if (command != NULL) {
		if (command[0] == 'G') {
			//G code 
			Serial.println("G Code");

			if(isdigit(command[1])) {  //is the second character a number?
				codenum = (int)strtod(&command[1],NULL);
			} else { 
				codenum = -1; //something is wrong with the received command
			}
			Serial.println(codenum); 
			hasXVal = getValue('X', command, &xVal);
			hasYVal = getValue('Y', command, &yVal);
			hasPVal = getValue('P', command, &pVal);
			hasFVal = getValue('F', command, &fVal);


			if(hasXVal) {
				Serial.print("X-value : ");
				Serial.println(xVal);
			}

			if(hasYVal) {
				Serial.print("Y-value : ");
				Serial.println(yVal);
			}
			if(hasPVal) {
				Serial.print("P-value : ");
				Serial.println(pVal);
			}
			if(hasFVal) {
				Serial.print("F-value : ");
				Serial.println(fVal);
			}


			switch(codenum)
			{
			case 0: //G0, Rapid positioning
				Serial.println("NOW IN CASE 0");
				break;
			case 1: //G1, linear interpolation at specified speed
				Serial.println("G1 not implemented");
				break;
			case 2: //G2, Clockwise arc
			case 3: //G3, Counterclockwise arc
				Serial.println("G2/G3 not implemented");
				break;
			case 4: //G4, Delay P ms
				Serial.println("G4 not implemented");
				break;
			case 90: //G90, Absolute Postioning
				Serial.println("G90 not implemented");
				break;
			case 91: //G91, Incremental Positioning
				Serial.println("G91 not implemented");
				break;
			default:
				Serial.println("you've entered the default case!! error!?!?");
			}      


		} else if (command[0] == 'M') {
			//M code
			Serial.println("M Code");

			if(isdigit(command[1])) {  //is the second character a number?
				codenum = (int)strtod(&command[1],NULL);
			} else { 
				codenum = -1; //something is wrong with the received command
			}
			Serial.println(codenum); 

			hasSVal = getValue('S', command, &sVal);

			if(hasSVal) {
				Serial.print("S-value : ");
				Serial.println(sVal);
			}



			switch(codenum)
			{
			case 18: //M18, disable drives
				Serial.println("M18 not implemented");
				break;
			case 300: //M300, servo position
				//Serial.println("M300 not implemented");
				if (hasSVal) {
					servoEnabled = true;
				}
				if (sVal <0.) {
					sVal = 0;
				} else if (sVal > 180.) {
					sVal = 50; //default position
					servo.write((int)sVal);
					for (int i=0;i<100;i++) {
						SoftwareServo::refresh();
						delay(4);
					}
					servoEnabled = false;
				}
				servo.write((int)sVal);

				break;
			case 400: //M400, Reset X-Axix-Stepper settings to new object diameter
				Serial.println("G400 not implemented");
				break;
			case 401: //M401, Reset Y-Axis-Stepper settings to new object diameter
				Serial.println("M401 not implemented");
				break;
			case 402: //M402, set global zoom factor
				Serial.println("M402 not implemented");
				break;
			default:
				Serial.println("you've entered the default case!! error!?!?");
			}      

		} else {
			//unknown command
			Serial.println("Unknown command");
		}
	}
}


//helper function to determine if a command contains a specific key value
boolean getValue(char key, char *command, double *value)
{
	//find key parameter
	strchr_pointer = strchr(command,key);
	if (strchr_pointer != NULL) //We found a key value
	{
		*value = (double)strtod(&command[strchr_pointer-command+1],NULL);
		return true;
	} else {
		return false;
	}
}




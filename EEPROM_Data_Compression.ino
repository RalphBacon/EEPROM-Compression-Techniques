#include "Arduino.h"
/*
 * This demo shows how to save memory in EEPROM by fitting 3 alphanumeric characters into
 * just two bytes, giving a 1/3 saving in memory. Uses a variant of RADIX-50, see this
 * article: https://en.wikipedia.org/wiki/DEC_Radix-50 , 16-bit systems part.
*/

// This is our RADIX-40 lookup table (although we're not using all 40 chars in this demo)
String lookup = " ABCDEFGHIJKLMNOPQRSTUVWXYZ!,."; // NB first char is a space

void setup() {
	Serial.begin(9600);
	Serial.println("Program started.");

	// Our string to be written to EEPROM (we won't do that in our demo)
	char testString[60]= "RALPH BACON IS AN ARDUINITE, WHO HAS A YOUTUBE CHANNEL!";

	// Ensure the string is a multiple of 3 bytes just for convenience in this demo
	while (strlen(testString) % 3 != 0) {
		char space[] = " ";
		strcat(testString, space);
	}

	// The return array of integers will be the two thirds of the message (integers take two bytes)
	unsigned int returnArraySize = (strlen(testString) / 3);

	// Create the return array of 2-byte integers that will each contain 3 alpha chars
	unsigned int returnIntArray[returnArraySize];

	// ENCODE the text message
	encodeArray(testString, returnIntArray);

	// This is our return array of integers
	for (unsigned int cnt = 0; cnt < returnArraySize; cnt++) {
		Serial.print(returnIntArray[cnt]);Serial.print(", ");
	}
	Serial.println("");

	// DECODE the compacted integer array
	decodeArray(returnIntArray, returnArraySize);

	// Memory savings?
	Serial.print("Original text size: "); Serial.println(strlen(testString));
	Serial.print("Compacted int size: "); Serial.println(returnArraySize * 2);
	Serial.print("Savings: ");Serial.print(((float)(strlen(testString) - returnArraySize *2) / strlen(testString)) * 100); Serial.println("%");
}

//---------------------------------------------------------------------------------------
// Packing routine - note: array are always passed by reference, a pointer
//---------------------------------------------------------------------------------------
void encodeArray(char * unpackedMessage, unsigned int * returnArray) {
	Serial.println("---------------------");
	Serial.println("Encode Array started.");

	unsigned int packed3chars;
	unsigned int iteration = 0;
	for (unsigned int cnt = 0; cnt < strlen(unpackedMessage); cnt += 3) {

		// find the char in the lookup. We need the index (the nth value)
		int charLocation = lookup.indexOf(unpackedMessage[cnt]);

		// 1st char is multiplied by 40 ^ 2 (to the power of 2)
		packed3chars = charLocation * (40 * 40);

		// 2nd characters is multiplied by 40 ^1 (which is just 40)
		charLocation = lookup.indexOf(unpackedMessage[cnt + 1]);
		packed3chars += charLocation * (40);

		// 3rd character is multiplied by 40 ^ 0 (which is 1)
		charLocation = lookup.indexOf(unpackedMessage[cnt + 2]);
		packed3chars += charLocation * (1); //Yes, it's superfluous (it's just the char location value) but demonstrates the pattern

		// Now packed3chars contains two bytes (an integer) but represents 3 characters. Add its value to our array.
		returnArray[iteration++] = packed3chars;
	}

	Serial.println("Encode Array finished.");
	Serial.println("---------------------");
}

//---------------------------------------------------------------------------------------
// Unpacking routine - note: arrays are always passed by reference (pointer)
//---------------------------------------------------------------------------------------
void decodeArray(unsigned int /* testPacked */ testPacked[], unsigned int length) {

	Serial.println("---------------------");
	Serial.println("Decode Array started.");

	// Debugging just to prove the input array of integers is as expected
	for (unsigned int cnt = 0; cnt < length; cnt++) {
		Serial.print(testPacked[cnt]);Serial.print(", ");
	}
	Serial.println("");

	// This is the final output for all packed characters
	String outputMessage;
	String finalMessage = "";

	// Uncompress each byte of the 2-byte integer array
	for (unsigned int intCnt = 0; intCnt < length; intCnt++) {

		// Initialise the output string for the next 3 characters
		outputMessage = "";

		// Get the integer value for this round (eg 24896)
		unsigned int currInt = testPacked[intCnt];

		// Three chars per integer so loop round (retrieved in reverse order)
		for (unsigned int cnt = 0; cnt < 3; cnt++)
		{
			// Take the modulus (remainder) value - this is our character (value)
			short int currChar = currInt % 40;

			// Prepend the char to the front of the existing string so they get put back in correct order
			outputMessage = lookup[currChar] + outputMessage;

			// Subtract the char value from remaining value, divide by 40 and loop
			currInt = (currInt - currChar) / 40;
		}

		// Each chunk of 3 characters for this integer is concatenated to final output string
		finalMessage += outputMessage;
	}

	Serial.println(finalMessage);
	Serial.println("Decode Array finished.");
	Serial.println("---------------------");
}

void loop() {
}

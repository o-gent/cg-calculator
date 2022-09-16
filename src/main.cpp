#include "HX711.h"
#include "LiquidCrystal_I2C.h"

/**
 * An example useage of the HX711 librabry https://github.com/bogde/HX711/blob/master/examples/HX711_full_example/HX711_full_example.ino
 * Screen example https://github.com/johnrickman/LiquidCrystal_I2C/blob/master/examples/HelloWorld/HelloWorld.pde
 */

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
HX711 C1_scale;
HX711 C2_scale;
HX711 C3_scale;
HX711 C4_scale;

// HX711 circuit wiring, the 8 HX711 data pins can use an digitial pin
const int C1_LOADCELL_DOUT_PIN = 3;
const int C1_LOADCELL_SCK_PIN = 2;
const int C2_LOADCELL_DOUT_PIN = 5;
const int C2_LOADCELL_SCK_PIN = 4;
const int C3_LOADCELL_DOUT_PIN = 7;
const int C3_LOADCELL_SCK_PIN = 6;
const int C4_LOADCELL_DOUT_PIN = 9;
const int C4_LOADCELL_SCK_PIN = 8;

float C1_reading;
float C2_reading;
float C3_reading;
float C4_reading;
float total;


void setup()
{
    Serial.begin(57600);
    C1_scale.begin(C1_LOADCELL_DOUT_PIN, C1_LOADCELL_SCK_PIN);
    C2_scale.begin(C2_LOADCELL_DOUT_PIN, C2_LOADCELL_SCK_PIN);
    C3_scale.begin(C3_LOADCELL_DOUT_PIN, C3_LOADCELL_SCK_PIN);
    C4_scale.begin(C4_LOADCELL_DOUT_PIN, C4_LOADCELL_SCK_PIN);

    Serial.println("Calibrate start, clear scales");
    C1_scale.get_scale();
    C1_scale.tare();

    C2_scale.get_scale();
    C2_scale.tare();

    C3_scale.get_scale();
    C3_scale.tare();

    C4_scale.get_scale();
    C4_scale.tare();

    Serial.println("place a known mass at the centre of the plate if you want a correct total mass reading");
    Serial.println("place any mass at the centre of the plate otherwise");
    Serial.println("you have 5 seconds");
    delay(5000);

    long r1 = C1_scale.get_units(10);
    long r2 = C2_scale.get_units(10);
    long r3 = C3_scale.get_units(10);
    long r4 = C4_scale.get_units(10);

    Serial.println("Enter mass (g) or press enter for unknown mass :");
    String mass_input = Serial.readStringUntil('\n');
    int mass = mass_input.toInt();
    if(mass==0) // mass will be 0 if they just pressed enter
        mass = 1000;

    C1_scale.set_scale(r1 / mass);
    C2_scale.set_scale(r2 / mass);
    C3_scale.set_scale(r3 / mass);
    C4_scale.set_scale(r4 / mass);
}


void loop()
{
    /**
     * Sensor readings and math
     */
    
    C1_reading = C1_scale.get_units(1);
    C2_reading = C2_scale.get_units(1);
    C3_reading = C3_scale.get_units(1);
    C4_reading = C4_scale.get_units(1);
    total = C1_reading + C2_reading + C3_reading + C4_reading;

    // converting CX_reading to float to do float division.
    // could maybe have this as a float in the first place
    float C1_load = ((float)C1_reading) / total;
    float C2_load = ((float)C2_reading) / total;
    float C3_load = ((float)C3_reading) / total;
    float C4_load = ((float)C4_reading) / total;

    /**
     * Serial debugging output
     */
    Serial.print("total weight: "); 
    Serial.println(total);
    
    Serial.print("% load C1, C2, C3, C4: ");
    Serial.print(C1_load); Serial.print(",");
    Serial.print(C2_load); Serial.print(",");
    Serial.print(C3_load); Serial.print(",");
    Serial.println(C4_load);
    
    Serial.print("X: ");
    Serial.println((C4_load + C3_load) / 1 - (C1_load + C2_load) / 1);
    Serial.print("Y: ");
    Serial.println((C1_load + C4_load) / 1 - (C2_load + C3_load) / 1);
    
    Serial.println();

    delay(500);
}
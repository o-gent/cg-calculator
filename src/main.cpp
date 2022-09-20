#include "HX711.h"
#include "LiquidCrystal_I2C.h"
#include <EEPROM.h>

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

float r1;float r2;float r3;float r4;
int cal_mass = 1000;
float C1_reading; float C2_reading;
float C3_reading; float C4_reading;
float total;
bool CALIBRATE;

/**
 * routine to print basic info + tare scales
 */
void startup()
{

    pinMode(12, INPUT_PULLUP);
    CALIBRATE = digitalRead(12);

    lcd.init();
    lcd.backlight();
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("--------------------");
    lcd.setCursor(0,3);
    lcd.print("--------------------");
    lcd.setCursor(0,1);
    lcd.print("CL CG finder");
    delay(1000);
    lcd.clear();
    
    lcd.setCursor(0,0);
    lcd.print("Calibration:");
    CALIBRATE == 1 ? lcd.print("true") : lcd.print("false");
    delay(2000);
    lcd.clear();
    
    C1_scale.begin(C1_LOADCELL_DOUT_PIN, C1_LOADCELL_SCK_PIN);
    C2_scale.begin(C2_LOADCELL_DOUT_PIN, C2_LOADCELL_SCK_PIN);
    C3_scale.begin(C3_LOADCELL_DOUT_PIN, C3_LOADCELL_SCK_PIN);
    C4_scale.begin(C4_LOADCELL_DOUT_PIN, C4_LOADCELL_SCK_PIN);

    Serial.println("tare start, clear scales");
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tare: Clear Scales");
    lcd.setCursor(0,1); lcd.print("3"); delay(1500);
    lcd.setCursor(0,2); lcd.print("2"); delay(1500);
    lcd.setCursor(0,3); lcd.print("1"); delay(1500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("keep clear..");

    
    C1_scale.get_scale(); C1_scale.tare();
    C2_scale.get_scale(); C2_scale.tare();
    C3_scale.get_scale(); C3_scale.tare();
    C4_scale.get_scale(); C4_scale.tare();
}


/**
 * Routine to get the calibration values for the scales
 */
void calibrate()
{
    Serial.println("place a known cal_mass at the centre of the plate if you want a correct total mass reading");
    Serial.println("place any mass at the centre of the plate otherwise");
    Serial.println("you have 5 seconds");

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Place known weight");
    lcd.setCursor(0,1); lcd.print("3"); delay(1500);
    lcd.setCursor(0,2); lcd.print("2"); delay(1500);
    lcd.setCursor(0,3); lcd.print("1"); delay(1500);
    lcd.clear();

    r1 = C1_scale.get_units(10);
    r2 = C2_scale.get_units(10);
    r3 = C3_scale.get_units(10);
    r4 = C4_scale.get_units(10);

    Serial.println("Enter mass (g) or press enter for unknown mass (you have 20s) :");
    Serial.setTimeout(20000);
    lcd.setCursor(0,0);
    lcd.print("enter mass through");
    lcd.setCursor(0,1);
    lcd.print("serial connection.");
    lcd.setCursor(0,2);
    lcd.print("press enter after");
    lcd.setCursor(0,2);
    lcd.print("number : )");
    String cal_mass_input = Serial.readStringUntil('\n');
    cal_mass = cal_mass_input.toInt();
    if(cal_mass==0) // cal_mass will be 0 if they just pressed enter
        cal_mass = 1000;
    // Store the calibrated value
    EEPROM.put(0, cal_mass);
    EEPROM.put(10, r1);
    EEPROM.put(20, r2);
    EEPROM.put(30, r3);
    EEPROM.put(40, r4);
}

void setup()
{
    Serial.begin(57600);

    startup();

    if(CALIBRATE)
        calibrate();
    else {
        Serial.println("Using stored calibration value");
        Serial.println("Set CALIBRATE to true in code to recal");
        delay(4000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Using stored cal val");
        lcd.setCursor(0,2);
        lcd.print("use switch to re-cal");
    }

    // read in the calibrated mass value from EEPROM
    EEPROM.get(0, cal_mass);
    EEPROM.get(10, r1); EEPROM.get(20, r2);
    EEPROM.get(30, r3); EEPROM.get(40, r4);
    Serial.println(cal_mass);

    C1_scale.set_scale(r1 / cal_mass);
    C2_scale.set_scale(r2 / cal_mass);
    C3_scale.set_scale(r3 / cal_mass);
    C4_scale.set_scale(r4 / cal_mass);
}


void loop()
{
    /**
     * Sensor readings and math
     */
    
    C1_reading = C1_scale.get_units(5);
    C2_reading = C2_scale.get_units(5);
    C3_reading = C3_scale.get_units(5);
    C4_reading = C4_scale.get_units(5);
    total = C1_reading + C2_reading + C3_reading + C4_reading;

    // converting CX_reading to float to do float division.
    // could maybe have this as a float in the first place
    float C1_load = ((float)C1_reading) / total;
    float C2_load = ((float)C2_reading) / total;
    float C3_load = ((float)C3_reading) / total;
    float C4_load = ((float)C4_reading) / total;

    float X = (C4_load + C3_load) / 1 - (C1_load + C2_load) / 1;
    float Y = (C1_load + C4_load) / 1 - (C2_load + C3_load) / 1;

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
    Serial.println(X);
    Serial.print("Y: ");
    Serial.println(Y);
    
    Serial.println();

    /**
     * LCD display
     */
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("T W:");
    lcd.print(total);

    lcd.setCursor(0,1);
    lcd.print("X:");
    lcd.print(X);
    
    lcd.setCursor(0,2);
    lcd.print("Y:");
    lcd.print(Y);
}
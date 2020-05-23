/* T. Simpson
 * 22-May-2020
 * 
 *  This is a modification of the Elegoo "phonecal.ino" test program.
 * The program gets a password from the user using a Elegoo 2.8" TFTLCD
 * shield touch screen as the input device.  Set the password in the global 
 * string variable "PassWord". Use the "Entr" key to check your entry.
 * A worthy upgrade to this would be to add a piezo to generate a tone
 * when a key is pressed.
 */
 
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

/******************* UI details */
#define BUTTON_X 40
#define BUTTON_Y 100
#define BUTTON_W 60
#define BUTTON_H 30
#define BUTTON_SPACING_X 20
#define BUTTON_SPACING_Y 20
#define BUTTON_TEXTSIZE 2

// text box where numbers go
#define TEXT_X 10
#define TEXT_Y 10
#define TEXT_W 220
#define TEXT_H 50
#define TEXT_TSIZE 3
#define TEXT_TCOLOR ILI9341_MAGENTA
// the data (phone #) we store in the textfield
#define TEXT_LEN 12
char textfield[TEXT_LEN+1] = "";
uint8_t textfield_i=0;
String PassWord="123*456"; // The password. Must be 12 numbers or less and can use * and #.
uint16_t identifier;       // LCD driver identifier

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920
// We have a status line for feedback
#define STATUS_X 10
#define STATUS_Y 65
#define MINPRESSURE 2     //was 10
#define MAXPRESSURE 3000  //was 1000

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

Elegoo_GFX_Button buttons[15];
/* create 15 buttons, in classic candybar phone style */
char buttonlabels[15][5] = {"Entr", "Clr", "End", "1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#" };
uint16_t buttoncolors[15] = {ILI9341_DARKGREEN, ILI9341_DARKGREY, ILI9341_RED, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_ORANGE, ILI9341_BLUE, ILI9341_ORANGE};
                             
void setup(void) 
{
  Serial.begin(9600);
  Serial.println(F("Keypad Touch Screen Password Entry"));
  Serial.println();
  
#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.8\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();
  identifier = tft.readID();
  
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
     identifier=0x9341;
     Serial.println(F("Found 0x9341 LCD driver"));
  }
  else 
  {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the breakout config, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
  }
  draw_keypad();
}

void loop(void) 
{
  bool pw_ok = false;
  Serial.println();
  Serial.println("Getting password");
  delay(1000);
  
  pw_ok = check_keypad_pw();  // draw keypad and get password
  
  if (pw_ok==true)
     Serial.println("The password entered was correct");
  else
     Serial.println("The password entered was NOT correct");  // exited without pw (End)

  // The rest of your code goes here. For testing, 
  // we are just clearing the screen and looping.

  tft.setCursor(0,0);
  tft.fillScreen(BLACK);                            // clear the screen
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);  // yellow text on black background
  tft.setTextSize(2);                    // size 2~18 chracters width in portrait mode
  tft.println("Main program");
  tft.println("would start here.");
  tft.println("Program will loop");
  tft.println("in 4 seconds...");

  Serial.println("waiting 4 seconds...");
  Serial.println();
  delay(3000);
  draw_keypad();         // re-draw the keypad and get password

}    // end loop

// *************** subroutines ******************

void draw_keypad()
{
  tft.begin(identifier);
  tft.setRotation(2);
  tft.fillScreen(BLACK);
  
  // create buttons
  for (uint8_t row=0; row<5; row++) 
  {
    for (uint8_t col=0; col<3; col++) 
    {
      buttons[col + row*3].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
        BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
        BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolors[col+row*3], ILI9341_WHITE,
        buttonlabels[col + row*3], BUTTON_TEXTSIZE); 
      buttons[col + row*3].drawButton();
    }
  }
  // create 'text field'
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, ILI9341_WHITE);
}

// Print something in the mini status bar with either flashstring
void status(const __FlashStringHelper *msg) 
{
  tft.fillRect(STATUS_X, STATUS_Y, 240, 8, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);  // was size 1
  tft.print(msg);
}

void clear_status_bar()
{
  tft.fillRect(STATUS_X, STATUS_Y, 240, 16, ILI9341_BLACK);  // was 8
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
}

// or charstring
void status(char *msg) 
{
  tft.fillRect(STATUS_X, STATUS_Y, 240, 8, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);    // was size 1
  tft.print(msg);
}

bool check_password(String pw)
{ 
  if (pw==PassWord)
    return true;
  else
    return false;
}

void clr_textfield()
{ // Clear the area that displays the keypad entries
  if (textfield > 0) 
  {
     for (int i=0; i<=TEXT_LEN; i++)
     {
        textfield[i] = ' ';
     }
     textfield_i = 0;
  }
  
  // update the current text field
  Serial.println(textfield);
  tft.setCursor(TEXT_X + 2, TEXT_Y+10);
  tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
  tft.setTextSize(TEXT_TSIZE);
  tft.print(textfield);  // re-create 'text field' border
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, ILI9341_WHITE);
}

bool check_keypad_pw()
{ // draw the keypad and get the entries
  status(F("Enter Password"));
  bool pwok=false;
  while (pwok==false)
  {
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!
    // Scale from ~0->4000 to tft.width using the calibration #'s
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) 
    {
      // scale from 0->1023 to tft.width
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    }
   
    // go thru all the buttons, checking if they were pressed
    for (uint8_t b=0; b<15; b++) 
    {
       if (buttons[b].contains(p.x, p.y)) 
       {
          Serial.print("Pressing: "); Serial.println(b);
          buttons[b].press(true);  // tell the button it is pressed
       } 
       else 
       {
          buttons[b].press(false);  // tell the button it is NOT pressed
       }
    }

    // now we can ask the buttons if their state has changed
    for (uint8_t b=0; b<15; b++) 
    {
      if (buttons[b].justReleased()) 
      {
        // Serial.print("Released: "); Serial.println(b);
        buttons[b].drawButton();  // draw normal
      }   
      if (buttons[b].justPressed()) 
      {
         buttons[b].drawButton(true);  // draw invert!
        
         // if a numberpad button, append the relevant # to the textfield
         if (b >= 3) 
         {
           if (textfield_i < TEXT_LEN) 
           {
             textfield[textfield_i] = buttonlabels[b][0];
             textfield_i++;
             textfield[textfield_i] = 0; // zero terminate           
             // fona.playDTMF(buttonlabels[b][0]);
           }
         }

         // clr button! delete char
         if (b == 1) 
         {
            textfield[textfield_i] = 0;
            if (textfield > 0) 
            {
              textfield_i--;
              textfield[textfield_i] = ' ';
            }
          }
          // update the current text field
          Serial.println(textfield);
          tft.setCursor(TEXT_X + 2, TEXT_Y+10);
          tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
          tft.setTextSize(TEXT_TSIZE);
          tft.print(textfield);
        
          // its always OK to just hang up
          if (b == 2) 
          {
             if (buttons[b].justPressed()) 
             {
                buttons[b].drawButton(true);  // draw invert!
                //fona.hangUp();
                pwok=false;
                Serial.println("End pressed");
                clear_status_bar();
                status(F("Ending"));
                delay(3000);
                clr_textfield();
                return false;
             }
          }
        
          // The "Entr" (enter) key was pressed.
          if (b == 0) 
          {
             if (buttons[b].justPressed()) 
             {
                buttons[b].drawButton(true);  // draw invert!
                clear_status_bar();
                status(F("Checking Password"));
                delay(1000);
                Serial.print("Checking Password: "); Serial.println(textfield);
                //fona.callPhone(textfield);
                if (check_password(textfield)==true)
                {
                   pwok=true;
                   clear_status_bar();
                   Serial.println("Password Accepted");
                   status(F("Password Accepted"));
                   delay(3000);
                   clear_status_bar();
                   clr_textfield();
                   return true;
                }
                else
                {
                   pwok=false;
                   clear_status_bar();
                   Serial.println("Invalid Password!");
                   status(F("Invalid Password!"));
                   delay(3000);
                   clear_status_bar();
                   clr_textfield();
                }
             }
          }
       }  
    }        
    delay(50); // UI debouncing    // was 100
  }
}   

# Password-Keypad
Purpose:  Use 2.8" TFT LCD to enter a password
Filename: pw_keypad.ino
Author:   T. Simpson
Date:     22-May-2020
Description
  This is a modification of the Elegoo "phonecal.ino" test program.
The program gets a password from the user using a Elegoo 2.8" TFTLCD
shield touch screen as the input device.  Set your password in the global 
string variable "PassWord". The password is checked until a good password
is entered or "End" is pressed.  Use the "Entr" key to check your entry.
A worthy upgrade to this would be to add a piezo to generate a tone
when a key is pressed.

Keypad Key Use:
  "Entr"      - checks the password you entered when pressed. Returns a
                "true" boolean status.
  "Clr"       - backspaces and erases the last character entered
  "End"       - exits password check without the password check. Returns
                a "false" boolean status.
  "*" or "#"  - no special meaning. These characters can be used in 
                the password.
  "0"-"9"     - password digits

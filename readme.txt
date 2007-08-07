Sample.cpp     - contains all of the appropriate plugin code,
                 this should be the only source file you need
                 to modify.

StringDefs.h   - contains the string declarations, you will need
                 to modify this whenever you add a new string.

Strings\       - the default strings for this project.  The ordering
                 of strings within the .lng files must match that
                 defined in IDs.h

Strings.cpp    - handles the string table and translation.

Plugins.cpp    - contains functions that all plugins need.

StrLib.cpp     - a small string library that simplifies
                 managing command parameters and working.



Ignoring the Default Libraries
==============================

By default, the project ignores the default libraries, which makes
the resulting dll much smaller (~30k smaller).

Unfortunately, this means that support for common C string managment
functions needs to be recreated, and is now available through either
the Windows API, or functions I've written in StrLib.cpp.


Old Function      New Function
============      ============
strcpy            lstrcpy
strlen            lstrlen
sprintf           wsprintf
strchr            mstrchr
strrchr           mstrrchr
strstr            mstrstr
strstri           mstrstri
strtok            mstrtok
strdup            mstrdup
ltoa              mltoa
atol              matol
memcpy            mmemcpy
memcmp            mmemcmp


That should be the most commonly used functions.  If there's anything that's
really lacking, you'll either need to code it yourself, or compile in the
default libraries.


Defining Strings
================

In an effort to make plugins as easily translateable as possible, the string
resources are stored in a textfile name <Language>.lng, where <Language> is
the user selected language ("English", by default).

Strings are declared in the file StringDefs.h, and are simply declare as
shown below:

To declare a string (Strings.h)
----------------------------------
   STR(COMMAND_TEXT)


To define the string text, just add a line to <Language>.lng:

To define a string (English.lng)
--------------------------------
   This is the command text.

NOTE: The strings must be defined in <Language>.lng in the SAME ORDER THAT
THEY WERE DECLARED in StringDefs.h.



Using Strings
=============

Strings can be referenced as STR(STRING_ID), where STRING_ID is the name declared
in StringDefs.h (COMMAND_TEXT, in the example above)

Since the strings are defined in a language file, you'll need to explicitly set
each string used in your dialogs.  The SET() command makes this process pretty
painless:

To set a dialog item's string
-----------------------------
   SET(IDC_COMMAND, STR(COMMAND_TEXT));

IDC_COMMAND is the resource ID.



Working with Parameters
=======================

Unless you're writing a relatively simple plugin, you're probably going to
need to let users configure your commands.  Using the strlib functions, you
can easily save and restore user inteface parameters.

Saving Parameters:
==================

The SAVE() and RETURN_SAVE() functions will automatially include calls to
BEGIN_SAVE() END_SAVE()

Available functions:

BEGIN_SAVE();
SaveItem(int id);
SaveCheck(int id, char *val);
SaveCombo(int id);
SAVERADIO();
SaveStr(char *param);
SaveInt(long val);
END_SAVE();


To save a textbox:
------------------
   SaveItem(IDC_EDIT_BOX);


To save a check box:
--------------------
   SaveCheck(IDC_CHECK, "checked");

"checked" is the value to save if the the box is checked.
If it is not checked, the paramater will be left blank.


To save a combo box:
--------------------
Assuming that the values in the dialog box have been added using AddCombo(),
(see the Restoring Parameters section to see how to do this) you can just call

   SaveCombo(IDC_COMBO);


To save a radio setting:
------------------------
   RADIO(3);                              // we have 3 radio buttons
   ADDRADIO(IDC_RADIO_ONE, "one");
   ADDRADIO(IDC_RADIO_TWO, "two");
   ADDRADIO(IDC_RADIO_THREE, "three");
   SAVERADIO();

The string value passed as the second paramater to ADDRADIO is what will be
saved as the paramater.


To save an exact string:
------------------------
   SaveStr("string");


To save an integer:
-------------------
   SaveInt(123);



Restoring Parameters:
=====================
You must call BEGIN_RESTORE() before calling any Restore functions, and call
END_RESTORE() when you're finished.

Available functions:

BEGIN_RESTORE();
char *RestoreItem(int id);
char *RestoreCheck(int id, char *value);
char *RestoreCombo(int id);
int   RestoreInt();
char *RestoreStr();
RESTORERADIO();
END_RESTORE();


To restore a text box:
----------------------
   RestoreItem(IDC_EDIT_BOX);


To restore a check box:
-----------------------
   RestoreCheck(IDC_CHECK, "checked");

If the parameter matches "checked", the box will be checked.


To restore a combo box:
--------------------
   COMBO(IDC_COMBO, STR(ONE), "one");
   COMBO(IDC_COMBO, STR(TWO), "two");
   COMBO(IDC_COMBO, STR(THREE), "three");
   RestoreCombo(IDC_COMBO);


To restore a radio setting:
---------------------------
   NEWRADIO(3);
   RADIO(IDC_RADIO_ONE, "one");
   RADIO(IDC_RADIO_TWO, "two");
   RADIO(IDC_RADIO_THREE, "three");
   RESTORERADIO();


To restore an exact string:
---------------------------
   RestoreStr(IDC_EDIT_BOX, "string");


To restore an integer:
----------------------
   RestoreInt(IDC_EDIT_BOX, 123);

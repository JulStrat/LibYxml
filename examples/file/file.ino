/*
  Parsing XML file from SD card example sketch

  sat.xml  - https://github.com/OpenPLi/tuxbox-xml/blob/master/xml/satellites.xml
  
  Put file sat.xml on SD card. Connect to Ardino board.
  Compile and upload sketch. Open Serial Monitor.

  Board: Arduino Uno
  SD card: SDHC 7.31Gb FAT32, sectorsPerCluster - 64
  SD chip select pin: 10
  Arduino IDE Serial Monitors settings: 115200 baud, no line ending.

  Created by Ioulianos Kakoulidis, 2021.
  This example code is in the Public Domain.
*/

#include <SPI.h>
#include <SD.h>
#include <yxml.h>

unsigned long startMillis;

File fxml;

int c;
yxml_ret_t r;
yxml_t x[1];
char stack[64];
int verbose = 0;

void y_printchar(char c) {
  if(c == '\x7F' || (c >= 0 && c < 0x20)) {
    Serial.print("\\x");
    Serial.print(c, HEX); 
  }
  else
    Serial.print(c); 
}

void y_printstring(const char *str) {
  while(*str) {
    y_printchar(*str);
    str++;
  }
}

void y_printtoken(yxml_t *x, const char *str) {
  Serial.println("");
  Serial.print(str);
}

void y_printres(yxml_t *x, yxml_ret_t r) {
  static int indata;
  int nextdata = 0;

  switch(r) {
  case YXML_OK:
    if (verbose) {
      y_printtoken(x, "ok");
      nextdata = 0;
    }
    else
      nextdata = indata;
    break;
  case YXML_ELEMSTART:
    y_printtoken(x, "elemstart ");
    y_printstring(x->elem);
    if (yxml_symlen(x, x->elem) != strlen(x->elem))
      y_printtoken(x, "assertfail: elem lengths don't match");
    if (r & YXML_CONTENT)
      y_printtoken(x, "content");
    break;
  case YXML_ELEMEND:
    y_printtoken(x, "elemend");
    break;
  case YXML_ATTRSTART:
    y_printtoken(x, "attrstart ");
    y_printstring(x->attr);
    if (yxml_symlen(x, x->attr) != strlen(x->attr))
      y_printtoken(x, "assertfail: attr lengths don't match");
    break;
  case YXML_ATTREND:
    y_printtoken(x, "attrend");
    break;
  case YXML_PICONTENT:
  case YXML_CONTENT:
  case YXML_ATTRVAL:
    if (!indata)
      y_printtoken(x, r == YXML_CONTENT ? "content " : r == YXML_PICONTENT ? "picontent " : "attrval ");
    y_printstring(x->data);
    nextdata = 1;
    break;
  case YXML_PISTART:
    y_printtoken(x, "pistart ");
    y_printstring(x->pi);
    if (yxml_symlen(x, x->pi) != strlen(x->pi))
      y_printtoken(x, "assertfail: pi lengths don't match");
    break;
  case YXML_PIEND:
    y_printtoken(x, "piend");
    break;
  default:
    y_printtoken(x, "error\n");
    exit(0);
  }
  indata = nextdata;
}


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  if (SD.begin(10)) {
    Serial.println("SPI OK.");
  }
  else {
    Serial.println("SPI error.");
    while (true) {
      ;
    }
  }

  if (fxml = SD.open("sat.xml")) {
    yxml_init(x, stack, sizeof(stack));

    while ((c = fxml.read()) != -1) {
      r = yxml_parse(x, c);
      y_printres(x, r);
    }

    y_printtoken(x, yxml_eof(x) < 0 ? "error\n" : "ok\n");
    fxml.close();
  }
  else {
    Serial.println("Can't open satellites.xml file.");
  }
}

void loop() {
}

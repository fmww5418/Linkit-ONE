#include <LGPS.h>

gpsSentenceInfoStruct info;
char buff[256];
char nors[2], eorw[2];
String latitude, longitude;

static unsigned char getComma(unsigned char num,const char *str)
{
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++)
  {
     if(str[i] == ',')
      j++;
     if(j == num)
      return i + 1; 
  }
  return 0; 
}

static double getDoubleNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

static double getIntNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atoi(buf);
  return rev; 
}

void parseGPGGA(const char* GPGGAstr)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */
  double latitude;
  double longitude;
  int tmp, hour, minute, second, num ;
  if(GPGGAstr[0] == '$')
  {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');
    
    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    Serial.println(buff);
    
    tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(3, GPGGAstr);
    strncpy(nors,  &GPGGAstr[tmp], 1);
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(5, GPGGAstr);
    strncpy(eorw,  &GPGGAstr[tmp], 1);
    convertCoords(latitude, longitude, nors, eorw);
    
    sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
    Serial.println(buff); 
    
    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);    
    sprintf(buff, "satellites number = %d", num);
    Serial.println(buff); 
  }
  else
  {
    Serial.println("Not get data"); 
  }
}

void convertCoords(float tmplat, float tmplong, const char* n_or_s, const char* e_or_w) { 
  /*
  Latitude  5213.2930,N --> 52d 13.2930' N
  52 degrees 13.2930 minutes NORTH
  52 + (13.2930 / 60) = 52.22155
  Because it is north of the equator, the number remains positive.
  +52.22155
  */
  
  float lat_return, lon_return;
  int lat_deg_int = int(tmplat / 100); //extract the first 2 chars to get the latitudinal degrees
  float latitude_float = tmplat - (lat_deg_int * 100); //remove the degrees part of the coordinates - so we are left with only minutes-seconds part of the coordinates
  lat_return = lat_deg_int + latitude_float / 60; //add back on the degrees part, so it is decimal degrees
  
  //Check if it is N or S, S will turn the value negative
  
  if (*n_or_s == 'S'){
    Serial.println("is South");
    lat_return *= -1;
  }

  sprintf(buff, "%.7f", lat_return);   
  latitude = buff;
  
  /*
  Longitude  00004.5337,W  --> 00d 04.5337' W
  00 degrees 4.5337 minutes WEST
  00 + (4.5337 / 60) = 0.0755616
  Because it is West, the number becomes negative.
  -0.0755616
  */
  
  int lon_deg_int = int(tmplong / 100);
  float longitude_float = tmplong - lon_deg_int * 100;
  lon_return = lon_deg_int + longitude_float / 60;  //add back on the degrees part, so it is decimal degrees
  if (*e_or_w == 'W'){
    Serial.println("is West");
    lon_return *= -1;
  }

  sprintf(buff, "%.7f", lon_return); 
  longitude = buff;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  LGPS.powerOn();
  Serial.println("LGPS Power on, and waiting ..."); 
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("LGPS loop"); 
  LGPS.getData(&info);
  Serial.println((char*)info.GPGGA); 
  parseGPGGA((const char*)info.GPGGA);
  delay(2000);
}



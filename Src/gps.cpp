#include "gps.h"
#include "UARTBuffs.h"
#include <string.h>
#include "NMEA_Parser.h"
#include "exeptions.h"
#include <string.h>
#include <stdio.h>

using namespace UART;

namespace GPS
{
  cGPS* GPS_global;
  
  cGPS::cGPS(uint8_t* rxBuff,//plase where kept recived data
                       uint8_t* txBuff,//plase where coppy transmited data for transmit
                       uint16_t rxBuffLen,// len of rxBuff
                       uint16_t txBuffLen,// ken of txBuff
                       UART::cUART_interfacePrototype* UART,
                       void (*power)(bool onOff))
  {
    buffersInit(rxBuff, txBuff, rxBuffLen, txBuffLen);
    this->UART = UART;
    isInited = false;
    this->power = power;
    if(GPS_global == NULL)
      
      GPS_global = this;
    else
      exception(__FILE__,__FUNCTION__,__LINE__,"Must be only one GPS!!!");
  }
  
  bool cGPS::init(bool on)
  {
    if(on == isInited)
      return true;
    power(on);
    isInited = on;
    NMEAParser_Init();
    UART->rxStart();
    return  true;
  }
  
  bool cGPS::getState(void)
  {
    return isInited;
  }
  
  void cGPS::control()
  {
    if(!isInited)
      return;
    uint32_t len;
    switch(UART->getRxState())
    {
    case TX_REDY:
      len = UART->getRxedDatalen();
      if(len > buffs.constRcvBuffLen)
      {
        UART->rxStart();
        return;
      }
      if(UART->getRxedData(buffs.rcvBuff, len))
      {
        NMEAParser_ParseBuffer(buffs.rcvBuff, len);
      }
      UART->rxStart();
      return;
    case TX_IN_PROCESS:
      return;
    case TX_ERROR:
      UART->reInit(0);
      return;
    default:
      break;
    }
  }
  
  void cGPS::restart(uint8_t resType)
  {
    uint32_t len = sizeof("$PSRF101,-2686700,-4304200,3851624,96000,497260,921,12,4");
    switch(resType)
    {
    case GPS_COLD_RES:
      memcpy(buffs.trmBuff, "$PSRF101,-2686700,-4304200,3851624,96000,497260,921,12,4", len);
      UART->txStart(buffs.trmBuff, len);
      break;
    case GPS_WARM2_RES:
      memcpy(buffs.trmBuff, "$PSRF101,-2686700,-4304200,3851624,96000,497260,921,12,3", len);
      UART->txStart(buffs.trmBuff, len);
      break;
    case GPS_WARM_RES:
      memcpy(buffs.trmBuff, "$PSRF101,-2686700,-4304200,3851624,96000,497260,921,12,2", len);
      UART->txStart(buffs.trmBuff, len);
      break;
    case GPS_HOT_RES:
      memcpy(buffs.trmBuff, "$PSRF101,-2686700,-4304200,3851624,96000,497260,921,12,1", len);
      UART->txStart(buffs.trmBuff, len);
      break;
    default:
      break;
    }
  }
}

using namespace GPS;

namespace MODES
{
  
  //static uint8_t curScreen = 0;
  
  void GPS_ColdRes(void)
  {
    GPS_global->restart(GPS_COLD_RES);
    NMEAParser_Reset();
    currentMode->openMenu(NULL, 0);
  }
  
  void GPS_WarmRes(void)
  {
    GPS_global->restart(GPS_WARM_RES);
    NMEAParser_Reset();
    currentMode->openMenu(NULL, 0);
  }
  
  void GPS_Warm2Res(void)
  {
    GPS_global->restart(GPS_WARM2_RES);
    NMEAParser_Reset();
    currentMode->openMenu(NULL, 0);
  }
  
  void GPS_HotRes(void)
  {
    GPS_global->restart(GPS_HOT_RES);
    NMEAParser_Reset();
    currentMode->openMenu(NULL, 0);
  }
  
  tagMenu gpsMenu[4];
  
  cGPS_mode::cGPS_mode()
  {
    InterfaceColor.red = 255;
    InterfaceColor.green = 0;
    InterfaceColor.blue = 0;
    FontColor.red = 0;
    FontColor.green = 255;
    FontColor.blue = 0;
    strcpy((char*)modeName, "GPS");
    //strcpy((char*)enterButtonText, "on");
    strcpy((char*)onButtonText, "menu");
    
    strcpy((char*)gpsMenu[0].name, "Cold Res");
    strcpy((char*)gpsMenu[1].name, "Warm Res");
    strcpy((char*)gpsMenu[2].name, "Warm2 Res");
    strcpy((char*)gpsMenu[3].name, "Hot Res");
    for(int i =0; i < 4; i++)
    {
      gpsMenu[i].isShow = true;
    }
    gpsMenu[0].func = GPS_ColdRes;
    gpsMenu[1].func = GPS_WarmRes;
    gpsMenu[2].func = GPS_Warm2Res;
    gpsMenu[3].func = GPS_HotRes;
    
    rKeyState = true;
    lKeyState = true;
    eKeyState = true;
    oKeyState = true;
    
    strcpy((char*)rightButtonText, "Spec");
    strcpy((char*)leftButtonText, "Geiger");
  }
  
  //convert coord in double to text
char* GPSMode_coord2txt(double dval, char* pBuf, char worldpart1, char worldpart2)
{
	char sym = worldpart1;
	if(dval<0)
	{
		sym = worldpart2;
		dval=-dval;
	}
	int deg = (int)dval;
	float sec = (dval-deg)*60.0;
	int min = (int)(sec);
	sec = (float)((sec-min)*60.0);
	//char buf;
	//buf = sym;
	//buf[1] = '\0';
	sprintf(pBuf, "%d %d'%.2f'' %s", deg, min, sec, &sym);
	return pBuf;
}
  
  void cGPS_mode::onShow(void)
  {
    cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue));
    
    if(GPS_global->getState())
    {
        char gps_text[60];
        char buf[20];
        
        cmd( BEGIN(LINES) );
        
        cmd( LINE_WIDTH(1 * 16) );
	cmd( VERTEX2F(1 * 16,60 * 16) );
	cmd( VERTEX2F(240 * 16,60 * 16) );
        
        cmd( END() );
        
	cmd_text(120, 60, 16, 512, (uint8_t*) "Common data");

	sprintf(gps_text, "Lat=%s", GPSMode_coord2txt(NMEAParserControl.commonGPS.Lat,buf,'N','S'));
        
	cmd_text(5, 100, 28, 0, (uint8_t*) gps_text);
	sprintf(gps_text, "Lon=%s", GPSMode_coord2txt(NMEAParserControl.commonGPS.Lon,buf,'E','W'));
        
	cmd_text(5, 120, 28, 0, (uint8_t*) gps_text);
	sprintf(gps_text, "Alt=%.2f", NMEAParserControl.commonGPS.Alt);
        
	cmd_text(5, 140, 28, 0, (uint8_t*) gps_text);
	sprintf(gps_text, "Spd=%.2f", NMEAParserControl.commonGPS.Spd);
        
	cmd_text(5, 160, 28, 0, (uint8_t*) gps_text);
	sprintf(gps_text, "Dir=%.2f", NMEAParserControl.commonGPS.Dir);
        
	cmd_text(5, 180, 28, 0, (uint8_t*) gps_text);
	sprintf(gps_text, "Sats=%d", (int)NMEAParserControl.m_btGGANumOfSatsInUse);
        
	cmd_text(5, 200, 28, 0, (uint8_t*) gps_text);
        
        sprintf(gps_text, "Setelits in View=%d", (int)NMEAParserControl.m_wGSVTotalNumSatsInView);
        
        cmd_text(5, 220, 28, 0, (uint8_t*) gps_text);
        
        //cmd_text(120, 220, 28, 512, (uint8_t*) "Signal qual");
        
        sprintf(gps_text, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,", (int)NMEAParserControl.m_GSVSatInfo[0].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[1].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[2].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[3].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[4].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[5].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[6].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[7].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[8].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[9].m_wSignalQuality);
        
        cmd_text(5, 240, 28, 0, (uint8_t*) gps_text);
        
        sprintf(gps_text, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,", (int)NMEAParserControl.m_GSVSatInfo[10].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[11].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[12].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[13].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[14].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[15].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[16].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[17].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[18].m_wSignalQuality,
                (int)NMEAParserControl.m_GSVSatInfo[19].m_wSignalQuality);
        
        cmd_text(5, 260, 28, 0, (uint8_t*) gps_text);
    }
    else
      cmd_text(120, 80, 30, 512, (uint8_t*) "TURNED OFF");
  }
  
  void cGPS_mode::userControl(void)
  {
    
  }
  
  void cGPS_mode::enterKeyAction(void)
  {
    /*
    if(GPS_global->getState())
    {
      GPS_global->init(false);
      strcpy((char*)enterButtonText, "on");
    }
    else
    {
      GPS_global->init(true);
      strcpy((char*)enterButtonText, "off");
    }
    */
  }
  
  void cGPS_mode::rKeyAction(void)
  {
    change(modeKarusel[3]);
  }
  
  void cGPS_mode::lKeyAction(void)
  {
    change(modeKarusel[1]);
  }
  
  void cGPS_mode::onKeyAction(void)
  {
    openMenu(gpsMenu, 4);
  }
  
};

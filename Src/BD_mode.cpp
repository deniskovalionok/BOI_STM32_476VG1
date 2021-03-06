#include "BD_mode.h"
#include "BD_definitions.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "geiger.h"

extern "C"
{
  #include "Spectrum.h"
  #include "identify.h"
}

#include "exeptions.h"

using namespace MODBUS;

namespace MODES
{
  void showVal(float val, uint16_t x, uint16_t y, uint16_t font, uint16_t options)
  {
      const char* pFltMsk;
      if(val<1)
              pFltMsk = "%.3f";
      else if(val<10)
              pFltMsk = "%.2f";
      else if(val<100)
              pFltMsk = "%.1f";
      else
              pFltMsk = "%.0f";
      char buff[10];
      sprintf(buff, pFltMsk, val);
      cmd_text(x, y, font, options, (uint8_t*)buff);
  }
  
    
  cMode_prototype* mode2;
  uint8_t mName[20];
  
  cBD_mode::cBD_mode(MODBUS::cBD* BD, cBD_config* conf)
  {
    InterfaceColor.red = 255;
    InterfaceColor.green = 0;
    InterfaceColor.blue = 0;
    FontColor.red = 0;
    FontColor.green = 255;
    FontColor.blue = 0;
    this->BD = BD;
    old_ID = 0;
    countMode = false;
    //strcpy((char*)enterButtonText, "menu");
    this->conf = conf;
    strcpy((char*)rightButtonText, "Geiger");
    strcpy((char*)leftButtonText, "Spec");
    
    rKeyState = true;
    lKeyState = true;
    eKeyState = true;
    oKeyState = true;
    
  }
  
    void cBD_mode::userControl(void)
    {
      if(conf->getMenuState())
        strcpy((char*)enterButtonText, "proces");
      else if(conf->getMenuLen() && BD->getBD_ID())
        strcpy((char*)enterButtonText, "menu");
      else
        strcpy((char*)enterButtonText, "    ");
      if(BD->getBD_ID() != old_ID)
        show();
    }
    
    void cBD_mode::onShow(void)
    {
      old_ID = BD->getBD_ID();
      if(old_ID)
      {
        conf->show();
        strcpy((char*)modeName, (char*)mName);
      }
      else
        strcpy((char*)modeName, "NO BD");
      
      if(modeName[0] == 'U' || modeName[0] == 'N')
      {
        char buff[40];
        sprintf(buff, "Geiger doserate: %0.3f uSv", geigerGetDose());
        cmd_text(120, 200, 27, 512, (uint8_t*)buff);
        sprintf(buff, "Geiger CPS: %0.3f cps", geigerGetCPS());
        cmd_text(120, 220, 27, 512,(uint8_t*) buff);
        sprintf(buff, "Geiger Error: %2.0f%%", geigerGetErr());
        cmd_text(120, 240, 27, 512,(uint8_t*) buff);
      }
    }
    
    void cBD_mode::enterKeyAction(void)
    {
      if(conf->getMenuLen() && !conf->getMenuState())
      {
          menuOpen = true;
          currentMenuLine = 0;
          screenPose = 0;
          show();
      }
    }
  
    void cBD_mode::rKeyAction(void)
    {
      change(modeKarusel[1]);
    }
  
    void cBD_mode::lKeyAction(void)
    {
      change(modeKarusel[3]);
    }
  
    void cBD_mode::onKeyAction(void)
    {
      
    }

   void cBD_mode::showMenu(void)
   {
            menuShownlen =  conf->getMenuLen();
            
            cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue) );
            //int index = 0;
            //int showCount = 0;
            cmd( COLOR_RGB(InterfaceColor.red, InterfaceColor.green, InterfaceColor.blue) );
            cmd( BEGIN(RECTS) );
            cmd( LINE_WIDTH(1 * 16) );
            cmd( VERTEX2F(40 * 16,(280 - min(menuShownlen*20, 260)) * 16) );
            cmd( VERTEX2F(200 * 16,280 * 16) );
            
            cmd( COLOR_RGB(0, 0, 0) );
            cmd( VERTEX2F(41 * 16,(281 - min(menuShownlen*20, 260)) * 16) );
            cmd( VERTEX2F(199 * 16,279 * 16) );
            
            cmd( COLOR_RGB(InterfaceColor.red, InterfaceColor.green, InterfaceColor.blue) );
            cmd( VERTEX2F(41 * 16,(281 -min(menuShownlen*20, 260) + (currentMenuLine - screenPose) * 20) * 16) );
            cmd( VERTEX2F(199 * 16,(301 -min(menuShownlen*20, 260) + (currentMenuLine - screenPose) * 20) * 16));
            
            cmd(END());
            /*
            for(uint16_t i = 0; i <= min( menuShownlen, 12); i++)
            {
              while(index < menuLen)
              {
                if(menu[index].isShow)
                {
                  showCount++;
                  if(showCount > screenPose)
                  {
                      if(i + screenPose == currentMenuLine)
                      {
                        cmd( COLOR_RGB(0, 0, 0) );
                        cmd_text(120, (280 -min(menuShownlen*20, 260)) + ((i) * 20), 27, 512, menu[index].name);
                        cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue));
                        currentMenuInArr = index;
                      }
                      else
                        cmd_text(120, (280 -min(menuShownlen*20, 260)) + ((i) * 20) + 1, 27, 512, menu[index].name);
                      index++;
                      break;
                  }
                }
                index++;
              }
            }
            */
            uint8_t line[21];
            for(uint16_t i = screenPose; i <= 12; i++)
            {
              if(!conf->getMenuLine(i + screenPose, line))
                break;
              if(i + screenPose == currentMenuLine)
              {
                        cmd( COLOR_RGB(0, 0, 0) );
                        cmd_text(120, (280 -min(menuShownlen*20, 260)) + ((i) * 20), 27, 512, line);
                        cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue));
              }
              else
                cmd_text(120, (280 -min(menuShownlen*20, 260)) + ((i) * 20), 27, 512, line);
            }
            if(messageClass == NULL)
            {
                cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue) );
                cmd_text(40, 290, 28, 512, "up");
                cmd_text(200, 290, 28, 512, "down");
                cmd_text(120, 281, 27, 512, "enter");
                cmd_text(120, 301, 27, 512, "exit");
            }
            
   }
   
   void cBD_mode::menuAction(void)
   {
     conf->menuLineAction(currentMenuLine);
     menuOpen = false;
     show();
   }

  
   void cBD_config::menuLineAction(uint16_t num)
   {
     if(num >= numOfMenuNames)
     {
       exception(__FILE__,__FUNCTION__,__LINE__,"Wrong menu line!");
       return;
     }
     uint8_t tempBuff[21];
     getMenuLine(num, tempBuff);
     do
     {
       if(curMenuPose >= numOfMenuLines * sizeof(tagBDMenuLine) + startOfMenuLines)
         break;
       if(filesystem_file_get(&filesystem, filesystem_open_file(&filesystem, nameBuff, "ini")
                           , &curMenuPose, (uint8_t*)&menuLine, sizeof(tagBDMenuLine)) != sizeof(tagBDMenuLine))
            exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corrupted.");
       switch(menuLine.menuCmds)
       {
       case 5:
         for(numOfCmd = 0; numOfCmd < 20; numOfCmd++)
         {
           if(!sendCmd(menuLine.name[numOfCmd]))
             break;
           else
             waitAnsw = true;
         }
         break;
       case LINE_CMD:
       case NONE_CMD:
       default:
         break;
       }
     }
     while(menuLine.menuCmds != LINE_CMD && menuLine.menuCmds != NONE_CMD && !sendInProcess);
     if(menuLine.menuCmds == LINE_CMD || menuLine.menuCmds == NONE_CMD)
       curMenuLine++;
     else if(!sendInProcess)
     {
        curMenuLine = 0;
        curMenuPose = startOfMenuLines;
     }
   }
   
   bool cBD_config::sendCmd(uint8_t id)
   {
     if(!id)
     {
       sendInProcess = false;
       return false;
     }
     if(curCmdId != id)
       curCmdPose = startOfCmds;
     uint16_t pose = curCmdPose;
     HFILE file = filesystem_open_file(&filesystem, nameBuff, "ini");
     while(curCmdId != id)
     {
       pose = curCmdPose += curCmdLen + 2;
       if(filesystem_file_get(&filesystem, file, (int*)&pose, &curCmdId, 1) != 1)
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
       if(filesystem_file_get(&filesystem, file, (int*)&pose, &curCmdLen, 1) != 1)
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
     }
     if(filesystem_file_get(&filesystem, file, (int*)&pose, cmdBuff, curCmdLen) != curCmdLen)
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
     BD->toQeue();
     if(!BD->sendBuff(cmdBuff, curCmdLen + 2) && !sendInProcess)
     {
       sendInProcess = true;
       return false;
     }
     return true;
   }
   
   bool cBD_config::getMenuState(void)
   {
     return sendInProcess;
   }
   
   void cBD_config::show(void)
   {
     if(findConf())
       showScreen();
     else
       strcpy((char*)mName, "UNKNOWN BD");
   }
  
    
   cBD_config::cBD_config(MODBUS::cBD* BD, TIMER::cMS_timer_prototype* tim)
   {
     this->BD = BD;
     this->tim = tim;
     
       sigmaSt.stLenCur = 0;
       sigmaSt.cpsPointer = 0;
       sigmaSt.sigmaStart = false;
       
       sigmaSt.sigmaMax = 0;
       sigmaSt.sigmaMin = 0;
       
       sigmaSt.cpsPreBuffPointer = 0;
       
       this->spec = NULL;
       
   }
   
   cBD_config::cBD_config(MODBUS::cBD* BD,TIMER::cMS_timer_prototype* tim, cBD_Spec_mode* spec)
   {
     this->BD = BD;
     this->tim = tim;
     
       sigmaSt.stLenCur = 0;
       sigmaSt.cpsPointer = 0;
       sigmaSt.sigmaStart = false;
       
       sigmaSt.sigmaMax = 0;
       sigmaSt.sigmaMin = 0;
       
       sigmaSt.cpsPreBuffPointer = 0;
       
       this->spec = spec;
       
   }
  
   bool cBD_config::findConf(void)
   {
     char id = BD->getBD_ID();
     if(curBDid == id)
     {
       if(spec != NULL && !spec->inited && id != 0)
            spec->initRead(false, BDConf.dataUpdatePeriod);
       return true;
     }
     if(!id)
     {
       sigmaSt.stLenCur = 0;
       sigmaSt.cpsPointer = 0;
       sigmaSt.sigmaStart = false;
       sigmaSt.summCPS = 0;
       sigmaSt.calcStart = false;
       sigmaSt.cpsPreBuffPointer = 0;
       
       sigmaSt.sigmaMax = 0;
       sigmaSt.sigmaMin = 0;
       
       waitAnsw = false;
       sendInProcess = false;
       return false;
     }
     sprintf(nameBuff, "BDConf_%02X", id);
     HFILE file  = filesystem_open_file(&filesystem, nameBuff, "ini");
     if(file == NULL)
       return false;
     fileStartClaster = filesystem.fileRecordsOnSector[file - 1].wdStartClaster;
     fileLen = filesystem.fileRecordsOnSector[file - 1].dwLength;
     curClasterNum = 0;
     curClasterPose = fileStartClaster;
     curScreenNum = 1;
     curLineNum = 0xFFFF;
     BD->resetPoll(CON_REGS);
     BD->resetPoll(DATA_REGS);
     curBDid = id;
     int pose = 0;
     //readFromConf(0, &BDConf, sizeof(tagBDConf));
     if(filesystem_file_get(&filesystem, file, &pose, (uint8_t*)&BDConf, sizeof(tagBDConf)) != sizeof(tagBDConf))
       exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
     /*
     //pose = BDConf.startOfVals;
     for(int i = 0; i < BDConf.numOfVals; i++)
     {
       //readFromConf(BDConf.startOfVals, &val, sizeof(tagBDVal));
       if(filesystem_file_get(&filesystem, file, &pose, (uint8_t*)&val, sizeof(tagBDVal)) != sizeof(tagBDVal))
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
       BD->setPollRegs(val.regType, val.startReg, (val.type == INT16)?1:2);
     }
     */
     if(BDConf.regTypeCPS != NULL_REG)
       BD->setPollRegs(BDConf.regTypeCPS, BDConf.diagCPS, 2);
     
     if(filesystem_file_get(&filesystem, file, &pose, (uint8_t*)&numOfScreens, 2) != 2)
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
     
     screensStart = pose;
     curScreenNum = 0xffff;
     curScreenPose = screensStart;
     
     openScreen(0);
     BD->setPollPeriod(BDConf.dataUpdatePeriod);
     BD->setStartupCmds(BDConf.bInfoRegs);
     uint16_t len;
     pose = startOfMenuLines + numOfMenuLines * sizeof(tagBDMenuLine);
     for(uint16_t i = 0; i < (int16_t)numOfScreens - 1; i++)
     {
       pose += sizeof(tagBDScreeen);
       if(filesystem_file_get(&filesystem, file, &pose, (uint8_t*)&len, 2) != 2)
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
       pose += len * sizeof(tagScreenLine);
       if(filesystem_file_get(&filesystem, file, &pose, (uint8_t*)&len, 2) != 2)
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
       pose += len * sizeof(tagBDMenuLine);
     }
     startOfCmds = pose;
     curCmdPose = startOfCmds;
     uint8_t tempVal;
     for(uint16_t i = 0; i < BDConf.initCmdsNum; i++)
     {
       if(filesystem_file_get(&filesystem, file, &pose, &curCmdId, 1) != 1)
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
       if(filesystem_file_get(&filesystem, file, &pose, &curCmdLen, 1) != 1)
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
       if(filesystem_file_get(&filesystem, file, &pose, cmdBuff, curCmdLen) != curCmdLen)
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
       tim->stop(0);
       tim->start(5000, 0);
       BD->toQeue();
       while(BD->sendBuff(cmdBuff, curCmdLen + 2))                                                  //Vot tut KOSTIL!!! Sdelat optimalno!!!
       {
         BD->control();
         if(tim->check(0))
           exception(__FILE__,__FUNCTION__,__LINE__,"BD Init Error!");
       }
       while(!BD->getAnsw(&tempVal, 1))
       {
         BD->control();
         if(tim->check(0))
           exception(__FILE__,__FUNCTION__,__LINE__,"BD Init Error!");
       }
     }
     if(BDConf.regTypeCPS != NULL_REG)
     {
       tim->stop(0);
       tim->start(BDConf.dataUpdatePeriod, 0);
       
       tLit = (uint16_t)round(2 / (BDConf.dataUpdatePeriod * 0.001));
       
       tMaxInt = (uint16_t)round(4 / (BDConf.dataUpdatePeriod * 0.001));
       
       tBig = (uint16_t)round(30 / (BDConf.dataUpdatePeriod * 0.001));
     }
     return true;
  }
  
  void cBD_config::openScreen(uint16_t screenNum)
  {
    if(screenNum == 0xFFFF)
      exception(__FILE__,__FUNCTION__,__LINE__,"to many Screens!!!");
    if(curScreenNum != 0xFFFF && screenNum == curScreenNum)
      return;
    if(curScreenNum <= screenNum)
      exception(__FILE__,__FUNCTION__,__LINE__,"to many Screens!!!");
    //readFromConf(BDConf.startOfScreens + (sizeof(tagBDScreeen) * screenNum), &curScreen, sizeof(curScreen));0.
    int pose = screensStart + (sizeof(tagBDScreeen) * screenNum);
    if(filesystem_file_get(&filesystem, filesystem_open_file(&filesystem, nameBuff, "ini")
                           , &pose, (uint8_t*)&curScreen, sizeof(tagBDScreeen)) != sizeof(tagBDScreeen))
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
    curScreenNum = screenNum;
    BD->resetPoll(CON_REGS);
    BD->resetPoll(DATA_REGS);
    if(BDConf.regTypeCPS != NULL_REG)
       BD->setPollRegs(BDConf.regTypeCPS, BDConf.diagCPS, 2);
    if(curScreen.numOfConReg)
      BD->setPollRegs(CON_REGS, curScreen.startConReg, curScreen.numOfConReg);
    if(curScreen.numOfDataReg)
      BD->setPollRegs(DATA_REGS, curScreen.startDataReg, curScreen.numOfDataReg);
    if(filesystem_file_get(&filesystem, filesystem_open_file(&filesystem, nameBuff, "ini")
                           , &pose, (uint8_t*)&scrLen, 2) != 2)
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
    readMenu();
  }
  
  int ddddd = 0;
  
  void cBD_config::showScreen(void)
  {
    if(curScreenNum == 0xFFFF)
      return;
    char buff[40];
    ddddd++;
    //tagValBuff valBuff;
    float cgVal = 0;
    strcpy((char*)mName, curScreen.scrName);
    for(int i = 0; i < scrLen; i++)
    {
      readLine(i);
      tagBDInf inf = BD->getBDInfo();
      if(curLine.showWithUnit == 0xffff || curLine.showWithUnit == inf.CurUnit)
      {
            switch(curLine.regType)
            {
            case CON_REGS:
            case DATA_REGS:
                switch(curLine.type)
                {
                case INT16:
                  BD->getDataFromRegs(curLine.regType, curLine.startReg, (int16_t*)&cgVal);
                  if(curLine.tenPow)
                    cgVal *= pow(10, curLine.tenPow);
                  break;
                case INT32:
                  BD->getDataFromRegs(curLine.regType, curLine.startReg, (int32_t*)&cgVal);
                  if(curLine.tenPow)
                    cgVal *= pow(10, curLine.tenPow);
                  break;
                case FLOAT:
                  BD->getDataFromRegs(curLine.regType, curLine.startReg, (float*)&cgVal);
                  if(curLine.tenPow)
                    cgVal *= pow(10, curLine.tenPow);
                  break;
                default:
                  exception(__FILE__,__FUNCTION__,__LINE__,"BD file broken");
                  return;
                }
                break;
            case 3:
              switch(curLine.startReg)
              {
              case 0:
                  BD->getDataFromRegs(BDConf.regTypeCPS, BDConf.diagCPS, &cgVal);
                  if(curLine.tenPow)
                    cgVal *= pow(10, curLine.tenPow);
                  break;
              case 1:
                  BD->getDataFromRegs(BDConf.regTypeDR, BDConf.regStartDR, &cgVal);
                  if(curLine.tenPow)
                    cgVal *= pow(10, curLine.tenPow);
                  break;
              default:
                exception(__FILE__,__FUNCTION__,__LINE__,"Wrong reg!!!");
                break;
              }
              break;
            case 4:
              switch(curLine.startReg)
              {
              case 0:
                  cgVal = geigerGetCPS();
                  if(curLine.tenPow)
                    cgVal *= pow(10, curLine.tenPow);
                  break;
              case 1:
                  cgVal = geigerGetDose();
                  if(curLine.tenPow)
                    cgVal *= pow(10, curLine.tenPow);
                  break;
              case 2:
                  cgVal = geigerGetErr();
                  if(curLine.tenPow)
                    cgVal *= pow(10, curLine.tenPow);
                  break;
              default:
                exception(__FILE__,__FUNCTION__,__LINE__,"Wrong reg!!!");
                break;
              }
              break;
            case NULL_REG:
            default:
              break;
            }
          //memcpy(&valBuff, value, 32*4);
          //value *= 1000;
          //memset(&cgVal, 1, 4);
          if(curLine.lineString[0] != 0)
          {
              sprintf(buff, curLine.lineString, cgVal);
              cmd( COLOR_RGB(curLine.color[0], curLine.color[1], curLine.color[2]));
              cmd_text(curLine.poseX, curLine.poseY, curLine.font, curLine.options, (uint8_t*)buff);
          }
          else
          {
            float tempVal = 0;
            switch(curLine.lineString[1])
            {
            case 1:
              cmd(COLOR_RGB(curLine.color[0], curLine.color[1], curLine.color[2]));
              cmd( BEGIN(LINES) );
                  cmd( LINE_WIDTH(curLine.font * ((curLine.options & 1)  ? 16 : 8)) );
                  cmd( VERTEX2F(curLine.poseX * 16,curLine.poseY * 16) );
                  cmd( VERTEX2F(curLine.lineNums[1] * 16,curLine.lineNums[2] * 16) );
              cmd( END() );
              break;
              
            case 2:
              cmd(COLOR_RGB(curLine.lineString[6], curLine.lineString[7], curLine.lineString[8]));
              cmd( BEGIN(RECTS) );
                  cmd( LINE_WIDTH(1 * 16) );
                  
                  cmd( VERTEX2F(curLine.poseX * 16,curLine.poseY * 16) );
                  cmd( VERTEX2F(curLine.lineNums[1] * 16,curLine.lineNums[2] * 16) );
                  
                  cmd(COLOR_RGB(0, 0, 0));
                  cmd( VERTEX2F((curLine.poseX + 1) * 16,(curLine.poseY + 1) * 16) );
                  cmd( VERTEX2F((curLine.lineNums[1] - 1) * 16,(curLine.lineNums[2] - 1) * 16) );
              cmd( END() );
                  
                  curLine.lineNums[5] = (curLine.lineNums[2] - curLine.poseY) * 3 / 4;
                  if(curLine.lineNums[5] < 8)
                    exception(__FILE__,__FUNCTION__,__LINE__,"Litle space!");
                  
              cmd(COLOR_RGB(curLine.lineString[6], curLine.lineString[7], curLine.lineString[8]));
              cmd( BEGIN(LINES) );
                  cmd( LINE_WIDTH(1 * 8) );
                  
                  cmd( VERTEX2F((curLine.poseX + 1) * 16,(curLine.poseY + curLine.lineNums[5]) * 16) );
                  cmd( VERTEX2F((curLine.lineNums[1] - 1) * 16,(curLine.poseY + curLine.lineNums[5]) * 16) );
              
              cmd( END() );
                if(curLine.options != 0)
                {
                    sigmaSt.sigmaStlen = (curLine.lineNums[1] - curLine.poseX - 4) / curLine.options;
                    curLine.lineNums[6] = ((curLine.lineNums[1] - curLine.poseX - 4) % curLine.options)/2;
                }
                else
                  exception(__FILE__,__FUNCTION__,__LINE__,"Col. width = 0!!!");
                
                if(sigmaSt.sigmaStlen < 1)
                  exception(__FILE__,__FUNCTION__,__LINE__,"Litle space!");
                
                if(sigmaSt.sigmaStlen < sigmaSt.stLenCur)
                  sigmaSt.stLenCur = 0;
                  
              cmd( BEGIN(RECTS) );
                cmd( LINE_WIDTH(1 * 16) );
                cmd( COLOR_RGB(curLine.color[0], curLine.color[1], curLine.color[2]) );
                
                for(int i = sigmaSt.sigmaStPointer - 1; i >= sigmaSt.sigmaStPointer - sigmaSt.stLenCur; i--)
                {
                  if(i < 0)
                    curLine.lineNums[7] = sigmaSt.stLenCur + i;
                  else
                    curLine.lineNums[7] = i;
                  tempVal = (sigmaSt.sigmas[curLine.lineNums[7]] >= 0)?
                         ((sigmaSt.sigmas[curLine.lineNums[7]] * curLine.lineNums[5]) / sigmaSt.sigmaMax):
                           ((sigmaSt.sigmas[curLine.lineNums[7]] * curLine.lineNums[5]) / (-3 * sigmaSt.sigmaMin)) ;
                  if(tempVal)
                  {
                      cmd( VERTEX2F((curLine.lineNums[1] - 2 - curLine.options * (sigmaSt.sigmaStPointer - 1 - i)) * 16,
                                    (uint16_t)(curLine.poseY + curLine.lineNums[5] - tempVal) * 16) );
                      cmd( VERTEX2F((curLine.lineNums[1] - 2 - curLine.lineNums[6] - curLine.options * (sigmaSt.sigmaStPointer - i)) * 16,
                                    (curLine.poseY + curLine.lineNums[5]) * 16) );
                  }
                }
                
              cmd( END() );
              break;
            case 3:
              cmd(COLOR_RGB(curLine.color[0], curLine.color[1], curLine.color[2]));
              switch(curLine.regType)
              {
              case CON_REGS:
              case DATA_REGS:
                BD->getDataFromRegs(curLine.regType, curLine.startReg, &cgVal);
                if(curLine.tenPow)
                    cgVal *= pow(10, curLine.tenPow);
                break;
              case 3:
                  switch(curLine.startReg)
                  {
                  case 0:
                      BD->getDataFromRegs(BDConf.regTypeCPS, BDConf.diagCPS, &cgVal);
                      if(curLine.tenPow)
                        cgVal *= pow(10, curLine.tenPow);
                      break;
                  case 1:
                      BD->getDataFromRegs(BDConf.regTypeDR, BDConf.regStartDR, &cgVal);
                      if(curLine.tenPow)
                        cgVal *= pow(10, curLine.tenPow);
                      break;
                  default:
                    exception(__FILE__,__FUNCTION__,__LINE__,"Wrong reg!!!");
                    break;
                  }
                  break;
              case 4:
                  switch(curLine.startReg)
                  {
                  case 0:
                      cgVal = geigerGetCPS();
                      if(curLine.tenPow)
                        cgVal *= pow(10, curLine.tenPow);
                      break;
                  case 1:
                      cgVal = geigerGetDose();
                      if(curLine.tenPow)
                        cgVal *= pow(10, curLine.tenPow);
                      break;
                  case 2:
                      cgVal = geigerGetErr();
                      if(curLine.tenPow)
                        cgVal *= pow(10, curLine.tenPow);
                      break;
                  default:
                    exception(__FILE__,__FUNCTION__,__LINE__,"Wrong reg!!!");
                    break;
                  }
                  break;
                case NULL_REG:
                default:
                  break;
              }
              showVal(cgVal, curLine.poseX, curLine.poseY, curLine.font, curLine.options);
              break;
            case 4:
              if(curLine.options&0x0001 || !strcmp(identifyControl.report, "NOTHING"))
              {
                  if(curLine.lineString[9]&0x0002)
                  {
                      cmd(COLOR_RGB(curLine.lineString[6], curLine.lineString[7], curLine.lineString[8]));
                      cmd( BEGIN(RECTS) );
                          cmd( LINE_WIDTH(1 * 16) );
                          
                          cmd( VERTEX2F(curLine.poseX * 16,curLine.poseY * 16) );
                          cmd( VERTEX2F(curLine.lineNums[1] * 16,curLine.lineNums[2] * 16) );
                          
                          cmd(COLOR_RGB(0, 0, 0));
                          cmd( VERTEX2F((curLine.poseX + 1) * 16,(curLine.poseY + 1) * 16) );
                          cmd( VERTEX2F((curLine.lineNums[1] - 1) * 16,(curLine.lineNums[2] - 1) * 16) );
                      cmd( END() );
                      cmd(COLOR_RGB(curLine.color[0], curLine.color[1], curLine.color[2]));
                  }
                  
                  if(curLine.lineString[9]&0x0004)
                  {
                    switch(curLine.options)
                    {
                    case OPT_CENTERX:
                        cmd_text((curLine.poseX + curLine.lineNums[1])/2, curLine.poseY + 2, curLine.font, curLine.options, (uint8_t*)identifyControl.report);
                        break;
                    case OPT_CENTERY:
                        cmd_text(curLine.poseX + 2, (curLine.poseY + curLine.lineNums[2])/2, curLine.font, curLine.options, (uint8_t*)identifyControl.report);
                        break;
                    case OPT_RIGHTX:
                        cmd_text(curLine.lineNums[1] - 2, (curLine.poseY + curLine.lineNums[2])/2, curLine.font, curLine.options, (uint8_t*)identifyControl.report);
                        break;
                    case 0:
                    default:
                        cmd_text(curLine.poseX + 2, curLine.poseY + 2, curLine.font, curLine.options, (uint8_t*)identifyControl.report);
                        break;
                    }
                  }
                  else
                    cmd_text((curLine.poseX + curLine.lineNums[1])/2, (curLine.poseY + curLine.lineNums[2])/2, curLine.font, OPT_CENTER, (uint8_t*)identifyControl.report);
              }
              
            default:
              break;
            }
          }
      }
    }
  }
  
  void cBD_config::readMenu(void)
  {
    uint16_t pose = curScreenPose + sizeof(tagBDScreeen) + scrLen * sizeof(tagScreenLine) + 2;
    if(filesystem_file_get(&filesystem, filesystem_open_file(&filesystem, nameBuff, "ini")
                           , (int*)&pose, (uint8_t*)&numOfMenuLines, 2) != 2)
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
    startOfMenuLines = pose;
    curMenuLine = 0;
    curMenuPose = pose;
    numOfMenuNames = 0;
    for(uint16_t i = 0; i < numOfMenuLines; i++)
    {
        if(filesystem_file_get(&filesystem, filesystem_open_file(&filesystem, nameBuff, "ini")
                           , (int*)&pose, (uint8_t*)&menuLine, sizeof(tagBDMenuLine)) != sizeof(tagBDMenuLine))
            exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corrupted.");
        if(menuLine.menuCmds == LINE_CMD)
          numOfMenuNames++;
    }
  }
  
  uint16_t cBD_config::getMenuLen(void)
  {
    return numOfMenuNames;
  }
  
  bool cBD_config::getMenuLine(uint16_t num, uint8_t* name)
  {
    if(num >= numOfMenuNames)
      return false;
    if(num < curMenuLine)
    {
      curMenuLine = 0;
      curMenuPose = startOfMenuLines;
    }
    
    for(; curMenuLine == num; curMenuLine++)
    {
      do
      {
        if(filesystem_file_get(&filesystem, filesystem_open_file(&filesystem, nameBuff, "ini")
                           , &curMenuPose, (uint8_t*)&menuLine, sizeof(tagBDMenuLine)) != sizeof(tagBDMenuLine))
            exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corrupted.");
        //if(curMenuPose >= (startOfMenuLines + numOfMenuLines*sizeof(tagBDMenuLine)))
          //return false;
      }
      while(menuLine.menuCmds != LINE_CMD);
    }
    memcpy(name, menuLine.name, 21);
    return true;
  }
  
  void cBD_config::control(void)
  {
    if(findConf())
    {
      if(spec != NULL)
       spec->specRead();
      uint8_t tempVal;
      if(waitAnsw && BD->getAnsw(&tempVal, 1))
         waitAnsw = false;
      if((BDConf.regTypeCPS != NULL_REG) && tim->check(0))
      {
          if(tim->check(0) > 0)
            calcSigma();
          tim->stop(0);
          tim->start(BDConf.dataUpdatePeriod, 0);
      }
      if(sendInProcess)
      {
        for(; numOfCmd < 20; numOfCmd++)
        {
           if(!sendCmd(menuLine.name[numOfCmd]))
             break;
        }
        if(numOfCmd >= 20)
          sendInProcess = false;
        if(!sendInProcess)
        {
         do
         {
           if(filesystem_file_get(&filesystem, filesystem_open_file(&filesystem, nameBuff, "ini")
                               , &curMenuPose, (uint8_t*)&menuLine, sizeof(tagBDMenuLine)) != sizeof(tagBDMenuLine))
                exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corrupted.");
           switch(menuLine.menuCmds)
           {
           case 5:
             for(numOfCmd = 0; numOfCmd < 20; numOfCmd++)
             {
               if(!sendCmd(menuLine.name[numOfCmd]))
                 break;
               else
                 waitAnsw = true;
             }
             break;
           case LINE_CMD:
           case NONE_CMD:
           default:
             break;
           }
         }
         while(menuLine.menuCmds != LINE_CMD && menuLine.menuCmds != NONE_CMD);
         if(menuLine.menuCmds == LINE_CMD || menuLine.menuCmds == NONE_CMD)
            curMenuLine++;
         else if(!sendInProcess)
         {
            curMenuLine = 0;
            curMenuPose = startOfMenuLines;
         }
        }
      }
    }
    else
      spec->stopSpecRead();
  }
  
  bool gri = true;
  
  void cBD_config::calcSigma(void)
  {
    if(!BD->getBD_ID() || (BDConf.regTypeCPS == NULL_REG))
    {
      sigmaSt.stLenCur = 0;
      return;
    }
    float bgCps;
    int32_t momCps;
    BD->getDataFromRegs(BDConf.regTypeCPS, BDConf.diagCPS, &momCps);
    //BD->getDataFromRegs(BDConf.regTypeBG, BDConf.diagBgCPS, &bgCps);
    
    if(!sigmaSt.sigmaStart)
      sigmaSt.cpsPreBuff[sigmaSt.cpsPreBuffPointer++] = momCps;
    
    if(sigmaSt.cpsPointer >= tBig)
    {
      sigmaSt.cpsPointer = 0;
      if(!sigmaSt.sigmaStart)
      {
        bgCps = sigmaSt.summCPS / tBig;
        for(int i = 0; i < tLit; i++)
            sigmaSt.sigmaPorog[i] = quantile_poisson_in_sigm(1.0 - (BDConf.dataUpdatePeriod * 0.001 * (i + 1)) / 3600.0, bgCps * BDConf.dataUpdatePeriod * 0.001 * (i + 1));
        sigmaSt.sigmaStart = true;
      }
    }
    
    if(!sigmaSt.sigmaStart)
    {
      sigmaSt.cpss[sigmaSt.cpsPointer++] = momCps;
      sigmaSt.summCPS += momCps;
      sigmaSt.cpsPreBuffPointer = 0;
      sigmaSt.calcStart = false;
      return;
    }
    
    if(!sigmaSt.cpsPreBuffrel[sigmaSt.cpsPreBuffPointer] && sigmaSt.calcStart)
    {
        sigmaSt.summCPS += sigmaSt.cpsPreBuff[sigmaSt.cpsPreBuffPointer] - sigmaSt.cpss[sigmaSt.cpsPointer];
        
        sigmaSt.cpss[sigmaSt.cpsPointer++] = sigmaSt.cpsPreBuff[sigmaSt.cpsPreBuffPointer];
        
        bgCps = sigmaSt.summCPS / tBig;
    }
    else
      gri = false;
    
    if(! sigmaSt.cpsPreBuffrel[sigmaSt.cpsPreBuffPointer])
      sigmaSt.cpss[sigmaSt.cpsPointer++] = sigmaSt.cpsPreBuff[sigmaSt.cpsPreBuffPointer];
    sigmaSt.cpsPreBuffrel[sigmaSt.cpsPreBuffPointer] = false;
    sigmaSt.cpsPreBuff[sigmaSt.cpsPreBuffPointer++] = momCps;
    
    if(sigmaSt.cpsPreBuffPointer >= tMaxInt)
    {
      sigmaSt.cpsPreBuffPointer = 0;
      sigmaSt.calcStart = true;
      
      bgCps = sigmaSt.summCPS / tBig;
    }
    
    if(!bgCps || !sigmaSt.calcStart)
      return;
    
    uint32_t cpsSum = 0;
    
    float sigmaCur;
    
    uint16_t k = 0;
    
    cpsSum = 0;
    
    float tInt = 0;
    
    for(int i = 0; i < tLit; i++)
    {
      tInt = BDConf.dataUpdatePeriod * 0.001 * (i + 1);
      if((int16_t)sigmaSt.cpsPreBuffPointer - i >= 0)
        k = sigmaSt.cpsPreBuffPointer - i;
      else
        k = tMaxInt - (i - sigmaSt.cpsPreBuffPointer);
      cpsSum += sigmaSt.cpsPreBuff[k];
      
      sigmaCur = (((cpsSum - bgCps) * tInt) / sqrt(bgCps * tInt));
      if(sigmaCur > sigmaSt.sigmaPorog[i])
      {
        for(int j = 0; j < i + tLit; j++)
        {
          if(sigmaSt.cpsPreBuffPointer - j >= 0)
            k = (int16_t)sigmaSt.cpsPreBuffPointer - j;
          else
            k = tMaxInt - (j - sigmaSt.cpsPreBuffPointer);
          sigmaSt.cpsPreBuffrel[k] = true;
        }
      }
    }
    
    ddddd = 0;
    
    if(sigmaSt.cpsPreBuffPointer - 1 >= 0)
            k = (int16_t)sigmaSt.cpsPreBuffPointer - 1;
          else
            k =  tMaxInt - 1;
    float lval = (((sigmaSt.cpsPreBuff[k] - bgCps) * BDConf.dataUpdatePeriod * 0.001) / sqrt(bgCps * BDConf.dataUpdatePeriod * 0.001));;
    /*
    if(lval>32767)
      lval=32767;
    else if(lval<-32768)
      lval=-32768;
    */
    if(sigmaSt.sigmaMax < lval)
      sigmaSt.sigmaMax = lval;
    else if(sigmaSt.sigmaMin >  lval)
      sigmaSt.sigmaMin = lval;
    
    if(sigmaSt.sigmas[sigmaSt.sigmaStPointer] == sigmaSt.sigmaMax)
    {
      sigmaSt.sigmaMax = 0;
      for(int i = 0; i < sigmaSt.stLenCur; i++)
      {
        if(sigmaSt.sigmaMax < sigmaSt.sigmas[i])
          sigmaSt.sigmaMax = sigmaSt.sigmas[i];
      }
    }
    else if(sigmaSt.sigmas[sigmaSt.sigmaStPointer] == sigmaSt.sigmaMin)
    {
      sigmaSt.sigmaMin = 0;
      for(int i = 0; i < sigmaSt.stLenCur; i++)
      {
        if(sigmaSt.sigmaMin > sigmaSt.sigmas[i])
          sigmaSt.sigmaMin = sigmaSt.sigmas[i];
      }
    }
    
    sigmaSt.sigmas[sigmaSt.sigmaStPointer] = lval;
    if(++sigmaSt.sigmaStPointer >= sigmaSt.sigmaStlen)
      sigmaSt.sigmaStPointer = 0;
    if(sigmaSt.stLenCur < sigmaSt.sigmaStlen)
      sigmaSt.stLenCur++;
  }
  
  float cBD_config::quantile_poisson_in_sigm(float _probability, float _mid)
{
	if (_mid <= 0.0 || _mid > 4.0E9)
		return 0.0;

	float sigma_n = 10.0;
	float mid_sigma_val = sigma_n * ((_mid > 2.25) ? sqrt(_mid) : 1.5);
	float left = _mid - mid_sigma_val;
	float right = _mid + mid_sigma_val;
	if (left < 0.0)
		left = 0.0;
	unsigned long nleft = (unsigned long)left;
	unsigned long nright = 1 + (unsigned long)right;

	float c2 = log(_mid) + 1.0f;
	float threshold = (1.0 - _probability) * 2.506628274631000502415;
	float sum = 0.0;
	for (unsigned long i = nright; i > nleft; i--)
	{
		float di = (float)i;
		float v_i = log(di);
		v_i = di * c2 - _mid - (di + 0.5f) * v_i;
		v_i = exp(v_i);
		sum += v_i;
		if (sum > threshold)
			return ((di - _mid) / sqrt(_mid));
	}
	return (((float)nleft - _mid) / sqrt(_mid));
}

  
  
  int rez, siz;
  void cBD_config::readLine(uint16_t lineNum)
  {
    if(lineNum == 0xFFFF || curScreenNum == 0xFFFF)
      exception(__FILE__,__FUNCTION__,__LINE__,"to many lines!!!");
    if(lineNum == curLineNum)
      return;

    //readFromConf(curScreen.scrStart + (sizeof(tagScreenLine) * curLineNum), &curLine, sizeof(tagScreenLine));
    int pose = curScreenPose + sizeof(tagBDScreeen) + 2 + (sizeof(tagScreenLine) * lineNum);
    rez = filesystem_file_get(&filesystem, filesystem_open_file(&filesystem, nameBuff, "ini"), &pose, (uint8_t*)&curLine, sizeof(tagScreenLine));
    siz = sizeof(tagScreenLine);
    if(rez != siz)
        exception(__FILE__,__FUNCTION__,__LINE__,"Cofig file corupted.");
    curLineNum = lineNum;
  }
  
  tagMenu specMenue[3];
  
  static uint32_t specTime;
  
  cChangeValue changeAcTime((char*)"Acquiring time", &specTime, 4, 65535, 0);
  
  cOpenFileDialog openSpectrum((char*)"ini");
  
  void changeAqTime(void)
  {
    currentMode->showMessage(&changeAcTime);
    currentMode->openMenu(NULL, 0);
  }
  
  void changeOpenSpec(void)
  {
    openSpectrum.start();
    currentMode->showMessage(&openSpectrum);
    currentMode->openMenu(NULL, 0);
  }
  
  void exitSpecMod(void)
  {
    currentMode->change(modeKarusel[0]);
    currentMode->openMenu(NULL, 0);
  }
  
  cBD_Spec_mode::cBD_Spec_mode(MODBUS::cBD* BD, TIMER::cMS_timer_prototype* tim)
  {
    InterfaceColor.red = 255;
    InterfaceColor.green = 0;
    InterfaceColor.blue = 0;
    FontColor.red = 0;
    FontColor.green = 255;
    FontColor.blue = 0;
    this->BD = BD;
    old_ID = 0;
    countMode = false;
    started = false;
    inited = false;
    this->tim = tim;
    mod = MARKER;
    strcpy((char*)specMenue[0].name, "Change Aq time");
    specMenue[0].func = changeAqTime;
    strcpy((char*)specMenue[1].name, "Open spectrum");
    specMenue[1].func = changeOpenSpec;
    strcpy((char*)specMenue[2].name, "Exit spec. mode");
    specMenue[2].func = exitSpecMod;
    for(int i = 0; i < 3; i++)
      specMenue[i].isShow = true;
    
  }
  
  void cBD_Spec_mode::userControl(void)
  {
    if(!poseInSpec)
      show();
    if(keyHold)
      keyHold = false;
    else
      markerSpeed = 10;
  }
  
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
  
  void cBD_Spec_mode::initRead(bool isComp, uint16_t specUpdatePeriod)
  {
    tagBDInf inf = BD->getBDInfo();
    this->numOfCanal = inf.numOfCanal;
    if(numOfCanal > 2048)
      exception(__FILE__,__FUNCTION__,__LINE__,"To many canals!!!");
    this->isComp = isComp;
    this->specUpdatePeriod = specUpdatePeriod;
    if(numOfCanal > 2048 || numOfCanal < 256)
      exception(__FILE__,__FUNCTION__,__LINE__,"Wrong canal number!");
    poseInSpec = 0;
    waitForRx = false;
    inited = true;
    inLine = false;
    memcpy((char*)onButtonText, "menu", 4);
    memcpy((char*)enterButtonText, "Marker", 6);
    memcpy((char*)rightButtonText, "Right", 6);
    memcpy((char*)leftButtonText, "Left", 6);
    specNum = 2097;
    poseInBuff = 0;
    strcpy((char*)modeName, "Spectrum");
    markerPose = numOfCanal/4;
    specTime = 0xffff;
    started = false;
    reInint = true;
    scale = 1000;
    scrlen = numOfCanal;
    scrPose = 0;
    markerSpeed = 10;
    rKeyState = false;
    lKeyState = false;
    eKeyState = true;
    oKeyState = true;
  }
  
  bool cBD_Spec_mode::startSpecRead(void)
  {
    if(!inited || specNum)
      return false;
    reInint = true;
    started = false;
    return true;
  }
  
  
  void cBD_Spec_mode::specRead(void)
  {
    if(!inited || (!started && !specNum))
      return;
    if(waitForRx)
    {
      if(BD->getAnsw(tempBuff, BD->getAnswLen()))
      {
        if(tempBuff[1] == 0x0B || tempBuff[1] == 0x09)
        {
          long* buff;
          if(!specNum)
            //memcpy(&specBuff[poseInSpec], &tempBuff[3], tempBuff[2]);
            buff = specBuff;
          else if(specNum == 2097)
            //memcpy(&energyBuff[poseInSpec], &tempBuff[3], tempBuff[2]);
            buff = sigmaBuff;
          else
            //memcpy(&sigmaBuff[poseInSpec], &tempBuff[3], tempBuff[2]);
            buff = energyBuff;
          short i = 0;
          for(; poseInBuff < numOfCanal; poseInBuff++)
          {
            BD->reversCpy(&tempBuff[i + 3 - 2] , &buff[poseInBuff], 4);
            buff[poseInBuff] &= 0x00ffffff;
            i += 3;
            if(i >= tempBuff[2])
              break;
          }
          poseInBuff++;
          if(poseInBuff >= numOfCanal)
          {
            BD->reversCpy(&tempBuff[i + 2], &aliveTime, 2);
            BD->reversCpy(&tempBuff[i + 4], &hiEnergyCPS, 4);
          }
          poseInSpec += 240;
          if(poseInSpec >= numOfCanal * 3 + 6)
          {
              if(specNum == 2097)
                specNum = 2099;
              else if(specNum == 2099)
              {
                specNum = 0;
                identify_InitIdent(specBuff, energyBuff, sigmaBuff, numOfCanal);
                if(identify_read_identify_ini() == S_OK)
                    identify_open_library();
              }
              else
                identify_identify(1);
              
              poseInSpec = 0;
              poseInBuff = 0;
              tim->stop(0);
              tim->start(specUpdatePeriod, 0);
              
          }
        }
        waitForRx = false;
      }
    }
    else if(!isComp && (started || specNum))
    {
        if(!inLine)
        {
            BD->toQeue();
            inLine = true;
        }
        
          if(!specNum)
          {
            if(BD->sendCmdWithMunOfBytes(0x0B, poseInSpec, min(240, numOfCanal * 3 + 6 - poseInSpec)))
            {
                waitForRx = true;
                inLine = false;
            }
          }
          else
          {
            if(BD->sendCmdWithMunOfBytes(0x09, specNum, poseInSpec, min(240, numOfCanal * 3 + 6 - poseInSpec)))
            {
                waitForRx = true;
                inLine = false;
            }
          }
    }
    if(reInint)
    {
      if(!waitForRx)
      {
        switch(tempBuff[1])
        {
        case 0x05:
            if(tempBuff[3] == 0x00)
            {
              if(!inLine)
                {
                    BD->toQeue();
                    inLine = true;
                }
              if(BD->sendCmd(0x06, 0x0000, (uint16_t)specTime))
              {
                inLine = false;
                waitForRx = true;
              }
            }
            else if(tempBuff[3] == 0x01)
            {
              reInint = false;
              started = true;
            }else
              exception(__FILE__,__FUNCTION__,__LINE__,"Unknown cmd!!!");
            break;
        case 0x06:
            if(!inLine)
            {
                BD->toQeue();
                inLine = true;
            }
             if(BD->sendCmd(0x05, 0x0001, 0xff00))
              {
                inLine = false;
                waitForRx = true;
              }
             break;
        default:
            if(!inLine)
            {
                BD->toQeue();
                inLine = true;
            }
             if(BD->sendCmd(0x05, 0x0000, 0xff00))
              {
                inLine = false;
                waitForRx = true;
              }
            break;
        }
      }
    }
  }
  
  void cBD_Spec_mode::stopSpecRead(void)
  {
    started = false;
  }
  
  
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
  
   void cBD_Spec_mode::onShow(void)
   {
      float poseX = 0;
      uint16_t lastPoseX = 0;
      float lastPoseY = 250;
      float canSumm = 0;
      uint16_t chanCount;
      float chanLen = (float)240/scrlen;
      curMaxCps = 0;
      cntSumm = 0;
      for(int i = scrPose; i < scrPose + scrlen; i++)
      {
        if(logView && specBuff[i] && log(specBuff[i]) > curMaxCps)
            curMaxCps = log(specBuff[i]);
        else if(specBuff[i] > curMaxCps)
          curMaxCps = specBuff[i];
      }
      for(int i = 0; i < numOfCanal; i++)
      {
        if(mod != WINDOW)
            cntSumm += specBuff[i];
        else if(i  >= min(markerPose, windowPose) && i <= max(markerPose, windowPose))
            cntSumm += specBuff[i];
      }
      /*
      uint16_t iPow = 0;
      if(curMaxCps >= 6000)
        iPow = 2000;
      else if(curMaxCps >= 3000)
        iPow = 1000;
      else if(curMaxCps >= 1000)
        iPow = 500;
      else if(curMaxCps >= 600)
        iPow = 200;
      else if(curMaxCps >= 300)
        iPow = 100;
      else if(curMaxCps >= 100)
        iPow = 50;
      else if(curMaxCps >= 60)
        iPow = 20;
      else if(curMaxCps >= 30)
        iPow = 10;
      else if(curMaxCps >= 10)
        iPow = 5;
      else if(curMaxCps >= 6)
        iPow = 2;
      else if(curMaxCps >= 3)
        iPow = 1;
      for(uint16_t i = 0; i * iPow < curMaxCps; i++)
      {
          cmd_number(1, 250 - (i*iPow*100/scale) * 100/curMaxCps, 16, 0, i*iPow);
      }
      */
      cmd( COLOR_RGB(255, 255, 0) );
      cmd( BEGIN(LINES) );
      cmd( LINE_WIDTH(1 * 16) );
      lastPoseY = 250 - (specBuff[scrPose]*100/scale) * 100/curMaxCps;
      for(int i = scrPose; i < scrPose + scrlen; i++)
      {
        poseX += chanLen;
        if(logView && canSumm)
            canSumm += log(specBuff[i]);
        else if(!logView)
            canSumm += specBuff[i];
        chanCount++;
        if(round(poseX) > lastPoseX)
        {
            cmd( VERTEX2F(lastPoseX * 16,(uint16_t)round(lastPoseY) * 16) );
            lastPoseX = (uint16_t)round(poseX);
            lastPoseY = 250 - (canSumm*1000/chanCount/scale) * 100/curMaxCps;
            if(lastPoseY < 150 || lastPoseY > 250)
              lastPoseY = 150;
            canSumm = 0;
            chanCount = 0;
            cmd( VERTEX2F(lastPoseX * 16,(uint16_t)round(lastPoseY) * 16) );
        }
      }
        
      
      cmd( COLOR_RGB(255, 0, 0) );
      cmd( VERTEX2F((uint16_t)round(240*(markerPose - scrPose)/scrlen) * 16,150 * 16) );
      cmd( VERTEX2F((uint16_t)round(240*(markerPose - scrPose)/scrlen) * 16,250 * 16) );
      if(mod == WINDOW)
      {
        cmd( VERTEX2F((uint16_t)round(240*(windowPose - scrPose)/scrlen) * 16,150 * 16) );
        cmd( VERTEX2F((uint16_t)round(240*(windowPose - scrPose)/scrlen) * 16,250 * 16) );
      }
      cmd( END() );
      cmd( COLOR_RGB(255, 255, 0) );
      //cmd_text(120, 28, 30, 512, (uint8_t*)identifyControl.report);
      uint8_t buff[10];
      sprintf((char*)buff, "%d s", aliveTime);
      cmd_text(5, 60, 27, 0, buff);
      memset(buff, 0, 10);
      sprintf((char*)buff, "%d cnt", cntSumm);
      cmd_text(120, 60, 27, OPT_CENTERX, buff);
      memset(buff, 0, 10);
      sprintf((char*)buff, "%.1f cps", (float)cntSumm/aliveTime);
      cmd_text(235, 60, 27, OPT_RIGHTX, buff);
      
      memset(buff, 0, 10);
      sprintf((char*)buff, "%d ch", (mod == WINDOW)?windowPose:markerPose);
      cmd_text(5, 255, 27, 0, buff);
      memset(buff, 0, 10);
      sprintf((char*)buff, "%d keV", energyBuff[(mod == WINDOW)?windowPose:markerPose]);
      cmd_text(120, 255, 27, OPT_CENTERX, buff);
      memset(buff, 0, 10);
      sprintf((char*)buff, "%d cnt", specBuff[(mod == WINDOW)?windowPose:markerPose]);
      cmd_text(235, 255, 27, OPT_RIGHTX, buff);
      
      float position = markerPose;
      float sigma;
      
      cmd( COLOR_RGB(255, 125, 0) );
      if(Spectrum_peakProc_ex(&position, &sigma))
      {
          memset(buff, 0, 10);
          sprintf((char*)buff, "N=%.1f", position);
          cmd_text(5, 80, 27, 0, buff);
          
          const float sqrt2ln2=200.0*1.17774100225;
          
          memset(buff, 0, 10);
          sprintf((char*)buff, "R=%.1f%%", sigma*sqrt2ln2/position);
          cmd_text(120, 80, 27, OPT_CENTERX, buff);
          
          int en = (int)(identify_EnergyFromChannel(position)+0.5);
          memset(buff, 0, 10);
          sprintf((char*)buff, "E=%u keV", en);
          cmd_text(235, 80, 27, OPT_RIGHTX, buff);
          
          int is = (int)(position-sigma*3.5), ie = (int)(position+sigma*3.5);
          
          uint32_t dwarea=0;
          
          for(;is<=ie;is++)dwarea+=specBuff[is];
          
          memset(buff, 0, 10);
          sprintf((char*)buff, "PA=%u cnt", dwarea);
          cmd_text(5, 100, 27, 0, buff);
          
          cmd_text(235, 100, 27, OPT_RIGHTX, (uint8_t*)identifyControl.report);
      }
      
   }
  
    
    
    void cBD_Spec_mode::enterKeyAction(void)
    {
      switch(mod)
      {
      case MARKER:
        mod = ACQUIR;
        memset(enterButtonText, 0, 7);
        memset(rightButtonText, 0, 7);
        memset(leftButtonText, 0, 7);
        memcpy((char*)enterButtonText, "Acquir", 6);
        memcpy((char*)rightButtonText, "Stop", 4);
        memcpy((char*)leftButtonText, "Start", 5);
        keyChangeMod(R_KEY, UNLOCK_KEY);
        keyChangeMod(L_KEY, UNLOCK_KEY);
        break;
      case ACQUIR:
        mod = ZOOM;
        memset(enterButtonText, 0, 7);
        memset(rightButtonText, 0, 7);
        memset(leftButtonText, 0, 7);
        memcpy((char*)enterButtonText, "Zoom", 4);
        memcpy((char*)rightButtonText, "Out", 2);
        memcpy((char*)leftButtonText, "In", 2);
        keyChangeMod(R_KEY, UNLOCK_KEY);
        keyChangeMod(L_KEY, UNLOCK_KEY);
        break;
      case ZOOM:
        mod = YSCALE;
        memset(enterButtonText, 0, 7);
        memset(rightButtonText, 0, 7);
        memset(leftButtonText, 0, 7);
        memcpy((char*)enterButtonText, "YScale", 6);
        memcpy((char*)rightButtonText, "Lower", 5);
        memcpy((char*)leftButtonText, "Higher", 6);
        keyChangeMod(R_KEY, UNLOCK_KEY);
        keyChangeMod(L_KEY, UNLOCK_KEY);
        break;
      case YSCALE:
        mod = WINDOW;
        windowPose = markerPose;
        memset(enterButtonText, 0, 7);
        memset(rightButtonText, 0, 7);
        memset(leftButtonText, 0, 7);
        memcpy((char*)enterButtonText, "Window", 6);
        memcpy((char*)rightButtonText, "Right", 5);
        memcpy((char*)leftButtonText, "Left", 4);
        keyChangeMod(R_KEY, UNLOCK_KEY);
        keyChangeMod(L_KEY, UNLOCK_KEY);
        break;
      case WINDOW:
        mod = VIEW;
        memset(enterButtonText, 0, 7);
        memset(rightButtonText, 0, 7);
        memset(leftButtonText, 0, 7);
        memcpy((char*)enterButtonText, "View", 4);
        memcpy((char*)rightButtonText, "Normal", 6);
        memcpy((char*)leftButtonText, "Log", 3);
        keyChangeMod(R_KEY, LOCK_KEY);
        keyChangeMod(L_KEY, LOCK_KEY);
        break;
      case VIEW:
        mod = MARKER;
        memset(enterButtonText, 0, 7);
        memset(rightButtonText, 0, 7);
        memset(leftButtonText, 0, 7);
        memcpy((char*)enterButtonText, "Marker", 6);
        memcpy((char*)rightButtonText, "Right", 5);
        memcpy((char*)leftButtonText, "Left", 4);
        keyChangeMod(R_KEY, UNLOCK_KEY);
        keyChangeMod(L_KEY, UNLOCK_KEY);
        break;
      default:
        exception(__FILE__,__FUNCTION__,__LINE__,"Unknown cmd!!!");
      }
      show();
    }
  
    void cBD_Spec_mode::rKeyAction(void)
    {
      uint16_t scrInc = 0;
      switch(mod)
      {
      case MARKER:
        if(markerPose < numOfCanal)
        {
          markerPose += (uint16_t)round((float)markerSpeed/10);
          if(markerSpeed < 100)
            markerSpeed++;
          keyHold = true;
        }
        while(markerPose > scrPose + scrlen)
          scrPose++;
        show();
        break;
      case ACQUIR:
        stopSpecRead();
        break;
      case ZOOM:
        if(scrlen < numOfCanal)
        {
          scrInc = (uint16_t)round(numOfCanal/20);
          scrlen += round(numOfCanal/10);
          if(scrPose - scrInc > 0)
            scrPose -= scrInc;
          else
            scrPose = 0;
          if(scrPose + scrlen + scrInc > numOfCanal)
            scrPose = numOfCanal - scrlen;
          show();
        }
        break;
      case YSCALE:
        if(scale < 1000)
        {
          scale += markerSpeed;
          if(markerSpeed < 100)
            markerSpeed++;
          keyHold = true;
        }
        show();
        break;
      case WINDOW:
        if(windowPose < numOfCanal)
        {
            windowPose += (uint16_t)round((float)markerSpeed/10);
            if(markerSpeed < 100)
                markerSpeed++;
            keyHold = true;
        }
        while(windowPose > scrPose + scrlen)
          scrlen++;
        show();
        break;
      case VIEW:
        logView = false;
        show();
        break;
      default:
        break;
      }
    }
  
    void cBD_Spec_mode::lKeyAction(void)
    {
      uint16_t scrOld = 0;
      switch(mod)
      {
      case MARKER:
        if(markerPose > 0)
        {
          markerPose -= (uint16_t)round((float)markerSpeed/10);
          if(markerSpeed < 100)
                markerSpeed++;
          keyHold = true;
        }
        while(markerPose < scrPose)
          scrPose--;
        show();
        break;
      case ACQUIR:
        startSpecRead();
        break;
      case ZOOM:
        if(scrlen > numOfCanal/5)
        {
          scrOld = scrlen;
          scrlen -= round(numOfCanal/10);
          if(markerPose - scrlen/2 > scrPose)
            scrPose = markerPose - (int)round(scrlen/2);
          if(markerPose + scrlen/2 > scrPose + scrOld)
            scrPose += scrOld - scrlen;
          show();
        }
        break;
      case YSCALE:
        if((10*1000/scale) * 100/curMaxCps < 150)
        {
          scale -= markerSpeed;
          if(markerSpeed < 100)
                markerSpeed++;
          keyHold = true;
        }
        show();
        break;
      case WINDOW:
        if(windowPose > 0)
        {
            windowPose -= (uint16_t)round((float)markerSpeed/10);
            if(markerSpeed < 100)
                markerSpeed++;
            keyHold = true;
        }
        while(windowPose < scrPose)
        {
          scrPose--;
          scrlen++;
        }
        show();
        break;
      case VIEW:
        logView = true;
        show();
        break;
      default:
        break;
      }
    }
  
    void cBD_Spec_mode::onKeyAction(void)
    {
      openMenu(specMenue, 3);
    }
  
    
}
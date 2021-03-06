#include "Mode_prototype.h"
#include "HedLine.h"
#include "keys.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "exeptions.h"

using namespace KEYBORD;

cKey* rKey;
cKey* lKey;
cKey* enterKey;
cKey* onKey;



namespace MODES
{
  cMode_prototype* currentMode;
  
  cMode_prototype* modeKarusel[4]; 
  
  //метод который будет крутиься в мэйне. обрабатывает нажатия кнопок. определен только здесь. 
  //для остальных классов определяется userControl
  void cMode_prototype::control(void)
  {
    
    hedline_control();
    
    if(messageClass != NULL)
    {
      if(rKey->wasClicked())
          messageClass->rKeyAction();
        if(lKey->wasClicked())
          messageClass->lKeyAction();
        if(enterKey->wasClicked())
          messageClass->enterKeyAction();
        if(onKey->wasClicked())
          messageClass->onKeyAction();
    }else if(menuOpen)
    {
        if(lKey->wasClicked())
        {
          if(currentMenuLine > 0)
          {
             currentMenuLine--;
             if(currentMenuLine < screenPose)
                screenPose--;
          }
          else
          {
            currentMenuLine = menuShownlen - 1;
            if(menuShownlen > 12)
              screenPose = menuShownlen - 13;
            else
              screenPose = 0;
          }
          show();
        }
        if(rKey->wasClicked())
        {
          if(currentMenuLine < menuShownlen - 1)
          {
             currentMenuLine++;
             if(currentMenuLine > screenPose + 12)
                screenPose++;
          }
          else
          {
            currentMenuLine = 0;
            screenPose = 0;
          }
          show();
        }
        if(enterKey->wasClicked())
          menuAction();
        if(onKey->wasClicked())
        {
          menuOpen = false;
          show();
        }
    }else
    //if(!menuOpen)
    {
        if(rKey->wasClicked())
          rKeyAction();
        if(lKey->wasClicked())
          lKeyAction();
        if(enterKey->wasClicked())
          enterKeyAction();
        if(onKey->wasClicked())
          onKeyAction();
    }
    
    userControl();
  }
            
  void cMode_prototype::menuAction(void)
  {
     if(menu[currentMenuInArr].func != NULL)
       menu[currentMenuInArr].func();
  }
  
  //этот метод надо вызывать всякий раз когда нужно перерисовать экран.
  void cMode_prototype::show(void)
  {
    cmd(CMD_DLSTART);
    cmd(CLEAR_COLOR_RGB(0,0,0));
    cmd(CLEAR(1,1,1));
    
        hedline_show();
	
	// верхняя линия клавиатуры.
        cmd( COLOR_RGB(InterfaceColor.red, InterfaceColor.green, InterfaceColor.blue) );
	cmd( BEGIN(LINES) );
        cmd( LINE_WIDTH(1 * 8) );
	cmd( VERTEX2F(1 * 16,280 * 16) );
	cmd( VERTEX2F(240 * 16,280 * 16) );
	
	// левый гориз. разделитель
	cmd( VERTEX2F(80 * 16,281 * 16) );
	cmd( VERTEX2F(80 * 16,319 * 16) );
	
	//правый гориз. разделитель
	cmd( VERTEX2F(160 * 16,281 * 16) );
	cmd( VERTEX2F(160 * 16,319 * 16) );
	
	//вертикальный разделитьель клавиш
        cmd( VERTEX2F(81 * 16,300 * 16) );
	cmd( VERTEX2F(159 * 16,300 * 16) );
	
	//черта над названием
	cmd( VERTEX2F(1 * 16,20 * 16) );
	cmd( VERTEX2F(240 * 16,20 * 16) );
	
        cmd( END() );
        if(messageClass != NULL)
        {
            cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue) );
            //надписи на кнопках
            cmd_text(40, 290, 28, 512, messageClass->leftButtonText);
            cmd_text(200, 290, 28, 512, messageClass->rightButtonText);
            cmd_text(120, 281, 27, 512, messageClass->enterButtonText);
            cmd_text(120, 301, 27, 512, messageClass->onButtonText);
            cmd( COLOR_RGB(InterfaceColor.red, InterfaceColor.green, InterfaceColor.blue) );
        }
        else if(!menuOpen)
        {
          
            cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue) );
            //надписи на кнопках
            cmd_text(40, 290, 28, 512, leftButtonText);
            cmd_text(200, 290, 28, 512, rightButtonText);
            cmd_text(120, 281, 27, 512, enterButtonText);
            cmd_text(120, 301, 27, 512, onButtonText);
            cmd( COLOR_RGB(InterfaceColor.red, InterfaceColor.green, InterfaceColor.blue) );
        }
        if(messageClass == NULL || !messageClass->clearScreen)
        {
            if(modeName != NULL)
            {
                cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue) );
                cmd_text(120, 28, 30, 512, modeName);
                cmd( COLOR_RGB(InterfaceColor.red, InterfaceColor.green, InterfaceColor.blue) );
            }
              
            onShow();
            
            if(menuOpen)
              showMenu();
        }
        if(messageClass != NULL)
          messageClass->onShow();
        
    cmd(DISPLAY());
    cmd(CMD_SWAP); 
  }
  
  // показывает меню
  void cMode_prototype::openMenu(tagMenu* menu,//ссылка на массив структур со строчками меню
                                 uint16_t len)//его длинна
  {
    
    
    if(menu == NULL)
    {
      menuOpen = false;
      return;
    }
    this->menu = menu;
    menuLen = len;
    menuShownlen = 0;
    for(int i = 0; i < menuLen; i++)
    {
      if(menu[i].isShow)
        menuShownlen++;
    }
    menuOpen = true;
    currentMenuLine = 0;
    screenPose = 0;
    show();
    rKey->keyLock(false);
    lKey->keyLock(false);
    enterKey->keyLock(true);
    onKey->keyLock(true);
    
  }
  
    //нарисовать меню
  //автоматичекий запускается в show если меню открыто
   void cMode_prototype::showMenu(void)
   {
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
            
            
            cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue) );
            int index = 0;
            int showCount = 0;
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
            if(messageClass == NULL)
            {
                cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue) );
                cmd_text(40, 290, 28, 512, "up");
                cmd_text(200, 290, 28, 512, "down");
                cmd_text(120, 281, 27, 512, "enter");
                cmd_text(120, 301, 27, 512, "exit");
            }
            
            
   }
  
  // это метод показывает/скрывает элементы меню
  //!!! ПРОВОДИТЬ ДАННЫЕ ОПЕРАЦИИ ЛУЧШЕ ТОЛЬКО ПРИ ПОМОЩИ НЕГО!!!
  void cMode_prototype::menuShowHideLine(uint16_t numOfLine,//номер строки в массиве
                                         bool show)//показать/скрыть
  {
    menu[numOfLine].isShow = show;
    uint16_t menuShownlenOld = menuShownlen;
    menuShownlen = 0;
    for(int i = 0; i < menuLen; i++)
    {
      if(menu[i].isShow)
        menuShownlen++;
    }
    if(menuShownlen > menuShownlenOld && currentMenuInArr > numOfLine)
    {
      currentMenuLine++;
      if(currentMenuLine > screenPose + 12)
                screenPose++;
    }
    if(menuShownlen < menuShownlenOld && currentMenuInArr > numOfLine)
    {
      if(screenPose > 0)
      {
        screenPose--;
        currentMenuLine--;
      }
      else
        currentMenuLine--;
    }
    this->show();
  }
  
  // этот метод для смены текущего активного мода.
  void cMode_prototype::change(cMode_prototype *newMode)
  {
    openMenu(NULL, 0);
    currentMode = newMode;
    currentMode->show();
    rKey->keyLock(currentMode->rKeyState);
    lKey->keyLock(currentMode->lKeyState);
    enterKey->keyLock(currentMode->eKeyState);
    onKey->keyLock(currentMode->oKeyState);
  }
  
  void cMode_prototype::keyChangeMod(tagKey key, tagKeyMod mod)
  {
    cKey* keyClass = NULL;
    switch(key)
    {
    case R_KEY:
      keyClass = rKey;
      rKeyState = mod;
      break;
    case L_KEY:
      keyClass = lKey;
      lKeyState = mod;
      break;
    case O_KEY:
      keyClass = onKey;
      oKeyState = mod;
      break;
    case E_KEY:
      keyClass = enterKey;
      eKeyState = mod;
    default:
      break;
    }
    if(keyClass != NULL)
        keyClass->keyLock(mod?true:false);
    
  }
  
  void cMode_prototype::showMessage(cMessage_portotype* message)
  {
    messageClass = message;
    if(messageClass != NULL)
        messageClass->start();
    show();
    rKey->keyLock(messageClass->rKeyState);
    lKey->keyLock(messageClass->lKeyState);
    enterKey->keyLock(messageClass->eKeyState);
    onKey->keyLock(messageClass->oKeyState);
  }
  
  cChangeValue::cChangeValue(char* valueName, float* value, uint16_t formatI, uint16_t formatF, float upkeep, float downkeep)
  {
    if(*value > upkeep || *value < downkeep)
      exception(__FILE__,__FUNCTION__,__LINE__,"value out of range");
    InterfaceColor.red = 222;
    InterfaceColor.green = 0;
    InterfaceColor.blue = 0;
    FontColor.red = 0;
    FontColor.green = 222;
    FontColor.blue = 0;
    
    strcpy((char*)valueName, valueName);
    
    valueF = value;
    valueUI = NULL;
    this->formatI = formatI;
    this->formatF = formatF;
    this->upkeep = upkeep;
    this->downkeep = downkeep;
    
    curPos = 0;
    
    strcpy((char*)leftButtonText,"<-");
    strcpy((char*)rightButtonText, "->");
    strcpy((char*)enterButtonText, "++");
    strcpy((char*)onButtonText, "quit");
    
    this->value = *valueF;
    clearScreen = true;
  }
  
  cChangeValue::cChangeValue(char* valueName, uint32_t* value, uint16_t formatI, float upkeep, float downkeep)
  {
    if(*value > upkeep || *value < downkeep)
      exception(__FILE__,__FUNCTION__,__LINE__,"value out of range");
    InterfaceColor.red = 222;
    InterfaceColor.green = 0;
    InterfaceColor.blue = 0;
    FontColor.red = 0;
    FontColor.green = 222;
    FontColor.blue = 0;
    
    strcpy((char*)this->valueName, valueName);
    
    valueUI = value;
    valueF = NULL;
    this->formatI = formatI;
    this->formatF = 0;
    this->upkeep = upkeep;
    this->downkeep = downkeep;
    
    curPos = 0;
    
    strcpy((char*)leftButtonText,"<-");
    strcpy((char*)rightButtonText, "->");
    strcpy((char*)enterButtonText, "++");
    strcpy((char*)onButtonText, "quit");
    
    this->value = (float)*valueUI;
  }
  
  void cChangeValue::start()
  {
      if(valueF != NULL)
        value = *valueF;
      else
        value = (float)*valueUI;
      exitPos = false;
      curPos = 0;
  }
  
  void cChangeValue::onShow(void)
  {
    if(value > upkeep || value < downkeep)
      exception(__FILE__,__FUNCTION__,__LINE__,"value out of range");
    cmd( COLOR_RGB(InterfaceColor.red, InterfaceColor.green, InterfaceColor.blue) );
    cmd( BEGIN(RECTS) );
    
    cmd( LINE_WIDTH(1 * 16) );
    cmd( VERTEX2F(5 * 16,70 * 16) );
    cmd( VERTEX2F(235 * 16,170 * 16) );
    
    cmd( COLOR_RGB(0, 0, 0) );
    cmd( VERTEX2F(6 * 16,71 * 16) );
    cmd( VERTEX2F(234 * 16,169 * 16) );
    

    cmd( COLOR_RGB(InterfaceColor.red, InterfaceColor.green, InterfaceColor.blue) );
    if(!exitPos)
    {
      cmd( VERTEX2F((212 - curPos*20) * 16,125 * 16) );
      cmd( VERTEX2F((228 - curPos*20) * 16,150 * 16) );
    }
    else
    {
      cmd( VERTEX2F(90 * 16,155 * 16) );
      cmd( VERTEX2F(150 * 16,170 * 16) );
    }
    
    cmd(END());
    
    if(exitPos)
    {
        cmd( COLOR_RGB(0, 0, 0) );
        cmd_text(120, 150, 22, 512, "save");
    }
    
    cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue) );
    
    if(!exitPos)
        cmd_text(120, 150, 22, 512, "save");
    
    cmd_text(120, 80, 28, 512, valueName);
    
    uint8_t str[40];
    sprintf((char*)str, "From %.*f to %.*f",formatF, downkeep, formatF, upkeep);
    
    cmd_text(120, 105, 20, 512, str);
   
    char buff;
    /*
    char mask[7];
    
    strcpy(mask, "%+00.0f");
    
    mask[3] += formatI + formatF + 2;
    mask[5] += formatF;
    */
    sprintf((char*)val, "%+0*.*f", formatI + formatF + (formatF?2:1), formatF, value);
    /*
    for(int i = 0; i < 10; i ++)
    {
      val[i] = ' ';
    }
    */
    for(int i = 0; i < formatF + formatI; i++)
    {
      uint16_t posInVal = formatF + formatI - i;
      if(i < formatF)
        posInVal++;
      buff = val[posInVal];
      if(i == curPos && !exitPos)
        cmd( COLOR_RGB(0, 0, 0) );
      cmd_text_var(220 - i*20, 120, 30, 512, &buff, 1);
      if(i == curPos && !exitPos)
        cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue) );
    }
    
    
    if(formatF)
    {
        buff = '.';
        cmd_text_var(210 - (formatF-1)*20, 120, 30, 512, &buff, 1);
    }
    if(valueF != NULL && val[0] == '-' && downkeep < 0)
    {
          buff = '-';
          cmd_text_var(220 - (formatF + formatI)*20, 120, 30, 512, &buff, 1);
    }
    
  }
  
  void cChangeValue::enterKeyAction(void)
  {
    
    if(exitPos)
    {
      if(valueF != NULL)
        *valueF = value;
      else
        *valueUI = (uint32_t)value;
      currentMode->showMessage(NULL);
      currentMode->show();
      return;
    }
    
      if(curPos == formatF + formatI)
      {
        float tempVal = value;
        tempVal *= -1;
        if(tempVal <= upkeep && tempVal >= downkeep)
        {
          value = tempVal;
          currentMode->show();
        }
        return;
      }
      uint16_t posInVal = formatI + formatF + (formatF?1:0) - curPos;
      if(valueF != NULL && formatF != 0 && curPos >= formatF)
        posInVal--;
      float nevVal;
      if(val[0] != '-')
      {
          val[posInVal]++;
          nevVal = atof(val);
          if(val[posInVal] > '9' || nevVal > upkeep)
          {
            val[posInVal] = '0';
            nevVal = atof(val);
            while(nevVal < downkeep)
            {
              val[posInVal]++;
              nevVal = atof(val);
            }
          }
      }
      else
      {
        val[posInVal]--;
        nevVal = atof(val);
        if(val[posInVal] < '0' || nevVal < downkeep)
        {
            val[posInVal] = '9';
            nevVal = atof(val);
            while(nevVal < downkeep)
            {
              val[posInVal]--;
              nevVal = atof(val);
            }
        }
      }
      value = nevVal;
    currentMode->show();
  }
  
  void cChangeValue::rKeyAction(void)
  {
    if(exitPos)
    {
      curPos = formatF + formatI - ((formatF && downkeep < 0)?0:1);
      strcpy((char*)enterButtonText, "++");
      exitPos = false;
    }
    else if(curPos > 0)
      curPos--;
    else
    {
      exitPos = true;
      strcpy((char*)enterButtonText, "save");
      
    }
    currentMode->show();
  }
  
  void cChangeValue::lKeyAction(void)
  {
    if(exitPos)
    {
      curPos = 0;
      strcpy((char*)enterButtonText, "++");
      exitPos = false;
    }
    else if(curPos < formatF + formatI - ((formatF && downkeep < 0)?0:1))
      curPos++;
    else
    {
      exitPos = true;
      strcpy((char*)enterButtonText, "save");
    }
    currentMode->show();
  }
  
  void cChangeValue::onKeyAction(void)
  {
        currentMode->showMessage(NULL);
        currentMode->show();
  }
  
#ifdef FILESYSTEM
  
    cOpenFileDialog::cOpenFileDialog(char* ext)
   {
     memcpy(this->ext, ext, FILE_EXT_SZ);
     strcpy((char*)leftButtonText,"up");
     strcpy((char*)rightButtonText, "down");
     strcpy((char*)enterButtonText, "open");
     strcpy((char*)onButtonText, "cancel");
     
     InterfaceColor.red = 222;
     InterfaceColor.green = 0;
     InterfaceColor.blue = 0;
     FontColor.red = 0;
     FontColor.green = 222;
     FontColor.blue = 0;
     
     rKeyState = true;
     lKeyState = true;
     eKeyState = true;
     oKeyState = true;
     
     clearScreen = true;
   }


  void cOpenFileDialog::start(void)
  {
    HFILE file;
    numOfFiles = filesystem_calc_files_number(&filesystem, ext);
    for(uint16_t i =  0; i < min(12, numOfFiles); i++)
    {
      file = filesystem_open_num(&filesystem, ext, i);
      if(file == NULL)
        break;
      else
        memcpy(&curFiles[i], &filesystem.fileRecordsOnSector[filesystem_get_first_filerecordIndex(&filesystem, file)], sizeof(tagFileRecord));
    }
    scrPose = 0;
    curCursorPose = 0;
    openedFile = NULL;
  }
  
  void cOpenFileDialog::onShow(void)
  {
     cmd( COLOR_RGB(InterfaceColor.red, InterfaceColor.green, InterfaceColor.blue) );
     cmd( BEGIN(RECTS) );
     cmd( LINE_WIDTH(1 * 16) );
     
     cmd( VERTEX2F(1 * 16,(281 - 260 + (curCursorPose) * 20) * 16) );
     cmd( VERTEX2F(239 * 16,(301 - 260 + (curCursorPose) * 20) * 16));
     
     cmd(END());
     
     
     cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue) );
     
     char buff[FILE_NAME_SZ + FILE_EXT_SZ + 1];
     for(uint16_t i = 0; i <= min(numOfFiles - 1, 12); i++)
     {
        //sprintf(buff, "%s.%s", curFiles[i].name, curFiles[i].ext);
        memset(buff, 0, FILE_NAME_SZ + FILE_EXT_SZ + 1);
        strcpy(buff, curFiles[i].name);
        strncat(buff, ".", 1);
        strncat(buff, curFiles[i].ext, FILE_EXT_SZ);
        /*bool stNul = false;
        for(int j = 0; j < FILE_NAME_SZ + FILE_EXT_SZ + 1; j++)
        {
          if(stNul)
            buff[j] = '\0';
          else
          {
            if(buff[j] == '\0')
              stNul = true;
          }
        }*/
        if(i == curCursorPose)
        {
          cmd( COLOR_RGB(0, 0, 0) );
          cmd_text(5, (280 - 260) + ((i) * 20), 27, 0, (uint8_t*)buff);
          cmd( COLOR_RGB(FontColor.red, FontColor.green, FontColor.blue));
        }
        else
          cmd_text(5, (280 -260) + ((i) * 20) + 1, 27, 0, (uint8_t*)buff);
     }
  }
  
  void cOpenFileDialog::enterKeyAction(void)
  {
        openedFile = curCursorPose;
        currentMode->showMessage(NULL);
        currentMode->control();
        currentMode->show();
  }
  
  HFILE cOpenFileDialog::getOpenedFile(void)
  {
    if(openedFile == E_FAIL)
      return NULL;
    else
      return filesystem_open_file(&filesystem, curFiles[openedFile].name, curFiles[openedFile].ext);
  }
  
  void cOpenFileDialog::rKeyAction(void)
  {

     if(curCursorPose < numOfFiles - 1)
        curCursorPose++;
     else
     {
       if(numOfFiles < 12)
       {
        curCursorPose = 0;
        scrPose = 0;
       }
       else
       {
         HFILE file = filesystem_open_num(&filesystem, ext, 12 + scrPose);
         if(file == NULL)
         {
            curCursorPose = 0;
            scrPose = 0;
         }
         else
         {
            memcpy(&curFiles[numOfFiles], &filesystem.fileRecordsOnSector[file + 1], sizeof(tagFileRecord));
            scrPose++;
         }
       }
     }
     currentMode->show();
  }
  
  void cOpenFileDialog::lKeyAction(void)
  {
     HFILE file;
     if(curCursorPose > 0)
        curCursorPose--;
     else
     {
       /*
       if(numOfFiles <= 12)
       {
        for(uint32_t i = 0; 1; i++)
          file = filesystem_open_num(&filesystem, ext, i + 1);
       }
       else
       {
         file = filesystem_open_num(&filesystem, ext, 12 + scrPose);
         if(file == NULL)
         {
            curCursorPose = 0;
            scrPose = 0;
         }
         else
         {
            memcpy(&curFiles[numOfFiles], &filesystem.fileRecordsOnSector[file + 1], sizeof(tagFileRecord));
            scrPose++;
         }
       }
       */
       curCursorPose = min(numOfFiles - 1, 12);
       if(numOfFiles > 12)
       {
         scrPose = numOfFiles - 12;
         for(int i = numOfFiles; i < numOfFiles - 12; i++)
         {
           file = filesystem_open_num(&filesystem, ext, i);
           if(file == NULL)
             break;
           else
             memcpy(&curFiles[numOfFiles - i], &filesystem.fileRecordsOnSector[filesystem_get_first_filerecordIndex(&filesystem, file)], sizeof(tagFileRecord));
         }
       }
     }
     currentMode->show();
  }
  
  void cOpenFileDialog::onKeyAction(void)
  {
        openedFile = E_FAIL;
        currentMode->showMessage(NULL);
        currentMode->control();
        currentMode->show();
  }
  
#endif //FILESYSTEM
  
}
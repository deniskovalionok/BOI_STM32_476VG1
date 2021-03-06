#ifndef _MODE_PROTOTYPE_H
#define _MODE_PROTOTYPE_H

#include <stdint.h>
#include "ft8xx.h"

#ifdef __cplusplus

#ifdef FILESYSTEM
extern "C"
{
  
#include "filesystem.h"
  
}
#endif //#ifdef FILESYSTEM

namespace MODES
{
  
#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
  
  // RGB �������
  struct tagRGBColor{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };
  
  struct tagMenu{
    uint8_t name[21];
    void (*func)(void);
    bool isShow;
  };
  
  enum tagKey {
    R_KEY,
    L_KEY,
    O_KEY,
    E_KEY,
  };
  
  enum tagKeyMod{
    UNLOCK_KEY = 0,
    LOCK_KEY,
  };
  
  class cMode_prototype;
  
  class cMessage_portotype
  {
  public:
  protected:
    virtual void onShow(void) = 0;
    virtual void userControl(void) = 0;
    
    virtual void start(void) = 0;
    
    virtual void enterKeyAction(void) = 0;
    virtual void rKeyAction(void) = 0;
    virtual void lKeyAction(void) = 0;
    virtual void onKeyAction(void) = 0;
    
    uint8_t leftButtonText[7];
    uint8_t rightButtonText[7];
    uint8_t enterButtonText[7];
    uint8_t onButtonText[7];
    
    bool rKeyState;
    bool lKeyState;
    bool eKeyState;
    bool oKeyState;
    
    bool clearScreen;
    
    friend cMode_prototype;
    
    // ���� ����� �������� ��������� ���������
    struct tagRGBColor InterfaceColor;
    
    // ���� ������
    struct tagRGBColor FontColor;
  };
  
  class cChangeValue : public cMessage_portotype
  {
  public:
    explicit  cChangeValue(char* valueName, float* value, uint16_t formatI, uint16_t formatF, float upkeep, float downkeep);
    explicit  cChangeValue(char* valueName, uint32_t* value, uint16_t formatI, float upkeep, float downkeep);
    
    
  protected:
    
    void start(void);
    
    void onShow(void);
    void userControl(void);
    
    uint8_t valueName[20];
    
    char val[22];
    
    
    char fromBuff[10];
    char toBuff[10];
    float* valueF;
    uint32_t* valueUI;
    
    float value;
    
    uint16_t formatI;
    uint16_t formatF;
    
    uint16_t curPos;
    
    float upkeep;
    float downkeep;
    
    bool exitPos;
    
    void enterKeyAction(void);
    void rKeyAction(void);
    void lKeyAction(void);
    void onKeyAction(void);
  };
  
  class cMode_prototype
  {
  public:
    //����� ������� ����� �������� � �����. ������������ ������� ������. ��������� ������ �����. 
    //��� ��������� ������� ������������ userControl
    void control(void);
    
    // ���� ����� ��� ����� �������� ��������� ����.
    void change(cMode_prototype *newMode);
    
    //���� ����� ���� �������� ������ ��� ����� ����� ������������ �����.
    void show(void);
    
    // ���������� ����
    void openMenu(tagMenu* menu,//������ �� ������ �������� �� ��������� ����
                                 uint16_t len);//��� ������
    
    void menuChangeValue(uint32_t* value, uint16_t pose, uint16_t format, uint32_t upTreshhold);
    
    void menuChangeValue(float value, uint16_t pose, uint16_t formatInt, uint16_t formatFloat, uint32_t upTreshhold);
    
    void showMessage(cMessage_portotype* message);
  protected:
    
    cMessage_portotype* messageClass;
    
    //���������� ����
  //������������� ����������� � show ���� ���� �������
    virtual void showMenu(void);
    
    void keyChangeMod(tagKey key, tagKeyMod mod);
    
    //����� mode controll ������������ � ������� �������. ����� ��������� � �����
    virtual void userControl(void) = 0;
    
    // ��� ����� ����������/�������� �������� ����
    //!!! ��������� ������ �������� ����� ������ ��� ������ ����!!!
    void menuShowHideLine(uint16_t numOfLine,//����� ������ � �������
                                         bool show);//��������/������
    
    virtual void menuAction(void);
    
    // ����� show ������������ � �������� �������. ���� ������ ���� ����������, ����� ����.
    //------------------------!!!�� ��������� ��� ����� ������� InterfaceColor !!!--------
    virtual void onShow(void) = 0;
    
    uint8_t modeName[20];
    
    //����� ������
    uint8_t leftButtonText[7];
    uint8_t rightButtonText[7];
    uint8_t enterButtonText[7];
    uint8_t onButtonText[7];
    
    // �������� ��� ������� ������
    virtual void enterKeyAction(void) = 0;
    virtual void rKeyAction(void) = 0;
    virtual void lKeyAction(void) = 0;
    virtual void onKeyAction(void) = 0;
    
    bool rKeyState;
    bool lKeyState;
    bool eKeyState;
    bool oKeyState;
    
    //������ �� ������ ����� ����
    tagMenu* menu;
    // ��� ������
    uint16_t menuLen;
    
    //���������� ������������ �����
    uint16_t menuShownlen;
    
    //��������� ������ � ����
    uint16_t currentMenuLine;
    
    //����� ���������� ������ � �������
    uint16_t currentMenuInArr;
    
    //������ ������� ���� ������������ �� ������
    uint16_t screenPose;
    
    //������� �� ������ ����
    bool menuOpen;
    
    // ���� ����� �������� ��������� ���������
    struct tagRGBColor InterfaceColor;
    
    // ���� ������
    struct tagRGBColor FontColor;
  };
  
#ifdef FILESYSTEM
  
  
  class cOpenFileDialog : public cMessage_portotype
  {
  public:
    explicit cOpenFileDialog(char* ext);
    
    HFILE getOpenedFile(void);
    
    void start(void);
  private:
    tagFileRecord curFiles[12];
    uint16_t numOfFiles;
    uint16_t scrPose;
    uint16_t curCursorPose;
    
    int openedFile;
    
    char ext[FILE_EXT_SZ];
    
    void onShow(void);
    void userControl(void);
    
    void enterKeyAction(void);
    void rKeyAction(void);
    void lKeyAction(void);
    void onKeyAction(void);
  };
#endif //FILESYSTEM
  
  extern cMode_prototype* currentMode; 
  
  extern cMode_prototype* modeKarusel[4]; 
}
#endif  //#ifdef __cplusplus
#endif	//#ifndef _MODE_PROTOTYPE_H
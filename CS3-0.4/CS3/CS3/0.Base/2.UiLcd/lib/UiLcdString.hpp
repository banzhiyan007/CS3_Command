#ifndef UI_STRING_HPP
#define UI_STRING_HPP

class UiLcdString
{
public:
	char BATTERY_LOW[16+1];
	char POWER_OFF[16+1];
	char NO_FN_FILE[16+1];
	char USB_MODE[16+1];
protected:
	virtual void language_english(void)
	{
		strcpy(BATTERY_LOW,"BATTERY LOW");
		strcpy(POWER_OFF,"POWER OFF");
		strcpy(NO_FN_FILE,"NO FN FILE");
	}
	virtual void language_zhcn(void)
	{
		strcpy(BATTERY_LOW,"������,����");
		strcpy(POWER_OFF,"���ڹػ�...");
		strcpy(NO_FN_FILE,"û�п���ļ�");
	}
public:
    UiLcdString(){update_language();}
	void update_language(void)
	{
		int langID=reg_get_u32("LANGUAGE",0);
		if(langID==1)language_zhcn();
		else language_english();
	}
};

#endif


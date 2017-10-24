#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H

#include <list>
#include <Block.hpp>
#include <Font.hpp>

class FontManager
{
public:
    list<Font*> FontList;
public:
    FontManager()
	{
		add_fonts("/disk_sys",16);
	}
    Font *select_font(const char *font_name)
	{
		list<Font*>::iterator item;
		for(item=FontList.begin();item!=FontList.end();++item)
		{
			Font *font=*item;
			if(strcasecmp(font->Name,font_name)==0)
			{
				return font;
			}
		}
		return NULL;
	}
    Font *select_font_by_id(uint16_t id)
	{
		list<Font*>::iterator item;
		for(item=FontList.begin();item!=FontList.end();++item)
		{
			Font *font=*item;
			if(font->ID==id)
			{
				return font;
			}
		}
		return NULL;
	}
    void remove_font(const char *font_name)
	{
		list<Font*>::iterator item;
		for(item=FontList.begin();item!=FontList.end();++item)
		{
			Font *font=*item;
			if(strcasecmp(font->Name,font_name)==0)
			{
				FontList.remove(font);
				delete font;
				return;
			}
		}
	}
	int strcasecmp(const char *s1, const char *s2)
	{
		#define __tolower(c)    ((('A' <= (c))&&((c) <= 'Z')) ? ((c) - 'A' + 'a') : (c))
		const unsigned char *p1 = (const unsigned char *) s1;
		const unsigned char *p2 = (const unsigned char *) s2;
		int result = 0;

		if (p1 == p2)
		{
			return 0;
		}

		while ((result = __tolower(*p1) - __tolower(*p2)) == 0)
		{
			if (*p1++ == '\0')
			{
				break;
			}
			p2++;
		}

		return result;
	}
	void add_fonts(const char *font_dir,int max_font_count=0xFFFF)
	{
		int n=0;
		DIR *dp;
		struct dirent *dirp;
		dp = opendir(font_dir);
		if(dp==NULL)return;
		while((dirp = readdir(dp)) != NULL)
		{
			char fullname[64];
			sprintf(fullname,"%s/%s",font_dir,dirp->d_name);
			if(strcasecmp(&fullname[strlen(fullname)-5],".ffnt")==0)
			{
				if(n<max_font_count)
				{
					Font *font = new FontFFNT(fullname);
					if(!font->Valid){delete font;}
					else
					{
						remove_font(font->Name);
						n++;
						FontList.push_back(font);
						//printf("font %s added %s\r\n",font->Name,fullname);
					}
				}
			}
		}
		closedir(dp);
	}
};

#endif


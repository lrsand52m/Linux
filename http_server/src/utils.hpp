#pragma once
#include<iostream>
#include<stdio.h>
#include<unistd.h>
#include<string>
#include<sstream>
#include<vector>
#include<time.h>
#include<stdlib.h>
#include<unordered_map>
using std::string;
using std::stringstream;
using std::vector;
using std::unordered_map;
std::unordered_map<std::string,std::string> ERR_DES = 
{
	{"200","Ok"},
	{"304","Not Modified"},
	{"400","Bad Request "},
	{"403","Forbidden"},
	{"404","Not Found"},
	{"405","Method Not Allowed"},
	{"500","Server Error"}
};
class Utils
{
public:
	static int Split(std::string &src,const std::string& seg,std::vector<std::string>&list)
	{
		int num = 0;
		int idx = 0;
		size_t pos;
		//key: value\r\nkey: value\r\n...
		while(idx<src.length())
		{
			pos = src.find(seg,idx);
			if(pos == std::string::npos)
			{
				break;
			}
			num++;
			idx = pos+seg.length();
			list.push_back(src.substr(idx,pos-idx));
		}
		if(idx<src.length())
		{
			list.push_back(src.substr(idx));
			num++;
		}
		return num;
	}
	static std::string GetErrDes(std::string& code)
	{
        auto it = ERR_DES.find(code);
		if(it == ERR_DES.end())
		{
			return "Unkown Error!";
		}
		return ERR_DES[code];
	}
	static void TimeToGMT(time_t t,std::string &time)
	{
		struct tm* mt = gmtime(&t);
		char tmp[256] = {0};
		int len = strftime(tmp,255,"%a, %d %b %Y %H:%M:%S GMT",mt);
		time.assign(tmp,len);
	}
	static void dtos(int64_t num,std::string& str)
	{
		std::stringstream ss;
		ss<<num;
		str = ss.str();
	}
	static int64_t stod(std::string& str)
	{
		int64_t ret;
		std::stringstream ss;
		ss<<str;
		ss>>ret;
		return ret;
	}
	static void MakeEtag(int64_t size,int64_t inode,int64_t mtime,std::string & etag)
	{
		std::stringstream ss;
		ss<<"\"";
		ss<<std::hex<<inode;
		ss<<"-";
		ss<<std::hex<<size;
		ss<<"-";
		ss<<std::hex<<mtime;
		ss<<"-";
		etag = ss.str();
	}
    //提供各类工具接口
};

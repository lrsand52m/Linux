#pragma once 
#include"threadpool.hpp"
#include"utils.hpp"
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/socket.h>
class RequestInfo
{
    //包含已经解析的请求信息
public:
    std::string _method;//请求方法
    std::string _version;//版本信息
    std::string _path_info;//资源路径
    std::string _path_phys;//请求资源的实际路径
    std::string _query_string;//查询字符串
    std::unordered_map<std::string,std::string> _hdr_list;//头部信息键值对
    struct stat _st;
	std::string _err_code;
	bool RequestIsCGI()
	{
		if((_method == "GET"&&!_query_string.empty())||_method == "POST")
			return true;
		return false;
	}
};
#define WWWROOT "wwwroot"
class HttpRequest
{
    //http数据的接收与解析
    //对未提供获取处理结果的接口
private:
    int _cli_sock;
    std::string _http_header;
    RequestInfo _req_info;

public:
    HttpRequest(int sock = -1):_cli_sock(sock){}
    bool RecvHttpHeader(RequestInfo& info)//接收http请求头
    {
        return true;
    }
	bool PathIsLegal(std::string& path,RequestInfo& info)
	{
		std::string file = WWWROOT + path;
		if(stat(file.c_str(),&info._st)<0)
		{
			info._err_code = "404";
			return false;
		}
		char tmp[256] = {0};
		realpath(file.c_str(),tmp);
		info._path_phys = tmp;
		if(info._path_phys.find(WWWROOT) == std::string::npos)
		{
			info._err_code = "403";
			return false;
		}
		return true;
	}
	bool ParseFirstLine(std::string& line, RequestInfo info)
	{
		std::vector<std::string> line_list;
		if(Utils::Split(line ," ",line_list)!=3)
		{
			info._err_code = "400";
			return false;
		}
		std::string url;
		info._method = line_list[0];
		url = line_list[1];
		info._version = line_list[2];
		if(info._method!="GET"&&info._method!="POST"&&info._method!="HEAD")
		{
			info._err_code = "405";
			return false;
		}
		if(info._version!="HTTP/0.9"&&info._version!="HTTP/1.0"&&info._version!="HTTP/1.1")
		{
			info._err_code = "400";
			return false;
		}
		size_t pos;
		pos = url.find("?");
		if(pos == std::string::npos)
		{
			info._path_info = url;
		}
		else
		{
			info._path_info = url.substr(0,pos);
			info._query_string = url.substr(pos+1);
		}
		
		return PathIsLegal(info._path_info,info);
	}
    bool ParseHttpHeader(RequestInfo &info)//解析请求头
	{
		std::vector<std::string> hdr_list;
		Utils::Split(_http_header,"\r\n",hdr_list);
		if(!ParseFirstLine(hdr_list[0],info))
		{
			return false;
		}
		hdr_list.erase(hdr_list.begin());
		for(auto &e:hdr_list)
		{
			size_t pos = e.find(": ");
			info._hdr_list[e.substr(0,pos)] = e.substr(pos+2);
		}
		return true;
	}
    RequestInfo&  GetReqInfo();//提供解析结果
};
class HttpResponse
{
    //提供各类请求的相应接口
    //文件请求（下载，列表）接口
    //CGI请求接口
private:
    int _cli_sock;
	//ETag: "inode-fsize-mtime"\r\n
    std::string _etag;//请求文件是否是源文件，是否修改过
    std::string _mtime;//最后一次修改时间
	std::string _date;//系统的响应时间
    std::string _cont_len;
public:
    HttpResponse(int sock = -1):_cli_sock(sock){}
    bool Init(RequestInfo &info)//初始化请求的响应信息
	{
		int64_t size = info._st.st_size;
		int64_t inode = info._st.st_ino;
		int64_t mtime = info._st.st_mtime;
		Utils::dtos(mtime,_mtime);
		Utils::MakeEtag(size, inode, mtime,_etag);
		time_t t = time(NULL);
		Utils::TimeToGMT(t,_date);
		return true;
	}
	bool SendData(const std::string& buf)
	{
		if(send(_cli_sock,buf.c_str(),buf.length(),0)<0)
		{
			return false;
		}
		return true;
	}
	bool SendCData(std::string& buf)
	{
		if(buf.empty())
		{
			SendData("0\r\n\r\n");
		}
		std::stringstream ss;
		ss<<std::hex<<buf.length();
		ss<<"\r\n";
		SendData(ss.str());
		ss.clear();
		SendData(buf);
		SendData("\r\n");
		return true;
	}
    bool ProcessFile(RequestInfo& info)//下载
	{
		return true;
	}
    bool ProcessList(RequestInfo& info)//文件列表
	{
		std::string rsp_header;
		rsp_header = info._version+" "+"200 Ok\r\n";
		rsp_header += "Cotent-Type: text/html\r\n";
		rsp_header += "Connection: close\r\n";
		if(info._version == "HTTP/1.1")
		{
			rsp_header += "Transfer-Encodng: chunked\r\n";
		}
		rsp_header+= "ETag: "+_etag+"\r\n";
		rsp_header+= "Last-Modified: "+_mtime+"\r\n";
		rsp_header+= "Data: "+_date+"\r\n";
		SendData(rsp_header);
		std::string rsp_body;
		rsp_body = "<html><head>";
		rsp_body+="<title>"+info._path_info+"</title>";
		rsp_body+="<meta charset='UTF-8'>";
		rsp_body+="</head><body>";
		rsp_body+="<h1>"+info._path_info+"</h1><hr />";
		
		while(1)
		{
			std::string file;
			SendCData(file);
		}
		rsp_body = "</body></html>";
		SendCData(rsp_body);
		SendData("0\r\n\r\n");
		return true;
	}
    bool ProcessCGI(RequestInfo& info)//处理CGI请求
	{
		return true;
	}
	bool FileHandler(RequestInfo& info)
	{
		return true;
	}
	bool CGIHandler(RequestInfo& info)
	{
		return true;
	}
	bool FileIsDir(RequestInfo& info)
	{
		return info._st.st_mode&S_IFDIR;
	}
	bool ErrHandler(RequestInfo& info)
	{
		std::string rsp_header;
		rsp_header = info._version+" "+info._err_code+" ";
		rsp_header += Utils::GetErrDes(info._err_code)+"\r\n";
		
		time_t t = time(NULL);
		std::string gmt ;
		Utils::TimeToGMT(t,gmt);
		rsp_header+="Date: "+gmt+"\r\n";
		std::string rsp_body;
		rsp_body = "<html><body><h1>"+info._err_code+"<h1></body></html>";
		std::string cont_len;
		Utils::dtos(rsp_body.length(),cont_len);
		rsp_header+="Content-Length: "+cont_len+"\r\n\r\n";
		send(_cli_sock,rsp_header.c_str(),rsp_header.length(),0);
		send(_cli_sock,rsp_body.c_str(),rsp_body.length(),0);
		return true;
	}
};
class Upload
{
    //CGI外部程序中的文件上传功能处理接口
};

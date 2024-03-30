#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <map>
#include <sys/types.h>
#include <unistd.h>
#include <gtkmm.h>
#include "Utility.h"

namespace utility
{
    std::vector<std::string> strSplit(const std::string& s, const std::string& delimeter)
    {
        std::vector<std::string> token;
        int startIndex = 0;
        int endIndex = 0;

        while((endIndex = s.find(delimeter, startIndex)) < (int)s.size())
        {
            if(endIndex == -1)
                break;

            std::string val = s.substr(startIndex, endIndex - startIndex);
            token.push_back(val);
            startIndex = endIndex + delimeter.size();
        }
        if(startIndex < (int)s.size())
        {
            std::string val = s.substr(startIndex);
            token.push_back(val);
        }
        return token;
    }
    
    std::vector<std::string> strSplit(const std::string& s, const char delimiter)
    {
       std::vector<std::string> tokens;
       std::string token;
       std::istringstream tokenStream(s);
       while (std::getline(tokenStream, token, delimiter))
       {
          tokens.push_back(token);
       }
       return tokens;
    }

    std::map<std::string, std::string> deserializeKeyValue(const std::string &sz, const std::string elementSeparator, const std::string lineSeparator) 
    {
        std::map<std::string, std::string> result;
        std::size_t begin{0};
        std::size_t end{0};
        while (begin < sz.size()) 
        {
            // Search key
            end = sz.find(elementSeparator, begin);
            auto key = utility::strTrim(sz.substr(begin, end - begin));
            begin = end + elementSeparator.size();

            // Seach value
            end = sz.find(lineSeparator, begin);
            auto value = utility::strTrim(sz.substr(begin, end == std::string::npos ? std::string::npos : end - begin));
            begin = (end == std::string::npos) ? sz.size() : end + lineSeparator.size();

            // Store key-value
            result.emplace(std::move(key), std::move(value));
        }
        return result;
    }
    
    void strToUpper(std::string& input)
    {
        std::for_each(std::begin(input), std::end(input), [](char& c) 
        {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        });
    }

    void strToLower(std::string& input)
    {
        std::for_each(std::begin(input), std::end(input), [](char& c) 
        {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        });
    }

    std::string strToLower(const std::string& input)
    {
        std::string s = input;
        std::for_each(std::begin(s), std::end(s), [](char& c) 
        {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        });
        return s;
    }
    std::string strTrim(const std::string &s)
    {
        auto start = s.begin();
        while (start != s.end() && std::isspace(*start)) {
            start++;
        }

        auto end = s.end();
        do 
        {
            end--;
        } while (std::distance(start, end) > 0 && std::isspace(*end));

        return std::string(start, end + 1);
    }
    
    std::string strReplace(std::string subject, const std::string& search, const std::string& replace) 
    {
        size_t pos = 0;
        while((pos = subject.find(search, pos)) != std::string::npos) 
        {
             subject.replace(pos, search.length(), replace);
             pos += replace.length();
        }
        return subject;
    }
    
    void padTo(std::string &str, const size_t num, const char paddingChar)
    {
        if(num > str.size())
            str.insert(str.end(), num - str.size(), paddingChar);
    }
    
    std::string stripCurrencyDecoration(const std::string val)
    {
        std::string s = std::string();
        for (char c : val) 
        {
            if (std::isdigit(c) || c == '.') s += c;
        }
        return s;
    }
    
    bool syscommand(std::string aCommand, std::string& result) 
    {
        FILE * f;
        if ( !(f = popen(aCommand.c_str(), "r" )) ) 
        {
            std::cout << "Can not open file" << std::endl;
            return false;
        }
        const int BUFSIZE = 4096;
        char buf[ BUFSIZE ];
        if (fgets(buf,BUFSIZE,f)!=NULL) 
        {
            result = buf;
        }
        pclose( f );
        return true;
    }
    
    std::string getBundleName (void) 
    {
        pid_t procpid = getpid();
        std::stringstream toCom;
        toCom << "cat /proc/" << procpid << "/comm";
        std::string fRes="";
        syscommand(toCom.str(),fRes);
        size_t last_pos = fRes.find_last_not_of(" \n\r\t") + 1;
        if (last_pos != std::string::npos) 
        {
            fRes.erase(last_pos);
        }
        return fRes;
    }
    
    std::string getBundlePath (void) 
    {
        pid_t procpid = getpid();
        std::string appName = getBundleName();
        std::stringstream command;
        command <<  "readlink /proc/" << procpid << "/exe | sed \"s/\\(\\/" << appName << "\\)$//\"";
        std::string fRes;
        syscommand(command.str(),fRes);
        return strTrim(fRes);
    }
    
    void writeLogFile(std::string logEntry)
    {
        time_t now = time(0);
        char* dt = ctime(&now);
        
        std::stringstream ss;
        ss << utility::getBundlePath() << "/DbStudio.log";
        std::ofstream file;
        file.open (ss.str(), std::ios::app);
        file << dt << logEntry << std::endl;
        file.close();
    }
    
    void doEvents(void)
    {
        while(Gtk::Main::events_pending()) 
            Gtk::Main::iteration(false);
    }
    
    long getExcelDateTime(std::string datetime)
    {
        struct tm tm;
        std::stringstream ss(datetime);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        if (ss.fail())
            std::cout << "Parse failed\n";
        //2209248000 check excel and libre office
        return (2209161600 + mktime(&tm)) / (60 * 60 * 24);
    }
    
    long getExcelDate(std::string date)
    {
        struct tm tm;
        std::stringstream ss;
        ss << date << " 00:00:00";
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        if (ss.fail())
            std::cout << "Parse failed\n";  
                
       return (2209161600 + mktime(&tm)) / (60 * 60 * 24);
    }
    
    std::string escapeXml(const std::string s)
    {
        std::string val = utility::strReplace(s, "\n", " ");
        val = utility::strReplace(val, "\t", " ");
        val = utility::strReplace(val, "&", "&amp;");
        val = utility::strReplace(val, "\"", "&quot;");
        val = utility::strReplace(val, "'", "&apos;");
        val = utility::strReplace(val, "<", "&lt;");
        val = utility::strReplace(val, ">", "&gt;");
        return val;
    }
}

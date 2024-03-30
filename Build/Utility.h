#ifndef UTILITY_H
#define UTILITY_H

namespace utility
{
    std::vector<std::string> strSplit(const std::string& s, const char delimiter);
    std::vector<std::string> strSplit(const std::string& s, const std::string& delimiter);
    std::map<std::string, std::string> deserializeKeyValue(const std::string &sz, const std::string elementSeparator = "=", const std::string lineSeparator = ";");
    void strToUpper(std::string& input);
    void strToLower(std::string& input);
    std::string strToLower(const std::string& input);
    std::string strTrim(const std::string &s);
    std::string strReplace(std::string subject, const std::string& search, const std::string& replace);
    void padTo(std::string &str, const size_t num, const char paddingChar = ' ');
    std::string stripCurrencyDecoration(const std::string val);
    bool syscommand(std::string aCommand, std::string& result);
    std::string getBundleName (void);
    std::string getBundlePath (void);
    void writeLogFile(std::string logEntry);
    void doEvents(void);
    long getExcelDateTime(std::string datetime);
    long getExcelDate(std::string datetime);
    std::string escapeXml(const std::string val);
}
#endif

/*
 *  @landyliu
 */

#ifndef PANDORA_CONFIG_H
#define PANDORA_CONFIG_H

#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName)\
                TypeName(const TypeName&);\
            void operator=(const TypeName&)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <map>
#include <vector>
#include <iterator>
#include <string>

using namespace std;

namespace pandora
{

class Config
{
public:
    Config() {};
    ~Config() {};
    
    //@return false, if couldn't read the config file or found illegal format in file
    bool Load(const string& path);

    //@param default_value: [IN], set default value if not found the key
    string  GetString(const string& section, const string& key, const string& default_value = "");
    int64_t GetInt(const string& section, const string& key, const int64_t default_value = 0);
    float   GetFloat(const string& section, const string& key, const float default_value = 0.0);
    /*  
     * @param return_list: [OUT], store the lists of string tokens.
     * @return int: return the size of return list. 
     */
    int GetStringLists(const string& section, const string& key, 
                       vector<string>& return_list, const string& m_delim = ";");
    /*  
     * @param return_list: [OUT], store the lists of int tokens.
     * @return int: return the size of return list. 
     */
    int GetIntList(const string& section, const string& key, 
                    vector<int>& return_list, const string& m_delim = ";");

    /* 
     * @param create: [IN], if not found the key, create it or not
     * @return bool: return false if not found the key (create is set as false).
     */
    bool SetString(const string& section, const string& key, const string& value, bool create = true);
    bool SetInt(const string& section, const string& key, const int& value, bool create = true);
    bool SetFloat(const string& section, const string& key, const float& value, bool create = true); 

    /*
     * @param keys: [OUT], store the return name of keys in special section.
     * @return int: return the number of keys.
     */     
    int GetKeys(const string& section, vector<string>& keys);
    /*
     * @param sections: [OUT], store the return name of sections.
     * @return int: return the number of sections.
     */     
    int GetSections(vector<string>& sections);

    string ToString();

private:
    inline string IntToStr(const int n);
    inline string FloatToStr(const float f);
    inline string Trim(string& str);

    map<string, map<string, string> > config_items_;

    DISALLOW_COPY_AND_ASSIGN(Config);
};

}//namespace pandora

#endif//PANDORA_CONFIG_H

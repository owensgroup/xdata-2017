#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

int Seperate_String(string str_in, char seperator, vector<string> &strs)
//vector<string> Seperate_String(string str_in, char seperator)
//int Seperate_String(string str_in, char seperator, int item_limit, string *strs)
{
    //vector<string> strs;
    strs.clear();
    int depth = 0;
    string str = "";
    int item_counter = 0;
    for (unsigned int i = 0; i < str_in.length(); i++)
    {
        //cout << int(str_in[i]) << " ";
        if (str_in[i] == 13)
        {}
        else if (str_in[i] == seperator && depth == 0)
        {
            strs.push_back(str);
            //strs[item_counter] = str;
            //cout << item_counter << " " << str << endl;
            str = "";
            item_counter ++;
            //if (item_counter == item_limit) break;
        } else {
            if (str_in[i] == '"')
            {
                depth = 1-depth;
            }
            str = str + str_in[i];
        }
    }
    //cout << item_counter << " " << str << endl;
    strs.push_back(str);
    //if (item_counter < item_limit) strs[item_counter] = str;
    item_counter ++;
    return item_counter;
}

string Remove_NullChars(string str_in, string null_chars)
{
    string str_out="";
    bool valid_found = false;
    int str_length = str_in.length();
    while (str_length != 0)
    {
        if (null_chars.find(str_in[str_length-1]) != string::npos)
        {
            str_in.pop_back();
            str_length --;
        } else break;
    }
 
    for (unsigned int i=0; i<str_in.length(); i++)
    {
        if (null_chars.find(str_in[i]) == string::npos || valid_found)
        {
            str_out = str_out + str_in[i];
            valid_found = true;
        }
    }
    return str_out;
}

int main()
{
    string account_dir = "/data/xdata-2017/grouped/accounts/";
    ifstream file_list;
    //string *file_record = new string[128];
    //string *account_records = new string[128];

    file_list.open("accounts_list.txt");
    while (!file_list.eof())
    {
        string file_str;
        vector<string> file_record;
        getline(file_list, file_str);
        if (file_str.length() == 0) continue;
        if (file_str[0] == '#') continue;
        Seperate_String(file_str, ',', file_record);
        //file_record = Seperate_String(file_str, ',');
        //int num_file_records = Seperate_String(file_str, ',', 128, file_record);
        //for (auto it=file_record.begin(); it != file_record.end(); it++)
        //    cout << *it << " | " ;
        //cout << endl;

        string file_name = file_record[0];
        int preoffset_lines = atoi(file_record[1].c_str());
        int postoffset_lines = atoi(file_record[2].c_str());
        int preoffset_chars = atoi(file_record[3].c_str());
        int postoffset_chars = atoi(file_record[4].c_str());
        string seperator = file_record[5];
        string null_chars = file_record[6];
        map<unsigned int, string> file_columns;
        file_columns.clear();
        for (unsigned int i=7; i+1<file_record.size(); i+=2)
        {
            file_columns[atoi(file_record[i].c_str())] = file_record[i+1];
        }
        cout << file_name << endl;
        for (auto it = file_columns.begin(); it != file_columns.end(); it++)
            cout << it -> first << " -> " << it -> second << endl;
        if (seperator == "\",\"") seperator = ',';
        if (seperator == "\"\"") seperator = '\"';
        if (null_chars == "\"\"") null_chars = "\"";
        cout << "seperator = " << char(seperator[0]) << endl; 

        ifstream accounts_in;
        long long account_counter = 0;
        accounts_in.open((account_dir + file_record[0]).c_str());
        if (!accounts_in.is_open()) continue;
        while (!accounts_in.eof())
        {
            string account_str;
            getline(accounts_in, account_str);
            account_counter ++; 
            if (account_counter <= preoffset_lines) continue;
            //cout << account_str << endl; 
            if (accounts_in.eof()) continue;

            vector<string> account_records;
            Seperate_String(account_str, seperator[0], account_records);
            //account_record = Seperate_String(account_str, seperator[0]);
            //int num_account_records = Seperate_String(account_str, seperator[0], 128, account_records);
            cout << account_str << endl;
            //for (auto it = account_records.begin(); it != account_records.end(); it++)
            //{
            //    *it = Remove_NullChars(*it, null_chars);
            //    cout << *it << " | ";
            //}
            //for (int i=0; i<num_account_records; i++)
            //    cout << account_records[i] << " | ";
            for (auto it = file_columns.begin(); it != file_columns.end(); it++)
            {
                if ( it -> first >= account_records.size()) continue;
                string column = it -> second;
                string content = account_records[it -> first];
                content = Remove_NullChars(content, null_chars);
                if (column == "name" || column == "address" || column == "email"
                    || column == "username" || column == "pwd"
                    || column == "pwd_SHA1" || column == "pwd_MD5"
                    || column == "pwd_hint" || column == "city")
                {
                    for (unsigned int i=0; i<content.size(); i++)
                        content[i] = tolower(content[i]);
                }
                if (content != "") cout << column << " = " << content << " | ";
            }
            cout << endl;
            if (account_counter > 100) break;
        }
        accounts_in.close();
        //break;
    }
    file_list.close();
    return 0;
}


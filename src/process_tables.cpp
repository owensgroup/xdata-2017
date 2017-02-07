#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <omp.h>
#include <algorithm>

using namespace std;

struct Str2Cell_Pointer
{
    string str;
    int table_index;
    int cell_index;

    Str2Cell_Pointer()
    :   str (""),
        table_index (0),
        cell_index  (0)
    {
    }

    Str2Cell_Pointer(string str, int table_index, int cell_index)
    :   str(str),
        table_index(table_index),
        cell_index (cell_index)
    {
    }
};

bool operator< (const Str2Cell_Pointer lhs, const Str2Cell_Pointer rhs)
{
    return lhs.str < rhs.str;
}

struct Result_Item
{
    int table_index;
    int cell_index;

    Result_Item()
    :   table_index (0),
        cell_index (0)
    {
    }

    Result_Item(int table_index, int cell_index)
    :   table_index (table_index),
        cell_index (cell_index)
    {
    }

    bool operator< (const Result_Item rhs)
    {
        if (table_index < rhs.table_index) return true;
        if (table_index > rhs.table_index) return false;
        if (cell_index < rhs.cell_index) return true;
        return false;
    }
};

bool operator< (const Result_Item lhs, const Result_Item rhs)
{
    if (lhs.table_index < rhs.table_index) return true;
    if (lhs.table_index > rhs.table_index) return false;
    if (lhs.cell_index < rhs.cell_index) return true;
    return false;
}


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

void V2Str(vector<string> v, string &str)
{
    str = "";
    for (auto it = v.begin(); it != v.end(); it++)
        str += (it == v.begin() ? "" : ",") + (*it); 
}

int main()
{
    string table_dir = "/data/xdata-2017/grouped/tables/";
    ifstream table_list_in;
    vector<string> table_list;

    table_list_in.open("tables_list.txt");
    while (!table_list_in.eof())
    {
        string table_str;
        getline(table_list_in, table_str);
        if (table_list_in.eof()) continue;
        if (table_str == "") continue;
        if (table_str[0] == '#') continue;
        table_list.push_back(table_str);
    }
    table_list_in.close();
    int num_tables = table_list.size();

    vector<vector<string> > *tables = new vector<vector<string> >[num_tables];
    int table_count = 0;
    long long row_count = 0;
    #pragma omp parallel for schedule (dynamic, 1)
    for (int k=0; k < num_tables; k++)
    {
        vector<vector<string> > &table = tables[k];
        //printf("Reading %s\n", table_list[k].c_str());
        ifstream table_in;
        table_in.open((table_dir + table_list[k]).c_str());
        if (!table_in.is_open()) continue;
        
        while (!table_in.eof())
        {
            string account_str;
            getline(table_in, account_str);
            if (table_in.eof()) continue;
            vector<string> account_records;
            Seperate_String(account_str, '\t', account_records);
            table.push_back(account_records);
        }
        table_in.close();
        
        vector<string> &column_header = table[0];
        string header;
        V2Str(column_header, header);
        #pragma omp atomic
            table_count ++;
        #pragma omp atomic
            row_count += (table.size() -1);
        printf("%d : %s -> %lld rows of {%s}\n", 
            table_count, table_list[k].c_str(), 
            (long long)table.size() -1, header.c_str());
    }
   
    map<string, int> column_names;
    vector<string>   column_headers;
    map<string, int> *column_maps = new map<string, int>[num_tables];
    int column_count = 0; 
    for (int k=0; k < num_tables; k++)
    {
        //printf("%s\n", table_list[k].c_str()); fflush(stdout);
        if (tables[k].size() == 0) continue;
        vector<string> &table_header = tables[k][0];
        //string header;
        //V2Str(table_header, header);
        //printf("%s\n", header.c_str()); fflush(stdout);
        int i=0;
        for (auto it = table_header.begin(); it != table_header.end(); it++)
        {
            string column_name = *it;
            auto column_index = column_names.find(column_name);
            if (column_index == column_names.end())
            {
                column_names[column_name] = column_count;
                column_count ++;
                column_index = column_names.find(column_name);
                column_headers.push_back(column_name);
            }
            column_maps[k][column_name] = i;
            i++;
        }
    }
    cout << "Toal : " << num_tables << " tables, " << row_count << " rows, " 
         << column_count << " columns." << endl;

    vector<Str2Cell_Pointer> *cell_pointers = new vector<Str2Cell_Pointer>[column_count]; 
    #pragma omp parallel for schedule (dynamic, 1)
    //for (auto it = column_names.begin(); it != column_names.end(); it++)
    for (int column_index = 0; column_index < column_count; column_index++)
    {
        //string column_name = it -> first;
        //int column_index = it -> second;
        string column_name = column_headers[column_index];
        vector<Str2Cell_Pointer> &cell_pointers_ = cell_pointers[column_index];
        for (int table_index = 0; table_index<num_tables; table_index++)
        {
            auto column_it = column_maps[table_index].find(column_name);
            if (column_it == column_maps[table_index].end()) continue;
            unsigned int j = column_it -> second;
            auto &table = tables[table_index];
            //printf("%s[%d] -> %s\n", table_list[table_index].c_str(), j, column_name.c_str());
            //fflush(stdout);
            for (unsigned int cell_index=1; cell_index<table.size(); cell_index++)
            {
                vector<string> &row = table[cell_index];
                if (j >= row.size())
                {
                    //string row_str;
                    //V2Str(row, row_str);
                    //printf("%s[%d] : %s\n", table_list[table_index].c_str(), 
                    //    j, row_str.c_str());
                    continue;
                }
                string cell_str = row[j];
                if (cell_str == "") continue;
                cell_pointers_.push_back(Str2Cell_Pointer(cell_str, table_index, cell_index));
            }
        }

        sort(cell_pointers_.begin(), cell_pointers_.end(), 
            [](Str2Cell_Pointer &a, Str2Cell_Pointer &b) {
                return a.str < b.str;
            });

        int unique_cell_count = 0;
        string curr_str = "";
        for (auto cell_it = cell_pointers_.begin(); cell_it != cell_pointers_.end();
            cell_it ++)
        {
            string& str = (*cell_it).str;
            if (curr_str != str)
            {
                curr_str = str;
                unique_cell_count ++;
            }
        }
        printf("column %s : %lld non-empty, %d unique.\n",
            column_name.c_str(), (long long)cell_pointers_.size(), unique_cell_count);
        fflush(stdout);
    }

    bool to_continue = true;
    set<Result_Item> results;
    while (to_continue)
    {
        cout << "? ";
        string command_str;
        vector<string> command_tokens;
        getline(cin, command_str);
        Seperate_String(command_str, ' ', command_tokens);
        if (command_tokens.size() == 0) continue;
        
        if (command_tokens[0] == "quit")
        {
            to_continue = false;
            continue;
        }

        if (command_tokens[0] == "help")
        {
            cout << "quit : quit the program" << endl
                 << "help : print this help" << endl
                 << "show : show current results" << endl
                 << "clear : clear the results" << endl
                 << "and column string : filter current results with those 'column' contains 'string'" << endl
                 << "or column string : include those 'column' contains 'string' to current results" << endl
                 << "exclude column string : remove those 'column' contains 'string'" << endl
                 << "advance column : expand current results with those with equal content in 'column', 'column' default to {all} - {race, gender, timezone, state, city}" << endl
                 << "explor column : similar to advance, but show the linkage instead of changing the results" << endl
                 << "remove result_id|result_id_range [result_id|result_id_range...] : remove specific results" << endl;
            continue; 
        }

        if (command_tokens[0] == "clear")
        {
            results.clear();
            continue;
        }

        if (command_tokens[0] == "show")
        {
            int curr_table = -1;
            int table_count = 0;
            int result_count = 0;
            string str;
            for (auto it = results.begin(); it != results.end(); it ++)
            {
                int table_index = (*it). table_index;
                int cell_index = (*it).cell_index;
                if (curr_table != table_index)
                {
                    cout << "\t" << table_list[table_index] << endl;
                    V2Str(tables[table_index][0], str);
                    cout << "\t" << str << endl;
                    curr_table = table_index;
                    table_count ++;
                }
                V2Str(tables[table_index][cell_index], str);
                cout << result_count << " : " << str << endl;
                result_count ++;
            }
            cout << "\t" << result_count << " results in " 
                << table_count << " tables." << endl;
            continue;
        }

        if (command_tokens[0] == "or")
        {
            if (command_tokens.size() < 3)
            {
                cout << "usage : or column string" << endl;
                continue;
            }
            vector<string> columns, strs;
            Seperate_String(command_tokens[1], '|', columns);
            Seperate_String(command_tokens[2], '|', strs);
            
            for (auto column_it = columns.begin(); column_it != columns.end(); column_it++)
            {
                string column_name = *column_it;
                auto column_it_ = column_names.find(column_name);
                if (column_it_ == column_names.end())
                {
                    cout << "[Warning] column " << column_name << " does not exist." << endl;
                    continue;
                }
                int column_index = column_it_ -> second;
                auto &cell_pointers_ = cell_pointers[column_index];
                #pragma omp parallel for schedule (dynamic)
                for (unsigned int i=0; i<cell_pointers_.size(); i++)
                {
                    bool to_include = false;
                    auto &cell_ptr = cell_pointers_[i];
                    for (auto str_it = strs.begin(); str_it != strs.end(); str_it ++)
                    {
                        if (cell_ptr.str.find(*str_it) != string::npos)
                        {
                            to_include = true;
                            break;
                        }
                    }

                    if (to_include)
                    {
                        #pragma omp critical (result_look)
                        {
                            results.insert(Result_Item(
                                cell_ptr.table_index, cell_ptr.cell_index));
                        }
                    } 
                }
            }
            continue;
        }

        if (command_tokens[0] == "and" || command_tokens[0] == "exclude")
        {
            if (command_tokens.size() < 3)
            {
                cout << "usage : and column string" << endl
                     << "        exclude column string" << endl;
                continue;
            }
            vector<string> columns, strs;
            Seperate_String(command_tokens[1], '|', columns);
            Seperate_String(command_tokens[2], '|', strs);
 
            auto result_it = results.begin();
            int curr_table_index = -1;
            vector<int> header_marker;
            while (result_it != results.end())
            {
                int table_index = result_it -> table_index;
                int cell_index = result_it -> cell_index;
                
                if (table_index != curr_table_index)
                {
                    header_marker.clear();
                    vector<string> &header = tables[table_index][0];
                    for (auto header_it = header.begin(); header_it != header.end(); header_it ++)
                    {
                        bool match_found = false;
                        for (auto column_it = columns.begin(); column_it != columns.end(); column_it ++)
                        {
                            if (*header_it == *column_it)
                            {
                                match_found = true;
                                break;
                            }
                        }
                        header_marker.push_back(match_found ? 1 : 0);
                    }
                    curr_table_index = table_index;
                }

                bool match_found = false;
                vector<string> &row = tables[table_index][cell_index];
                for (unsigned int i = 0; i<header_marker.size(); i++)
                {
                    if (header_marker[i] == 0) continue;
                    if (i >= row.size()) continue;
                    string str = row[i];

                    for (auto strs_it = strs.begin(); strs_it != strs.end(); strs_it ++)
                    {
                        if (str.find(*strs_it) == string::npos) continue;
                        match_found = true;
                        break;
                    }
                    if (match_found) break;
                }

                if ((command_tokens[0] == "and" && (!match_found)) 
                    || (command_tokens[0] == "exclude" && (match_found)))
                {
                    result_it = results.erase(result_it);
                } else result_it ++;
            } 
            continue;
        }

        if (command_tokens[0] == "explore" || command_tokens[0] == "advance")
        {
            if (results.size() == 0) continue;

            vector<string> columns;
            if (command_tokens.size() >= 2 and command_tokens[1] != "")
            {
                Seperate_String(command_tokens[1], '|', columns);
            } else {
                for (auto column_it = column_headers.begin();
                    column_it != column_headers.end(); column_it ++)
                {
                    string column_header = *column_it;
                    if (column_header == "race") continue;
                    if (column_header == "gender") continue;
                    if (column_header == "timezone") continue;
                    if (column_header == "state") continue;
                    if (column_header == "city") continue;
                    columns.push_back(column_header);
                }
            }

            vector<Result_Item> temp_results;
            for (auto result_it = results.begin(); result_it != results.end(); result_it ++)
            {
                temp_results.push_back(*result_it);
            }
           
            set<Result_Item> *neighbors = new set<Result_Item>[results.size()]; 
            #pragma omp parallel for schedule (dynamic)
            for (unsigned int i=0; i<results.size(); i++)
            {
                Result_Item &result = temp_results[i];
                int table_index = result.table_index;
                int cell_index = result.cell_index;
                vector<string> &header = tables[table_index][0];
                vector<string> &row = tables[table_index][cell_index];
                set<Result_Item> &neighbor_list = neighbors[i];

                //for (auto header_it = header.begin(); header_it != header.end(); header_it ++)
                for (unsigned int j=0; j<header.size(); j++)
                {
                    if (j >= row.size()) continue;
                    string column_name = header[j];
                    bool column_found = false;
                    for (auto columns_it = columns.begin();
                        columns_it != columns.end();
                        columns_it ++)
                    {
                        if (column_name == *columns_it)
                        {
                            column_found = true;
                            break;
                        }
                    }
                    if (!column_found) continue;

                    auto column_it = column_names.find(column_name);
                    if (column_it == column_names.end()) continue;
                    int column_index = column_it -> second;
                    auto &cell_pointers_ = cell_pointers[column_index];
                    string str = row[j];
                    Str2Cell_Pointer temp_pointer(str, 1, 1);
                    auto start_it = lower_bound(
                        cell_pointers_.begin(),
                        cell_pointers_.end(),
                        temp_pointer);
                    auto neighbor_it = start_it;
                    while (neighbor_it != cell_pointers_.end())
                    {
                        if (neighbor_it -> str != str) break;
                        if (neighbor_it -> table_index != table_index
                            || neighbor_it -> cell_index != cell_index)
                        {
                            neighbor_list.insert(Result_Item(
                                neighbor_it -> table_index,
                                neighbor_it -> cell_index));
                        }
                        neighbor_it ++;
                    }
                }
            }

            unsigned int org_results_size = results.size();
            if (command_tokens[0] == "explore")
            {
                for (unsigned int i=0; i<results.size(); i++)
                {
                    int table_index = temp_results[i].table_index;
                    int cell_index = temp_results[i].cell_index;
                    auto &neighbor_list = neighbors[i];
                    string str = "";
                    V2Str(tables[table_index][cell_index], str);
                    cout << i << " : " << table_list[table_index] 
                        << " " << str << endl;
                    for (auto neighbor_it = neighbor_list.begin();
                        neighbor_it != neighbor_list.end();
                        neighbor_it ++)
                    {
                        V2Str(tables[neighbor_it -> table_index]
                            [neighbor_it -> cell_index], str);
                        cout << "\t" << table_list[neighbor_it -> table_index]
                            << " " << str << endl;
                    }
                }
            } else if (command_tokens[0] == "advance")
            {
                for (unsigned int i=0; i<org_results_size; i++)
                {
                    auto &neighbor_list = neighbors[i];
                    for (auto neighbor_it = neighbor_list.begin();
                        neighbor_it != neighbor_list.end();
                        neighbor_it ++)
                    {
                        results.insert(Result_Item(
                            neighbor_it -> table_index,
                            neighbor_it -> cell_index));
                    }
                }
            }

            for (unsigned int i=0; i<org_results_size; i++)
            {
                neighbors[i].clear();
            }
            delete[] neighbors; neighbors = NULL;
            temp_results.clear();

            continue;
        }

        if (command_tokens[0] == "remove")
        {
            unsigned int results_size = results.size();
            if (results_size == 0) continue;
            int *result_markers = new int[results_size];
            for (unsigned int i=0; i<results_size; i++)
                result_markers[i] = 0;
            for (unsigned int j=1; j<command_tokens.size(); j++)
            {
                string token = command_tokens[j];
                bool valid_token = true;
                vector<string> range;
                
                if (token == "") valid_token = false;
                for (unsigned int t = 0; t<token.length(); t++)
                {
                    if (token[t] == '~') continue;
                    if (token[t] >= '0' && token[t] <= '9') continue;
                    valid_token = false;
                    break;
                }
                if (valid_token)
                {
                    Seperate_String(token, '~', range);
                    if (range.size() < 1 || range.size() > 2)
                    {
                        valid_token = false;
                    } else {
                        for (unsigned t = 0; t<range.size(); t++)
                            if (range[t] == "") valid_token = false;
                    }
                }
                
                if (!valid_token)
                {
                    cout << "[Warning] " << token << " is not a valid range." << endl;
                    continue;
                }

                if (range.size() == 1)
                {
                    unsigned int result_index = atoi(range[0].c_str());
                    if (result_index >= results_size)
                    {
                        cout << "[Warning] index " << result_index << " is invalid." << endl;
                        continue;
                    }
                    result_markers[result_index] = 1;
                } else if (range.size() == 2)
                {
                    unsigned int range_begin = atoi(range[0].c_str());
                    unsigned int range_end = atoi(range[1].c_str());
                    if (range_begin >= results_size || range_end >= results_size)
                    {
                        cout << "[Warning] range " << token << " is invalid." << endl;
                        continue;
                    }

                    for (unsigned int result_index=range_begin; result_index<= range_end; result_index++)
                        result_markers[result_index] = 1;
                }
            }

            auto result_it = results.begin();
            int result_index = 0;
            while (result_it != results.end())
            {
                if (result_markers[result_index] == 1)
                {
                    result_it = results.erase(result_it);
                } else result_it ++;
                result_index ++;
            } 

            delete[] result_markers; result_markers = NULL;
            continue;
        }
    }
    return 0;
}


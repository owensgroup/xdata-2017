#include <iostream>
#include <string>
#include <fstream>
#include <dirent.h>
using namespace std;

int main(){
	int count = 0;	
	ifstream d("./lsOutput.txt");
	string str,str1;
//	ifstream c("/data/xdata-2017/grouped/dox/Kate.txt",ios::binary);
	
	ofstream fout("dox.txt");
	while(getline(d,str1)){
		string filename = "/data/xdata-2017/grouped/dox/"+str1;
		ifstream c; c.open(filename.c_str());
		cout << filename << endl;
		fout << str1 << ' ';
        
        while (!c.eof())
		//while (getline(c,str))
        {
            getline(c, str);
            if (c.eof()) continue;
			if(!str.empty() && str[str.size() - 1] == '\r')
				str.erase(str.size() - 1);
            for (unsigned int i=0; i < str.length(); i++)
                if (str[i] == '\t') str[i] = ' ';
			fout << str << ' ';
		}
		fout << endl;
		count++;
		c.close();
	}
	
	cout << count << endl;
	fout.close();
	d.close();

/*
	DIR *dir;
	struct dirent *ent;
	ofstream fout("output.txt");
	if ((dir = opendir ("/data/xdata-2017/grouped/dox/")) != NULL) {
  	while ((ent = readdir (dir)) != NULL) {
    	
		//printf ("%s\n", ent->d_name);
		string str;
		string filename = ent->d_name;
		filename = "/data/xdata-2017/grouped/dox/" + filename;
		cout << filename << endl;
    	ifstream c(filename);
		
    	while (getline(c,str)){
			str.erase(str.size() - 1);
        	fout << str << ' ';
    	}
		fout << endl;

  	}
  	closedir (dir);

	} else {
  		perror ("");
  		return EXIT_FAILURE;
	
}
*/
	return 0;
}

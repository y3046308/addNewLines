// reads text file (Korean & English), which doesn't have any line breaks and create new lines after every 4th periods.
// directory of the file is set to 	"D:\\bcs\\old\\";
// user can continuously work with another text file after one another.
// to finish, type "zz"
// haha
// update: added folder directory function so user can pick a text file(or multiple text files)
// from any folder and rename them as pleased

// Setting: Project -> Property -> Use of Mfc -> Use MFC in a Shared DLL
#include <stdio.h>
#include <fstream>
#include <tchar.h>
#include <iostream>
#include <locale>
#include <vector>
#include <afxdlgs.h>	// for CFileDialog
#include <conio.h>		// for getch

using namespace std;

// # of periods we want in single paragraph
#define REPETITION 4

void run(string in, string out) {
	// open a file
	ifstream myfile(in);

	string line;

	int index = 0;

	if (myfile.is_open()) {
		// create an output file
		ofstream newFile(out);

		while (std::getline(myfile, line)) {
			int tmp_index = index;
			string newline = "";

			if (line != "") {
				//char prevVal = '\0';
				int prevInd = -100;

				for (int i = 0; i < line.length(); i++) {
					string tmp = "";
					char curr = line.at(i);

					if ((curr & 0x80) != 0x80) {	// if ascii
						tmp += curr;				// first add it to newfile

						// if we see period and it's not followed by another period
						if (curr == '.') {				
							if (prevInd + 1 != i) {

								// dont want period in front of current period
								if (i + 1 < line.length() && line.at(i + 1) != '.') {
									if (tmp_index == REPETITION) {	// if this is 5th period, add newlines
										tmp += "\n\n";
										tmp_index = 0;
									}
									else {
										tmp_index++;			// increment index
									}
								}

							}
							// keep track of previous periods so there is no space b/w consecutive periods
							prevInd = i;
						}
					}

					// If korean
					else if ((curr & 0x80) == 0x80) {
						tmp = line.substr(i, 2);
						i++;
					}

					//std::cout << i << ": " << tmp << endl;
					newline += tmp;
				}
				index = tmp_index;
				newFile << newline;
			}
			else {
				tmp_index = 0;
				newFile << endl << endl;
			}
		}
		myfile.close();
		newFile.close();
	}
	else {
		std::cout << "Failed to read " << in << endl;
	}
}

// each file path ends with double nulls
// if multiple files are selected, they are separated by nulls
// if single file is selected, there is no separator
bool multi_file_selected(char* in) {
	char prev = '1';
	bool hasSeparator = false;

	while (1) {
		char curr = *in;

		if (prev == '\0' && curr == '\0') break;
		if (prev == '\0' && curr != '\0') hasSeparator = true;

		prev = curr;
		in++;
	}
	std::cout << endl;
	return hasSeparator;
}

// extract filenames from input
vector<string> extract_fileNames(char* in) {
	vector<string> out;

	//std::cout << "START---------------------------------" << endl;
	if (multi_file_selected(in)) {
		char prev = '1';
		string tmp = "";

		while (1) {
			char curr = *in;

			if (curr == '\0') {
				if (prev == '\0') {
					break;
				}
				out.push_back(tmp);
				tmp = "";
			}
			else {
				std::cout << tmp << endl;
				tmp += curr;
			}
			prev = curr;
			in++;
		}

		// add \ to end of directory
		tmp = out[0];
		out[0] = tmp + "\\";

	}
	else {	// only single file selected
		string new_str(in);

		// find the last occurrence of '\'
		int loc = new_str.rfind("\\");

		// separate dir and filename
		string dir = new_str.substr(0, loc + 1);
		string fileName = new_str.substr(loc + 1);

		// index 0 of vector is directory of files
		out.push_back(dir);
		out.push_back(fileName);

	}
	/*
	std::cout << "end---------------------------------" << endl << endl;
	std::cout << "extract_fileNames---------------------------------------" << endl;
	for (int i = 0; i < out.size(); i++) {
	std::cout << out.at(i) << endl;
	}
	std::cout << "extract_fileNames---------------------------------------" << endl;
	*/
	return out;
}

// create new files
vector<string> getNewName(vector<string> in, string prefix) {
	vector<string> out;
	out.push_back(in[0]);	// add a directory

	//std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	for (int i = 1; i < in.size(); i++) {
		string newFileName = prefix + in[i];
		//std::cout << in[i] << "->" << newFileName << endl;
		out.push_back(newFileName);
	}
	//std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	return out;
}

bool operator==(const string& lhs, const string& rhs) {
	if (lhs.compare(rhs) == 0) return true;
	return false;
}

int main() {
	std::cout << "*************WELCOME*************" << endl;
	std::cout << "This program reads a text file, which doesnt have line breaks," << endl;
	std::cout << "and add line breaks" << endl;
	std::cout << "By default, new text file is named \"new_(txt_name)\"" << endl;
	std::cout << "but you can also select new prefix" << endl;
	std::cout << "Press enter to get started" << endl;
	string c;  std::getline(cin, c);

	// when ctrl-c is pressed, exit the loop
	while (1) {
		/*
		std::cout << "Enter a text filename (without an extension and type \"zz\" to end): ";
		string read;
		std::getline(cin, read);

		// type 'zzz' to finish
		if (!read.compare("zz")) break;

		string dir = "D:\\bcs\\old\\";
		string fileName(read);
		string fileExtension = ".txt";

		string fileFullName = fileName + fileExtension;

		string in = dir + fileFullName;
		string out = dir + "new_" + fileFullName;
		*/
		OPENFILENAME ofn;       // common dialog box structure
		char szFile[1500];       // buffer for file name
		HWND hwnd;              // owner window
		HANDLE hf;              // file handle

								// Initialize OPENFILENAME
								/* reference: https://msdn.microsoft.com/en-us/library/windows/desktop/ms646829(v=vs.85).aspx#open_file*/
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = NULL;	// the dialog box has no owner
		ofn.lpstrFile = szFile;

		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);

		// no filter set
		//ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
		ofn.lpstrFilter = "Text File\0*.txt\0All\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;

		// initial directory set to D:\bcs:\old
		ofn.lpstrInitialDir = _T("D:\\bcs\\old\\");

		// OFN_PATHMUSTEXIST: user can only type valid paths & file names
		// OFN_FILEMUSTEXIST: The user can type only names of existing files in the File Name entry field.
		// user is allowed to select multiple files at once
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER | OFN_ALLOWMULTISELECT;

		// Display the Open dialog box. 
		if (GetOpenFileName(&ofn) == TRUE) {
			//hf = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

			string prefix;

			while (1) {
				std::cout << "1. use default prefix(new_)" << endl;
				std::cout << "2. use desired prefix" << endl;
				std::cout << "3. overwrite original files" << endl;
				std::cout << "Choose your option to save your file(s):";
				// getch reads a character without using any buffer.
				// so no needs to press enter
				int choice = _getch();

				std::cout << endl << endl;

				if (choice == '1') {
					prefix = "new_";
					break;
				}
				else if (choice == '2') {
					std::cout << "Enter your prefix with an underscore:";
					std::getline(cin, prefix);
					std::cout << endl << endl;

					break;
				}
				else if (choice == '3') {
					prefix = "";
					break;
				}
				else {
					std::cout << "No such option" << endl << endl;
				}
			}

			char* lst = ofn.lpstrFile;

			vector<string> inputList = extract_fileNames(lst);
			vector<string> outputList = getNewName(inputList, prefix);
			
			for (int i = 1; i < inputList.size(); i++) {
				string in = inputList[0] + inputList[i];
				string out = outputList[0] + outputList[i];
				run(in, out);
			}
		}
		else {	// exit the loop if no file is selected
			break;
		}
		std::cout << ">>>>>>>>>>>>>>>>>>>>Successfully  Done<<<<<<<<<<<<<<<<<<<<" << endl;
	}

	std::cout << "************************************************" << endl;
	std::cout << "************************************************" << endl;
	std::cout << "********************FINISHED********************" << endl;
	std::cout << "************************************************" << endl;
	std::cout << "************************************************" << endl;
	c;  std::getline(cin, c);
}

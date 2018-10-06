#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;
int main(int argc, char** argv) {
	  vector<string> keys;
	  // List of keywords recognised by SpaceShower.
	  keys.push_back("isr:murfac");
	  keys.push_back("isr:g2gg:murfac");
	  keys.push_back("isr:q2qg:murfac");
	  keys.push_back("isr:q2gq:murfac");
	  keys.push_back("isr:x2xg:murfac");
	  keys.push_back("isr:g2qq:murfac");
	  keys.push_back("isr:cns");
	  keys.push_back("isr:g2gg:cns");
	  keys.push_back("isr:q2qg:cns");
	  keys.push_back("isr:q2gq:cns");
	  keys.push_back("isr:x2xg:cns");
	  keys.push_back("isr:g2qq:cns");
	  keys.push_back("isr:pdf:plus");
	  keys.push_back("isr:pdf:minus");
	  keys.push_back("isr:pdf:member");

	  // Get uncertainty variations from Settings (as list of strings to parse).
	  vector<string> uVars = {
		  "   alphashi fsr:murfac=0.5 isr:murfac=0.5",
		  "     alphaslo fsr:murfac=2.0 isr:murfac=2.0"
	  };
	  size_t varSize = uVars.size();
	  vector<string> uniqueVars;

	  // Expand uVars if PDFmembers has been chosen
	  string tmpKey("isr:pdf:family");
	  // Parse each string in uVars to look for recognized keywords.
	  for (size_t iWeight = 0; iWeight < varSize; ++iWeight) {
		  // Convert to lowercase (to be case-insensitive). Also remove "=" signs
		  // and extra spaces, so "key=value", "key = value" mapped to "key value"
		  string& uVarString = uVars[iWeight];
		  while (uVarString.find(" ") == 0) uVarString.erase( 0, 1);
		  int iEnd = uVarString.find(" ", 0);
		  uVarString.erase(0,iEnd+1);
		  while (uVarString.find("=") != string::npos) {
			  int firstEqual = uVarString.find_first_of("=");
			  string testString = uVarString.substr(0, firstEqual);
			  iEnd = uVarString.find_first_of(" ", 0);
			  if( iEnd<0 ) iEnd = uVarString.length();
			  string insertString = uVarString.substr(0,iEnd);
			  // does the key match an fsr one?
			  if( find(keys.begin(), keys.end(), testString) != keys.end() ) {
				  if( uniqueVars.size() == 0 ) {
					  uniqueVars.push_back(insertString);
				  } else if ( find(uniqueVars.begin(), uniqueVars.end(), insertString)
						  == uniqueVars.end() ) {
					  uniqueVars.push_back(insertString);
				  }
			  } 
			  uVarString.erase(0,iEnd+1);
		  }
	  }

	  cout << "Unique variables: ";
	  for(auto& var: uniqueVars) {
		  cout << "|" << var;
	  }
	  cout << endl;


	  return 0;
}

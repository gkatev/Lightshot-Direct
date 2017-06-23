/* 	Copyright (C) 2017 Georgios Katevainis-Bitzos

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

// g++ -c -DBUILDING_ANALYZE_PAGE_DELL analyze.cpp
// g++ -shared -o analyze.dll analyze.o -lcurldll

#include <string>
#include <regex>
#include <curl/curl.h>

#define ID_LEN 22
#define ID_LEN_S "22"

using namespace std;

// Declare the regex as a global so that it is built only once
static const regex l_exp("([\\w\\-]{" ID_LEN_S "}\\.jpeg)");

// Searches for the direct link in the source string
static string search_link(string source) {
	smatch m;
	
	if(regex_search(source, m, l_exp)) return "https://image.prntscr.com/image/" + m[1].str();
	else return "";
}

// Called by libcurl to write the downloaded data
// For each call the data chunk is searched for the direct link
// If found the download stops
/* Otherwise part of the chunk is kept(in case the chunk contained part of the link) 
	in the userdata string to be concatenated with the next chunk */
static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *userdata) {
	((string *)userdata)->append((char *) ptr, size * nmemb);
	
	string data = *(string *)userdata;
	string l = search_link(data);
	
	if(!l.empty()) {
		*(string *)userdata = l;
		return 0;
	}
	
	if(data.length() > ID_LEN - 1) data = data.substr(data.length() - (ID_LEN - 1));
	*(string *) userdata = data;
	
	return size * nmemb;
}

// Analyzes the link to extract the direct link to the image
// Stores the direct link in buf which is of size size
// Assumes an input link format of: http://prntscr.com/fncadj
/* Return value:
	0: All is good
	-1: libcurl initialization error
	-2: Direct link not found in webpage
	otherwise: Return value of curl_easy_perform()
*/
extern "C" __declspec(dllexport) int __stdcall analyze_result(const char *link, char *buf, size_t size) {
	string page_data;
	
	CURL *curl = curl_easy_init();
	
	// libcurl initialization error
	if(!curl) return -1;
	
	/* The link output by lightshot redirects to the actual page. 
		To avoid the extra time cost of the redirection we convert the link using a simple formula */
	string dl_link = "https://prnt.sc" + string(link).substr(string(link).find_last_of('/'));
	
	// Set the page to download
	curl_easy_setopt(curl, CURLOPT_URL, dl_link.c_str());
	
	// Apparently a user agent is nescessary for accessing/downloading the page
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
	
	// Set up write back
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &page_data);

	// Download the page and deallocate libcurl
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	
	// Check for download error
	switch(res) {
		case CURLE_WRITE_ERROR : break;
		case CURLE_OK: return -2;
		default: return res;
	}
	
	/* Copy the direct link to the passed buffer */
	
	size_t i;
	for(i = 0; i < page_data.length() && i < size - 1; i++) {
		buf[i] = page_data[i];
	}
	
	buf[i] = '\0';
	
	return 0;
}

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

using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

extern "C" __declspec(dllexport) int __stdcall analyze_result(const char *link, char *buf, size_t size) {
	string page_data;
	
	CURL *curl = curl_easy_init();
	if(!curl) return 1;

	curl_easy_setopt(curl, CURLOPT_URL, link);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &page_data);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if(res != CURLE_OK) return 2;
	
	regex direct("\\\"(https:\\/\\/image\\.prntscr\\.com\\/image\\/[\\w\\-]+\\.jpeg)\\\"");
	smatch matches;
	
	if(regex_search(page_data, matches, direct)) {
		if(matches.size() != 2) return 3;
		
		size_t i;
		for(i = 0; i < matches[1].str().length() && i < size - 1; i++) {
			buf[i] = matches[1].str()[i];
		}
		
		buf[i] = '\0';
	} else return 4;
	
	return 0;
}

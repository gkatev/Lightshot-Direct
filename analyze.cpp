// g++ -c -DBUILDING_ANALYZE_PAGE_DELL analyze.cpp
// g++ -shared -o analyze.dll analyze.o -lcurldll

#include <iostream>
#include <string>
#include <regex>
#include <assert.h>
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

	if(res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		return 2;
	}
	
	regex direct("\\\"(https:\\/\\/image\\.prntscr\\.com\\/image\\/\\w+\\.jpeg)\\\"");
	smatch matches;
	
	if(regex_search(page_data, matches, direct)) {
		assert(matches.size() == 2);
		
		size_t i;
		for(i = 0; i < matches[1].str().length() && i < size - 1; i++) {
			buf[i] = matches[1].str()[i];
		}
		
		buf[i] = '\0';
	} else return 3;
	
	return 0;
}

/* int main(int argc, char **argv) {
	if(argc != 2) return 1;
	string url = argv[1];	
	
	char buf[1024];
	analyze_result(url.c_str(), buf, 1024);
	cout << buf << endl;
	
	return 0;
}
*/

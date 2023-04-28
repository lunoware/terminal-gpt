#include <stdio.h>
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <regex>


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

std::string contextBase = "{\"role\": \"system\", \"content\": \"I'm Sammy a software developer and you're my trusted assistant, your goal is to help me solve my tasks fast and smart.\"}";


std::string ask_chat_gpt(std::string prompt)
{
  std::string readBuffer = "";
  CURL *curl;
  CURLcode res;
  curl = curl_easy_init();
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.openai.com/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Authorization: Bearer ENTER-YOUR-TOKEN-HERE");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // create new prompt string and add to context prompt stings.
    std::string newMessage = ",{\"role\": \"user\", \"content\": \"" + prompt + "\"}";

    contextBase += newMessage;

    std::string messages = contextBase;
    std::string dataString = "{\n    \"model\": \"gpt-3.5-turbo\",\n    \"messages\":[" + messages + "]\n}";

    const char *data = dataString.c_str();
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
  }
  curl_easy_cleanup(curl);
  return readBuffer;
}

std::string find_answer(std::string response)
{

  std::string str = response;

  std::string firstStr = "[{\"message\":{\"role\":\"assistant\",\"content\":\"";

  unsigned first = str.find(firstStr);
  unsigned last = str.find("\"},\"finish_reason\":\"stop\",\"index\":0}");
  std::string strNew = str.substr(first + firstStr.length(), (last - first) - firstStr.length());

  return strNew;
}

std::string escape_special_chars(std::string str) {
    std::string res;
    for (char c : str) {
        switch (c) {
            case '\"':
                res += "\\\"";
                break;
            case '\\':
                res += "\\";
                break;
            default:
                res.push_back(c);
        }
    }
    return res;
}

std::string un_escape_special_chars(std::string str) {
    std::string res;
    for (char c : str) {

        switch (c) {
            case '\\':
                res += '\n';
                break;
            default:
                res.push_back(c);
        }
    }
    //return res;

    std::string fixedString = std::regex_replace(str, std::regex(R"(\\n\\n)"), "\n"); 
    fixedString = std::regex_replace(fixedString, std::regex(R"(\\\\n)"), "\n");
    fixedString = std::regex_replace(fixedString, std::regex(R"(\\\n)"), "\n");
    fixedString = std::regex_replace(fixedString, std::regex(R"(\\n)"), "\n"); 
    fixedString = std::regex_replace(fixedString, std::regex(R"(<br>)"), "\n"); 
    fixedString = std::regex_replace(fixedString, std::regex(R"(\\')"), "\'"); 
    fixedString = std::regex_replace(fixedString, std::regex(R"(\\\\")"), "\""); 
    fixedString = std::regex_replace(fixedString, std::regex(R"(\\")"), "\""); 
    return fixedString;
}

int main(int argc, char *argv[])
{
  std::string promptAll = "";
  while (true)
  {
    std::cout << "Prompt: ";

    std::string prompt;
    std::getline(std::cin, prompt);

    prompt += "\\n";

    promptAll = prompt;

    std::string r = ask_chat_gpt(escape_special_chars(promptAll));

    std::cout << un_escape_special_chars(find_answer(r)) << std::endl;
  }
  return 0;
}



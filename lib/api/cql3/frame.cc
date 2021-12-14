#include "api/cql3/frame.hh"
#include <iostream>
#include <list>
#include <map>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>
using namespace dibibase::api::cql3;

using namespace std;

class Frame {
public:
  char *buffer{nullptr};
  char version,flags,opcode;
  unsigned char streamID[2] = {'0', '0'};   //initially filled with zeroes instead of null
  char length[4] = {'0', '0', '0', '0'};    //also initially filled with zeroes 
  int max_size;
  char *body{nullptr};
  Frame(char *buff, int size) {
    buffer = buff;
    version = 0x03;         //hardcoding the version (not the best way but there is quite difference between versions)
    max_size = size;
  }
    /*
    *parse() is used to divide the header bytes of the incoming buffer to get the needed bytes and assign them to the prevously declared vars.
    */
  void parse() {
    version = *buffer;
    flags = buffer[1];

    streamID[0] = buffer[2];
    streamID[1] = buffer[3];

    opcode = buffer[4];

    length[0] = buffer[5];
    length[1] = buffer[6];
    length[2] = buffer[7];
    length[3] = buffer[8];
    // if buffer contains something other than header (ie. body of msg)
    if (max_size > 9) {
      body = substr(buffer, 9, max_size - 9);
    }
  }
  //re-inventing the substr 
  char *substr(char *arr, int begin, int len) {
    char *res = new char[len + 1];
    for (int i = 0; i < len; i++)
      res[i] = *(arr + begin + i);
    res[len] = 0;
    return res;
  }
};

/*
* Class ServerMsg actually controls the server-->client msgs .
*/
class ServerMsg {
private:
  enum flag { NONE = 0x00, COMPRESSED = 0x01, TRACING = 0x02 };
  enum Opcode {
    ERROR = 0x00,          /**< enum value ERROR. */
    STARTUP = 0x01,        /**< enum value STARTUP. */
    READY = 0x02,          /**< enum value READY. */
    AUTHENTICATE = 0x03,   /**< enum value AUTHENTICATE. */
    OPTIONS = 0x05,        /**< enum value OPTIONS. */
    SUPPORTED = 0x06,      /**< enum value SUPPORTED. */
    QUERY = 0x07,          /**< enum value QUERY. */
    RESULT = 0x08,         /**< enum value RESULT. */
    PREPARE = 0x09,        /**< enum value PREPARE. */
    EXECUTE = 0x0A,        /**< enum value EXECUTE. */
    REGISTER = 0x0B,       /**< enum value REGISTER. */
    EVENT = 0x0C,          /**< enum value EVENT. */
    BATCH = 0x0D,          /**< enum value BATCH. */
    AUTH_CHALLENGE = 0x0E, /**< enum value AUTH_CHALLENGE. */
    AUTH_RESPONSE = 0x0F,  /**< enum value AUTH_RESPONSE. */
    AUTH_SUCCESS = 0x10    /**< enum value AUTH_SUCCESS. */
  };
  enum Version { VERSION = 0x83, KEY = 0x83 };
  Frame v() {
    char q[21] = {'0', '3', '0', '0', '0', '0', '0', '0', '0',
                  '5', '0', '0', '0', '0', '0', '2', '3', '4'}; // ex frame
    Frame framee(q, sizeof(q));
    return framee;
  }
  Frame frame = v();

public:
  ServerMsg(Frame f) { frame = f; }
  unsigned char Header[4096];

  int SupportedMessage(map<string, list<string>> supportedOptions,unsigned char *Header) {
    int index = 9;
    for (auto it = supportedOptions.begin(); it != supportedOptions.end();++it) {
      string s = it->first;
      if(it == supportedOptions.begin()){
          Header[index] = 0;
          Header[++index] = supportedOptions.size();
          index++;
         
      }
      Header[index] = 0;
      Header[++index] = s.length();
      index++;
      for (unsigned int i = 0; i < s.length(); i++) {
        Header[index] = (int)s[i];
        index++;
     
      }
  
      for (list<string>::iterator it2 = (it->second).begin();it2 != (it->second).end(); ++it2) {
        if(it2== (it->second).begin()){
          Header[index] = 0;
          Header[++index] = it->second.size();
          index++;
        }
        Header[index] = 0;
       Header[++index] = (*it2).length();
        index++;


        string list_elem = *it2;
        for (unsigned int k=0 ;k < list_elem.length();k++){
            Header[index] = (int)list_elem[k];
            index++;
           
        }

      }
    }
    Header[8] = (int)(index - 9);      //count number of bytes (should be < 255 otherwise use Header[7]&Header[6])// 
    return index;
  }

  int CreateResponse() {
    int opcode = int(frame.opcode);
    int msg_length = 9;   //default minimum is 9 decimals (HEADER ONLY NO DATA)
    list<string> versions, comp_types, cql_versions;
    switch (opcode) {
    case OPTIONS: {
      Header[0] = 131;
      Header[1] = frame.flags;
      Header[2] = frame.streamID[0];Header[3] = frame.streamID[1];
      Header[4] = SUPPORTED;       
      Header[5] = 0;Header[6] = 0;Header[7] = 0;
      versions.push_back("3/V3");
      comp_types.push_back("snappy");
      comp_types.push_back("lz4");
      cql_versions.push_back("3.4.5");

      map<string, list<string>> supportedOptions;
      supportedOptions.insert(make_pair("PROTOCOL_VERSIONS", versions));
      supportedOptions.insert(make_pair("CQL_VERSION", cql_versions));
      supportedOptions.insert(make_pair("COMPRESSION", comp_types));
      msg_length = SupportedMessage(supportedOptions, Header);
      break;
    }
    case STARTUP: {
      Header[0] = 131;
      Header[1] = frame.flags;
      Header[2] = frame.streamID[0];Header[3] = frame.streamID[1];
      Header[4] = READY;       
      Header[5] = 0;Header[6] = 0;Header[7] = 0;Header[8] = 0;
      msg_length = 9;
      
      break;
    }
    case REGISTER:{
      Header[0] = 131;
      Header[1] = frame.flags;
      Header[2] = frame.streamID[0];Header[3] = frame.streamID[1];
      Header[4] = READY;       
      Header[5] = 0;Header[6] = 0;Header[7] = 0;Header[8] = 0;
      msg_length = 9;
      
      break;
    }
    case QUERY:{
      // system_schema queries handling




      //general queries handling

        //move to parser

      break;
    }
    case AUTHENTICATE:
      cout << "AUth";
      break;
    }
    return msg_length;
  }
};


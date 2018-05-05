#ifndef TRACKER_H
#define TRACKER_H

#include <string>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <memory>       //For shared_ptr
#include <vector>
#include <ctime>        //For generating tracker key
#include <boost/thread.hpp>


#include <curl/curl.h>
#include <curl/easy.h>
#include "bencode.h"    //For processing response
#include "peer.h"

#include <glog/logging.h>   //Logging Library


//Error Codes
#define CURL_NOT_INIT -10
#define CURL_ESCAPE_ERROR -11
#define EMPTY_TRACKER -9

//Constant
#define MAX_TRACKER_URLS 10
#define MAX_PORT_VALUE 65535


namespace tracker{

    using namespace std;

    extern boost::mutex peer_list_mutex;

    //Struct that contains all tracker param

    struct TParameter{
        string info_hash;
        string peer_id;
        uint port;
        uint uploaded;
        uint downloaded;
        uint left;
        char* info_hash_raw;
    };


    enum event_type{ 
        STARTED,
        STOPPED
    };


    typedef vector<string> TList;

    /*****************************************************************************************
     * 
     *  Tracker Request
     * 
     *  GET Parameter List
     * 
     *  info_hash : urlencoded 20-byte SHA1 hash of the value of the info key from the Metainfo file 
     *  peer_id: urlencoded 20-byte string used as a unique ID for the client, generated by the client at startup.
     *  port: The port number that the client is listening on.
     *  uploaded: The total amount uploaded in base ten ASCII.
     *  downloaded: The total amount downloaded in base ten ASCII
     *  left: The number of bytes this client still has to download in base ten ASCII.
     *  compact: Setting this to 1 indicates that the client accepts a compact response. 
     *  no_peer_id: Indicates that the tracker can omit peer id field in peers dictionary.
     *  event: If specified, must be one of started, completed, stopped
     *      started: The first request to the tracker must include the event key with this value.
     *      stopped: Must be sent to the tracker if the client is shutting down gracefully.
     *      completed: Must be sent to the tracker when the download completes.
     * 
     * 
     * **************************************************************************************/


    int tracker_send_request(shared_ptr<string> url, string  *response, CURL *curl = NULL);
    bool check_url(const string &url, CURL *curl=NULL);
    shared_ptr<string> url_builder(const string &tracker_url, const TParameter &t_param, event_type event, CURL *curl=NULL,  bool tls = false);
    int urlencode_paramenter(TParameter *param, CURL *curl = NULL);
    int start_tracker_request(TParameter *param, const TList &tracker_list);
    void process_tracker_request(const string& tracker_url, const TParameter *param, pwp::PeerList peer_list);
    int scrape_request(string &url, string *response, CURL *curl = NULL);
    shared_ptr<string> get_scrape_url(const string &url);
    string create_tracker_key();
    int parse_dict_peer(be_node *node, pwp::PeerList peer_list);
    int parse_binary_peers(char *str);
    bool is_compact_response(const string *response);
    int process_tracker_response(string *response, pwp::PeerList peer_list);
    uint remove_duplicate_peers(pwp::PeerList& peer_list);
}   //End namespace tracker

#endif
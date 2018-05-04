#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

#include "bencode.h"
#include "torrentparser.hpp"
#include "tracker.h"
#include "filehandler.hpp"
#include "peer.h"

using namespace std;


int main(int argc, char* argv[]) {

  if (argc <= 1) {
    cout << "Usage: pass a .torrent file as a parameter" << endl;
    return 1;
  }

  google::InitGoogleLogging(argv[0]); //Initialize GLog with passed argument

  curl_global_init(CURL_GLOBAL_DEFAULT);

  ifstream myfile(argv[1]);
  if (myfile.is_open()) {
    // parsing .torrent file as a string
    string line;
    string torrent_str;
    while (getline(myfile, line))
      torrent_str += line + '\n';
    myfile.close();

    // decoding
    be_node *node;
    long long len = torrent_str.length();
    node = be_decoden(torrent_str.c_str(), len);
    if (node) {
      Torrent mytorrent;
      parse_torrent(node, mytorrent);
      be_free(node);
      print_torrent(mytorrent);

      TrackerParameter param;

      param.info_hash_raw = get_info_node_hash(&torrent_str, &mytorrent.pieces);
      param.tracker_urls = mytorrent.trackers;

      param.left = mytorrent.piece_length * mytorrent.pieces.size();
      get_peer_id(&param.peer_id);

      start_tracker_request(&param);

      free(param.info_hash_raw);
    }
    else
      cout << "Parsing of " << argv[1] << " failed!" << endl;
  }
  else
    cout << "Unable to open the file." << endl;




  curl_global_cleanup();
  return 0;
}

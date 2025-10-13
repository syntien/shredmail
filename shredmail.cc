/*
   Generate a log line with the SMTP id and user name for a trashed mail message seen
   on standard input stream, while flushing the stream so the mail is not delivered.
*/

#include <cstdio>
#include <cstdlib>
#include <format>
#include <iostream>
#include <regex>
#include <string>

#include <pwd.h>
#include <unistd.h>

using namespace std;
using namespace std::regex_constants;

int
main( int argc, char** argv, char **arge )
{
   /*
    * Process command line options
    */
   int opt;
   string status( "Shredded" );
   while ( ( opt = getopt(argc, argv, "s:") ) != -1 )
      if ( opt == 's' )
         status = optarg;
   string info( argc > optind ? string(" info=") + argv[optind] : "" );

   /*
    * Get user name
    */
   uid_t uid = geteuid();
   struct passwd *pw = getpwuid(uid);
   string ctladdr( pw ? pw->pw_name : "unknown" );

   /*
    * Get the first "Received: " header and all its continuations
    * as a single line, while flushing all input
    */

   bool searching = true;
   string header, line;
   int n;

   while ( cin.good() && ( getline( cin, line ), cin.good() ) ) {
      if ( searching && line.size() > 0 && ! line.substr( 0, 10 ).compare( "Received: " ) ) {
         for ( header = line ; searching && cin.good() && ( getline( cin, line ), cin.good() ) ; ) {
            n = line.find_first_not_of( " \t" );
            if ( n != 0 && n != string::npos )
               header += " " + line.substr( n );
            else
               searching = false;
         }
      }
   }

   /* Extract the SMTP id from the header line */

   smatch m;
   regex rx( R"(\s*by .* id ([0-9A-Za-z]+))", ECMAScript );
   string id( regex_search( header, m, rx, match_default ) ? m[1].str() : "non-match" );

   /* log message with the gathered info */

   string msg( format( "{:s}: user={:s} status={:s} {:s}", id, ctladdr, status, info ) );
   return execlp( "logger", "logger", "-i", "-t", "shredmail", "-p", "mail.info", msg.c_str(), (char *) NULL );
}

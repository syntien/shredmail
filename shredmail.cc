/*
   Generate a log line with the SMTP id and user name for a trashed mail message seen
   the standard input stream, while flushing the stream so the mail is not delivered.
*/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <pwd.h>
#include <regex.h>
#include <unistd.h>

using namespace std;

int
main( int argc, char** argv, char **arge )
{
   /*
    * Get user name
    */
   int opt;
   uid_t uid = geteuid();
   struct passwd *pw = getpwuid(uid);
   string ctladdr( pw ? pw->pw_name : "unknown" );
   string status( "Shredded" );

   while ( ( opt = getopt(argc, argv, "s:") ) != -1 )
      if ( opt == 's' )
         status = optarg;

   /*
    * Get the first "Received: " header and all its continuations
    * as a single line, while flushing all input
    */

   string info( argc > optind ? string(" info=") + argv[optind] : "" );
   string id( "non-match" );
   string h, k, s;
   bool searching = true;
   int n;

   while ( cin.good() && ( getline( cin, s ), cin.good() ) ) {
      if ( searching && s.size() > 0 && ! s.substr( 0, 10 ).compare( ( k = "Received: " ) ) ) {
         for ( h = s ; searching && cin.good() && ( getline( cin, s ), cin.good() ) ; ) {
            n = s.find_first_not_of( " \t" );
            if ( n != 0 && n != string::npos )
               h += " " + s.substr( n );
            else
               searching = false;
         }
      }
   }

   /* Extract the SMTP id from the header line */

   regex_t reg;
   regmatch_t match[2];

   if ( 0 == regcomp( &reg, " *by .* id ([[:alnum:]]+)", REG_EXTENDED ) )
      if ( 0 == regexec( &reg, h.c_str(), 2, match, 0 ) )
         id = h.substr( match[1].rm_so, match[1].rm_eo - match[1].rm_so );

   /* log message with the gathered info */

   stringstream logger;
   logger << "logger -i -t shredmail -p mail.info '" << id << ": user=" << ctladdr << " status=" << status << info << "'";
   system( logger.str().c_str() );

   return 0;
}

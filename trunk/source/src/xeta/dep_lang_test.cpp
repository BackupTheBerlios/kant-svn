#include <iostream>
#include <string>
#include <list>
#include <stdexcept>
#include "packagespec.h"
#include "depparser.h"
#include <iterator>

using namespace std;
using namespace xeta;



int main ()
{
	xeta::DependencyParser parser;
	while ( true )
	{
		string input; 
		std::getline( cin, input );

		if( input.empty() ) return 0;

		if( parser.parse( input ) )
		{
			cout << " The Input:|" << input << "| is valid" << endl;

			typedef std::list<std::pair<bool, PackageInfo> > q_list;
			q_list quests;
			std::list< PackageInfo> deps;
			parser.get_questions( quests );

			for( q_list::iterator it = quests.begin(), e = quests.end();
					it != e; ++it )
			{
				it->first = true;
				cout << it->second << endl; 
			}

			std::set<std::string> flags;
			flags.insert( "blubextended" );
			flags.insert( "ein_flag" );
			flags.insert( "blub" );
			parser.get_packages( flags, deps, quests );
		
			ostream_iterator<PackageInfo> out( cout, "\n" );
			cout << "Deps are:" << endl;

			for( std::list<PackageInfo>::iterator it = deps.begin(), e = deps.end();
					it != e; ++it )
			{
				cout << *it << endl; 
			}

			
		}
		else
			cout << " The Input:|" << input << "| is invalid" << endl;
	}

	return 0;
}



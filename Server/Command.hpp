#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <cstring>

namespace irc {
	class	Command {
		private :
			const char* content;

			Command ();

		public :
			Command (const char* content);
			~Command ();

			void	parseCommand();

	};
];

#endif

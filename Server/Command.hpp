#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <cstring>

namespace irc {
	class	Command {
		private :
			const std::string content;

			Command ();

		public :
			Command (const std::string content);
			~Command ();

			void	parseCommand();

	};
];

#endif

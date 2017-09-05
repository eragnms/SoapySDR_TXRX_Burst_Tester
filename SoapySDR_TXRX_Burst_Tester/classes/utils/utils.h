#ifndef __UTILS_H__
#define __UTILS_H__

#include <iostream>
#include <vector>
#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/asio/signal_set.hpp>

namespace utils
{
	//mutex meant to be blocked while displaying cout and cerr messages from different threads scope
	extern boost::mutex msg_mutex;

	//variable meant to be set during closing procedure
	extern bool stop;

	//function that will return current time as ptime object
	boost::posix_time::ptime current_time();

	//function that will convert given ptime object into string
	std::string ptime_to_str
	(
		const boost::posix_time::ptime & given_ptime = current_time(),
		const bool & fractional_seconds = true,
		const bool & filename_mode = false
	);

	//message type
	enum msg_type_t
	{
		INFO,
		WARNING,
		ERROR
	};

	//function that will format given text as a message
	std::string to_msg
	(
		const std::string text,
		msg_type_t msg_type=INFO,
		const bool& add_timestamp = true,
		const bool& add_msg_type_prefix = true
	);

	//function that will print given text on cout/cerr depending on message type
	std::string msg
	(
		const std::string text,
		msg_type_t msg_type=INFO,
		const bool& add_timestamp = true,
		const bool& add_msg_type_prefix = true
	);

	//function that will return a set of "words" separated by a given delimiter in an input line
	std::vector<std::string> explode
	(
		std::string const & line,
		const char & delimiter = ',',
		const unsigned int expected_array_size = 0
	);

	//function that will allow to clean up some things before actual closing of an app
	extern void cleanup();

	//class that will take care of handling the interrupt signal (ctrl+c)
	class Signal_Handler
	{
	private:
		boost::asio::io_service io_service;
		boost::asio::signal_set signals;
		void handle_signal
		(
			const boost::system::error_code& error_code,
			const int& signal_number,
			bool& stop
		);
	public:
		Signal_Handler();
		boost::asio::io_service& get_io_service();
	};

	//signal handler meant to handle CTRL+C application closing routine
	extern Signal_Handler signal_handler;

	//function that will tell whether given char is a non-printable ASCII character or not
	bool is_nonprintable_char(char c);

	//function that will remove all non-printable ASCII characters from a given string
	void remove_nonprintable_chars(std::string & str);
}

#endif

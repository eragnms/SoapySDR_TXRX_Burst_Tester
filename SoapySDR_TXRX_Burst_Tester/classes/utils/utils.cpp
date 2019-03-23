#include "utils.h"

namespace pt = boost::posix_time;
using namespace std;

namespace utils
{
	boost::mutex msg_mutex;
	bool stop;

	pt::ptime current_time()
	{
		return pt::ptime(pt::microsec_clock::universal_time());
	}

	string ptime_to_str
	(
		const pt::ptime & given_ptime,
		const bool & fractional_seconds,
		const bool & filename_mode
	)
	{
		string ret;

		stringstream ss;

		if (fractional_seconds)
			if (filename_mode)
				ss.imbue(locale(ss.getloc(), new pt::time_facet("%Y-%m-%d__%H-%M-%S_%f")));
			else
				ss.imbue(locale(ss.getloc(), new pt::time_facet("%Y-%m-%d %H:%M:%S.%f")));
		else
			if (filename_mode)
				ss.imbue(locale(ss.getloc(), new pt::time_facet("%Y-%m-%d__%H-%M-%S")));
			else
				ss.imbue(locale(ss.getloc(), new pt::time_facet("%Y-%m-%d %H:%M:%S")));

		if (given_ptime.is_not_a_date_time())
			ret = "NOT-A-DATE-TIME";
		else
		{
			ss << given_ptime;
			ret = ss.str();
		}

		return ret;
	}

	string to_msg
	(
		const string text,
		msg_type_t msg_type,
		const bool& add_timestamp,
		const bool& add_msg_type_prefix
	)
	{
		string msg_text = "";

		if (add_timestamp)
			msg_text += ptime_to_str() + " ";

		if (add_msg_type_prefix)
		{
			if (msg_type == INFO)
				msg_text += "INFO -     ";
			else if (msg_type == WARNING)
				msg_text += "WARNING -  ";
			else if (msg_type == ERROR)
				msg_text += "ERROR -    ";
		}

		msg_text += text;

		return msg_text;
	}

	string msg
	(
		const string text,
		msg_type_t msg_type,
		const bool& add_timestamp,
		const bool& add_msg_type_prefix
	)
	{
		boost::lock_guard<boost::mutex> guard(msg_mutex);

		string msg_text = to_msg(text, msg_type, add_timestamp, add_msg_type_prefix);

		if (msg_type == INFO)
			cout << endl << msg_text;
		else
			cerr << endl << msg_text;

		return msg_text;
	}

	vector<string> explode
	(
		string const & line,
		const char & delimiter,
		const unsigned int expected_array_size
	)
	{
		vector<string> result;

		if (expected_array_size > 0)
			result.reserve(expected_array_size);

		istringstream iss(line);

		for (string token; std::getline(iss, token, delimiter); )
		{
			result.push_back(move(token));
		}

		return result;
	}

	Signal_Handler::Signal_Handler()
	:
		signals(io_service)
	{
		signals.add(SIGINT);
		signals.add(SIGTERM);
		signals.async_wait
		(
			boost::bind
			(
				&Signal_Handler::handle_signal,
				this,
				_1,
				_2,
				boost::ref(stop)
			)
		);
	}

	void Signal_Handler::handle_signal
	(
		const boost::system::error_code& error_code,
		const int& signal_number,
		bool& stop
	)
	{
		msg("utils: " + string(strsignal(signal_number)) + " signal was caught with a following error code: " + string(error_code.message()));
		stop = true;

		cleanup();
	}

	boost::asio::io_context& Signal_Handler::get_io_service()
	{
		return io_service;
	}

	Signal_Handler signal_handler;

	bool is_nonprintable_char(char c)
	{
		return !isprint(static_cast<unsigned char>(c));
	}

	void remove_nonprintable_chars(std::string & str)
	{
		str.erase(remove_if(str.begin(),str.end(), is_nonprintable_char), str.end());
	}
}

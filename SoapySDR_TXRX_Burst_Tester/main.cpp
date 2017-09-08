/*
 * SoapySDR_TXTX_Burst_Tester v1.03
 *
 *  Created on: 2017-09-08
 *      Author: ccsh
 *
 * Changelog:
 * v1.00 (2017-09-04)
 *			First version
 * v1.01 (2017-09-06)
 *			Time in future to start stream has been added to runtime configuration
 *			Default values of SDR_Config has been updated to these used by LimeSDR
 *			Changed namespace 'sdr_device' to 'sdr'
 *			Changed last sleep command to usleep in order to support timeouts < 1 s
 * v1.02 (2017-09-07)
 *			Fixed calls to activateStream, readStream and deactivateStream for RX
 *			TX and RX streams MTU is now displayed
 *			Updated configuration so only 100 samples are about to be transmitted or received
 *			Other minor fixes
 * v1.03 (2017-09-08)
 *			Making sure that TX loop will fire after RX loop, also making sure that first TX burst timestamp will be later than RX one
 */
#include <iostream>
#include <stdio.h>
#include <vector>
#include <fstream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/asio/signal_set.hpp>

#include "classes/utils/utils.h"
#include "classes/sdr/SDR_Device_Wrapper.h"

#include <SoapySDR/Time.hpp>

namespace po = boost::program_options;
namespace pt = boost::posix_time;

using namespace std;
using namespace utils;
using namespace sdr;

namespace utils
{
	void cleanup()
	{
		msg("main: Cleaning up...");
	}
}

void rx_thread_function
(
	SDR_Device_Wrapper::sptr_t sdr_device_wrapper,
	vector<complex<float>>& rx_buffer,
	int64_t rx_start_tick,
	int64_t no_of_ticks_per_bursts_period,
	bool rx_active
)
{
	try
	{
		int64_t tick = rx_start_tick;

		if (rx_active)
			msg("rx_thread: RX streaming started!");

		while (!stop)
		{
			if (rx_active)
			{
				sdr_device_wrapper->receive_samples(tick, &rx_buffer[0], rx_buffer.size());
				tick += no_of_ticks_per_bursts_period;
			}
			else
				sleep(1);

			signal_handler.get_io_service().poll();
		}
	}
	catch (const boost::thread_interrupted& e)
	{
		msg("rx_thread: thread has been interrupted.");
	}
	catch (const std::exception& e)
	{
		msg("rx_thread: " + string(e.what()), WARNING);
	}
	catch (...)
	{
		msg("rx_thread: Unexpected exception occurred.", ERROR);
	}
}


int main(int argc, char *argv[])
{
	//disable cout and cerr buffering
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	//following variables are going to be filled while reading cfg file and command line arguments
	string cfg_filepath;

	//prepare usrp_cfg structure on the basis of given configuration
	SDR_Device_Config::sptr_t device_cfg(new SDR_Device_Config);

	//[SIGNAL]
	double burst_period;
	double tx_burst_length;
	double rx_burst_length;
	double rx_tx_separation;
	double time_in_future;

	/*********************************************************************************************************/
	//following code will read the content of a cfg file and command line arguments, print help messages etc.
	po::options_description help("Help options - may be provided via command line arguments");
	help.add_options()
		("help,h", "Prints a list of all available options.")
		("helpg,g", "Prints a list of all general configuration options.")
		;

	po::options_description generic("Generic options - they can be set only via command line arguments");
	generic.add_options()
		("cfg,c", po::value<string>(&cfg_filepath)->default_value("../cfg/cfg_LIME_SDR.ini"), "Path of the cfg *ini file.")
	    ;

	po::options_description cmdline_options;
	cmdline_options.add(help).add(generic);

	boost::shared_ptr<po::variables_map> cfg_sptr(new po::variables_map());
	po::variables_map& cfg = *(cfg_sptr.get());

	po::store(po::command_line_parser(argc, argv).options(cmdline_options).allow_unregistered().run(), cfg);
	po::notify(cfg);

	po::options_description all_visible("");
	all_visible.add(help).add(generic);

	po::options_description general_visible("");
	general_visible.add(help).add(generic);

	if (cfg.count("cfg"))
	{
		po::options_description config("General configuration options - they may be set both via cfg file or via command line arguments (which override cfg ones)");
		config.add_options()
			("SDR.args,d", po::value<string>(&device_cfg->args)						->default_value(""), "Device arguments (e.g. addr=192.168.10.1,type=usrp2)")
			("SDR.clock_source,e", po::value<string>(&device_cfg->clock_source)		->default_value(""), "Device clock source.")
			("SDR.time_source,x", po::value<string>(&device_cfg->time_source)		->default_value(""), "Device time source.")
			("SDR.tx_channel,o", po::value<short>(&device_cfg->channel_tx)			->default_value(0), "Transmitting channel number.")
			("SDR.rx_channel,p", po::value<short>(&device_cfg->channel_rx)			->default_value(0), "Receiving channel number.")
			("SDR.tx_antenna,n", po::value<string>(&device_cfg->antenna_tx)			->default_value("BAND1"), "Transmitting antenna name.")
			("SDR.rx_antenna,a", po::value<string>(&device_cfg->antenna_rx)			->default_value("LNAL"), "Receiving antenna name.")
			("SDR.tx_gain,q", po::value<double>(&device_cfg->G_tx)					->default_value(52), "TX chain gain in [dB].")
			("SDR.rx_gain,w", po::value<double>(&device_cfg->G_rx)					->default_value(20), "RX chain gain in [dB].")
			("SDR.center_frequency,f", po::value<double>(&device_cfg->f_c_tx)		->default_value(1457e6), "Center frequency in [Hz].")
			("SDR.master_clock_rate,m", po::value<double>(&device_cfg->f_clk)		->default_value(133.333333e6), "Master clock rate in [Hz].")
			("SDR.clock_divider,d", po::value<uint16_t>(&device_cfg->D_tx)			->default_value(8), "Master clock divider used to obtain sampling rate in [Hz].")
			("SDR.tx_bandwidth,b", po::value<double>(&device_cfg->B_tx)				->default_value(16.666667), "TX baseband filter bandwidth in [Hz].")
			("SDR.rx_bandwidth,i", po::value<double>(&device_cfg->B_rx)				->default_value(16.666667), "RX baseband filter bandwidth in [Hz].")
			("SDR.timeout,j", po::value<double>(&device_cfg->T_timeout)				->default_value(2), "Streaming timeout in [s].")
			("SDR.tx_thread_active,t", po::value<bool>(&device_cfg->tx_active)		->default_value(true), "Whether to start TX thread or not.")
			("SDR.rx_thread_active,r", po::value<bool>(&device_cfg->rx_active)		->default_value(true), "Whether to start RX thread or not.")

			("SIGNAL.burst_period,u", po::value<double>(&burst_period)				->default_value(100e-3), "TX/RX bursts cycle period in [s].")
			("SIGNAL.tx_burst_length,k", po::value<double>(&tx_burst_length)		->default_value(5e-3), "Length of TX bursts in [s].")
			("SIGNAL.rx_burst_length,l", po::value<double>(&rx_burst_length)		->default_value(5e-3), "Length of RX bursts in [s].")
			("SIGNAL.rx_tx_separation,s", po::value<double>(&rx_tx_separation)		->default_value(1e-3), "RX and TX bursts separation in [s].")
			("SIGNAL.time_in_future,y", po::value<double>(&time_in_future)			->default_value(1), "Time in future to start streaming in [s].")
			;

		cmdline_options.add(config);

		po::options_description config_file_options;
		config_file_options.add(config);

		all_visible.add(config);
		general_visible.add(config);

		ifstream config_stream(cfg["cfg"].as<string>().c_str());

		po::store(po::parse_command_line(argc, argv, cmdline_options), cfg);
		po::store(po::parse_config_file(config_stream, config_file_options), cfg);
		po::notify(cfg);
	}
	else
	{
		cout << all_visible;
		return 0;
	}

	if (cfg.count("help"))
	{
		cout << all_visible;
		return 0;
	}

	if (cfg.count("helpg"))
	{
		cout << general_visible;
		return 0;
	}

	/*********************************************************************************************************/

	device_cfg->D_rx = device_cfg->D_tx;
	device_cfg->f_c_rx = device_cfg->f_c_tx;

	float sampling_rate = device_cfg->f_clk / (double)device_cfg->D_tx;

	device_cfg->buffer_size_tx = tx_burst_length * sampling_rate;
	device_cfg->buffer_size_rx = rx_burst_length * sampling_rate;

	//initialize SDR device with given configuration
	SDR_Device_Wrapper::sptr_t sdr_device_wrapper(new SDR_Device_Wrapper(device_cfg));

	int64_t no_of_ticks_per_bursts_period = device_cfg->D_tx * burst_period * sampling_rate;
	int64_t no_of_tx_samples = tx_burst_length * sampling_rate;
	int64_t no_of_rx_samples = rx_burst_length * sampling_rate;

	vector<complex<float>> tx_buffer(no_of_tx_samples, complex<float>(1.0f, 0.0f));
	vector<complex<float>> rx_buffer(no_of_rx_samples);

	int64_t current_hardware_time = sdr_device_wrapper->get_device()->getHardwareTime();

	int64_t now_tick = SoapySDR::timeNsToTicks(current_hardware_time, device_cfg->f_clk);

	//make sure that first TX timestamp will be later than first RX timestamp
	//(fix for LimeSDR)
	int64_t rx_start_tick = now_tick + SoapySDR::timeNsToTicks((time_in_future + burst_period + rx_tx_separation) * 1e9, device_cfg->f_clk);
	int64_t tx_start_tick = now_tick + SoapySDR::timeNsToTicks((time_in_future + 2 * burst_period) * 1e9, device_cfg->f_clk);

	boost::thread rx_thread = boost::thread
	(
		&rx_thread_function,
		sdr_device_wrapper,
		boost::ref(rx_buffer),
		rx_start_tick,
		no_of_ticks_per_bursts_period,
		device_cfg->rx_active
	);

	try
	{
		int64_t tick = tx_start_tick;

		//make sure that RX loop will start first
		//(fix for LimeSDR)
		usleep((int)(1e6*0.5*device_cfg->T_timeout));

		if (device_cfg->tx_active)
			msg("tx_thread: TX streaming started!");

		while (!stop)
		{
			if (device_cfg->tx_active)
			{
				sdr_device_wrapper->send_samples(tick, &tx_buffer[0], tx_buffer.size());
				tick += no_of_ticks_per_bursts_period;
			}
			else
				sleep(1);

			signal_handler.get_io_service().poll();
		}
	}
	catch (const boost::thread_interrupted& e)
	{
		msg("tx_thread: thread has been interrupted.");
	}
	catch (const std::exception& e)
	{
		msg("tx_thread: " + string(e.what()), WARNING);
	}
	catch (...)
	{
		msg("tx_thread: Unexpected exception occurred.", ERROR);
	}

	usleep((int)1e6*device_cfg->T_timeout);


	msg("All done!\n");
	msg("", INFO, false, false);
	/*********************************************************************************************************/

	return 0;
}

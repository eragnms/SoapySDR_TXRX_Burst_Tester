#include "SDR_Device_Wrapper.h"

#include <iostream>

#include <boost/format.hpp>

#include <SoapySDR/Errors.hpp>
#include <SoapySDR/Time.hpp>
#include <SoapySDR/Logger.hpp>

#include "../utils/utils.h"

using namespace std;
using namespace utils;

namespace sdr_device
{
	SDR_Device_Wrapper::SDR_Device_Wrapper(SDR_Device_Config::sptr_t device_cfg)
	{
		this->device_cfg = device_cfg;

		msg("sdr: Trying to initialize SDR Device...");

		bool init_successfull = false;

		do
		{
			try
			{
				SoapySDR::setLogLevel(SoapySDR::LogLevel::SOAPY_SDR_INFO);

				if (device_cfg->debug_settings)
					msg("sdr: Creating the SDR device with: args=" + device_cfg->args);
				msg("", INFO, false, false);
				device = SoapySDR::Device::make(device_cfg->args);
				msg("", INFO, false, false);

				if (device == nullptr)
					throw(std::runtime_error("Unable to create SDR device!"));
				else if (device_cfg->debug_settings)
					msg("SDR device successfully created!");

				if (device_cfg->clock_source != "")
				{
					//Set clock source
					if (device_cfg->debug_settings)
						msg("sdr: Trying to set the following clock source: " + device_cfg->clock_source);
					device->setClockSource(device_cfg->clock_source);
					if (device_cfg->debug_settings)
						msg("sdr: Actual clock source: " + device->getClockSource());
				}

				if (device_cfg->time_source != "")
				{
					//Set time source
					if (device_cfg->debug_settings)
						msg("sdr: Trying to set the following time source: " + device_cfg->time_source);
					device->setTimeSource(device_cfg->time_source);
					if (device_cfg->debug_settings)
						msg("sdr: Actual time source: " + device->getTimeSource());
				}

				//set the master clock rate
				if (device_cfg->debug_settings)
					msg("sdr: Trying to set the following master clock rate: " + to_string(device_cfg->f_clk/1e6) + " [MHz]");
				msg("", INFO, false, false);
				device->setMasterClockRate(device_cfg->f_clk);
				if (device_cfg->debug_settings)
					msg("sdr: Actual master clock rate: " + to_string(device->getMasterClockRate()/1e6) + " [MHz]");

				if (device_cfg->tx_active)
				{
					//set the tx sample rate
					if (device_cfg->debug_settings)
						msg("sdr: Trying to set the following TX sample rate on channel " + to_string(device_cfg->channel_tx) + ": " + to_string((device_cfg->f_clk / (double)device_cfg->D_tx)/1e6) + " [MHz]");
					device->setSampleRate(SOAPY_SDR_TX, device_cfg->channel_tx, device_cfg->f_clk / (double)device_cfg->D_tx);
					if (device_cfg->debug_settings)
						msg("sdr: Actual TX sample rate on channel " + to_string(device_cfg->channel_tx) + ": " + to_string(device->getSampleRate(SOAPY_SDR_TX, device_cfg->channel_tx)/1e6) + " [MHz]");
				}

				if (device_cfg->rx_active)
				{
					//set the rx sample rate
					if (device_cfg->debug_settings)
						msg("sdr: Trying to set the following RX sample rate on channel " + to_string(device_cfg->channel_rx) + ": " + to_string((device_cfg->f_clk / (double)device_cfg->D_rx)/1e6) + " [MHz]");
					device->setSampleRate(SOAPY_SDR_RX, device_cfg->channel_rx, device_cfg->f_clk / (double)device_cfg->D_rx);
					if (device_cfg->debug_settings)
						msg("sdr: Actual RX sample rate on channel " + to_string(device_cfg->channel_rx) + ": " + to_string(device->getSampleRate(SOAPY_SDR_RX, device_cfg->channel_rx)/1e6) + " [MHz]");
				}

				if ((device_cfg->tx_active) && (device_cfg->B_tx > 0))
				{
					//set the tx bandwidth
					if (device_cfg->debug_settings)
						msg("sdr: Trying to set the following TX filter bandwidth on channel " + to_string(device_cfg->channel_tx) + ": " + to_string(device_cfg->B_tx / 1e6) + " [MHz]");
					device->setBandwidth(SOAPY_SDR_TX, device_cfg->channel_tx, device_cfg->B_tx);
					if (device_cfg->debug_settings)
						msg("sdr: Actual TX bandwidth on channel " + to_string(device_cfg->channel_tx) + ": " + to_string(device->getBandwidth(SOAPY_SDR_TX, device_cfg->channel_tx)/1e6) + " [MHz]");
				}

				if ((device_cfg->rx_active) && (device_cfg->B_rx > 0))
				{
					//set the tx bandwidth
					if (device_cfg->debug_settings)
						msg("sdr: Trying to set the following RX filter bandwidth on channel " + to_string(device_cfg->channel_rx) + ": " + to_string(device_cfg->B_rx / 1e6) + " [MHz]");
					device->setBandwidth(SOAPY_SDR_RX, device_cfg->channel_rx, device_cfg->B_rx);
					if (device_cfg->debug_settings)
						msg("sdr: Actual RX bandwidth on channel " + to_string(device_cfg->channel_rx) + ": " + to_string(device->getBandwidth(SOAPY_SDR_RX, device_cfg->channel_rx)/1e6) + " [MHz]");
				}

				if (device_cfg->tx_active)
				{
					//set the tx RF chain gain
					if (device_cfg->debug_settings)
						msg("sdr: Trying to set the following TX gain on channel " + to_string(device_cfg->channel_tx) + ": " + to_string(device_cfg->G_tx) + " [dB]");
					device->setGain(SOAPY_SDR_TX, device_cfg->channel_tx, device_cfg->G_tx);
					if (device_cfg->debug_settings)
						msg("sdr: Actual TX gain on channel " + to_string(device_cfg->channel_tx) + ": " + to_string(device->getGain(SOAPY_SDR_TX, device_cfg->channel_tx)) + " [dB]");
				}

				if (device_cfg->rx_active)
				{
					//set the rx RF chain gain
					if (device_cfg->debug_settings)
						msg("sdr: Trying to set the following RX gain on channel " + to_string(device_cfg->channel_rx) + ": " + to_string(device_cfg->G_rx) + " [dB]");
					device->setGain(SOAPY_SDR_RX, device_cfg->channel_rx, device_cfg->G_rx);
					if (device_cfg->debug_settings)
						msg("sdr: Actual RX gain on channel " + to_string(device_cfg->channel_rx) + ": " + to_string(device->getGain(SOAPY_SDR_RX, device_cfg->channel_rx)) + " [dB]");
				}

				if (device_cfg->tx_active)
				{
					//set the tx antenna
					if (device_cfg->debug_settings)
						msg("sdr: Trying to set the following TX antenna on channel " + to_string(device_cfg->channel_tx) + ": " + device_cfg->antenna_tx);
					device->setAntenna(SOAPY_SDR_TX, device_cfg->channel_tx, device_cfg->antenna_tx);
					if (device_cfg->debug_settings)
						msg("sdr: Actual TX antenna on channel " + to_string(device_cfg->channel_tx) + ": " + device->getAntenna(SOAPY_SDR_TX, device_cfg->channel_tx));
				}

				if (device_cfg->rx_active)
				{
					//set the rx antenna
					if (device_cfg->debug_settings)
						msg("sdr: Trying to set the following RX antenna on channel " + to_string(device_cfg->channel_rx) + ": " + device_cfg->antenna_rx);
					device->setAntenna(SOAPY_SDR_RX, device_cfg->channel_rx, device_cfg->antenna_rx);
					if (device_cfg->debug_settings)
						msg("sdr: Actual RX antenna on channel " + to_string(device_cfg->channel_rx) + ": " + device->getAntenna(SOAPY_SDR_RX, device_cfg->channel_rx));
				}

				if (device_cfg->tx_active)
				{
					//Set tx frequency
					if (device_cfg->debug_settings)
						msg("sdr: Trying to set the following TX frequency on channel " + to_string(device_cfg->channel_tx) + ": " + to_string(device_cfg->f_c_tx/1e6) + " [MHz]");

					device->setFrequency(SOAPY_SDR_TX, device_cfg->channel_tx, device_cfg->f_c_tx);

					//Check TX LO lock...
					msg("sdr: Waiting for TX LO lock on channel " + to_string(device_cfg->channel_tx) + "...");

					bool tx_lo_locked = false;

					while (!(stop || tx_lo_locked))
					{
						string tx_locked = device->readSensor(SOAPY_SDR_TX, device_cfg->channel_tx, "lo_locked");

						if (tx_locked == "true")
							tx_lo_locked = true;

						usleep(100);
						signal_handler.get_io_service().poll();
					}

					msg("sdr: TX LO lock detected on channel " + to_string(device_cfg->channel_tx) + "!");

					if (device_cfg->debug_settings)
						msg("sdr: Actual TX frequency on channel " + to_string(device_cfg->channel_tx) + ": " + to_string(device->getFrequency(SOAPY_SDR_TX, device_cfg->channel_tx)/1e6) + " [MHz]");
				}

				if (device_cfg->rx_active)
				{
					//Set rx frequency
					if (device_cfg->debug_settings)
						msg("sdr: Trying to set the following RX frequency on channel " + to_string(device_cfg->channel_rx) + ": " + to_string(device_cfg->f_c_rx/1e6) + " [MHz]");

					device->setFrequency(SOAPY_SDR_RX, device_cfg->channel_rx, device_cfg->f_c_rx);

					//Check RX LO lock...
					msg("sdr: Waiting for RX LO lock on channel " + to_string(device_cfg->channel_rx) + "...");

					bool rx_lo_locked = false;

					while (!(stop || rx_lo_locked))
					{
						string rx_locked = device->readSensor(SOAPY_SDR_RX, device_cfg->channel_rx, "lo_locked");

						if (rx_locked == "true")
							rx_lo_locked = true;

						usleep(100);
						signal_handler.get_io_service().poll();
					}

					msg("sdr: RX LO lock detected on channel " + to_string(device_cfg->channel_rx) + "!");

					if (device_cfg->debug_settings)
						msg("sdr: Actual RX frequency on channel " + to_string(device_cfg->channel_rx) + ": " + to_string(device->getFrequency(SOAPY_SDR_RX, device_cfg->channel_rx)/1e6) + " [MHz]");
				}

				if (device_cfg->tx_active)
				{
					//setup TX stream
					if (device_cfg->debug_settings)
						msg("sdr: Trying to setup TX stream...");
					tx_stream = device->setupStream(SOAPY_SDR_TX, "CF32", vector<size_t>{(size_t)device_cfg->channel_tx});

					if (tx_stream == nullptr)
						throw(std::runtime_error("Unable to setup TX stream!"));
					else if (device_cfg->debug_settings)
						msg("sdr: TX stream has been successfully set up!");
				}

				if (device_cfg->rx_active)
				{
					//setup RX stream
					if (device_cfg->debug_settings)
						msg("sdr: Trying to setup RX stream...");
					rx_stream = device->setupStream(SOAPY_SDR_RX, "CF32", vector<size_t>{(size_t)device_cfg->channel_rx});

					if (rx_stream == nullptr)
						throw(std::runtime_error("Unable to setup RX stream!"));
					else if (device_cfg->debug_settings)
						msg("sdr: RX stream has been successfully set up!");
				}

				if (device_cfg->tx_active)
				{
					//activate TX stream
					if (device_cfg->debug_settings)
						msg("sdr: Trying to activate TX stream...");

					int ret = device->activateStream(tx_stream, SOAPY_SDR_HAS_TIME);

					if (ret != 0)
						msg("sdr: Following problem occurred while activating TX stream: " + string(SoapySDR::errToStr(ret)), ERROR);
					else if (device_cfg->debug_settings)
						msg("sdr: TX stream has been successfully activated!");
				}


				if (device_cfg->rx_active)
				{
					//activate RX stream
					if (device_cfg->debug_settings)
						msg("sdr: Trying to activate RX stream...");

					int ret = device->activateStream(rx_stream, SOAPY_SDR_HAS_TIME);

					if (ret != 0)
						msg("sdr: Following problem occurred while activating RX stream: " + string(SoapySDR::errToStr(ret)), ERROR);
					else if (device_cfg->debug_settings)
						msg("sdr: RX stream has been successfully activated!");
				}

				init_successfull = true;
			}
			catch (const std::exception& e)
			{
				msg("sdr: Following exception occurred during initialization of the SDR device: " + string(e.what()), ERROR);
			}
			catch (...)
			{
				msg("sdr: Unexpected exception was caught during initialization of the SDR device", ERROR);
			}

			signal_handler.get_io_service().poll();
		}
		while ((!stop) && (!init_successfull));
	}

	SDR_Device_Wrapper::~SDR_Device_Wrapper()
	{
		if (device != nullptr)
		{
			if ((device_cfg->tx_active) && (tx_stream != nullptr))
			{
				int ret = device->deactivateStream(tx_stream, SOAPY_SDR_HAS_TIME | SOAPY_SDR_END_BURST);

				if (ret == 0)
					msg("sdr: TX stream has been successfully deactivated!");
				else
					msg("sdr: Following problem occurred while deactivating TX stream: " + string(SoapySDR::errToStr(ret)), ERROR);

				device->closeStream(tx_stream);
			}

			usleep(100);

			if ((device_cfg->rx_active) && (rx_stream != nullptr))
			{
				int ret = device->deactivateStream(rx_stream, SOAPY_SDR_HAS_TIME);

				if (ret == 0)
					msg("sdr: RX stream has been successfully deactivated!");
				else
					msg("sdr: Following problem occurred while deactivating RX stream: " + string(SoapySDR::errToStr(ret)), ERROR);

				device->closeStream(rx_stream);
			}

			usleep(100);

			SoapySDR::Device::unmake(device);

			msg("", INFO, false, false);
		}
	}

	bool SDR_Device_Wrapper::send_samples(const int64_t tick, const std::complex<float>* samples, int no_of_requested_samples, const bool ack)
	{
		bool res = false;

		std::vector<std::complex<float>> buf(1, {0, 0});

		if (no_of_requested_samples <= 0)
		{
			samples = &buf[0];
			no_of_requested_samples = buf.size();
		}

		string tx_verbose_msg = "[TX] ";

		int64_t burst_time = SoapySDR::ticksToTimeNs(tick, device_cfg->f_clk);
		int64_t current_time = device->getHardwareTime();
		//int64_t current_tick = SoapySDR::timeNsToTicks(current_time, device_cfg->f_clk);
		tx_verbose_msg += " current_time=" + to_string(current_time) + ", burst_time: " + to_string(burst_time);

		const vector<void*> buffs(1, (void*)samples);
		const void* const* buffs_ptr = &buffs[0];

		int flags = SOAPY_SDR_HAS_TIME | SOAPY_SDR_END_BURST;

		int no_of_transmitted_samples = device->writeStream
		(
			tx_stream,
			buffs_ptr,
			no_of_requested_samples,
			flags,
			burst_time,
			1e6 * device_cfg->T_timeout
		);

		if (no_of_transmitted_samples != no_of_requested_samples)
		{
			tx_verbose_msg += ", write_code: ";

			switch (no_of_transmitted_samples)
			{
				case SOAPY_SDR_TIMEOUT:
					tx_verbose_msg += "SOAPY_SDR_TIMEOUT";
					break;
				case SOAPY_SDR_STREAM_ERROR:
					tx_verbose_msg += "SOAPY_SDR_STREAM_ERROR";
					break;
				case SOAPY_SDR_CORRUPTION:
					tx_verbose_msg += "SOAPY_SDR_CORRUPTION";
					break;
				case SOAPY_SDR_OVERFLOW:
					tx_verbose_msg += "SOAPY_SDR_OVERFLOW";
					break;
				case SOAPY_SDR_NOT_SUPPORTED:
					tx_verbose_msg += "SOAPY_SDR_NOT_SUPPORTED";
					break;
				case SOAPY_SDR_END_BURST:
					tx_verbose_msg += "SOAPY_SDR_END_BURST";
					break;
				case SOAPY_SDR_TIME_ERROR:
					tx_verbose_msg += "SOAPY_SDR_TIME_ERROR";
					break;
				case SOAPY_SDR_UNDERFLOW:
					tx_verbose_msg += "SOAPY_SDR_UNDERFLOW";
					break;
			}

			tx_verbose_msg += (boost::format(", no_of_transmitted_samples: %u/%u") % no_of_transmitted_samples % no_of_requested_samples).str();
			msg("sdr: " + tx_verbose_msg, WARNING);
			res = false;
		}
		else
		{
			if (!ack)
			{
				tx_verbose_msg += (boost::format(", no_of_transmitted_samples: %u/%u") % no_of_transmitted_samples % no_of_requested_samples).str();
				msg(tx_verbose_msg);
				res = true;
			}
			else
			{
				tx_verbose_msg += ", ack_code: ";

				size_t chan_mask = (1 << device_cfg->channel_tx);

				int stream_status = device->readStreamStatus
				(
					tx_stream,
					chan_mask,
					flags,
					burst_time,
					1e6 * device_cfg->T_timeout
				);

				switch(stream_status)
				{
					case SOAPY_SDR_TIMEOUT:
						tx_verbose_msg += "SOAPY_SDR_TIMEOUT";
						break;
					case SOAPY_SDR_STREAM_ERROR:
						tx_verbose_msg += "SOAPY_SDR_STREAM_ERROR";
						break;
					case SOAPY_SDR_CORRUPTION:
						tx_verbose_msg += "SOAPY_SDR_CORRUPTION";
						break;
					case SOAPY_SDR_OVERFLOW:
						tx_verbose_msg += "SOAPY_SDR_OVERFLOW";
						break;
					case SOAPY_SDR_NOT_SUPPORTED:
						tx_verbose_msg += "SOAPY_SDR_NOT_SUPPORTED";
						break;
					case SOAPY_SDR_END_BURST:
						tx_verbose_msg += "SOAPY_SDR_END_BURST";
						break;
					case SOAPY_SDR_TIME_ERROR:
						tx_verbose_msg += "SOAPY_SDR_TIME_ERROR";
						break;
					case SOAPY_SDR_UNDERFLOW:
						tx_verbose_msg += "SOAPY_SDR_UNDERFLOW";
						break;
					default:
						tx_verbose_msg += "NO_ERROR";
						break;
				}

				tx_verbose_msg += (boost::format(", no_of_transmitted_samples: %u/%u") % no_of_transmitted_samples % no_of_requested_samples).str();

				if ((stream_status == SOAPY_SDR_END_BURST) && (no_of_transmitted_samples == no_of_requested_samples))
				{
					msg("sdr: " + tx_verbose_msg);
					res = true;
				}
				else
				{
					msg("sdr: " + tx_verbose_msg, WARNING);
					res = false;
				}
			}
		}

		return res;
	}

	void SDR_Device_Wrapper::send_samples_void(bool& success, const int64_t tick, const std::complex<float>* samples, int no_of_requested_samples, const bool ack)
	{
		success = send_samples(tick, samples, no_of_requested_samples, ack);
	}

	bool SDR_Device_Wrapper::receive_samples(const int64_t tick, std::complex<float>* samples, int no_of_requested_samples)
	{
		bool res = false;

		std::vector<std::complex<float>> buf(1, {0, 0});

		if (no_of_requested_samples <= 0)
		{
			samples = &buf[0];
			no_of_requested_samples = buf.size();
		}

		string rx_verbose_msg = "[RX] ";

		int64_t burst_time = SoapySDR::ticksToTimeNs(tick, device_cfg->f_clk);
		int64_t current_time = device->getHardwareTime();
		//int64_t current_tick = SoapySDR::timeNsToTicks(current_time, device_cfg->f_clk);
		rx_verbose_msg += " current_time=" + to_string(current_time) + ", burst_time: " + to_string(burst_time);

		vector<void*> buffs(1, (void*)samples);
		void* const* buffs_ptr = &buffs[0];

		int flags = SOAPY_SDR_HAS_TIME;

		int no_of_received_samples = device->readStream
		(
			rx_stream,
			buffs_ptr,
			no_of_requested_samples,
			flags,
			burst_time,
			1e6 * device_cfg->T_timeout
		);

		rx_verbose_msg += ", read_code: ";

		if (no_of_received_samples == no_of_requested_samples)
		{
			rx_verbose_msg += "NO_ERROR";
		}
		else
		{
			switch (no_of_received_samples)
			{
				case SOAPY_SDR_TIMEOUT:
					rx_verbose_msg += "SOAPY_SDR_TIMEOUT";
					break;
				case SOAPY_SDR_STREAM_ERROR:
					rx_verbose_msg += "SOAPY_SDR_STREAM_ERROR";
					break;
				case SOAPY_SDR_CORRUPTION:
					rx_verbose_msg += "SOAPY_SDR_CORRUPTION";
					break;
				case SOAPY_SDR_OVERFLOW:
					rx_verbose_msg += "SOAPY_SDR_OVERFLOW";
					break;
				case SOAPY_SDR_NOT_SUPPORTED:
					rx_verbose_msg += "SOAPY_SDR_NOT_SUPPORTED";
					break;
				case SOAPY_SDR_END_BURST:
					rx_verbose_msg += "SOAPY_SDR_END_BURST";
					break;
				case SOAPY_SDR_TIME_ERROR:
					rx_verbose_msg += "SOAPY_SDR_TIME_ERROR";
					break;
				case SOAPY_SDR_UNDERFLOW:
					rx_verbose_msg += "SOAPY_SDR_UNDERFLOW";
					break;
			}
		}

		rx_verbose_msg += (boost::format(", no_of_received_samples: %u/%u") % no_of_received_samples % no_of_requested_samples).str();

		if (no_of_received_samples == no_of_requested_samples)
		{
			msg("sdr: " + rx_verbose_msg);
			res = true;
		}
		else
		{
			msg("sdr: " + rx_verbose_msg, WARNING);
			res = false;
		}

		return res;
	}

	void SDR_Device_Wrapper::receive_samples_void(bool& success,const int64_t tick, std::complex<float>* samples, int no_of_requested_samples)
	{
		success = receive_samples(tick, samples, no_of_requested_samples);
	}

	SoapySDR::Device* SDR_Device_Wrapper::get_device()
	{
		return device;
	}

	SDR_Device_Config::sptr_t SDR_Device_Wrapper::get_device_config()
	{
		return device_cfg;
	}
}

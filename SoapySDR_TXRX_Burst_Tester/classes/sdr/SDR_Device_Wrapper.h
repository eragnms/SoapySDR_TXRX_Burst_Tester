#ifndef __SDR_DEVICE_WRAPPER_H__
#define __SDR_DEVICE_WRAPPER_H__

#include <fstream>

#include <boost/thread.hpp>

#include <SoapySDR/Device.hpp>

#include "SDR_Config.h"

namespace sdr_device
{
	class SDR_Device_Wrapper
	{
	private:
		SoapySDR::Device* device = nullptr;
		SDR_Device_Config::sptr_t device_cfg;

		SoapySDR::Stream* tx_stream = nullptr;
		SoapySDR::Stream* rx_stream = nullptr;

	public:
		typedef boost::shared_ptr<SDR_Device_Wrapper> sptr_t;

		SDR_Device_Wrapper(SDR_Device_Config::sptr_t device_cfg);
		~SDR_Device_Wrapper();

		bool send_samples(const int64_t tick, const std::complex<float>* samples, int no_of_requested_samples, const bool ack = true);
		void send_samples_void(bool& success, const int64_t tick, const std::complex<float>* samples, int no_of_requested_samples, const bool ack = true);

		bool receive_samples(const int64_t tick, std::complex<float>* samples, int no_of_requested_samples);
		void receive_samples_void(bool& success, const int64_t tick, std::complex<float>* samples, int no_of_requested_samples);

		SoapySDR::Device* get_device();
		SDR_Device_Config::sptr_t get_device_config();
	};
}

#endif

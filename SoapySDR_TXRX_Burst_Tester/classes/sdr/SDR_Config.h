#ifndef __SDR_DEVICE_CONFIG_H__
#define __SDR_DEVICE_CONFIG_H__

#include <stdint.h>

namespace sdr_device
{
	struct SDR_Device_Config
	{
		typedef boost::shared_ptr<SDR_Device_Config> sptr_t;

		std::string args = "";
		std::string clock_source = "";
		std::string time_source = "";
		double f_clk = 33.333333e6;
		short channel_tx = 0;
		short channel_rx = 0;
		uint16_t D_tx = 2;
		uint16_t D_rx = D_tx;
		double B_tx = 0;
		double B_rx = 0;
		double G_tx = 89.75;
		double G_rx = 76;
		std::string antenna_tx = "TX/RX";
		std::string antenna_rx = "RX2";
		double f_c_tx = 1457e6;
		double f_c_rx = f_c_tx;
		double T_timeout = 10;
		bool debug_settings = true;
		int64_t buffer_size_tx = 0;
		int64_t buffer_size_rx = 0;
		bool tx_active = true;
		bool rx_active = true;
	};
}

#endif

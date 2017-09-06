#ifndef __SDR_DEVICE_CONFIG_H__
#define __SDR_DEVICE_CONFIG_H__

#include <stdint.h>

namespace sdr
{
	struct SDR_Device_Config
	{
		typedef boost::shared_ptr<SDR_Device_Config> sptr_t;

		std::string args = "";
		std::string clock_source = "";
		std::string time_source = "";
		double f_clk = 133.333333e6;
		short channel_tx = 0;
		short channel_rx = 0;
		uint16_t D_tx = 8;
		uint16_t D_rx = D_tx;
		double B_tx = 16.666667e6;
		double B_rx = 16.666667e6;
		double G_tx = 52;
		double G_rx = 20;
		std::string antenna_tx = "BAND1";
		std::string antenna_rx = "LNAL";
		double f_c_tx = 1457e6;
		double f_c_rx = f_c_tx;
		double T_timeout = 2;
		bool debug_settings = true;
		int64_t buffer_size_tx = 0;
		int64_t buffer_size_rx = 0;
		bool tx_active = true;
		bool rx_active = true;
	};
}

#endif

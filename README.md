# SoapySDR_TXRX_Burst_Tester

This application is a simple example of using both TX and RX calls from UHD API in order to achieve burst-based transmission (like in TDD). TX operations are performed in main thread, while another thread is spawned for dealing with RX operations. By default configuration, device is suppose to send TX burst (5ms), wait for 1ms, receive RX burst (5ms), and wait for another 89ms. Then the whole cycle is repeated. For sake of simplicity, application does not support MIMO configuration.

Dependencies: boost soapysdr

compilation flags: -D__GXX_EXPETIMENTAL_CXX0X__ -std=c++0x

Program can be compiled as follows:

cd UHD_TXRX_Burst_Tester/Release make

Help screen may be found with a following command:

./UHD_TXRX_Burst_Tester -h

This is the example of using this application with USRP B200 mini device:

./UHD_TXRX_Burst_Tester -c ../cfg/cfg_USRP_B200_MINI.ini

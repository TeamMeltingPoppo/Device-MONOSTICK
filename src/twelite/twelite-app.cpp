// use twelite mwx c++ template library
#include <TWELITE>
#include <NWK_SIMPLE>

/*** Config part 
 * Meisterと同様のセンサーネットワークシステムであるため、Meisterと相談して設定すること
*/


// application ID

const uint32_t APP_ID = 0x1234dcba;

// channel
const uint8_t CHANNEL = 13;

// Global Variables

/*** setup procedure (run once at cold boot) */
void setup()
{
	/*** SETUP section */

	// the twelite main class
	the_twelite
		<< TWENET::appid(APP_ID)	// set application ID (identify network group)
		<< TWENET::channel(CHANNEL) // set channel (pysical channel)
		<< TWENET::rx_when_idle();	// open receive circuit (if not set, it can't listen packts from others)

	auto &&nwksmpl = the_twelite.network.use<NWK_SIMPLE>();
	nwksmpl << NWK_SIMPLE::logical_id(0x01) // set Logical ID. (0x00 means a parent device)
			<< NWK_SIMPLE::dup_check(16, 50, 5)								  // set
			<< NWK_SIMPLE::repeat_max(0);									  // can repeat a packet up to three times. (being kind of a router)
	the_twelite.begin();													  // start twelite!

	// Serial.begin(9600); // defaultでは115200bpsで通信
}

/*** loop procedure (called every event) */
void loop()
{
}

void on_rx_packet(packet_rx &rx, bool_t &handled)
{
	auto &&pkt = rx.get_payload();
	uint8_t cobs_buf[256];
	auto len = rx.get_length();
	uint8_t cobs_buf_idx = 0;

	for (uint8_t i = 0; i < len; i++)
	{
		if (pkt[i] == 0x00)
		{
			Serial1.write(cobs_buf_idx + 1);
			Serial.write(cobs_buf_idx + 1);
			for (uint8_t j = 0; j < cobs_buf_idx; j++)
			{
				Serial1.write(cobs_buf[j]);
				Serial.write(cobs_buf[j]);
			}
			cobs_buf_idx = 0;
		}
		else
		{
			cobs_buf[cobs_buf_idx] = pkt[i];
			cobs_buf_idx++;
		}
	}
	Serial1.write(cobs_buf_idx + 1);
	Serial.write(cobs_buf_idx + 1);
	for (uint8_t j = 0; j < cobs_buf_idx; j++)
	{
		Serial1.write(cobs_buf[j]);
		Serial.write(cobs_buf[j]);
	}
	Serial1.write(0x00);
	Serial.write(0x00);
}

/* Copyright (C) 2019-2021 Mono Wireless Inc. All Rights Reserved. *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE     *
 * AGREEMENT).                                                     */
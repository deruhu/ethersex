/* (aus: ethernet_ecdm:)
 * (Copyright (C) 2007, 2008 Christian Dietrich <stettberger@dokucode.de>)
 * (Copyright (c) 2009 by David Gräff <david.graeff@web.de>)
 *
 * Copyright (c) 2012 Sebastian Noack <weidenhocker@gmx.net>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License (either version 2 or
 * version 3) as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <string.h>
#include "udpkuechenlicht.h"
#include "protocols/uip/uip.h"
#include "protocols/uip/uip_router.h"
#include "core/debug.h"
#include "protocols/ecmd/parser.h"
#include "protocols/ecmd/ecmd-base.h"

#include "config.h"

#define BUF ((struct uip_udpip_hdr *) (uip_appdata - UIP_IPUDPH_LEN))

kuechenLichtStatus status={
		{
		 KUECHENLICHT_MAGIC,
		 0,
		 0,
		 KUECHENLICHT_ID_STAT_STATUS,
		 sizeof(kuechenLichtStatus)
		},
		{
		 0,
		 0,
		 0,
		 0,
		},
		 0};

void kuechenproto_net_init() {
	uip_ipaddr_t ip;
	uip_ipaddr_copy(&ip, all_ones_addr);

	uip_udp_conn_t *kuechen_conn = uip_udp_new(&ip, 0, kuechenproto_net_main);
	if(! kuechen_conn) {
		debug_printf("Kuenchenlicht: udp failed\n");
		return;
	}

	uip_udp_bind (kuechen_conn, HTONS(KUECHENLICHT_UDP_PORT));

	DDRB |= (1<<OC1A) | (1<<OC1B); // Ports OC1A, OC1B mit angeschlossener LED als Ausgang
	TCCR1A = (1<<WGM11) | (1<<WGM12) | (1<<WGM13) & ~(1<<WGM10) | (1<<COM1A1) | (1<<COM1B1); // PWM, fast, 16 bit.
	TCCR1B = (1<<CS10) & ~(1<<CS11) & ~(1<<CS12); // Prescaler 1 = Enable counter

	TCCR2B = (1<<CS21);
	TIMSK2 = (1<<OCIE2A)|(1<<TOIE2)

	ICR1=0xffff;

	OCR1A=0;
	OCR1B=0;

	OCR2A=0x0;
}

void kuechenproto_net_main() {
	if (!uip_newdata ())
		return;
	uint16_t datalength=uip_datalen();
	int16_t i=0;

	while((datalength-i)>=sizeof(kuechenLichtHeader))
	{
		kuechenLichtHeader* pParseHeader = (kuechenLichtHeader*)(uip_appdata+i);
		if(*pParseHeader->magic!=KUECHENLICHT_MAGIC)
		{
			debug_printf("Kuenchenlicht: received non-Kuechenlicht message (wrong header->magic)\n");
			i++;
		}

		else
		{
			debug_printf("Kuenchenlicht: received message, parsing started\n");


			switch (pParseHeader->messageID)
			{
			case KUECHENLICHT_ID_CMD_SET:
				if ((datalength-i)>=sizeof(kuechenLicht_cmd_set))
					handleKuechenLichtSetCommand((kuechenLicht_cmd_set*)pParseHeader);
					i+=sizeof(kuechenLicht_cmd_set);
				break;

			default:
				debug_printf("Kuenchenlicht: unknown messageID received\n");
				i++;
				break;
			}
		}
	}



		for(i=0;i<4;++i)
		{
			led[i]=*(++pMessage);
		}

		if led[3]!=led[0]^led[1]^led[2];
		{
			debug_printf("Kuechenlicht: checksum-check failed\n");
			return;
		}
		else debug_printf("Kuenchenlicht: colour received\n");



		/* Add \0 to the data and remove \n from the data */
		do
		{
			if (*pMessage == '\r' || *pMessage == '\n') {
				break;
			}
		} while ( ++pMessage <= ((char *)uip_appdata + uip_datalen()));

		/* Parse the Data */
		*pMessage = 0;
		char cmd[pMessage - (char *)uip_appdata];

		strncpy(cmd, uip_appdata, pMessage - (char *)uip_appdata + 1);

		uip_slen = 0;
		while (uip_slen < UIP_BUFSIZE - UIP_IPUDPH_LEN) {
			int16_t len = ecmd_parse_command(cmd, ((char *)uip_appdata) + uip_slen,
					(UIP_BUFSIZE - UIP_IPUDPH_LEN) - uip_slen);
			uint8_t real_len = len;
			if (!is_ECMD_FINAL(len)) { /* what about the errors ? */
				/* convert ECMD_AGAIN back to ECMD_FINAL */
				real_len = (uint8_t) ECMD_AGAIN(len);
			}
			uip_slen += real_len + 1;
			((char *)uip_appdata)[uip_slen - 1] = '\n';
			if (real_len == len || len == 0)
				break;
		}

		/* Sent data out */

		uip_udp_conn_t echo_conn;
		uip_ipaddr_copy(echo_conn.ripaddr, BUF->srcipaddr);
		echo_conn.rport = BUF->srcport;
		echo_conn.lport = HTONS(ECMD_UDP_PORT);

		uip_udp_conn = &echo_conn;
		uip_process(UIP_UDP_SEND_CONN);
		router_output();

		uip_slen = 0;

}

void handleKuechenLichtSetCommand(kuechenLicht_cmd_set* pSetCommand)
{
	if(pSetCommand->checksum==(pSetCommand->kLEDStatus.rot ^ pSetCommand->kLEDStatus.blau ^ pSetCommand->kLEDStatus.gruen))
		status.kLEDStatus=pSetCommand->kLEDStatus;

	return;
}

/*
  -- Ethersex META --
  header(protocols/kuechenlichtprotikoll/udpkuechenlicht.h)
  net_init(kuechenproto_net_init)
*/

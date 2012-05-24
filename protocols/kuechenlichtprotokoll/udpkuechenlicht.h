/*
 * Copyright (C) 2012 Sebastian Noack <weidenhocker@gmx.net>
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

#ifndef KUECHENPROTO_UDP_H
#define KUECHENPROTO_UDP_H

//Port:
#define 	KUECHENLICHT_UDP_BC_MYPORT	13897
#define 	KUECHENLICHT_UDP_BC_PORT	13898
#define         KUECHENLICHT_UDP_CMD_PORT       13899
#define         KUECHENLICHT_UDP_TEST_PORT      13901

//magic
#define		KUECHENLICHT_MAGIC		0xABCD

//Nachrichten:

//Commands:
#define		KUECHENLICHT_ID_CMD_SET		0xC001
#define		KUECHENLICHT_ID_CMD_CAYF	0xC002
#define         KUECHENLICHT_ID_CMD_GOTCHA      0xC003

//Responds:
#define		KUECHENLICHT_ID_RSP_SET		0xA001
#define		KUECHENLICHT_ID_RSP_CAYF	0xA002

//Stati:
#define		KUECHENLICHT_ID_STAT_STATUS	0xB001

typedef struct
{
	uint16_t	magic;
	uint32_t        modulAddress;
	uint32_t	senderAddress;
	uint16_t	messageID;
	uint16_t	messageLength;
}kuechenLichtHeader;

typedef struct
{
	uint8_t		rot;
	uint8_t		gruen;
	uint8_t		blau;
}kuechenLichtLEDStatus;

typedef struct
{
	kuechenLichtHeader                      kHeader;

	kuechenLichtLEDStatus                   kLEDStatus;

	uint8_t					checksum;
}kuechenLicht_cmd_set;

typedef struct
{
	kuechenLichtHeader                      kHeader;

	kuechenLichtLEDStatus                   kLEDStatus;

	uint8_t					checksum;

	bool					an;
}kuechenLicht_rsp_set;

typedef struct
{
	kuechenLichtHeader                      kHeader;

	uint8_t					kNumberofCalls;

}kuechenLicht_cmd_come_all_ye_faithful;

typedef struct
{
        kuechenLichtHeader                      kHeader;

        kuechenLichtLEDStatus                   kLEDStatus;

        uint8_t                                 kReactedtoCall;

	bool					an;
}kuechenLicht_rsp_cayf;

typedef struct
{
        kuechenLichtHeader                      kHeader;

        bool					gotcha;
}kuechenLicht_cmd_gotcha;


typedef struct
{
	kuechenLichtHeader                      kHeader;

	kuechenLichtLEDStatus                   kLEDStatus;

}kuechenLichtStatus;


void kuechenproto_net_init(void);
void kuechenproto_net_main(void);
void handleKuechenLichtSetCommand(kuechenLicht_cmd_set*);


#endif /* KUECHENLICHT_UDP_H */

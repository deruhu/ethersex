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

//magic
#define		KUECHENLICHT_MAGIC			0xABCD

//Nachrichten:

//Commands:
#define		KUECHENLICHT_ID_CMD_SET		0xC001

//Responds:
#define		KUECHENLICHT_ID_RSP_SET		0xA001

//Stati:
#define		KUECHENLICHT_ID_STAT_STATUS	0xB001

typedef struct
{
	uint16_t	magic;
	uint32_t	senderAddress;
	uint16_t	messageID;
	uint16_t	messageLength;
}kuechenLichtHeader;

typedef struct
{
	uint8_t		rot;
	uint8_t		gruen;
	uint8_t		blau;
	bool		schalter;
}kuechenLichtLEDStatus;

typedef struct
{
	kuechenLichtHeader		kHeader;

	kuechenLichtLEDStatus	kLEDStatus;

	uint8_t					checksum;
	uint8_t					lock;
	uint8_t					prioritaet;
}kuechenLicht_cmd_set;

typedef struct
{
	kuechenLichtHeader		kHeader;

	kuechenLichtLEDStatus	kLEDStatus;

	uint8_t					checksum;
	bool					done;
}kuechenLicht_rsp_set;



typedef struct
{
	kuechenLichtHeader		kHeader;

	kuechenLichtLEDStatus	kLEDStatus;

	uint32_t				lastSender;

}kuechenLichtStatus;


void kuechenproto_net_init(void);
void kuechenproto_net_main(void);
void handleKuechenLichtSetCommand(kuechenLicht_cmd_set*);


#endif /* KUECHENLICHT_UDP_H */

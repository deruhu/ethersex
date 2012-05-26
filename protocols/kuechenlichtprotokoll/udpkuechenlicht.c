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

#define KUECHENLICHT_ETHER

#include <string.h>
#include "udpkuechenlicht.h"
#include "protocols/uip/uip.h"
#include "protocols/uip/uip_router.h"
#include "core/debug.h"
#include "protocols/ecmd/parser.h"
#include "protocols/ecmd/ecmd-base.h"

#include "config.h"

#define BUF ((struct uip_udpip_hdr *) (uip_appdata - UIP_IPUDPH_LEN))

volatile kuechenLichtStatus status={
        {       //Header
                KUECHENLICHT_MAGIC,
                0,
                0,
                KUECHENLICHT_ID_STAT_STATUS,
                sizeof(kuechenLichtStatus)
        },
        {       //kLEDStatus
                0,
                0,
                0,
                0,
        },
        0};

const uint16_t gammakorr[256]={ 0,1,2,4,7,12,18,24,33,42,53,65,79,94,111,129,
        149,170,193,217,243,270,299,330,362,396,432,469,508,549,592,636,
        682,730,779,830,883,938,995,1053,1114,1176,1240,1306,1373,1443,1514,1588,
        1663,1740,1819,1900,1983,2068,2155,2244,2334,2427,2522,2618,2717,2818,2920,3025,
        3131,3240,3351,3463,3578,3695,3814,3935,4058,4183,4310,4439,4570,4703,4839,4976,
        5116,5258,5401,5547,5695,5846,5998,6153,6309,6468,6629,6792,6957,7125,7294,7466,
        7640,7816,7995,8175,8358,8543,8730,8920,9111,9305,9501,9699,9900,10103,10308,10515,
        10725,10936,11150,11367,11585,11806,12029,12255,12482,12712,12945,13179,13416,13655,13897,14141,
        14387,14635,14886,15139,15394,15652,15912,16174,16439,16706,16976,17248,17522,17798,18077,18358,
        18642,18928,19216,19507,19800,20096,20394,20694,20997,21302,21609,21919,22231,22546,22863,23183,
        23505,23829,24156,24485,24817,25151,25488,25827,26168,26512,26858,27207,27558,27912,28268,28627,
        28988,29352,29718,30086,30457,30831,31207,31585,31966,32350,32735,33124,33515,33908,34304,34703,
        35104,35507,35913,36322,36733,37146,37562,37981,38402,38826,39252,39681,40112,40546,40982,41421,
        41863,42307,42753,43203,43654,44109,44566,45025,45487,45952,46419,46888,47361,47836,48313,48793,
        49276,49761,50249,50739,51232,51728,52226,52727,53231,53737,54245,54757,55271,55787,56306,56828,
        57353,57880,58409,58942,59477,60014,60554,61097,61643,62191,62742,63295,63851,64410,64971,65350};


void kuechenproto_net_init() {
    uip_ipaddr_t ip;
    uip_ipaddr_copy(&ip, all_ones_addr);

    uip_udp_conn_t *kuechen_conn = uip_udp_new(&ip, 0, kuechenproto_net_cmd_handler);

    if(! kuechen_conn) {
        debug_printf("Kuenchenlicht: UDP failed\n");
        return;
    }

    uip_udp_conn_t *kuechen_bc_conn = uip_udp_new(&ip, 0, kuechenproto_net_bc_handler);

    if(! kuechen_bc_conn) {
        debug_printf("Kuenchenlicht: UDP_BCast failed\n");
        return;
    }

    uip_udp_bind (kuechen_conn, HTONS(KUECHENLICHT_UDP_CMD_PORT));
    uip_udp_bind (kuechen_bc_conn, HTONS(KUECHENLICHT_UDP_BC_PORT));

    DDRB |= (1<<OC1A) | (1<<OC1B); // Ports OC1A, OC1B mit angeschlossener LED als Ausgang
    TCCR1A = (1<<WGM11) | (1<<WGM12) | (1<<WGM13) & ~(1<<WGM10) | (1<<COM1A1) | (1<<COM1B1); // PWM, fast, 16 bit.
    TCCR1B = (1<<CS10) & ~(1<<CS11) & ~(1<<CS12); // Prescaler 1 = Enable counter

    TCCR2B = (1<<CS21);
    TIMSK2 = (1<<OCIE2A)|(1<<TOIE2);

    ICR1=0xffff;

    OCR1A=0;
    OCR1B=0;

    OCR2A=0x0;
}

void kuechenproto_net_cmd_handler() {
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

            case KUECHENLICHT_ID_CMD_GOTCHA:
                if ((datalength-i)>=sizeof(kuechenLicht_cmd_gotcha))
                    handleKuechenLichtGotchaCommand((kuechenLicht_cmd_gotcha*)pParseHeader);
                i+=sizeof(kuechenLicht_cmd_set);
                break;

            default:
                debug_printf("Kuenchenlicht: unknown messageID received\n");
                i++;
                break;
            }
        }
    }

    OCR1A=gammakorr[status.kLEDStatus.rot];
    OCR1B=gammakorr[status.kLEDStatus.gruen];

}

void kuechenproto_net_bc_handler(void)
{
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
                case KUECHENLICHT_ID_CMD_CAYF:
                    if ((datalength-i)>=sizeof(kuechenLicht_cmd_set))
                        handleKuechenLichtCAYFCommand((kuechenLicht_cmd_come_all_ye_faithful*)pParseHeader);
                    i+=sizeof(kuechenLicht_cmd_come_all_ye_faithful);
                    break;

                default:
                    debug_printf("Kuenchenlicht: unknown messageID received\n");
                    i++;
                    break;
                }
            }
        }

        OCR1A=gammakorr[status.kLEDStatus.rot];
        OCR1B=gammakorr[status.kLEDStatus.gruen];
}

void handleKuechenLichtSetCommand(kuechenLicht_cmd_set* pSetCommand)
{
    if(pSetCommand->checksum==(pSetCommand->kLEDStatus.rot ^ pSetCommand->kLEDStatus.blau ^ pSetCommand->kLEDStatus.gruen))
    {
        status.kLEDStatus=pSetCommand->kLEDStatus;
        uip_ipaddr_copy(status.kHeader.senderAddress, BUF->srcipaddr);

        kuechenLicht_rsp_set* antwort=uip_appdata;

        antwort->kHeader=status.kHeader;
        antwort->kLEDStatus=status.kLEDStatus;
        antwort->checksum=pSetCommand->checksum;

        uip_slen=sizeof(kuechenLicht_rsp_set);

        uip_udp_conn_t echo_conn;
        uip_ipaddr_copy(echo_conn.ripaddr, BUF->srcipaddr);
        echo_conn.rport = BUF->srcport;
        echo_conn.lport = HTONS(KUECHENLICHT_UDP_CMD_PORT);

        uip_udp_conn = &echo_conn;
        uip_process(UIP_UDP_SEND_CONN);
        router_output();

        uip_slen = 0;
    }

    return;
}

void handleKuechenLichtCAYFCommand(kuechenLicht_cmd_come_all_ye_faithful* pCAYFCommand)
{


    kuechenLicht_rsp_cayf* antwort=uip_appdata;

    antwort->kHeader=status.kHeader;
    antwort->kLEDStatus=status.kLEDStatus;
    antwort->kReactedtoCall=pCAYFCommand->kNumberofCalls;

    uip_slen=sizeof(kuechenLicht_rsp_cayf);

    uip_udp_conn_t echo_conn;
    uip_ipaddr_copy(echo_conn.ripaddr, BUF->srcipaddr);
    echo_conn.rport = HTONS(KUECHENLICHT_UDP_CMD_PORT);
    echo_conn.lport = HTONS(KUECHENLICHT_UDP_CMD_PORT);

    uip_udp_conn = &echo_conn;
    uip_process(UIP_UDP_SEND_CONN);
    router_output();

    uip_slen = 0;
}

void handleKuechenLichtGotchaCommand(kuechenLicht_cmd_gotcha* pGotchaCommand)
{

}

/*
  -- Ethersex META --
  header(protocols/kuechenlichtprotikoll/udpkuechenlicht.h)
  net_init(kuechenproto_net_init)
 */

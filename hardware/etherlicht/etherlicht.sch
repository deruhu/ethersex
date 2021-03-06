EESchema Schematic File Version 2  date Mi 29 Feb 2012 23:05:16 CET
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:etherlicht-cache
EELAYER 25  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 1 1
Title "etherlicht"
Date "29 feb 2012"
Rev "1"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	3000 3450 3550 3450
Wire Wire Line
	3550 3450 3550 3150
Connection ~ 2600 2450
Wire Wire Line
	4450 4850 2600 4850
Wire Wire Line
	2600 4850 2600 2450
Connection ~ 2650 1700
Wire Wire Line
	2650 1700 2650 2650
Wire Wire Line
	2650 2650 3550 2650
Connection ~ 2900 1700
Wire Wire Line
	1900 1100 1900 1700
Wire Wire Line
	1900 1700 4450 1700
Wire Wire Line
	3550 2250 2900 2250
Wire Wire Line
	2900 2250 2900 2200
Wire Wire Line
	4450 1700 4450 1950
Wire Wire Line
	1650 1100 1650 1200
Wire Wire Line
	1650 1200 1900 1200
Connection ~ 1900 1200
Wire Wire Line
	1750 2450 3550 2450
Wire Wire Line
	3550 2950 3450 2950
Wire Wire Line
	3450 2950 3450 2850
Wire Wire Line
	3450 2850 3000 2850
$Comp
L CRYSTAL 12M
U 1 1 4F4E84C4
P 3000 3150
F 0 "12M" H 3000 3300 60  0000 C CNN
F 1 "CRYSTAL" H 3000 3000 60  0000 C CNN
	1    3000 3150
	0    1    1    0   
$EndComp
$Comp
L PWR_FLAG #FLG?
U 1 1 4F4E8228
P 1650 1100
F 0 "#FLG?" H 1650 1370 30  0001 C CNN
F 1 "PWR_FLAG" H 1650 1330 30  0000 C CNN
	1    1650 1100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR?
U 1 1 4F4E81EE
P 1750 2400
F 0 "#PWR?" H 1750 2400 30  0001 C CNN
F 1 "GND" H 1750 2330 30  0001 C CNN
	1    1750 2400
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR?
U 1 1 4F4E81DF
P 1900 1100
F 0 "#PWR?" H 1900 1200 30  0001 C CNN
F 1 "VCC" H 1900 1200 30  0000 C CNN
	1    1900 1100
	1    0    0    -1  
$EndComp
$Comp
L R R?
U 1 1 4F4E81C2
P 2900 1950
F 0 "R?" V 2980 1950 50  0000 C CNN
F 1 "10k" V 2900 1950 50  0000 C CNN
	1    2900 1950
	1    0    0    -1  
$EndComp
$Comp
L ATMEGA8-P IC?
U 1 1 4F4E81A7
P 4450 3350
F 0 "IC?" H 3750 4600 50  0000 L BNN
F 1 "ATMEGA8-P" H 4700 1950 50  0000 L BNN
F 2 "DIL28" H 4950 1875 50  0001 C CNN
	1    4450 3350
	1    0    0    -1  
$EndComp
$EndSCHEMATC

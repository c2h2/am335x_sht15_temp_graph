#!/bin/bash
DIR=`pwd`
DB="./temp_humi.rrd" 
#set to C if using Celsius
TEMP_SCALE="C"
 
#define the desired colors for the graphs
TEMP_COLOR="#CC0000"
HUMI_COLOR="#0000FF"
 
W=715
H=420

#hourly
rrdtool graph $DIR/temp_hourly.png --width $W --height $H --start -4h \
DEF:temp=$DIR/temp_humi.rrd:temp:AVERAGE \
AREA:temp$TEMP_COLOR:"Temperature [deg $TEMP_SCALE]" \
DEF:humi=$DIR/temp_humi.rrd:humi:AVERAGE \
LINE1:humi$HUMI_COLOR:"Humidity [RH%]"
 
#daily
rrdtool graph $DIR/temp_daily.png --width $W --height $H --start -1d \
DEF:temp=$DIR/temp_humi.rrd:temp:AVERAGE \
AREA:temp$TEMP_COLOR:"Temperature [deg $TEMP_SCALE]" \
DEF:humi=$DIR/temp_humi.rrd:humi:AVERAGE \
LINE1:humi$HUMI_COLOR:"Humidity [RH%]"
 
#weekly
rrdtool graph $DIR/temp_weekly.png --width $W --height $H --start -1w \
DEF:temp=$DIR/temp_humi.rrd:temp:AVERAGE \
DEF:humi=$DIR/temp_humi.rrd:humi:AVERAGE \
AREA:temp$TEMP_COLOR:"Inside Temperature [deg $TEMP_SCALE]" \
LINE1:humi$HUMI_COLOR:"Humidity [deg $TEMP_SCALE]"
 
#monthly
rrdtool graph $DIR/temp_monthly.png --width $W --height $H --start -1m \
DEF:temp=$DIR/temp_humi.rrd:temp:AVERAGE \
DEF:humi=$DIR/temp_humi.rrd:humi:AVERAGE \
AREA:temp$TEMP_COLOR:"Inside Temperature [deg $TEMP_SCALE]" \
LINE1:humi$HUMI_COLOR:"Humidity [deg $TEMP_SCALE]"
 
#yearly
rrdtool graph $DIR/temp_yearly.png --width $W --height $H --start -1y \
DEF:temp=$DIR/temp_humi.rrd:temp:AVERAGE \
DEF:humi=$DIR/temp_humi.rrd:humi:AVERAGE \
AREA:temp$TEMP_COLOR:"Inside Temperature [deg $TEMP_SCALE]" \
LINE1:humi$HUMI_COLOR:"Humidity [deg $TEMP_SCALE]"

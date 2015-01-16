#!/bin/bash
DIR=`pwd`
DB="./temp_humi.rrd" 
RRD_PATH="./temp_humi.rrd" 
#set to C if using Celsius
TEMP_SCALE="C"
 
#define the desired colors for the graphs
TEMP_COLOR="#CC0000"
HUMI_COLOR="#0000FF"

DATE=`/bin/date +"%Y-%m-%d %H\:%M %P"`
 
W=650
H=375

rrdtool graph "minutely.png" \
--title 'Temperature (1 hour)' \
--vertical-label 'degrees (C), relative humidity (RH%)' \
--width $W \
--height $H \
--start end-1h \
--y-grid 5:1 \
--right-axis 1:0 \
--slope-mode \
"DEF:temp=$RRD_PATH:temp:AVERAGE" \
"DEF:humi=$RRD_PATH:humi:AVERAGE" \
'VDEF:temp_avg=temp,AVERAGE' \
'VDEF:temp_cur=temp,LAST' \
'VDEF:temp_min=temp,MINIMUM' \
'VDEF:temp_max=temp,MAXIMUM' \
'VDEF:humi_avg=humi,AVERAGE' \
'VDEF:humi_cur=humi,LAST' \
'VDEF:humi_min=humi,MINIMUM' \
'VDEF:humi_max=humi,MAXIMUM' \
COMMENT:"\t\t\t\t  Current\t    Avg\t\t   Min\t  Max\l" \
COMMENT:"\t" \
'LINE1:temp#00DD00FF' \
'AREA:temp#00FF0044:temp\t' \
'GPRINT:temp_cur:%6.1lf c\t' \
'GPRINT:temp_avg:%6.1lf c\t' \
'GPRINT:temp_min:%6.1lf c\t' \
'GPRINT:temp_max:%6.1lf c\l' \
COMMENT:"\t" \
'LINE2:humi#0000FF99:humi\t' \
'GPRINT:humi_cur:%6.1lf RH\t' \
'GPRINT:humi_avg:%6.1lf RH\t' \
'GPRINT:humi_min:%6.1lf RH\t' \
'GPRINT:humi_max:%6.1lf RH\t' \
COMMENT:"Generated $DATE"
#'VRULE:temp_max#0000FF' \
#'VRULE:humi_max#00CC00' \



rrdtool graph "hourly.png" \
--title 'Temperature (24 hour)' \
--vertical-label 'degrees (C), relative humidity (RH%)' \
--width $W \
--height $H \
--start end-1d \
--x-grid MINUTE:30:HOUR:1:HOUR:1:0:%l%p \
--y-grid 5:1 \
--right-axis 1:0 \
--slope-mode \
"DEF:temp=$RRD_PATH:temp:AVERAGE" \
"DEF:humi=$RRD_PATH:humi:AVERAGE" \
'VDEF:temp_avg=temp,AVERAGE' \
'VDEF:temp_cur=temp,LAST' \
'VDEF:temp_min=temp,MINIMUM' \
'VDEF:temp_max=temp,MAXIMUM' \
'VDEF:humi_avg=humi,AVERAGE' \
'VDEF:humi_cur=humi,LAST' \
'VDEF:humi_min=humi,MINIMUM' \
'VDEF:humi_max=humi,MAXIMUM' \
COMMENT:"\t\t\t\t  Current\t    Avg\t\t   Min\t  Max\l" \
COMMENT:"\t" \
'LINE1:temp#00DD00FF' \
'AREA:temp#00FF0044:temp\t' \
'GPRINT:temp_cur:%6.1lf c\t' \
'GPRINT:temp_avg:%6.1lf c\t' \
'GPRINT:temp_min:%6.1lf c\t' \
'GPRINT:temp_max:%6.1lf c\l' \
COMMENT:"\t" \
'LINE2:humi#0000FF99:humi\t' \
'GPRINT:humi_cur:%6.1lf RH\t' \
'GPRINT:humi_avg:%6.1lf RH\t' \
'GPRINT:humi_min:%6.1lf RH\t' \
'GPRINT:humi_max:%6.1lf RH\t' \
COMMENT:"Generated $DATE" 
#'VRULE:temp_max#0000FF' \
#'VRULE:humi_max#00CC00' \


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

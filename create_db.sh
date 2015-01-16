#!/bin/bash
rrdtool create temp_humi.rrd --start N --step 10 \
DS:temp:GAUGE:300:U:U \
DS:humi:GAUGE:300:U:U \
RRA:AVERAGE:0.5:1:12 \
RRA:AVERAGE:0.5:1:288 \
RRA:AVERAGE:0.5:12:168 \
RRA:AVERAGE:0.5:12:720 \
RRA:AVERAGE:0.5:288:365

#!/bin/sh

/usr/bin/rrdtool update temp_humi.rrd N:$1:$2

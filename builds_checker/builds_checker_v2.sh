#!/bin/sh

hour=`date '+%k'`
if [ 0$hour -lt 8 -o 0$hour -gt 20 ]; then
	echo "`date` it's not work time"
	exit 0
fi

day_of_week=`date '+%u'`
if [ 0$day_of_week -gt 5 ]; then
	echo "weekend days, no reason to notify"
	exit 0
fi

MP709=/usr/local/bin/mp709

DEV_TRAFFICLIGHT=0
DEV_ALERTLIGHT=1


response=`curl http://logtracker.qa.plesk.ru/yii/index.php?r=Site/Status`

count_bvt=`echo $response | sed -e 's/^.*"bvt":[ \t]*\([0-9][0-9]*\),.*$/\1/'`
count_regression=`echo $response | sed -e 's/^.*"regression":[ \t]*\([0-9][0-9]*\).*$/\1/'`

echo "`date`   bvt fails: $count_bvt;  regression fails: $count_regression"

if [ 0$count_regression -gt 0 ]; then
        $MP709 on $DEV_TRAFFICLIGHT
else
        $MP709 off $DEV_TRAFFICLIGHT
fi

sleep 1

if [ 0$count_bvt -gt 0 ]; then
	$MP709 on $DEV_TRAFFICLIGHT
        sleep 1
        $MP709 on $DEV_ALERTLIGHT
	sleep 30
        $MP709 off $DEV_ALERTLIGHT
else
	$MP709 off $DEV_TRAFFICLIGHT
        sleep 1
        $MP709 off $DEV_ALERTLIGHT
fi

echo "Builds checks are finished"

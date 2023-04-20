#!/bin/sh
#
# The MIT License (MIT)                                                        
# Copyright (c) 2016 Intel Corporation                                         
#                                                                              
# Permission is hereby granted, free of charge, to any person obtaining a copy 
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights 
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell    
# copies of the Software, and to permit persons to whom the Software is        
# furnished to do so, subject to the following conditions:                     
#                                                                              
# The above copyright notice and this permission notice shall be included in   
# all copies or substantial portions of the Software.                          
#                                                                              
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN    
# THE SOFTWARE.                                                                
#  


CONFIGFS_DIR=/sys/kernel/config/device-tree/overlays

BOARD_MODEL=$(cat /proc/device-tree/model)

# Checks to ensure that we have DT overlay support                              
if [ ! -e "$CONFIGFS_DIR" ]; then                       
        echo "Error: Device Tree Overlay not supported"                         
        exit -1                                                                 
fi 

DTBO_NAME=""
case "$BOARD_MODEL" in
	"Terasic DE10-Nano")
		DTBO_NAME="de10-nano.dtbo"
	;;
	*)
		echo "NO BOARD MODEL MATCH"
		exit -1
	;;	
esac

if [ ! -e /lib/firmware/$DTBO_NAME ]; then
	echo "$DTBO_NAME file not found"
	exit -1
fi

mkdir $CONFIGFS_DIR/$DTBO_NAME
echo $DTBO_NAME > $CONFIGFS_DIR/$DTBO_NAME/path

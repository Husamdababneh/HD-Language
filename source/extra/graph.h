/* ========================================================================
$File: graph.h
$Date: December 16th 2020 7:29 pm 
$Creator: Husam 
$Desc:  
=========================================================================*/

#pragma once


enum Shape_Type {
	NORMAL = 0,
	DIAMOND,
	MDIAMOND
};


struct Graph_Label {
	s32 hash = 0;
	Shape_Type type = NORMAL;
	String str;
};
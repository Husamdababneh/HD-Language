/* ========================================================================
$File: graph.h
$Date: December 16th 2020 7:29 pm 
$Creator: Husam 
$Desc:  
=========================================================================*/

#pragma once


enum class  Shape_Type {
	BOX = 0,
	POLYGON,
	ELLIPSE,
	OVAL,
	CIRCLE,
	EGG,
	TRIANGLE,
	DIAMOND,
	TRAPEZIUM,
	PARALLELOGRAM,
	HOUSE,
	PENTAGON,
	HEXAGON,
	SEPTAGON,
	OCTAGON,
	DOUBLECIRCLE,
	DOUBLEOCTAGON,
	TRIPLEOCTAGON,
	INVTRIANGLE,
	INVTRAPEZIUM,
	INVHOUSE,
	MDIAMOND,
	MSQUARE,
	MCIRCLE,
	RECT,
	RECTANGLE,
	SQUARE,
	STAR,
	CYLINDER,
	NOTE,
	TAB,
	FOLDER,
	BOX3D,
	COMPONENT,
	MAX,
};


struct Graph_Label {
	//s32 hash = 0;
	meow_u128 hash = {0};
	Shape_Type type = Shape_Type::BOX;
	StringView str;
};
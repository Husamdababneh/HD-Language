/* ========================================================================
$File: graph.h
$Date: December 16th 2020 7:29 pm 
$Creator: Husam 
$Desc:  
=========================================================================*/

#pragma once


enum Shape_Type {
	GRAPH_BOX = 0,
	GRAPH_POLYGON,
	GRAPH_ELLIPSE,
	GRAPH_OVAL,
	GRAPH_CIRCLE,
	GRAPH_EGG,
	GRAPH_TRIANGLE,
	GRAPH_DIAMOND,
	GRAPH_TRAPEZIUM,
	GRAPH_PARALLELOGRAM,
	GRAPH_HOUSE,
	GRAPH_PENTAGON,
	GRAPH_HEXAGON,
	GRAPH_SEPTAGON,
	GRAPH_OCTAGON,
	GRAPH_DOUBLECIRCLE,
	GRAPH_DOUBLEOCTAGON,
	GRAPH_TRIPLEOCTAGON,
	GRAPH_INVTRIANGLE,
	GRAPH_INVTRAPEZIUM,
	GRAPH_INVHOUSE,
	GRAPH_MDIAMOND,
	GRAPH_MSQUARE,
	GRAPH_MCIRCLE,
	GRAPH_RECT,
	GRAPH_RECTANGLE,
	GRAPH_SQUARE,
	GRAPH_STAR,
	GRAPH_CYLINDER,
	GRAPH_NOTE,
	GRAPH_TAB,
	GRAPH_FOLDER,
	GRAPH_BOX3D,
	GRAPH_COMPONENT,
	GRAPH_MAX,
};


struct Graph_Label {
	StringView str;
	U32 hash;
	Shape_Type type = Shape_Type::GRAPH_BOX;
};
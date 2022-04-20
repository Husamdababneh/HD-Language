/* ========================================================================
$File   : hdarena.h
$Date   : October 16th 2021 0:00 am
$Creator: Husam Dababneh
$Desc   : TODO(Husam)
=========================================================================*/

#ifndef HDARENA_H
#define HDARENA_H


struct MemoryArena
{
	Size size;
	PTR  base;
	Size used;
};


#if !defined(PushStruct) || !defined(PushArray)
#define PushStruct(arena, type) (type*) PushSize(arena, sizeof(type));
#define PushArray(arena, type, count)  (type*) PushSize(arena, sizeof(type)*count);
#endif



#endif //HDARENA_H

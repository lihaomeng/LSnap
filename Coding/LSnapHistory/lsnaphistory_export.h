#ifndef LSNAPHISTORY_EXPORT_H
#define LSNAPHISTORY_EXPORT_H

 #if defined LSNAPHISTORY_DLL
 #define LSNAPHISTORY_EXPORT __declspec(dllexport)
 #else
 #define LSNAPHISTORY_EXPORT __declspec(dllimport)
 #endif
 
#endif // LSNAPHISTORY_EXPORT_H


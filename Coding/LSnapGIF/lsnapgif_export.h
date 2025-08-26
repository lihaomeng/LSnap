#ifndef LSNAPGIF_EXPORT_H
#define LSNAPGIF_EXPORT_H

 #if defined LSNAPGIF_DLL
 #define LSNAPGIF_EXPORT __declspec(dllexport)
 #else
 #define LSNAPGIF_EXPORT __declspec(dllimport)
 #endif
 
#endif // LSNAPGIF_EXPORT_H


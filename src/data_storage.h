/* 
 * File:   data_storage.h
 * Author: david
 *
 * Created on November 1, 2022, 2:31 PM
 */

#ifndef DATA_STORAGE_H
#define	DATA_STORAGE_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct{
        uint8_t data[3];
        uint8_t cmd[1];
    }ADC;

#ifdef	__cplusplus
}
#endif

#endif	/* DATA_STORAGE_H */

